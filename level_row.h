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
					a row in the level rowset
*/

#pragma once

#include "soapH.h"

class level_row
{
public:
	wchar_t         m_catalog[256];
	wchar_t         m_schema[256];
	wchar_t         m_cube[256];
	wchar_t         m_dimension_unique_name[256];
	wchar_t         m_hierarcy_unique_name[256];
	wchar_t         m_level_name[256];
	wchar_t         m_level_unique_name[256];
	GUID            m_level_guid;
	wchar_t         m_level_caption[256];
	unsigned long   m_level_number;
	unsigned long   m_level_cardinality;
	long            m_level_type;
	wchar_t         m_description[256];

	level_row( row& a_row )
	{
		m_schema[0]=0;

		wcscpy_s( m_catalog, 256, FROM_STRING( a_row.CATALOG_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_cube, 256, FROM_STRING( a_row.CUBE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_dimension_unique_name, 256, FROM_STRING( a_row.DIMENSION_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_hierarcy_unique_name, 256, FROM_STRING( a_row.HIERARCHY_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_level_name, 256, FROM_STRING( a_row.LEVEL_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_level_unique_name, 256, FROM_STRING( a_row.LEVEL_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_level_caption, 256, FROM_STRING( a_row.LEVEL_USCORECAPTION, CP_UTF8 ) );
		wcscpy_s( m_description, 256, FROM_STRING( a_row.DESCRIPTION, CP_UTF8 ) );

		m_level_number = get_int( a_row.LEVEL_USCORENUMBER );
		m_level_cardinality = get_int( a_row.LEVEL_USCORECARDINALITY );
		m_level_type = get_int( a_row.LEVEL_USCORETYPE );
	}

	EMPTY_CONSTRUCTOR(level_row);

	static char* schema_name() { return "MDSCHEMA_LEVELS"; }

	BEGIN_PROVIDER_COLUMN_MAP( level_row )
	PROVIDER_COLUMN_ENTRY_WSTR( "CATALOG_NAME", 1, m_catalog )
	PROVIDER_COLUMN_ENTRY_WSTR( "SCHEMA_NAME", 2, m_schema )
	PROVIDER_COLUMN_ENTRY_WSTR( "CUBE_NAME", 3, m_cube )
	PROVIDER_COLUMN_ENTRY_WSTR( "DIMENSION_UNIQUE_NAME", 4, m_dimension_unique_name )
	PROVIDER_COLUMN_ENTRY_WSTR( "HIERARCHY_UNIQUE_NAME", 5, m_hierarcy_unique_name )
	PROVIDER_COLUMN_ENTRY_WSTR( "LEVEL_NAME", 6, m_level_name )
	PROVIDER_COLUMN_ENTRY_WSTR( "LEVEL_UNIQUE_NAME", 7, m_level_unique_name )
	PROVIDER_COLUMN_ENTRY_TYPE( "LEVEL_GUID", 8, VT_CLSID, m_level_guid )
	PROVIDER_COLUMN_ENTRY_WSTR( "LEVEL_CAPTION", 9, m_level_caption )
	PROVIDER_COLUMN_ENTRY_TYPE( "LEVEL_NUMBER", 10, VT_UI4, m_level_number )
	PROVIDER_COLUMN_ENTRY_TYPE( "LEVEL_CARDINALITY", 11, VT_UI4, m_level_cardinality )
	PROVIDER_COLUMN_ENTRY_TYPE( "LEVEL_TYPE", 12, VT_I4, m_level_type )
	PROVIDER_COLUMN_ENTRY_WSTR( "DESCRIPTION", 13, m_description )
	END_PROVIDER_COLUMN_MAP()
};