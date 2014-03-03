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
					axis rowset
*/

#pragma once

#include "axis_row.h"

class CXMLAAxisRowset: 
	public CRowsetImpl< CXMLAAxisRowset, char*, command, axis_row>
{
public:
	static ATLCOLUMNINFO* GetColumnInfo( CXMLAAxisRowset* pv, DBORDINAL* pcInfo )
	{
		return pv->m_rgRowData.GetColumnInfo( pcInfo );
	}

	HRESULT Execute(DBPARAMS * /*pParams*/, DBROWCOUNT* pcRowsAffected)
	{
		IExtendCommand* pIExtCommand = NULL;
		HRESULT hr = m_spUnkSite->QueryInterface( __uuidof( IExtendCommand ), ( void**) &pIExtCommand );
	
		if FAILED( hr ) return hr;

		connection_handler* connectionHandler;

		pIExtCommand->GetConnectionHandler(  (void**)&( connectionHandler ) );

		if ( NULL != connectionHandler ) {
			m_rgRowData.setup_data( *pcRowsAffected, connectionHandler );
		}

		pIExtCommand->Release();

		*pcRowsAffected = m_rgRowData.GetCount();

		return S_OK;
	}
};