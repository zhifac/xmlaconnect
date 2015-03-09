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
					a row in the set rowset
*/

#pragma once

#include "soapH.h"

class set_row
{
private:
	static const size_t MAX_BUF_SIZE = 10 * 1024 * sizeof( wchar_t );
private:
	void copy( const set_row& other )
	{
		m_schema = nullptr;
		m_description = nullptr;
		m_scope = 2;
		m_eval_context = 1;
		m_catalog = _wcsdup( other.m_catalog );
		m_cube = _wcsdup( other.m_cube );
		m_set_name = _wcsdup( other.m_set_name );
		m_expression = _wcsdup( other.m_expression );
		m_dimension = _wcsdup( other.m_dimension );
		m_set_caption = _wcsdup( other.m_set_caption );
		m_display_folder = _wcsdup( other.m_display_folder );
		m_description = _wcsdup( other.m_description );
	}
public:
	wchar_t*         m_catalog;
	wchar_t*         m_schema;
	wchar_t*         m_cube;
	wchar_t*         m_set_name;
	long             m_scope;
	wchar_t*         m_expression;
	wchar_t*         m_description;
	wchar_t*		 m_dimension;
	wchar_t*         m_set_caption;
	wchar_t*         m_display_folder;
	long             m_eval_context;

	set_row( row& a_row )
	{

		m_schema = nullptr;
		m_description = nullptr;
		m_scope = 2;
		m_eval_context = 1;

		m_catalog = _wcsdup( FROM_STRING( a_row.CATALOG_USCORENAME, CP_UTF8 ) );
		m_cube = _wcsdup( FROM_STRING( a_row.CUBE_USCORENAME, CP_UTF8 ) );
		m_set_name = _wcsdup( FROM_STRING( a_row.SET_USCORENAME, CP_UTF8 ) );
		m_expression = _wcsdup( FROM_STRING( a_row.EXPRESSION, CP_UTF8 ));
		m_dimension = _wcsdup( FROM_STRING( a_row.DIMENSIONS, CP_UTF8 ));
		m_set_caption = _wcsdup( FROM_STRING( a_row.SET_USCORECAPTION, CP_UTF8 ) );
		m_display_folder = _wcsdup( FROM_STRING( a_row.SET_USCOREDISPLAY_USCOREFOLDER, CP_UTF8 ) );
	}

	set_row( const set_row& other )
	{
		copy( other );
	}

	set_row& operator=( const set_row& other )
	{
		if ( this != &other )  {  copy( other ); }
		return *this;
	}

	~set_row()
	{
		delete[] m_catalog;
		delete[] m_cube;
		delete[] m_set_name;
		delete[] m_expression;
		delete[] m_dimension;
		delete[] m_set_caption;
		delete[] m_description;
	}

	EMPTY_CONSTRUCTOR(set_row);
	static char* schema_name() { return "MDSCHEMA_SETS"; }

	BEGIN_PROVIDER_COLUMN_MAP( set_row )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CATALOG_NAME", 1, MAX_BUF_SIZE, m_catalog )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "SCHEMA_NAME", 2, MAX_BUF_SIZE, m_schema )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CUBE_NAME", 3, MAX_BUF_SIZE, m_cube )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "SET_NAME", 4, MAX_BUF_SIZE, m_set_name )
	PROVIDER_COLUMN_ENTRY_TYPE( "SCOPE", 5, VT_I4, m_scope )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DESCRIPTION", 6, MAX_BUF_SIZE, m_description )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "EXPRESSION", 7, MAX_BUF_SIZE, m_expression )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DIMENSIONS", 8, MAX_BUF_SIZE, m_dimension )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "SET_CAPTION", 9, MAX_BUF_SIZE, m_set_caption )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "SET_DISPLAY_FOLDER", 10, MAX_BUF_SIZE, m_display_folder )
	PROVIDER_COLUMN_ENTRY_TYPE( "SET_EVALUATION_CONTEXT", 11, VT_I4, m_eval_context )
	END_PROVIDER_COLUMN_MAP()
};
