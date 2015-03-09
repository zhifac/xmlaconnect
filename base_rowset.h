/*
	ODBO provider for XMLA data stores
    Copyright (C) 2014-2015  ARquery LTD
	http://www.arquery.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	@description
					schema rowset
*/

#include "connection_handler.h"

#pragma once

//This is verbatim copy from atldb.h. It adds correct length definition
//for source data in the case of DBTYPE_WSTR + DBTYPE_BYREF source type

template <class T, class RowClass, class MapClass>
HRESULT TransferData2(
	_In_ _Prepost_bytecount_x_(sizeof(T)) T* pT,
	_In_ bool bReading,
	_In_opt_ void* pData,
	_In_ RowClass* pRow,
	_In_opt_ MapClass* /*pMap*/,
	_In_ HACCESSOR hAccessor)
{
	ATLTRACE(atlTraceDBProvider, 2, _T("TransferData\n"));
	bool bFailed = false;
	bool bSucceeded = false;
	HRESULT hr = S_OK;

	__if_exists(T::Fire_OnFieldChange)
	{
		CAtlArray<DBORDINAL> rgColumns;
		HROW hNotifyRow = NULL;
		//HROW hNotifyRow = pT->m_rgRowHandles.ReverseLookup(pRow);
		{
			POSITION pos = pT->m_rgRowHandles.GetStartPosition();
			while( pos != NULL )
			{
				MapClass::CPair* pPair = pT->m_rgRowHandles.GetNext( pos );
				ATLASSUME( pPair != NULL );
				if( pPair->m_value == pRow )
				{
					hNotifyRow = pPair->m_key;
					break;
				}
			}
		}
	}

	__if_exists(T::Fire_OnRowChange)
	{
		// We need to send the DBREASON_ROW_FIRSTCHANGE notification's
		// SYNCHAFTER phase in this function.  IFF. we're deferred and
		// we have a newly changed row.

		CComVariant varDeferred;
		bool bDeferred;
		hr = pT->GetPropValue(&DBPROPSET_ROWSET, DBPROP_IRowsetUpdate,
							&varDeferred);
		(FAILED(hr) || varDeferred.boolVal == ATL_VARIANT_FALSE) ? bDeferred = false : bDeferred = true;
	}

	// Check for a deleted row
	if( pRow->m_iRowset >= pT->m_rgRowData.GetCount() )
	{
		__if_exists(T::Fire_OnFieldChange)
		{
			if( !bReading )
			{
				SendRowsFirstChangeFailureNotification( pT, pRow, &hNotifyRow, bDeferred );
			}
		}
		return DB_E_DELETEDROW;
	}

	// NOTE: This was checking against DBPENDINGSTATUS_DELETED.  Instead, it
	// should check for DBPENDINGSTATUS_INVALIDROW (means a forced deleted
	// row).

	if (pRow->m_status == DBPENDINGSTATUS_INVALIDROW)
	{
		__if_exists(T::Fire_OnFieldChange)
		{
			if( !bReading )
			{
				SendRowsFirstChangeFailureNotification( pT, pRow, &hNotifyRow, bDeferred );
			}
		}
		return DB_E_DELETEDROW;
	}

	T::_BindType* pBinding;
	bool bFound = pT->m_rgBindings.Lookup((INT_PTR)hAccessor, pBinding);
	if (!bFound || pBinding == NULL)
	{
		__if_exists(T::Fire_OnFieldChange)
		{
			if( !bReading )
			{
				SendRowsFirstChangeFailureNotification( pT, pRow, &hNotifyRow, bDeferred );
			}
		}
		return DB_E_BADACCESSORHANDLE;
	}

	if (pData == NULL && pBinding->cBindings != 0)
	{
		__if_exists(T::Fire_OnFieldChange)
		{
			if( !bReading )
			{
				SendRowsFirstChangeFailureNotification( pT, pRow, &hNotifyRow, bDeferred );
			}
		}
		return E_INVALIDARG;
	}

	void* pDstData;
	void* pSrcData;
	if (bReading)
	{
		pDstData = pData;
		pSrcData = (void*)&(pT->m_rgRowData[(LONG)pRow->m_iRowset]);
	}
	else
	{
		pSrcData = pData;
		pDstData = (void*)&(pT->m_rgRowData[(LONG)pRow->m_iRowset]);
	}

	if (!bReading)
	{
		// Send the OKTODO notification
		__if_exists(T::Fire_OnFieldChange)
		{
			if (/* pRow->m_status != DBPENDINGSTATUS_NEW && */
				pRow->m_status != (DBPENDINGSTATUS_NEW | DBPENDINGSTATUS_UNCHANGED))
			{
				HRESULT hrNotify;
				for (DBORDINAL l=0; l<pBinding->cBindings; l++)
				{
					_ATLTRY
					{
						rgColumns.Add(pBinding->pBindings[l].iOrdinal);
					}
					_ATLCATCH( e )
					{
						_ATLDELETEEXCEPTION( e );
						return E_FAIL;
					}
				}

				hrNotify = pT->Fire_OnFieldChange(pT, hNotifyRow, pBinding->cBindings,
					rgColumns.GetData(), DBREASON_COLUMN_SET,
					DBEVENTPHASE_OKTODO, FALSE);
				if ((hrNotify != S_OK) && (hrNotify != E_FAIL))
				{
					__if_exists(T::Fire_OnRowChange)
					{
						if (bDeferred)
							pT->Fire_OnRowChange(pT, 1, &hNotifyRow,
								DBREASON_ROW_FIRSTCHANGE, DBEVENTPHASE_FAILEDTODO, TRUE);
						return DB_E_CANCELED;
					}
				}

				hrNotify = pT->Fire_OnFieldChange(pT, hNotifyRow, pBinding->cBindings,
					rgColumns.GetData(), DBREASON_COLUMN_SET, DBEVENTPHASE_ABOUTTODO,
					FALSE);
				if ((hrNotify != S_OK) && (hrNotify != E_FAIL))
				{
					__if_exists(T::Fire_OnRowChange)
					{
						if (bDeferred)
							pT->Fire_OnRowChange(pT, 1, &hNotifyRow,
								DBREASON_ROW_FIRSTCHANGE, DBEVENTPHASE_FAILEDTODO, TRUE);
						return DB_E_CANCELED;
					}
				}

				hrNotify = pT->Fire_OnFieldChange(pT, hNotifyRow, pBinding->cBindings,
					rgColumns.GetData(), DBREASON_COLUMN_SET,
					DBEVENTPHASE_SYNCHAFTER, FALSE);
				if ((hrNotify != S_OK) && (hrNotify != E_FAIL))
				{
					__if_exists(T::Fire_OnRowChange)
					{
						if (bDeferred)
							pT->Fire_OnRowChange(pT, 1, &hNotifyRow,
								DBREASON_ROW_FIRSTCHANGE, DBEVENTPHASE_FAILEDTODO, TRUE);
						return DB_E_CANCELED;
					}
				}
			}
		}

		__if_exists(T::Fire_OnRowChange)
		{
			if(bDeferred && pRow->m_status != DBPENDINGSTATUS_CHANGED &&
				pRow->m_status != (DBPENDINGSTATUS_NEW | DBPENDINGSTATUS_UNCHANGED))
			{
				HRESULT hrNotify = pT->Fire_OnRowChange(pT, 1, &hNotifyRow,
					DBREASON_ROW_FIRSTCHANGE, DBEVENTPHASE_SYNCHAFTER, FALSE);

				if ((hrNotify != S_OK) && (hrNotify != E_FAIL))
				{
					__if_exists(T::Fire_OnFieldChange)
					{
						pT->Fire_OnFieldChange(pT, hNotifyRow, pBinding->cBindings,
							rgColumns.GetData(), DBREASON_COLUMN_SET,
							DBEVENTPHASE_FAILEDTODO, TRUE);
					}
					return DB_E_CANCELED;
				}
			}
		}
	}


	DBORDINAL cCols;
	ATLCOLUMNINFO* pColInfo = T::GetColumnInfo(pT, &cCols);
	ATLENSURE_RETURN(pColInfo);
	for (DBORDINAL iBind =0; iBind < pBinding->cBindings; iBind++)
	{
		DBBINDING* pBindCur = &(pBinding->pBindings[iBind]);
		DBORDINAL iColInfo;
		for (iColInfo = 0;
			 iColInfo < cCols && pBindCur->iOrdinal != pColInfo[iColInfo].iOrdinal;
			 iColInfo++);
		if (iColInfo == cCols)
		{
			__if_exists(T::Fire_OnFieldChange)
			{
				if( !bReading )
				{
					SendColumnSetFailureNotification( pT, hNotifyRow, pBinding, rgColumns );
					SendRowsFirstChangeFailureNotification( pT, pRow, &hNotifyRow, bDeferred );
				}
			}
			return DB_E_BADORDINAL;
		}
		ATLCOLUMNINFO* pColCur = &(pColInfo[iColInfo]);
		// Ordinal found at iColInfo

		BYTE* pSrcTemp = (bReading) ? (BYTE*)pSrcData + pColCur->cbOffset :
									  (BYTE*)pSrcData + pBindCur->obValue;

		BYTE* pDstTemp = NULL;
		if (pBindCur->dwPart & DBPART_VALUE)
			pDstTemp = (bReading) ? (BYTE*)pDstData + pBindCur->obValue :
						 (BYTE*)pDstData + pColCur->cbOffset;


		if (!bReading)
		{
			// Check to see that the appropriate data parts are available
			if ((pBindCur->dwPart & DBPART_LENGTH) &&
				!(pBindCur->dwPart & DBPART_VALUE) &&
				!(pBindCur->dwPart & DBPART_STATUS))
			{
				__if_exists(T::Fire_OnFieldChange)
				{
					if (/* pRow->m_status != DBPENDINGSTATUS_NEW && */
						pRow->m_status != (DBPENDINGSTATUS_NEW | DBPENDINGSTATUS_UNCHANGED))
					{
						pT->Fire_OnFieldChange(pT, hNotifyRow, pBinding->cBindings,
							rgColumns.GetData(), DBREASON_COLUMN_SET, DBEVENTPHASE_FAILEDTODO,
							TRUE);
					}
				}

				// Not sure why you would want to run SetData here!
				bFailed = true;
				continue;
			}
		}

		// Handle the the status for any consumer issues
		DBSTATUS dbStat = DBSTATUS_S_OK;
		if (bReading)
		{
			dbStat = pT->GetDBStatus(pRow, pColCur);

			if (dbStat == DBSTATUS_S_ISNULL)
			{
				if (pBindCur->dwPart & DBPART_STATUS)
					*((DBSTATUS*)((BYTE*)(pDstData) + pBindCur->obStatus)) = dbStat;

				// Set the length to 0 as reqiured by the spec.
				if (pBindCur->dwPart & DBPART_LENGTH)
					*((DBLENGTH*)((BYTE*)(pDstData) + pBindCur->obLength)) = 0;

				// Set the destination value to NULL
				if (pBindCur->dwPart & DBPART_VALUE)
				{

					ATLENSURE_RETURN(pDstTemp);
					*pDstTemp = NULL;
				}

				continue;
			}
		}
		else
		{
			// Allow the provider to do checking for DBSTATUS_S_ISNULL
			if (pBindCur->dwPart & DBPART_STATUS)
			{
				dbStat = *((DBSTATUS*)((BYTE*)(pSrcData) + pBindCur->obStatus));

				// Return DBSTATUS_E_UNAVAILABLE if the status is DBSTATUS_S_OK
				//	and either the value part is not bound or the length part is
				//	bound and the type is DBTYPE_BYTES.

				// There was another entry of code here with LENGTH, NO VALUE,
				//	and status was not DBSTATUS_S_ISNULL.  May need to regenerate that
				if (dbStat == DBSTATUS_S_OK)
				{
					if (!(pBindCur->dwPart & DBPART_VALUE) ||
						((pBindCur->dwPart & DBPART_LENGTH) && (pBindCur->wType == DBTYPE_BYTES)))
					{
						// Can't set non-null columns w/o a value part
						__if_exists(T::Fire_OnFieldChange)
						{
							if (/* pRow->m_status != DBPENDINGSTATUS_NEW && */
								pRow->m_status != (DBPENDINGSTATUS_NEW | DBPENDINGSTATUS_UNCHANGED))
							{
								pT->Fire_OnFieldChange(pT, hNotifyRow, pBinding->cBindings,
									rgColumns.GetData(), DBREASON_COLUMN_SET, DBEVENTPHASE_FAILEDTODO,
									TRUE);
							}
						}

						bFailed = true;
						*((DBSTATUS*)((BYTE*)(pSrcData) + pBindCur->obStatus)) = DBSTATUS_E_UNAVAILABLE;
						continue;
					}
				}

				switch (dbStat)
				{
				case DBSTATUS_S_ISNULL:
					if (!(pColCur->dwFlags & DBCOLUMNFLAGS_ISNULLABLE) ||
						FAILED(pT->SetDBStatus(&dbStat, pRow, pColCur)))
					{
						__if_exists(T::Fire_OnFieldChange)
						{
							if (/* pRow->m_status != DBPENDINGSTATUS_NEW && */
								pRow->m_status != (DBPENDINGSTATUS_NEW | DBPENDINGSTATUS_UNCHANGED))
							{
								pT->Fire_OnFieldChange(pT, hNotifyRow,
									pBinding->cBindings, rgColumns.GetData(),
									DBREASON_COLUMN_SET, DBEVENTPHASE_FAILEDTODO,
									TRUE);
							}
						}

						// Special processing for attempting to write, read-only columns
						if (!(pColCur->dwFlags & DBCOLUMNFLAGS_ISNULLABLE))
							*((DBSTATUS*)((BYTE*)(pSrcData) + pBindCur->obStatus)) = DBSTATUS_E_INTEGRITYVIOLATION;

						bFailed = true;
					}
					else
					{
						__if_exists(T::Fire_OnFieldChange)
						{
							if (/* pRow->m_status != DBPENDINGSTATUS_NEW && */
								pRow->m_status != (DBPENDINGSTATUS_NEW | DBPENDINGSTATUS_UNCHANGED))
							{
								pT->Fire_OnFieldChange(pT, hNotifyRow, 1, &iBind,
									DBREASON_COLUMN_SET, DBEVENTPHASE_DIDEVENT, TRUE);
							}
						}
						bSucceeded = true;
						dbStat = DBSTATUS_S_OK;
						if (pBindCur->dwPart & DBPART_VALUE)
						{
							ATLENSURE_RETURN(pDstTemp);
							*pDstTemp = NULL;
						}
					}
					continue;
					break;
				case DBSTATUS_S_DEFAULT:
				case DBSTATUS_S_IGNORE:
				{
					HRESULT hrStatus = pT->SetDBStatus(&dbStat, pRow, pColCur);
					*((DBSTATUS*)((BYTE*)(pSrcData) + pBindCur->obStatus)) = dbStat;

					if (FAILED(hrStatus))
					{
						__if_exists(T::Fire_OnFieldChange)
						{
							if (/* pRow->m_status != DBPENDINGSTATUS_NEW && */
								pRow->m_status != (DBPENDINGSTATUS_NEW | DBPENDINGSTATUS_UNCHANGED))
							{
								pT->Fire_OnFieldChange(pT, hNotifyRow, pBinding->cBindings,
									rgColumns.GetData(), DBREASON_COLUMN_SET,
									DBEVENTPHASE_FAILEDTODO, TRUE);
							}
						}

						// Note, status should be set by SetDBStatus
						bFailed = true;
					}
					else
					{
						__if_exists(T::Fire_OnFieldChange)
						{
							if (/* pRow->m_status != DBPENDINGSTATUS_NEW && */
								pRow->m_status != (DBPENDINGSTATUS_NEW | DBPENDINGSTATUS_UNCHANGED))
							{
								pT->Fire_OnFieldChange(pT, hNotifyRow, pBinding->cBindings,
									rgColumns.GetData(), DBREASON_COLUMN_SET,
									DBEVENTPHASE_DIDEVENT, TRUE);
							}
						}
						bSucceeded = true;
					}
					continue;
					break;
				}
				case DBSTATUS_S_OK:
					// Still call SetDBStatus here as they may have locks on
					// integrity contstraints to observe
					if (FAILED(pT->SetDBStatus(&dbStat, pRow, pColCur)))
					{
						__if_exists(T::Fire_OnFieldChange)
						{
							if (/* pRow->m_status != DBPENDINGSTATUS_NEW && */
								pRow->m_status != (DBPENDINGSTATUS_NEW | DBPENDINGSTATUS_UNCHANGED))
							{
								pT->Fire_OnFieldChange(pT, hNotifyRow, pBinding->cBindings,
									rgColumns.GetData(), DBREASON_COLUMN_SET,
									DBEVENTPHASE_FAILEDTODO, TRUE);
							}
						}

						bFailed = true;
						*((DBSTATUS*)((BYTE*)(pSrcData) + pBindCur->obStatus)) = dbStat;
						continue;
					}
					break;
				default:
					*((DBSTATUS*)((BYTE*)(pSrcData) + pBindCur->obStatus)) = DBSTATUS_E_BADSTATUS;

					__if_exists(T::Fire_OnFieldChange)
					{
						if (/* pRow->m_status != DBPENDINGSTATUS_NEW && */
							pRow->m_status != (DBPENDINGSTATUS_NEW | DBPENDINGSTATUS_UNCHANGED))
						{
							pT->Fire_OnFieldChange(pT, hNotifyRow, pBinding->cBindings,
								rgColumns.GetData(), DBREASON_COLUMN_SET,
								DBEVENTPHASE_FAILEDTODO, TRUE);
						}
					}

					bFailed = true;
					continue;
					break;
				}
			}
		}

		// Determine sizes of input and output columns
		DBLENGTH cbCol = 0;
		DBLENGTH cbDst;
		if (bReading)
			cbDst = pBindCur->cbMaxLen;
		else
			cbDst = pColCur->ulColumnSize;

		switch (pColCur->wType)
		{
		case DBTYPE_STR:
			if (bReading)
				cbCol = lstrlenA((LPSTR)(((BYTE*)pSrcData) + pColCur->cbOffset));
			else
			{
				// Use the length field when setting data
				if (pBindCur->dwPart & DBPART_LENGTH)
					cbCol = *((DBLENGTH*)((BYTE*)(pSrcData) + pBindCur->obLength));
				else
					cbCol = lstrlenA((LPSTR)(pSrcTemp));	// was cbDst

				if (cbCol >= cbDst)
				{
					if (cbCol > (cbDst + 1)) // over maximum case
					{
						__if_exists(T::Fire_OnFieldChange)
						{
							if (/* pRow->m_status != DBPENDINGSTATUS_NEW && */
								pRow->m_status != (DBPENDINGSTATUS_NEW | DBPENDINGSTATUS_UNCHANGED))
							{
								pT->Fire_OnFieldChange(pT, hNotifyRow, pBinding->cBindings,
									rgColumns.GetData(), DBREASON_COLUMN_SET,
									DBEVENTPHASE_FAILEDTODO, TRUE);
							}
						}

						bFailed = true;
						if (pBindCur->dwPart & DBPART_STATUS)
							*((DBSTATUS*)((BYTE*)(pSrcData) + pBindCur->obStatus)) = DBSTATUS_E_CANTCONVERTVALUE;
						continue;
					}
				}
				cbCol = cbDst;	// Leave room for NULL term. need to copy for WSTR
			}
			break;
		case DBTYPE_WSTR + DBTYPE_BYREF:
			if ( nullptr == pSrcTemp || nullptr == *( (void**)pSrcTemp ) )
			{
				cbCol = 0;
			} else
			{
				cbCol = wcslen(*((wchar_t**)pSrcTemp))*2;
			}
			break;
		case DBTYPE_WSTR:
		case DBTYPE_BSTR:
			if (bReading)
				cbCol = lstrlenW((LPWSTR)(((BYTE*)pSrcData) + pColCur->cbOffset)) * sizeof(WCHAR);
			else
			{
				if (pBindCur->dwPart & DBPART_LENGTH)
					cbCol = *((DBLENGTH*)((BYTE*)(pSrcData) + pBindCur->obLength));
				else
					cbCol = lstrlenW((LPWSTR)(pSrcData)) * sizeof(WCHAR);

				if (cbCol >= cbDst)
				{
					if (cbCol > (cbDst + 1)) // over maximum case
					{
						__if_exists(T::Fire_OnFieldChange)
						{
							if (/* pRow->m_status != DBPENDINGSTATUS_NEW && */
								pRow->m_status != (DBPENDINGSTATUS_NEW | DBPENDINGSTATUS_UNCHANGED))
							{
								pT->Fire_OnFieldChange(pT, hNotifyRow, pBinding->cBindings,
									rgColumns.GetData(), DBREASON_COLUMN_SET,
									DBEVENTPHASE_FAILEDTODO, TRUE);
							}
						}

						bFailed = true;
						if (pBindCur->dwPart & DBPART_STATUS)
							*((DBSTATUS*)((BYTE*)(pSrcData) + pBindCur->obStatus)) = DBSTATUS_E_CANTCONVERTVALUE;
						continue;
					}
				}
				cbCol = cbDst;	// Leave room for NULL term. need to copy for WSTR
			}
			break;
		case DBTYPE_BYTES:
			if (bReading)
				cbCol = pColCur->ulColumnSize;
			else
			{
				if (pBindCur->dwPart & DBPART_LENGTH)
					cbCol = *((DBLENGTH *)((BYTE*)(pSrcData) + pBindCur->obLength));
				else
				{
					__if_exists(T::Fire_OnFieldChange)
					{
						if (/* pRow->m_status != DBPENDINGSTATUS_NEW && */
							pRow->m_status != (DBPENDINGSTATUS_NEW | DBPENDINGSTATUS_UNCHANGED))
						{
							pT->Fire_OnFieldChange(pT, hNotifyRow, pBinding->cBindings,
								rgColumns.GetData(), DBREASON_COLUMN_SET,
								DBEVENTPHASE_FAILEDTODO, TRUE);
						}
					}

					// If no length part is bound for DBTYPE_BYTES, it is an error
					bFailed = true;
					continue;
				}

				if (cbCol >= cbDst)
					cbCol = cbDst;	// Leave room for NULL term. need to copy for WSTR
			}
			break;
		default:
			if (bReading)
				cbCol = pColCur->ulColumnSize;
			else
				cbDst = pColCur->ulColumnSize;

			break;
		}
		cbDst=cbCol;

		// Handle cases where we have provider owned memory.  Note, these should be
		// with DBTYPE_BYREF (otherwise, it doesn't make sense).
		if (pBindCur->dwPart & DBPART_VALUE)
		{
			if (pBindCur->dwMemOwner == DBMEMOWNER_PROVIDEROWNED
				&& pBindCur->wType & DBTYPE_BYREF)
			{
				ATLENSURE_RETURN(pDstTemp);
				*(BYTE**)pDstTemp = pSrcTemp;
			}
			else
			{
				ATLASSERT(pT->m_spConvert != NULL);
				ATLASSUME(pDstTemp != NULL);

				if ( 0 != cbCol )
				{
					hr = pT->m_spConvert->DataConvert(pColCur->wType, pBindCur->wType,
							cbCol, &cbDst, pSrcTemp, pDstTemp, pBindCur->cbMaxLen,
							dbStat, &dbStat, pBindCur->bPrecision, pBindCur->bScale,0);
				} else {
					//pDstTemp[0]=0;
					//do nothing;
				}
			}
		}
		if (pBindCur->dwPart & DBPART_LENGTH)
		{
			if (bReading)
				*((DBLENGTH*)((BYTE*)(pDstData) + pBindCur->obLength)) = (dbStat == DBSTATUS_S_ISNULL) ? 0 : cbDst;
			else
				*((DBLENGTH*)((BYTE*)(pSrcData) + pBindCur->obLength)) = cbDst;
		}
		if (pBindCur->dwPart & DBPART_STATUS)
		{
			if (bReading)
				*((DBSTATUS*)((BYTE*)(pDstData) + pBindCur->obStatus)) = dbStat;
			else
				*((DBSTATUS*)((BYTE*)(pSrcData) + pBindCur->obStatus)) = dbStat;
		}

		if (FAILED(hr))
		{
			if (!bReading)
			{
				__if_exists(T::Fire_OnFieldChange)
				{
					if (/* pRow->m_status != DBPENDINGSTATUS_NEW && */
						pRow->m_status != (DBPENDINGSTATUS_NEW | DBPENDINGSTATUS_UNCHANGED))
					{
						pT->Fire_OnFieldChange(pT, hNotifyRow, pBinding->cBindings,
							rgColumns.GetData(), DBREASON_COLUMN_SET,
							DBEVENTPHASE_FAILEDTODO, TRUE);
					}
				}
			}

			bFailed = true;
		}
		else
		{
			bSucceeded = true;
		}
	}

	// Return error codes to the consumer
	if (bFailed)
	{
		__if_exists(T::Fire_OnFieldChange)
		{
			if( !bReading )
			{
//				SendColumnSetFailureNotification( pT, hNotifyRow, pBinding, rgColumns );
				SendRowsFirstChangeFailureNotification( pT, pRow, &hNotifyRow, bDeferred );
			}
		}
		return (bSucceeded != false) ? DB_S_ERRORSOCCURRED : DB_E_ERRORSOCCURRED;
	}
	else
	{
		if (!bReading)
		{
			__if_exists(T::Fire_OnFieldChange)
			{
				if (/* pRow->m_status != DBPENDINGSTATUS_NEW && */
					pRow->m_status != (DBPENDINGSTATUS_NEW | DBPENDINGSTATUS_UNCHANGED))
				{
					pT->Fire_OnFieldChange(pT, hNotifyRow, pBinding->cBindings,
						rgColumns.GetData(), DBREASON_COLUMN_SET,
						DBEVENTPHASE_DIDEVENT, TRUE);
				}
			}
		}

		return hr;
	}
}





