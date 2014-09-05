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
					a row in the function rowset
*/

#pragma once

#include "soapH.h"

class function_row
{
public:
	wchar_t         m_function_name[256];
	wchar_t         m_description[256];
	wchar_t         m_parameter_list[256];
	long			m_return_type;
	long            m_origin;
	wchar_t         m_interface_name[256];

	EMPTY_CONSTRUCTOR(function_row);

	function_row( row& a_row )
	{
		wcscpy_s( m_function_name, 256, FROM_STRING( a_row.FUNCTION_USCORENAME, CP_UTF8 ) );
		wcscpy_s( m_description, 256, FROM_STRING( a_row.DESCRIPTION, CP_UTF8 ) );
		wcscpy_s( m_parameter_list, 256, FROM_STRING( a_row.PARAMETER_USCORELIST, CP_UTF8 ) );
		m_return_type = get_int( a_row.RETURN_USCORETYPE );
		m_origin = get_int( a_row.ORIGIN );
		wcscpy_s( m_interface_name, 256, FROM_STRING( a_row.INTERFACE_USCORENAME, CP_UTF8 ) );

	}

	static char* schema_name() { return "MDSCHEMA_FUNCTIONS"; }

	BEGIN_PROVIDER_COLUMN_MAP( function_row )
		PROVIDER_COLUMN_ENTRY_WSTR( "FUNCTION_NAME", 1, m_function_name)
		PROVIDER_COLUMN_ENTRY_WSTR( "DESCRIPTION",  2, m_description)
		PROVIDER_COLUMN_ENTRY_WSTR( "PARAMETER_LIST", 3, m_parameter_list)
		PROVIDER_COLUMN_ENTRY_WSTR(	"RETURN_TYPE",	4,  m_return_type)
		PROVIDER_COLUMN_ENTRY_WSTR( "ORIGIN",  5, m_origin)
		PROVIDER_COLUMN_ENTRY_WSTR( "INTERFACE_NAME", 6, m_interface_name)
	END_PROVIDER_COLUMN_MAP()
};