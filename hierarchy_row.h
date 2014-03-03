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
					a row in the hierarchy rowset
*/

#pragma once

#include "soapH.h"

class hierarchy_row
{
public:
	wchar_t         m_catalog[256];
	wchar_t         m_schema[256];
	wchar_t         m_cube[256];
	wchar_t         m_dimension_unique_name[256];
	wchar_t         m_hierarchy_name[256];
	wchar_t         m_hierarchy_unique_name[256];
	GUID            m_hierarchy_guid;
	wchar_t         m_hierarchy_caption[256];
	short           m_dimension_type;
	unsigned long   m_hierarcy_cardinality;
	wchar_t         m_default_member[256];
	wchar_t         m_all_member[256];
	wchar_t         m_description[256];
	wchar_t         m_annotations[256];
	short           m_structure;

	hierarchy_row( row& a_row )
	{
		m_schema[0]=0;
		m_annotations[0]=0;

		wcscpy_s( m_catalog, 256, FROM_STRING( a_row.CATALOG_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_cube, 256, FROM_STRING( a_row.CUBE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_dimension_unique_name, 256, FROM_STRING( a_row.DIMENSION_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_hierarchy_name, 256, FROM_STRING( a_row.HIERARCHY_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_hierarchy_unique_name, 256, FROM_STRING( a_row.HIERARCHY_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_hierarchy_caption, 256, FROM_STRING( a_row.HIERARCHY_USCORECAPTION, CP_UTF8 ) );
		wcscpy_s( m_default_member, 256, FROM_STRING( a_row.DEFAULT_USCOREMEMBER, CP_UTF8 ) );
		wcscpy_s( m_all_member, 256, FROM_STRING( a_row.ALL_USCOREMEMBER, CP_UTF8 ) );
		wcscpy_s( m_description, 256, FROM_STRING( a_row.DESCRIPTION, CP_UTF8 ) );

		m_dimension_type = get_int( a_row.DIMENSION_USCORETYPE );
		m_hierarcy_cardinality = get_int( a_row.HIERARCHY_USCORECARDINALITY );
		m_structure = 2;//MD_STRUCTURE_UNBALANCED;
	}

	EMPTY_CONSTRUCTOR(hierarchy_row);

	static char* schema_name() { return "MDSCHEMA_HIERARCHIES"; }

	BEGIN_PROVIDER_COLUMN_MAP( hierarchy_row )
	PROVIDER_COLUMN_ENTRY_WSTR( "CATALOG_NAME", 1, m_catalog )
	PROVIDER_COLUMN_ENTRY_WSTR( "SCHEMA_NAME", 2, m_schema )
	PROVIDER_COLUMN_ENTRY_WSTR( "CUBE_NAME", 3, m_cube )
	PROVIDER_COLUMN_ENTRY_WSTR( "DIMENSION_UNIQUE_NAME", 4, m_dimension_unique_name )
	PROVIDER_COLUMN_ENTRY_WSTR( "HIERARCHY_NAME", 5, m_hierarchy_name )
	PROVIDER_COLUMN_ENTRY_WSTR( "HIERARCHY_UNIQUE_NAME", 6, m_hierarchy_unique_name )
	PROVIDER_COLUMN_ENTRY_TYPE( "HIERARCHY_GUID", 7, VT_CLSID, m_hierarchy_guid )
	PROVIDER_COLUMN_ENTRY_WSTR( "HIERARCHY_CAPTION", 8, m_hierarchy_caption )
	PROVIDER_COLUMN_ENTRY_TYPE( "DIMENSION_TYPE", 9, VT_I2, m_dimension_type )
	PROVIDER_COLUMN_ENTRY_TYPE( "HIERARCHY_CARDINALITY", 10, VT_UI4, m_hierarcy_cardinality )
	PROVIDER_COLUMN_ENTRY_WSTR( "DEFAULT_MEMBER", 11, m_default_member )
	PROVIDER_COLUMN_ENTRY_WSTR( "ALL_MEMBER", 12, m_all_member )
	PROVIDER_COLUMN_ENTRY_WSTR( "DESCRIPTION", 13, m_description )
	PROVIDER_COLUMN_ENTRY_WSTR( "ANNOTATIONS", 14, m_annotations )
	PROVIDER_COLUMN_ENTRY_TYPE( "STRUCTURE", 15, VT_I2, m_structure )
	END_PROVIDER_COLUMN_MAP()
};