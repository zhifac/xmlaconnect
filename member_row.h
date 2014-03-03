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
					a row in the member rowset
*/

#pragma once

#include "soapH.h"

class member_row
{
public:
	wchar_t         m_catalog[256];
	wchar_t         m_schema[256];
	wchar_t         m_cube[256];
	wchar_t         m_dimension_unique_name[256];
	wchar_t         m_hierarchy_unique_name[256];
	wchar_t         m_level_unique_name[256];
	unsigned long   m_level_number;
	unsigned long   m_member_ordinal;
	wchar_t         m_member_name[256];
	wchar_t         m_member_unique_name[256];
	long            m_member_type;
	GUID            m_member_guid;
	wchar_t         m_member_caption[256];
	unsigned long   m_children_cardinality;
	unsigned long   m_parent_level;
	wchar_t         m_parent_unique_name[256];
	unsigned long   m_parent_count;
	wchar_t         m_description[256];

	member_row( row& a_row )
	{
		wcscpy_s( m_catalog, 256, FROM_STRING( a_row.CATALOG_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_schema, 256, L"" );
		wcscpy_s( m_cube, 256, FROM_STRING( a_row.CUBE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_dimension_unique_name, 256, FROM_STRING( a_row.DIMENSION_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_hierarchy_unique_name, 256, FROM_STRING( a_row.HIERARCHY_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_level_unique_name, 256, FROM_STRING( a_row.LEVEL_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_member_name, 256, FROM_STRING( a_row.MEMBER_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_member_unique_name, 256, FROM_STRING( a_row.MEMBER_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_member_caption, 256, FROM_STRING( a_row.MEMBER_USCORECAPTION, CP_UTF8 ) );
		wcscpy_s( m_parent_unique_name, 256, FROM_STRING( a_row.PARENT_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_description, 256, FROM_STRING( a_row.DESCRIPTION, CP_UTF8 ) );
		
		m_level_number = get_int(a_row.LEVEL_USCORENUMBER);
		m_member_ordinal = get_int( a_row.MEMBER_USCOREORDINAL );
		m_member_type = get_int( a_row.MEMBER_USCORETYPE );
		m_children_cardinality = get_int(a_row.CHILDREN_USCORECARDINALITY);
		m_parent_level = get_int(a_row.PARENT_USCORELEVEL);
		m_parent_count = get_int(a_row.PARENT_USCORECOUNT);
	}
	
	EMPTY_CONSTRUCTOR(member_row);

	static char* schema_name() { return "MDSCHEMA_MEMBERS"; }

	BEGIN_PROVIDER_COLUMN_MAP( member_row )
	PROVIDER_COLUMN_ENTRY_WSTR( "CATALOG_NAME", 1, m_catalog )
	PROVIDER_COLUMN_ENTRY_WSTR( "SCHEMA_NAME", 2, m_schema )
	PROVIDER_COLUMN_ENTRY_WSTR( "CUBE_NAME", 3, m_cube )
	PROVIDER_COLUMN_ENTRY_WSTR( "DIMENSION_UNIQUE_NAME", 4, m_dimension_unique_name )
	PROVIDER_COLUMN_ENTRY_WSTR( "HIERARCHY_UNIQUE_NAME", 5, m_hierarchy_unique_name )
	PROVIDER_COLUMN_ENTRY_WSTR( "LEVEL_UNIQUE_NAME", 6, m_level_unique_name )
	PROVIDER_COLUMN_ENTRY_TYPE( "LEVEL_NUMBER", 7, VT_UI4, m_level_number )
	PROVIDER_COLUMN_ENTRY_TYPE( "MEMBER_ORDINAL", 8, VT_UI4, m_member_ordinal )
	PROVIDER_COLUMN_ENTRY_WSTR( "MEMBER_NAME", 9, m_member_name )
	PROVIDER_COLUMN_ENTRY_WSTR( "MEMBER_UNIQUE_NAME", 10, m_member_unique_name )
	PROVIDER_COLUMN_ENTRY_TYPE( "MEMBER_TYPE", 11, VT_I4, m_member_type )
	PROVIDER_COLUMN_ENTRY_TYPE( "LEVEL_GUID", 12, VT_CLSID, m_member_guid )
	PROVIDER_COLUMN_ENTRY_WSTR( "MEMBER_CAPTION", 13, m_member_caption )
	PROVIDER_COLUMN_ENTRY_TYPE( "CHILDREN_CARDINALITY", 14, VT_UI4, m_children_cardinality )
	PROVIDER_COLUMN_ENTRY_TYPE( "PARENT_LEVEL", 15, VT_UI4, m_parent_level )
	PROVIDER_COLUMN_ENTRY_WSTR( "PARENT_UNIQUE_NAME", 16, m_parent_unique_name )
	PROVIDER_COLUMN_ENTRY_TYPE( "PARENT_COUNT", 17, VT_UI4, m_parent_count )
	PROVIDER_COLUMN_ENTRY_WSTR( "DESCRIPTION", 18, m_description )
	END_PROVIDER_COLUMN_MAP()
};
