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
					a row in the measure rowset
*/

#pragma once

#include "soapH.h"

class measure_row
{
public:
	wchar_t         m_catalog[256];
	wchar_t         m_schema[256];
	wchar_t         m_cube[256];
	wchar_t         m_measure_name[256];
	wchar_t         m_measure_unique_name[256];
	wchar_t         m_measure_caption[256];
	GUID            m_measure_guid;
	long            m_measure_agregator;
	unsigned short  m_data_type;
	unsigned short  m_numeric_precision;
	short           m_numeric_scale;
	wchar_t         m_measure_units[256];
	wchar_t         m_description[256];

	measure_row( row& a_row )
	{
		m_schema[0]=0;
		m_measure_units[0]=0;

		m_measure_agregator	= get_int( a_row.MEASURE_USCOREAGGREGATOR );
		m_data_type			= get_int( a_row.DATA_USCORETYPE );
		m_numeric_precision	= get_int( a_row.NUMERIC_USCOREPRECISION );
		m_numeric_scale		= get_int( a_row.NUMERIC_USCORESCALE );

		wcscpy_s( m_catalog, 256, FROM_STRING( a_row.CATALOG_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_cube, 256, FROM_STRING( a_row.CUBE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_measure_name, 256, FROM_STRING( a_row.MEASURE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_measure_unique_name, 256, FROM_STRING( a_row.MEASURE_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_measure_caption, 256, FROM_STRING( a_row.MEASURE_USCORECAPTION, CP_UTF8 ) );
		wcscpy_s( m_description, 256, FROM_STRING( a_row.DESCRIPTION, CP_UTF8 ) );
	}

	EMPTY_CONSTRUCTOR(measure_row);

	static char* schema_name() { return "MDSCHEMA_MEASURES"; }

	BEGIN_PROVIDER_COLUMN_MAP( measure_row )
	PROVIDER_COLUMN_ENTRY_WSTR( "CATALOG_NAME", 1, m_catalog )
	PROVIDER_COLUMN_ENTRY_WSTR( "SCHEMA_NAME", 2, m_schema )
	PROVIDER_COLUMN_ENTRY_WSTR( "CUBE_NAME", 3, m_cube )
	PROVIDER_COLUMN_ENTRY_WSTR( "MEASURE_NAME", 4, m_measure_name )
	PROVIDER_COLUMN_ENTRY_WSTR( "MEASURE_UNIQUE_NAME", 5, m_measure_unique_name )
	PROVIDER_COLUMN_ENTRY_WSTR( "MEASURE_CAPTION", 6, m_measure_caption )
	PROVIDER_COLUMN_ENTRY_TYPE( "MEASURE_GUID", 7, VT_CLSID, m_measure_guid )
	PROVIDER_COLUMN_ENTRY_TYPE( "MEASURE_AGGREGATOR", 8, VT_I4, m_measure_agregator )
	PROVIDER_COLUMN_ENTRY_TYPE( "DATA_TYPE", 9, VT_UI2, m_data_type )
	PROVIDER_COLUMN_ENTRY_TYPE( "NUMERIC_PRECISION", 10, VT_UI2, m_numeric_precision )
	PROVIDER_COLUMN_ENTRY_TYPE( "NUMERIC_SCALE", 11, VT_I2, m_numeric_precision )
	PROVIDER_COLUMN_ENTRY_WSTR( "MEASURE_UNITS", 12, m_measure_units )
	PROVIDER_COLUMN_ENTRY_WSTR( "DESCRIPTION", 13, m_description )
	END_PROVIDER_COLUMN_MAP()
};
