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
					a row in the propery rowset
*/

#pragma once

#include "soapH.h"

class property_row
{
public:
	short           m_type;
	wchar_t         m_catalog[256];
	wchar_t         m_schema[256];
	wchar_t         m_cube[256];
	wchar_t         m_dimension_unique_name[256];
	wchar_t         m_hierarchy_unique_name[256];
	wchar_t         m_level_unique_name[256];
	wchar_t         m_member_unique_name[256];
	wchar_t         m_property_name[256];
	wchar_t         m_property_caption[256];
	unsigned short  m_data_type;
	unsigned long   m_ch_max_length;
	unsigned long   m_ch_octet_length;
	unsigned short  m_numeric_precision;
	short           m_numeric_scale;
	wchar_t         m_description[256];

	property_row( row& a_row )
	{
		m_schema[0]=0;
		m_member_unique_name[0]=0;

		wcscpy_s( m_catalog, 256, FROM_STRING( a_row.CATALOG_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_cube, 256, FROM_STRING( a_row.CUBE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_dimension_unique_name, 256, FROM_STRING( a_row.DIMENSION_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_hierarchy_unique_name, 256, FROM_STRING( a_row.HIERARCHY_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_level_unique_name, 256, FROM_STRING( a_row.LEVEL_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		m_type = get_int(a_row.PROPERTY_USCORETYPE);
		wcscpy_s( m_property_name, 256, FROM_STRING( a_row.PROPERTY_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_property_caption, 256, FROM_STRING( a_row.PROPERTY_USCORECAPTION, CP_UTF8 ) );
		wcscpy_s( m_description, 256, FROM_STRING( a_row.DESCRIPTION, CP_UTF8 ) );

		m_data_type = get_int(a_row.DATA_USCORETYPE);
		m_ch_max_length		= 0;
		m_ch_octet_length	= 0;
		m_numeric_precision	= 0;
		m_numeric_scale		= 0;		
	}

	EMPTY_CONSTRUCTOR(property_row);

	static char* schema_name() { return "MDSCHEMA_PROPERTIES"; }

	BEGIN_PROVIDER_COLUMN_MAP( property_row )
	PROVIDER_COLUMN_ENTRY_WSTR( "CATALOG_NAME", 1, m_catalog )
	PROVIDER_COLUMN_ENTRY_WSTR( "SCHEMA_NAME", 2, m_schema )
	PROVIDER_COLUMN_ENTRY_WSTR( "CUBE_NAME", 3, m_cube )
	PROVIDER_COLUMN_ENTRY_WSTR( "DIMENSION_UNIQUE_NAME", 4, m_dimension_unique_name )
	PROVIDER_COLUMN_ENTRY_WSTR( "HIERARCHY_UNIQUE_NAME", 5, m_hierarchy_unique_name )
	PROVIDER_COLUMN_ENTRY_WSTR( "LEVEL_UNIQUE_NAME", 6, m_level_unique_name )
	PROVIDER_COLUMN_ENTRY_WSTR( "MEMBER_UNIQUE_NAME", 7, m_member_unique_name )
	PROVIDER_COLUMN_ENTRY_TYPE( "PROPERTY_TYPE", 8, VT_I2, m_type )
	PROVIDER_COLUMN_ENTRY_WSTR( "PROPERTY_NAME", 9, m_property_name )
	PROVIDER_COLUMN_ENTRY_WSTR( "PROPERTY_CAPTION", 10, m_property_caption )
	PROVIDER_COLUMN_ENTRY_TYPE( "DATA_TYPE", 11, VT_UI2, m_data_type )
	PROVIDER_COLUMN_ENTRY_TYPE( "CHARACTER_MAXIMUM_LENGTH", 12, DBTYPE_UI4, m_ch_max_length )
	PROVIDER_COLUMN_ENTRY_TYPE( "CHARACTER_OCTET_LENGTH", 13, DBTYPE_UI4, m_ch_octet_length )
	PROVIDER_COLUMN_ENTRY_TYPE( "NUMERIC_PRECISION", 14, DBTYPE_UI2, m_numeric_precision )
	PROVIDER_COLUMN_ENTRY_TYPE( "NUMERIC_SCALE", 15, DBTYPE_I2, m_numeric_scale )
	PROVIDER_COLUMN_ENTRY_WSTR( "DESCRIPTION", 16, m_description )
	END_PROVIDER_COLUMN_MAP()
};
