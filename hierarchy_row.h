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
					a row in the hierarchy rowset
*/

#pragma once

#include "soapH.h"

class hierarchy_row
{
private:
	static const size_t MAX_BUF_SIZE = 10 * 1024 * sizeof( wchar_t );
private:
	void copy( const hierarchy_row& other )
	{
		m_schema = nullptr;

		m_catalog = _wcsdup( other.m_catalog );
		m_cube = _wcsdup( other.m_cube );
		m_dimension_unique_name = _wcsdup( other.m_dimension_unique_name );
		m_hierarchy_name = _wcsdup( other.m_hierarchy_name );
		m_hierarchy_unique_name = _wcsdup( other.m_hierarchy_unique_name );
		m_hierarchy_caption = _wcsdup( other.m_hierarchy_caption );		
		m_default_member = _wcsdup( other.m_default_member );
		m_all_member = _wcsdup( other.m_all_member );
		m_description = _wcsdup( other.m_description);
		m_dimension_type = other.m_dimension_type;
		m_hierarcy_cardinality = other.m_hierarcy_cardinality;
		m_structure = other.m_structure;
	}
public:
	wchar_t*         m_catalog;
	wchar_t*         m_schema;
	wchar_t*         m_cube;
	wchar_t*         m_dimension_unique_name;
	wchar_t*         m_hierarchy_name;
	wchar_t*         m_hierarchy_unique_name;
	GUID             m_hierarchy_guid;
	wchar_t*         m_hierarchy_caption;
	short            m_dimension_type;
	unsigned long    m_hierarcy_cardinality;
	wchar_t*         m_default_member;
	wchar_t*         m_all_member;
	wchar_t*         m_description;
	short            m_structure;

	hierarchy_row( row& a_row )
	{
		m_schema = nullptr;

		m_catalog = _wcsdup( FROM_STRING( a_row.CATALOG_USCORENAME, CP_UTF8 ) );
		m_cube = _wcsdup( FROM_STRING( a_row.CUBE_USCORENAME, CP_UTF8 ) );
		m_dimension_unique_name = _wcsdup( FROM_STRING( a_row.DIMENSION_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		m_hierarchy_name = _wcsdup( FROM_STRING( a_row.HIERARCHY_USCORENAME, CP_UTF8 ) );
		m_hierarchy_unique_name = _wcsdup( FROM_STRING( a_row.HIERARCHY_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		m_hierarchy_caption = _wcsdup(FROM_STRING( a_row.HIERARCHY_USCORECAPTION, CP_UTF8 ));
		m_description = _wcsdup( FROM_STRING( a_row.DESCRIPTION, CP_UTF8 ) );
		
		if ( nullptr != a_row.ALL_USCOREMEMBER ) {
			m_all_member = _wcsdup( FROM_STRING( a_row.ALL_USCOREMEMBER, CP_UTF8 ) );
		} else {
			m_all_member = nullptr;
		}

		if ( nullptr != a_row.DEFAULT_USCOREMEMBER ) {
			m_default_member = _wcsdup( FROM_STRING( a_row.DEFAULT_USCOREMEMBER, CP_UTF8 ) );
		} else {
			m_default_member = nullptr;
		}
		

		m_dimension_type = get_int( a_row.DIMENSION_USCORETYPE );
		m_hierarcy_cardinality = get_int( a_row.HIERARCHY_USCORECARDINALITY );
		m_structure = get_int( a_row.STRUCTURE );//2;//MD_STRUCTURE_UNBALANCED;
	}

	hierarchy_row( const hierarchy_row& other )
	{
		copy( other );
	}

	hierarchy_row& operator=( const hierarchy_row& other )
	{
		if ( this != &other )  {  copy( other ); }
		return *this;
	}

	~hierarchy_row()
	{
		delete[] m_catalog;
		delete[] m_cube;
		delete[] m_dimension_unique_name;
		delete[] m_hierarchy_name;
		delete[] m_hierarchy_unique_name;
		delete[] m_hierarchy_caption;
		if ( m_all_member ) { delete[] m_all_member; }
		if ( m_default_member) { delete[] m_default_member; }
		delete[] m_description; 
	}

	EMPTY_CONSTRUCTOR(hierarchy_row);

	static char* schema_name() { return "MDSCHEMA_HIERARCHIES"; }

	BEGIN_PROVIDER_COLUMN_MAP( hierarchy_row )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CATALOG_NAME", 1, MAX_BUF_SIZE, m_catalog )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "SCHEMA_NAME", 2, MAX_BUF_SIZE, m_schema )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CUBE_NAME", 3, MAX_BUF_SIZE, m_cube )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DIMENSION_UNIQUE_NAME", 4, MAX_BUF_SIZE, m_dimension_unique_name )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "HIERARCHY_NAME", 5, MAX_BUF_SIZE, m_hierarchy_name )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "HIERARCHY_UNIQUE_NAME", 6, MAX_BUF_SIZE, m_hierarchy_unique_name )
	PROVIDER_COLUMN_ENTRY_TYPE( "HIERARCHY_GUID", 7, VT_CLSID, m_hierarchy_guid )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR("HIERARCHY_CAPTION", 8, MAX_BUF_SIZE, m_hierarchy_caption)
	PROVIDER_COLUMN_ENTRY_TYPE( "DIMENSION_TYPE", 9, VT_I2, m_dimension_type )
	PROVIDER_COLUMN_ENTRY_TYPE( "HIERARCHY_CARDINALITY", 10, VT_UI4, m_hierarcy_cardinality )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DEFAULT_MEMBER", 11, MAX_BUF_SIZE, m_default_member )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "ALL_MEMBER", 12, MAX_BUF_SIZE, m_all_member )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DESCRIPTION", 13, MAX_BUF_SIZE, m_description )
	PROVIDER_COLUMN_ENTRY_TYPE( "STRUCTURE", 14, VT_I2, m_structure )
	END_PROVIDER_COLUMN_MAP()
};