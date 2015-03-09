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
					a row in the cube rowset
*/

#pragma once

#include "soapH.h"

class cube_row
{
	private:
	static const size_t MAX_BUF_SIZE = 1024 * sizeof( wchar_t );
private:
	void copy( const cube_row& other )
	{
		m_schema = nullptr;
		m_shema_updated_by = nullptr;
		m_data_updated_by = nullptr;
		ZeroMemory( &m_cube_guid, sizeof( m_cube_guid ) );
		ZeroMemory( &m_creation_time, sizeof( m_creation_time ) );
		m_last_data_update = m_last_schema_update = m_creation_time;

		m_drill_throught = other.m_drill_throught;
		m_catalog = _wcsdup( other.m_catalog );
		m_cube = _wcsdup( other.m_cube );
		m_cube_type = _wcsdup( other.m_cube_type );
		m_description = _wcsdup( other.m_description );
	}
public:
	wchar_t*			m_catalog;
	wchar_t*			m_schema;
	wchar_t*			m_cube;
	wchar_t*			m_cube_type;
	GUID				m_cube_guid;
	DBTIMESTAMP			m_creation_time;
	DBTIMESTAMP			m_last_schema_update;
	wchar_t*			m_shema_updated_by;
	DBTIMESTAMP			m_last_data_update;
	wchar_t*			m_data_updated_by;
	wchar_t*			m_description;
	DWORD				m_drill_throught;

	

	cube_row( row& a_row ) 
	{
		m_schema = nullptr;
		m_shema_updated_by = nullptr;
		m_data_updated_by = nullptr;
		ZeroMemory( &m_cube_guid, sizeof( m_cube_guid ) );
		ZeroMemory( &m_creation_time, sizeof( m_creation_time ) );
		m_last_data_update = m_last_schema_update = m_creation_time;
		
		m_catalog = _wcsdup( FROM_STRING( a_row.CATALOG_USCORENAME, CP_UTF8 ) ); 
		m_cube = _wcsdup( FROM_STRING( a_row.CUBE_USCORENAME, CP_UTF8 ) );
		m_cube_type = _wcsdup( FROM_STRING( a_row.CUBE_USCORETYPE, CP_UTF8 ) );
		m_description = _wcsdup( FROM_STRING( a_row.DESCRIPTION, CP_UTF8 ) );		
		m_drill_throught = a_row.IS_USCOREDRILLTHROUGH_USCOREENABLED;
	}

	cube_row( const cube_row& other )
	{
		copy( other );
	}

	cube_row& operator=( const cube_row& other )
	{
		if ( this != &other )  {  copy( other ); }
		return *this;
	}

	~cube_row()
	{
		delete[] m_catalog;
		delete[] m_cube;
		delete[] m_cube_type;
		delete[] m_description;
	}

	EMPTY_CONSTRUCTOR(cube_row);

	static char* schema_name() { return "MDSCHEMA_CUBES"; }

	BEGIN_PROVIDER_COLUMN_MAP( cube_row )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CATALOG_NAME", 1, MAX_BUF_SIZE, m_catalog )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "SCHEMA_NAME", 2, MAX_BUF_SIZE, m_schema )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CUBE_NAME", 3, MAX_BUF_SIZE, m_cube )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CUBE_TYPE", 4, MAX_BUF_SIZE, m_cube_type )
	PROVIDER_COLUMN_ENTRY_TYPE( "CUBE_GUID", 5, VT_CLSID, m_cube_guid )
	PROVIDER_COLUMN_ENTRY_TYPE( "CREATED_ON", 6, DBTYPE_DBTIMESTAMP, m_creation_time )
	PROVIDER_COLUMN_ENTRY_TYPE( "LAST_SCHEMA_UPDATE", 7, DBTYPE_DBTIMESTAMP, m_last_schema_update )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "SCHEMA_UPDATED_BY", 8, MAX_BUF_SIZE, m_shema_updated_by )
	PROVIDER_COLUMN_ENTRY_TYPE( "LAST_DATA_UPDATE", 9, DBTYPE_DBTIMESTAMP, m_last_data_update )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DATA_UPDATED_BY", 10, MAX_BUF_SIZE, m_data_updated_by )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DESCRIPTION", 11, MAX_BUF_SIZE, m_description )
	PROVIDER_COLUMN_ENTRY_TYPE( "IS_DRILLTHROUGH_ENABLED", 12, DBTYPE_BOOL, m_drill_throught )
	END_PROVIDER_COLUMN_MAP()
};
