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
					a row in the dimension rowset
*/

#pragma once

#include "soapH.h"

class dimension_row
{
public:
	wchar_t         m_catalog[256];
	wchar_t         m_schema[256];
	wchar_t         m_cube[256];
	wchar_t         m_dimension_name[256];
	wchar_t         m_dimension_unique_name[256];
	GUID            m_dimension_guid;
	wchar_t         m_dimension_caption[256];
	unsigned long   m_ordinal;
	short           m_type;
	unsigned long   m_cardinality;
	wchar_t         m_default_hierarcy[256];
	wchar_t         m_description[256];
	wchar_t         m_annotations[256];

	EMPTY_CONSTRUCTOR(dimension_row);

	dimension_row( row& a_row ) 
	{
		m_schema[0]=0;
		m_annotations[0]=0;

		wcscpy_s( m_catalog, 256, FROM_STRING( a_row.CATALOG_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_cube, 256, FROM_STRING( a_row.CUBE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_dimension_name, 256, FROM_STRING( a_row.DIMENSION_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_dimension_unique_name, 256, FROM_STRING( a_row.DIMENSION_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_dimension_caption, 256, FROM_STRING( a_row.DIMENSION_USCORECAPTION, CP_UTF8 ) );
		wcscpy_s( m_default_hierarcy, 256, FROM_STRING( a_row.DEFAULT_USCOREHIERARCHY, CP_UTF8 ) );
		wcscpy_s( m_description, 256, FROM_STRING( a_row.DESCRIPTION, CP_UTF8 ) );

		m_ordinal = atoi(a_row.DIMENSION_USCOREORDINAL);
		m_type = get_int( a_row.DIMENSION_USCORETYPE );
		m_cardinality = get_int( a_row.DIMENSION_USCORECARDINALITY );
	}


	static char* schema_name() { return "MDSCHEMA_DIMENSIONS"; }

	BEGIN_PROVIDER_COLUMN_MAP( dimension_row )
	PROVIDER_COLUMN_ENTRY_WSTR( "CATALOG_NAME", 1, m_catalog )
	PROVIDER_COLUMN_ENTRY_WSTR( "SCHEMA_NAME", 2, m_schema )
	PROVIDER_COLUMN_ENTRY_WSTR( "CUBE_NAME", 3, m_cube )
	PROVIDER_COLUMN_ENTRY_WSTR( "DIMENSION_NAME", 4, m_dimension_name )
	PROVIDER_COLUMN_ENTRY_WSTR( "DIMENSION_UNIQUE_NAME", 5, m_dimension_unique_name )
	PROVIDER_COLUMN_ENTRY_TYPE( "DIMENSION_GUID", 6, VT_CLSID, m_dimension_guid )
	PROVIDER_COLUMN_ENTRY_WSTR( "DIMENSION_CAPTION", 7, m_dimension_caption )
	PROVIDER_COLUMN_ENTRY_TYPE( "DIMENSION_ORDINAL", 8, VT_UI4, m_ordinal )
	PROVIDER_COLUMN_ENTRY_TYPE( "DIMENSION_TYPE", 9, VT_I2, m_type )
	PROVIDER_COLUMN_ENTRY_TYPE( "DIMENSION_CARDINALITY", 10, VT_UI4, m_cardinality )
	PROVIDER_COLUMN_ENTRY_WSTR( "DEFAULT_HIERARCHY", 11, m_default_hierarcy )
	PROVIDER_COLUMN_ENTRY_WSTR( "DESCRIPTION", 12, m_description )
	PROVIDER_COLUMN_ENTRY_WSTR( "ANNOTATIONS", 13, m_annotations )
	END_PROVIDER_COLUMN_MAP()
};
