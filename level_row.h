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
					a row in the level rowset
*/

#pragma once

#include "soapH.h"

class level_row
{
private:
	static const size_t MAX_BUF_SIZE = 10 * 1024 * sizeof( wchar_t );
private:
	void copy( const level_row& other )
	{
		m_schema = nullptr;

		m_level_number = other.m_level_number;
		m_level_cardinality = other.m_level_cardinality;
		m_level_type = other.m_level_type;

		m_catalog = _wcsdup( other.m_catalog );
		m_cube = _wcsdup( other.m_cube );
		m_dimension_unique_name = _wcsdup( other.m_dimension_unique_name );
		m_hierarcy_unique_name = _wcsdup( other.m_hierarcy_unique_name );
		m_level_name = _wcsdup( other.m_level_name );
		m_level_unique_name = _wcsdup( other.m_level_unique_name );
		m_level_caption = _wcsdup( other.m_level_caption );
		m_description = _wcsdup( other.m_description );
	}

public:
	wchar_t*         m_catalog;
	wchar_t*         m_schema;
	wchar_t*         m_cube;
	wchar_t*         m_dimension_unique_name;
	wchar_t*         m_hierarcy_unique_name;
	wchar_t*         m_level_name;
	wchar_t*         m_level_unique_name;
	GUID             m_level_guid;
	wchar_t*         m_level_caption;
	unsigned long    m_level_number;
	unsigned long    m_level_cardinality;
	long             m_level_type;
	wchar_t*         m_description;

	level_row( row& a_row )
	{
		m_schema = nullptr;

		m_catalog = _wcsdup( FROM_STRING( a_row.CATALOG_USCORENAME, CP_UTF8 ) );
		m_cube = _wcsdup( FROM_STRING( a_row.CUBE_USCORENAME, CP_UTF8 ) );
		m_dimension_unique_name = _wcsdup( FROM_STRING( a_row.DIMENSION_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		m_hierarcy_unique_name = _wcsdup( FROM_STRING( a_row.HIERARCHY_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		m_level_name = _wcsdup( FROM_STRING( a_row.LEVEL_USCORENAME, CP_UTF8 ) );
		m_level_unique_name = _wcsdup( FROM_STRING( a_row.LEVEL_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		m_level_caption = _wcsdup( FROM_STRING( a_row.LEVEL_USCORECAPTION, CP_UTF8 ) );
		m_description = _wcsdup( FROM_STRING( a_row.DESCRIPTION, CP_UTF8 ) );

		m_level_number = get_int( a_row.LEVEL_USCORENUMBER );
		m_level_cardinality = get_int( a_row.LEVEL_USCORECARDINALITY );
		m_level_type = get_int( a_row.LEVEL_USCORETYPE );
	}

	level_row( const level_row& other )
	{
		copy( other );
	}

	level_row& operator=( const level_row& other )
	{
		if ( this != &other )  {  copy( other ); }
		return *this;
	}

	~level_row()
	{
		delete[] m_catalog;
		delete[] m_cube;
		delete[] m_dimension_unique_name;
		delete[] m_hierarcy_unique_name;
		delete[] m_level_name;
		delete[] m_level_unique_name;
		delete[] m_level_caption;
		delete[] m_description;
	}

	EMPTY_CONSTRUCTOR(level_row);
	static char* schema_name() { return "MDSCHEMA_LEVELS"; }

	BEGIN_PROVIDER_COLUMN_MAP( level_row )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CATALOG_NAME", 1, MAX_BUF_SIZE, m_catalog )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "SCHEMA_NAME", 2, MAX_BUF_SIZE, m_schema )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CUBE_NAME", 3, MAX_BUF_SIZE, m_cube )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DIMENSION_UNIQUE_NAME", 4, MAX_BUF_SIZE, m_dimension_unique_name )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "HIERARCHY_UNIQUE_NAME", 5, MAX_BUF_SIZE, m_hierarcy_unique_name )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "LEVEL_NAME", 6, MAX_BUF_SIZE, m_level_name )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "LEVEL_UNIQUE_NAME", 7, MAX_BUF_SIZE, m_level_unique_name )
	PROVIDER_COLUMN_ENTRY_TYPE( "LEVEL_GUID", 8, VT_CLSID, m_level_guid )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "LEVEL_CAPTION", 9, MAX_BUF_SIZE, m_level_caption )
	PROVIDER_COLUMN_ENTRY_TYPE( "LEVEL_NUMBER", 10, VT_UI4, m_level_number )
	PROVIDER_COLUMN_ENTRY_TYPE( "LEVEL_CARDINALITY", 11, VT_UI4, m_level_cardinality )
	PROVIDER_COLUMN_ENTRY_TYPE( "LEVEL_TYPE", 12, VT_I4, m_level_type )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DESCRIPTION", 13, MAX_BUF_SIZE, m_description )
	END_PROVIDER_COLUMN_MAP()
};