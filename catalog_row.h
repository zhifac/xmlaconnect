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
					a row in the catalog rowset
*/

#pragma once

#include "soapH.h"

class catalog_row
{
private:
	static const size_t MAX_BUF_SIZE = 1024 * sizeof( wchar_t );
private:
	void copy( const catalog_row& other )
	{
		m_catalog = _wcsdup( other.m_catalog );
		m_description = _wcsdup( other.m_description );
	}
public:
	wchar_t*         m_catalog;
	wchar_t*         m_description;	

	catalog_row( row& a_row ) 
	{
		m_catalog = _wcsdup( FROM_STRING( a_row.CATALOG_USCORENAME, CP_UTF8 ) ); 
		m_description = _wcsdup( FROM_STRING( a_row.DESCRIPTION, CP_UTF8 ) );
	}

	catalog_row( const catalog_row& other )
	{
		copy( other );
	}

	catalog_row& operator=( const catalog_row& other )
	{
		if ( this != &other )  {  copy( other ); }
		return *this;
	}

	~catalog_row()
	{
		delete[] m_catalog;
		delete[] m_description;
	}

	EMPTY_CONSTRUCTOR(catalog_row);
	static char* schema_name() { return "DBSCHEMA_CATALOGS"; }

	BEGIN_PROVIDER_COLUMN_MAP( catalog_row )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CATALOG_NAME", 1, MAX_BUF_SIZE, m_catalog )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DESCRIPTION", 2, MAX_BUF_SIZE, m_description )
	END_PROVIDER_COLUMN_MAP()
};
