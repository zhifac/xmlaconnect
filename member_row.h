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
					a row in the member rowset
*/

#pragma once

#include "soapH.h"

class member_row
{
private:
	static const size_t MAX_BUF_SIZE = 10 * 1024 * sizeof( wchar_t );
private:
	void copy( const member_row& other )
	{
		m_catalog = _wcsdup( other.m_catalog );
		m_schema = nullptr;
		m_cube = _wcsdup( other.m_cube );
		m_dimension_unique_name = _wcsdup( other.m_dimension_unique_name );
		m_hierarchy_unique_name = _wcsdup( other.m_hierarchy_unique_name );
		m_level_unique_name = _wcsdup( other.m_level_unique_name );
		m_member_name = _wcsdup( other.m_member_name );
		m_member_unique_name = _wcsdup( other.m_member_unique_name );
		m_member_caption = _wcsdup( other.m_member_caption );
		m_parent_unique_name = _wcsdup( other.m_parent_unique_name );
		m_description = _wcsdup( other.m_description );
		
		m_level_number = other.m_level_number;
		m_member_ordinal = other.m_member_ordinal;
		m_member_type = other.m_member_type;
		m_children_cardinality = other.m_children_cardinality;
		m_parent_level = other.m_parent_level;
		m_parent_count = other.m_parent_count;
	}
public:
	wchar_t*         m_catalog;
	wchar_t*         m_schema;
	wchar_t*         m_cube;
	wchar_t*         m_dimension_unique_name;
	wchar_t*         m_hierarchy_unique_name;
	wchar_t*         m_level_unique_name;
	unsigned long   m_level_number;
	unsigned long   m_member_ordinal;
	wchar_t*         m_member_name;
	wchar_t*         m_member_unique_name;
	long            m_member_type;
	GUID            m_member_guid;
	wchar_t*         m_member_caption;
	unsigned long   m_children_cardinality;
	unsigned long   m_parent_level;
	wchar_t*         m_parent_unique_name;
	unsigned long   m_parent_count;
	wchar_t*         m_description;


	member_row( row& a_row )
	{
		m_catalog = _wcsdup( FROM_STRING( a_row.CATALOG_USCORENAME, CP_UTF8 ) ) ;
		m_schema = nullptr;
		m_cube = _wcsdup(FROM_STRING( a_row.CUBE_USCORENAME, CP_UTF8 ));
		m_dimension_unique_name = _wcsdup( FROM_STRING( a_row.DIMENSION_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		m_hierarchy_unique_name = _wcsdup( FROM_STRING( a_row.HIERARCHY_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		m_level_unique_name = _wcsdup( FROM_STRING( a_row.LEVEL_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		m_member_name = _wcsdup( FROM_STRING( a_row.MEMBER_USCORENAME, CP_UTF8 ) );
		m_member_unique_name = _wcsdup( FROM_STRING( a_row.MEMBER_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		m_member_caption = _wcsdup( FROM_STRING( a_row.MEMBER_USCORECAPTION, CP_UTF8 ) );
		m_parent_unique_name = _wcsdup( FROM_STRING( a_row.PARENT_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		m_description = _wcsdup( FROM_STRING( a_row.DESCRIPTION, CP_UTF8 ) );
		
		m_level_number = get_int(a_row.LEVEL_USCORENUMBER);
		m_member_ordinal = get_int( a_row.MEMBER_USCOREORDINAL );
		m_member_type = get_int( a_row.MEMBER_USCORETYPE );
		m_children_cardinality = get_int(a_row.CHILDREN_USCORECARDINALITY);
		m_parent_level = get_int(a_row.PARENT_USCORELEVEL);
		m_parent_count = get_int(a_row.PARENT_USCORECOUNT);
		
	}

	member_row( const member_row& other )
	{
		copy( other );
	}

	member_row& operator=( const member_row& other )
	{
		if ( this != &other )  {  copy( other ); }
		return *this;
	}

	~member_row()
	{
		delete[] m_dimension_unique_name;
		delete[] m_hierarchy_unique_name;
		delete[] m_level_unique_name;
		delete[] m_member_unique_name;
		delete[] m_catalog;
		delete[] m_cube;
		delete[] m_member_name;
		delete[] m_member_caption;
		delete[] m_parent_unique_name;
		delete[] m_description;
	}

	
	EMPTY_CONSTRUCTOR(member_row);

	static char* schema_name() { return "MDSCHEMA_MEMBERS"; }

	BEGIN_PROVIDER_COLUMN_MAP( member_row )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CATALOG_NAME", 1, MAX_BUF_SIZE, m_catalog )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "SCHEMA_NAME", 2, MAX_BUF_SIZE, m_schema )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CUBE_NAME", 3, MAX_BUF_SIZE, m_cube )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DIMENSION_UNIQUE_NAME", 4, MAX_BUF_SIZE, m_dimension_unique_name )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "HIERARCHY_UNIQUE_NAME", 5, MAX_BUF_SIZE, m_hierarchy_unique_name )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "LEVEL_UNIQUE_NAME", 6, MAX_BUF_SIZE, m_level_unique_name )
	PROVIDER_COLUMN_ENTRY_TYPE( "LEVEL_NUMBER", 7, VT_UI4, m_level_number )
	PROVIDER_COLUMN_ENTRY_TYPE( "MEMBER_ORDINAL", 8, VT_UI4, m_member_ordinal )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "MEMBER_NAME", 9, MAX_BUF_SIZE, m_member_name )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "MEMBER_UNIQUE_NAME", 10, MAX_BUF_SIZE, m_member_unique_name )
	PROVIDER_COLUMN_ENTRY_TYPE( "MEMBER_TYPE", 11, VT_I4, m_member_type )
	PROVIDER_COLUMN_ENTRY_TYPE( "MEMBER_GUID", 12, VT_CLSID, m_member_guid )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "MEMBER_CAPTION", 13, MAX_BUF_SIZE, m_member_caption )
	PROVIDER_COLUMN_ENTRY_TYPE( "CHILDREN_CARDINALITY", 14, VT_UI4, m_children_cardinality )
	PROVIDER_COLUMN_ENTRY_TYPE( "PARENT_LEVEL", 15, VT_UI4, m_parent_level )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "PARENT_UNIQUE_NAME", 16, MAX_BUF_SIZE, m_parent_unique_name )
	PROVIDER_COLUMN_ENTRY_TYPE( "PARENT_COUNT", 17, VT_UI4, m_parent_count )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DESCRIPTION", 18, MAX_BUF_SIZE, m_description )
	END_PROVIDER_COLUMN_MAP()
};
