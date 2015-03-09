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
					command methods
*/

#include "stdafx.h"
#include "rowset.h"
#include "tabular_rowset.h"
#include "session.h"
#include "connection_handler.h"
#include "axis_rowset.h"

int (*connection_handler::fparsehdr)(struct soap*, const char*, const char*) = nullptr;

STDMETHODIMP command::Execute(IUnknown * pUnkOuter, REFIID riid, DBPARAMS * pParams, DBROWCOUNT * pcRowsAffected, IUnknown ** ppRowset)
{
	IUnknown* pSessUnk = NULL;
	GetSite( __uuidof( IUnknown ), ( void** ) &pSessUnk );

	mConnectionHandler.reset( new connection_handler( pSessUnk ) );
	pSessUnk->Release();
	int result = mConnectionHandler->execute( CW2A( m_strCommandText.m_str, CP_UTF8 ) );
	if ( mConnectionHandler->no_session() ) {
		result = mConnectionHandler->execute( CW2A( m_strCommandText.m_str, CP_UTF8 ) );
	}
	if ( S_OK != result ) {
		make_error( FROM_STRING( mConnectionHandler->fault_string(), CP_UTF8 ) );
		return E_FAIL;
	}

	//RIA: this is to corect a "contradiction" in the curent ATL implementation. 
	
	//atldb.h has (line 6195)

	//if (InlineIsEqualGUID(IID_NULL, riid))
	//	{
	//		ATLTRACE(atlTraceDBProvider, 2, _T("IID_NULL was specified in Execute, returning S_OK"));
	//		return S_OK;
	//	}

	//the same method goes on as accepting IID_NULL and has toward the end (line 6306)

	//if (InlineIsEqualGUID(riid, IID_NULL) || ppRowset == NULL)
	//{
	//	if (ppRowset != NULL)
	//		*ppRowset = NULL;
	//	return hrExecute;
	//}

	//line 6306 is consistent to the documentation: http://msdn.microsoft.com/en-us/library/windows/desktop/ms718095(v=vs.85).aspx thas states:

	//If this is IID_NULL, ppRowset is ignored and no rowset is returned, even if the command would otherwise generate a rowset. Specifying IID_NULL 
	//is useful in the case of text commands that do not generate rowsets, such as data definition commands, as a hint to the provider that no rowset 
	//properties need to be verified.

	//I would interpret line 6195 as a bug in ATL because the documentation sugests the command is to be executed but no rowset should be returned.
	//The construct on 6195 prevents the command execution. I think the code on line 6306 aims to the functionality but it is never reached.
	//The following code is workaround this problem.

	if ( InlineIsEqualGUID(IID_NULL, riid) ) {
		IUnknown* dummy;
		HRESULT hr;
		
		if ( mConnectionHandler->has_tabular_data() )
		{
			tabular_rowset*		pRowset;
			hr = CreateRowset(pUnkOuter, riid, pParams, pcRowsAffected, &dummy, pRowset);
		} else
		{
			rowset*				pRowset;
			hr = CreateRowset(pUnkOuter, riid, pParams, pcRowsAffected, &dummy, pRowset);
		}
		if ( SUCCEEDED( hr ) ) {
			dummy->Release();
		}
		*ppRowset = NULL;
		return hr;
	}

	if ( mConnectionHandler->has_tabular_data() )
	{
		tabular_rowset*		pRowset;
		return CreateRowset(pUnkOuter, riid, pParams, pcRowsAffected, ppRowset, pRowset);
	} else
	{
		rowset*				pRowset;
		return CreateRowset(pUnkOuter, riid, pParams, pcRowsAffected, ppRowset, pRowset);
	}
}

STDMETHODIMP command::GetAxisRowset(IUnknown * pUnkOuter, REFIID riid, void * pParams, DBROWCOUNT * pcRowsAffected, IUnknown ** ppRowset)
{
	CXMLAAxisRowset* pRowset;
	return CreateRowset(pUnkOuter, riid, ( DBPARAMS* ) pParams, pcRowsAffected, ppRowset, pRowset);
}

STDMETHODIMP command::GetConnectionHandler( void** connection )
{
	*connection = mConnectionHandler.get();
	return S_OK;
}