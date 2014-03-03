/*
	ODBO provider for XMLA data stores
    Copyright (C) 2014  Yalos Software Labs
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

template <class T, class Storage, class CreatorClass >
class base_rowset :
	public CSchemaRowsetImpl< T, Storage, CreatorClass>,
	public IErrorRecordsImpl< base_rowset<T, Storage, CreatorClass>>
{
private:
	void make_error( LPOLESTR  a_description  )
	{
		ICreateErrorInfo	*cerr_info;
		IErrorInfo			*err_info;
		HRESULT				hr;

		if FAILED( hr = CreateErrorInfo(&cerr_info) ) return;

		hr = cerr_info->QueryInterface(IID_IErrorInfo, (LPVOID FAR*) &err_info);

		if ( FAILED( hr ) )
		{
			cerr_info->Release();
			return;
		}

		cerr_info->SetDescription( a_description );
		cerr_info->SetGUID(  __uuidof(IDBSchemaRowset) );
		SetErrorInfo(0, err_info);

		err_info->Release();
		cerr_info->Release();
	}
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
			//make_error( FROM_STRING( handler.faultString(), CP_UTF8 ) );
			return DB_S_ERRORSOCCURRED;
		}

		for ( int i = 0, e = handler.discover_response().cxmla__return__.root.__rows.__size; i < e; ++i ) {
			m_rgRowData.Add( Storage( handler.discover_response().cxmla__return__.root.__rows.row[i] ) );
		}

		*pcRowsAffected = (LONG) m_rgRowData.GetCount();
		return S_OK;
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