template <class T, class Storage, class CreatorClass >
class base_rowset :
	public CSchemaRowsetImpl< T, Storage, CreatorClass>,
	public IErrorRecordsImpl< base_rowset<T, Storage, CreatorClass>>
{
public:

	//IDBSchemaRowset implementation
	HRESULT Execute( DBROWCOUNT* pcRowsAffected, ULONG cRestrictions, const VARIANT* rgRestrictions ) 
	{
		if ( 0 == strcmp( "UNSUPORTED", Storage::schema_name() ) ) {
			pcRowsAffected = 0;
			return S_OK;
		}

		connection_handler handler( m_spUnkSite );

		int result = handler.discover( Storage::schema_name(), cRestrictions, rgRestrictions);
		if ( handler.no_session() ) {
			result = handler.discover( Storage::schema_name(), cRestrictions, rgRestrictions);
		}
		if ( S_OK != result ) {
			make_error( FROM_STRING( handler.fault_string(), CP_UTF8 ) );
			return E_FAIL;
		}

		for ( int i = 0, e = handler.discover_response().cxmla__return__.root.__rows.__size; i < e; ++i ) {			
			if ( 0 == strcmp( "MDSCHEMA_LEVELS", Storage::schema_name() ) ) {
				if ( 0 == strcmp( "[Measures]", handler.discover_response().cxmla__return__.root.__rows.row[i].DIMENSION_USCOREUNIQUE_USCORENAME ) )
				{
					//Somehow we will crash under excel without this hack.
					row dummy_level;
					dummy_level.CATALOG_USCORENAME = handler.discover_response().cxmla__return__.root.__rows.row[i].CATALOG_USCORENAME;
					dummy_level.CUBE_USCORENAME = handler.discover_response().cxmla__return__.root.__rows.row[i].CUBE_USCORENAME;
					dummy_level.DIMENSION_USCOREUNIQUE_USCORENAME = handler.discover_response().cxmla__return__.root.__rows.row[i].DIMENSION_USCOREUNIQUE_USCORENAME;
					dummy_level.HIERARCHY_USCOREUNIQUE_USCORENAME = handler.discover_response().cxmla__return__.root.__rows.row[i].HIERARCHY_USCOREUNIQUE_USCORENAME;
					dummy_level.LEVEL_USCORECARDINALITY = handler.discover_response().cxmla__return__.root.__rows.row[i].LEVEL_USCORECARDINALITY;
					dummy_level.LEVEL_USCORETYPE = handler.discover_response().cxmla__return__.root.__rows.row[i].LEVEL_USCORETYPE;
					dummy_level.LEVEL_USCORENAME = "Level00";
					dummy_level.LEVEL_USCOREUNIQUE_USCORENAME = "[Measures].[Level00]";
					dummy_level.LEVEL_USCORECAPTION = "Level00";
					dummy_level.LEVEL_USCORENUMBER = "-1";
					dummy_level.DESCRIPTION = "Level00";
					m_rgRowData.Add( Storage( dummy_level ) );
				}
			}
			m_rgRowData.Add( Storage( handler.discover_response().cxmla__return__.root.__rows.row[i] ) );
		}


		*pcRowsAffected = (LONG) m_rgRowData.GetCount();
		return S_OK;
	}

public:
	//Standard method from atldb.h. Maps instead to the locally enriched TransferData2
	STDMETHOD(GetData)(
		_In_ HROW hRow,
		_In_ HACCESSOR hAccessor,
		_In_opt_ void *pDstData)
	{
		T* pT = (T*)this;
		CSimpleRow* pRow;
		if (hRow == NULL ) { return DB_E_BADROWHANDLE; }

		if( !pT->m_rgRowHandles.Lookup((INT_PTR)hRow, pRow)) { return DB_E_BADROWHANDLE; }

		if (pRow == NULL) { return DB_E_BADROWHANDLE; }

		return TransferData2<T, CSimpleRow, CAtlMap < CSimpleRow::KeyType, CSimpleRow* >>
						   (pT, true, pDstData, pRow, &(pT->m_rgRowHandles), hAccessor);
	}

public:
	BEGIN_PROPSET_MAP( T )
	BEGIN_PROPERTY_SET( DBPROPSET_ROWSET )
	PROPERTY_INFO_ENTRY( IAccessor )
	PROPERTY_INFO_ENTRY( IColumnsInfo )
	PROPERTY_INFO_ENTRY( IConvertType )
	PROPERTY_INFO_ENTRY( IRowset )
	PROPERTY_INFO_ENTRY( IRowsetIdentity )
	PROPERTY_INFO_ENTRY( IRowsetInfo )
	PROPERTY_INFO_ENTRY( ISupportErrorInfo )
	PROPERTY_INFO_ENTRY( CANFETCHBACKWARDS )
	PROPERTY_INFO_ENTRY( CANHOLDROWS )
	PROPERTY_INFO_ENTRY( CANSCROLLBACKWARDS )
	PROPERTY_INFO_ENTRY_VALUE( MAXOPENROWS, 0 )
	PROPERTY_INFO_ENTRY_VALUE( MAXROWS, 0 )
	END_PROPERTY_SET( DBPROPSET_ROWSET )
	END_PROPSET_MAP()
};
