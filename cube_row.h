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
					a row in the cube rowset
*/

#pragma once

#include "soapH.h"

class cube_row
{
public:
	wchar_t			m_catalog[256];
	wchar_t			m_schema[256];
	wchar_t			m_cube[256];
	wchar_t			m_cube_type[256];
	GUID            m_cube_guid;
	DBTIMESTAMP		m_creation_time;
	DBTIMESTAMP		m_last_schema_update;
	wchar_t			m_shema_updated_by[256];
	DBTIMESTAMP		m_last_data_update;
	wchar_t			m_data_updated_by[256];
	wchar_t			m_description[256];
	DWORD			m_drill_throught;

	EMPTY_CONSTRUCTOR(cube_row);

	cube_row( row& a_row ) 
	{
		m_schema[0] = 0;
		m_shema_updated_by[0] = 0;
		m_data_updated_by[0] = 0;


		wcscpy_s( m_catalog, 256, FROM_STRING( a_row.CATALOG_USCORENAME, CP_UTF8 ) ); 
		wcscpy_s( m_cube, 256, FROM_STRING( a_row.CUBE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_cube_type, 256, FROM_STRING( a_row.CUBE_USCORETYPE, CP_UTF8 ) );
		wcscpy_s( m_description, 256, FROM_STRING( a_row.CATALOG_USCORENAME, CP_UTF8 ) );

		ZeroMemory( &m_cube_guid, sizeof( m_cube_guid ) );
		ZeroMemory( &m_creation_time, sizeof( m_creation_time ) );

		m_last_data_update = m_last_schema_update = m_creation_time;
		m_drill_throught = 0;
	}

	static char* schema_name() { return "MDSCHEMA_CUBES"; }

	BEGIN_PROVIDER_COLUMN_MAP( cube_row )
	PROVIDER_COLUMN_ENTRY_WSTR( "CATALOG_NAME", 1, m_catalog )
	PROVIDER_COLUMN_ENTRY_WSTR( "SCHEMA_NAME", 2, m_schema )
	PROVIDER_COLUMN_ENTRY_WSTR( "CUBE_NAME", 3, m_cube )
	PROVIDER_COLUMN_ENTRY_WSTR( "CUBE_TYPE", 4, m_cube_type )
	PROVIDER_COLUMN_ENTRY_TYPE( "CUBE_GUID", 5, VT_CLSID, m_cube_guid )
	PROVIDER_COLUMN_ENTRY_TYPE( "CREATED_ON", 6, DBTYPE_DBTIMESTAMP, m_creation_time )
	PROVIDER_COLUMN_ENTRY_TYPE( "LAST_SCHEMA_UPDATE", 7, DBTYPE_DBTIMESTAMP, m_last_schema_update )
	PROVIDER_COLUMN_ENTRY_WSTR( "SCHEMA_UPDATED_BY", 8, m_shema_updated_by )
	PROVIDER_COLUMN_ENTRY_TYPE( "LAST_DATA_UPDATE", 9, DBTYPE_DBTIMESTAMP, m_last_data_update )
	PROVIDER_COLUMN_ENTRY_WSTR( "DATA_UPDATED_BY", 10, m_data_updated_by )
	PROVIDER_COLUMN_ENTRY_WSTR( "DESCRIPTION", 11, m_description )
	PROVIDER_COLUMN_ENTRY_TYPE( "IS_DRILLTHROUGH_ENABLED", 12, DBTYPE_BOOL, m_drill_throught )
	END_PROVIDER_COLUMN_MAP()
};
