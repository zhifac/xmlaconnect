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
					a row in the measure rowset
*/

#pragma once

#include "soapH.h"

class measure_row
{
private:
	static const size_t MAX_BUF_SIZE = 1024 * sizeof( wchar_t );
private:
	void copy ( const measure_row& other )
	{
		m_schema = nullptr;
		m_measure_units = nullptr;

		m_expression = nullptr;
		m_measure_is_visible = true;
		m_level_list = nullptr;
		m_sql_col = nullptr;
		m_unqualified_cap = nullptr;
		m_group_name = nullptr;
		m_display_folder = nullptr;
//		m_format_str = nullptr;
		
		m_measure_agregator	= other.m_measure_agregator;
		m_data_type			= other.m_data_type;
		m_numeric_precision	= other.m_numeric_precision;
		m_numeric_scale		= other.m_numeric_scale;

		m_catalog = _wcsdup( other.m_catalog );
		m_cube = _wcsdup( other.m_cube );
		m_measure_name = _wcsdup( other.m_measure_name );
		m_measure_unique_name = _wcsdup( other.m_measure_unique_name );
		m_measure_caption = _wcsdup( other.m_measure_caption );
		m_description = _wcsdup( other.m_description );
	}
public:
	wchar_t*         m_catalog;
	wchar_t*         m_schema;
	wchar_t*         m_cube;
	wchar_t*         m_measure_name;
	wchar_t*         m_measure_unique_name;
	wchar_t*         m_measure_caption;
	GUID             m_measure_guid;
	long             m_measure_agregator;
	unsigned short   m_data_type;
	unsigned short   m_numeric_precision;
	short            m_numeric_scale;
	wchar_t*         m_measure_units;
	wchar_t*         m_description;
	wchar_t*		 m_expression;
	bool			 m_measure_is_visible;
	wchar_t*		 m_level_list;
	wchar_t*		 m_sql_col;
	wchar_t*		 m_unqualified_cap;
	wchar_t*		 m_group_name;
	wchar_t*		 m_display_folder;
//	wchar_t*		 m_format_str;

	measure_row( row& a_row )
	{
		m_schema = nullptr;
		m_measure_units = nullptr;

		m_expression = nullptr;
		m_measure_is_visible = true;
		m_level_list = nullptr;
		m_sql_col = nullptr;
		m_unqualified_cap = nullptr;
		m_group_name = nullptr;
		m_display_folder = nullptr;
//		m_format_str = nullptr;

		m_measure_agregator	= get_int( a_row.MEASURE_USCOREAGGREGATOR );
		m_data_type			= get_int( a_row.DATA_USCORETYPE );
		m_numeric_precision	= get_int( a_row.NUMERIC_USCOREPRECISION );
		m_numeric_scale		= get_int( a_row.NUMERIC_USCORESCALE );

		m_catalog = _wcsdup( FROM_STRING( a_row.CATALOG_USCORENAME, CP_UTF8 ) );
		m_cube = _wcsdup( FROM_STRING( a_row.CUBE_USCORENAME, CP_UTF8 ) );
		m_measure_name = _wcsdup( FROM_STRING( a_row.MEASURE_USCORENAME, CP_UTF8 ) );
		m_measure_unique_name = _wcsdup( FROM_STRING( a_row.MEASURE_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		m_measure_caption = _wcsdup( FROM_STRING( a_row.MEASURE_USCORECAPTION, CP_UTF8 ) );
		m_description = _wcsdup( FROM_STRING( a_row.DESCRIPTION, CP_UTF8 ) );
	}

	measure_row( const measure_row& other )
	{
		copy(other);
	}

	measure_row& operator=(const measure_row& other )
	{
		if ( this != &other )  {  copy( other ); }

		return *this;
	}
	
	~measure_row()
	{
		delete[] m_catalog;
		delete[] m_cube;
		delete[] m_measure_name;
		delete[] m_measure_unique_name;
		delete[] m_measure_caption;
		delete[] m_description;
	}


	EMPTY_CONSTRUCTOR(measure_row);

	static char* schema_name() { return "MDSCHEMA_MEASURES"; }

	BEGIN_PROVIDER_COLUMN_MAP( measure_row )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CATALOG_NAME", 1, MAX_BUF_SIZE, m_catalog )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "SCHEMA_NAME", 2, MAX_BUF_SIZE, m_schema )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CUBE_NAME", 3, MAX_BUF_SIZE, m_cube )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "MEASURE_NAME", 4, MAX_BUF_SIZE, m_measure_name )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "MEASURE_UNIQUE_NAME", 5, MAX_BUF_SIZE, m_measure_unique_name )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "MEASURE_CAPTION", 6, MAX_BUF_SIZE, m_measure_caption )
	PROVIDER_COLUMN_ENTRY_TYPE( "MEASURE_GUID", 7, VT_CLSID, m_measure_guid )
	PROVIDER_COLUMN_ENTRY_TYPE( "MEASURE_AGGREGATOR", 8, VT_I4, m_measure_agregator )
	PROVIDER_COLUMN_ENTRY_TYPE( "DATA_TYPE", 9, VT_UI2, m_data_type )
	PROVIDER_COLUMN_ENTRY_TYPE( "NUMERIC_PRECISION", 10, VT_UI2, m_numeric_precision )
	PROVIDER_COLUMN_ENTRY_TYPE( "NUMERIC_SCALE", 11, VT_I2, m_numeric_scale )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "MEASURE_UNITS", 12, MAX_BUF_SIZE, m_measure_units )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DESCRIPTION", 13, MAX_BUF_SIZE, m_description )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "EXPRESSION", 14, MAX_BUF_SIZE, m_expression )
	PROVIDER_COLUMN_ENTRY_TYPE( "MEASURE_IS_VISIBLE", 15, VT_BOOL, m_measure_is_visible )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "LEVELS_LIST", 16, MAX_BUF_SIZE, m_level_list )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "MEASURE_NAME_SQL_COLUMN_NAME", 17, MAX_BUF_SIZE, m_sql_col )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "MEASURE_UNQUALIFIED_CAPTION", 18, MAX_BUF_SIZE, m_unqualified_cap )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "MEASUREGROUP_NAME", 19, MAX_BUF_SIZE, m_group_name )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "MEASURE_DISPLAY_FOLDER", 20, MAX_BUF_SIZE, m_display_folder )
//	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DEFAULT_FORMAT_STRING", 21, MAX_BUF_SIZE, m_format_str )
	
	END_PROVIDER_COLUMN_MAP()
};
