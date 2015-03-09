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
					a row in the function rowset
*/

#pragma once

#include "soapH.h"

class function_row
{
private:
	static const size_t MAX_BUF_SIZE = 1024 * sizeof( wchar_t );
private:
	void copy( const function_row& other )
	{
		m_function_name = _wcsdup( other.m_function_name );
		m_description = _wcsdup( other.m_description );
		m_parameter_list = _wcsdup( other.m_parameter_list );
		m_return_type = other.m_return_type;
		m_origin = other.m_origin;
		m_interface_name = _wcsdup( other.m_interface_name );

	}
public:
	wchar_t*         m_function_name;
	wchar_t*         m_description;
	wchar_t*         m_parameter_list;
	long			 m_return_type;
	long             m_origin;
	wchar_t*         m_interface_name;
		
	function_row( row& a_row )
	{
		m_function_name = _wcsdup( FROM_STRING( a_row.FUNCTION_USCORENAME, CP_UTF8 ) );
		m_description = _wcsdup( FROM_STRING( a_row.DESCRIPTION, CP_UTF8 ) );
		m_parameter_list = _wcsdup( FROM_STRING( a_row.PARAMETER_USCORELIST, CP_UTF8 ) );
		m_return_type = get_int( a_row.RETURN_USCORETYPE );
		m_origin = get_int( a_row.ORIGIN );
		m_interface_name = _wcsdup( FROM_STRING( a_row.INTERFACE_USCORENAME, CP_UTF8 ) );
	}

	function_row( const function_row& other )
	{
		copy( other );
	}

	function_row& operator=( const function_row& other )
	{
		if ( this != &other )  {  copy( other ); }
		return *this;
	}

	~function_row()
	{
		delete[] m_function_name;
		delete[] m_description;
		delete[] m_parameter_list;
		delete[] m_interface_name;
	}

	EMPTY_CONSTRUCTOR(function_row);
	static char* schema_name() { return "MDSCHEMA_FUNCTIONS"; }

	BEGIN_PROVIDER_COLUMN_MAP( function_row )
		PROVIDER_COLUMN_ENTRY_VAR_WSTR( "FUNCTION_NAME", 1, MAX_BUF_SIZE, m_function_name)
		PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DESCRIPTION",  2, MAX_BUF_SIZE, m_description)
		PROVIDER_COLUMN_ENTRY_VAR_WSTR( "PARAMETER_LIST", 3, MAX_BUF_SIZE,  m_parameter_list)
		PROVIDER_COLUMN_ENTRY_TYPE(	"RETURN_TYPE",	4, VT_I4,  m_return_type)
		PROVIDER_COLUMN_ENTRY_TYPE( "ORIGIN",  5, VT_I4, m_origin)
		PROVIDER_COLUMN_ENTRY_VAR_WSTR( "INTERFACE_NAME", 6, MAX_BUF_SIZE, m_interface_name)
	END_PROVIDER_COLUMN_MAP()
};