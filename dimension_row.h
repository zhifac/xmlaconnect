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
					a row in the dimension rowset
*/

#pragma once

#include "soapH.h"

class dimension_row
{
private:
	static const size_t MAX_BUF_SIZE = 10 * 1024 * sizeof( wchar_t );
private:
	void copy( const dimension_row& other )
	{
		m_schema = nullptr;
	
		m_catalog = _wcsdup( other.m_catalog );
		m_cube = _wcsdup( other.m_cube );

		m_dimension_name = _wcsdup( other.m_dimension_name );
		m_dimension_unique_name = _wcsdup( other.m_dimension_unique_name );
		m_dimension_guid = other.m_dimension_guid;
		m_dimension_caption = _wcsdup( other.m_dimension_caption);
		m_ordinal = other.m_ordinal;
		m_type = other.m_type;
		m_cardinality = other.m_cardinality;
		m_default_hierarcy = _wcsdup( other.m_default_hierarcy);
		m_description = _wcsdup( other.m_description);
	
	}
public:
	wchar_t*         m_catalog;
	wchar_t*         m_schema;
	wchar_t*         m_cube;
	wchar_t*         m_dimension_name;
	wchar_t*         m_dimension_unique_name;
	GUID             m_dimension_guid;
	wchar_t*         m_dimension_caption;
	unsigned long    m_ordinal;
	short            m_type;
	unsigned long    m_cardinality;
	wchar_t*         m_default_hierarcy;
	wchar_t*         m_description;

	dimension_row( row& a_row ) 
	{
		m_schema = nullptr;		

		m_catalog = _wcsdup( FROM_STRING( a_row.CATALOG_USCORENAME, CP_UTF8 ) );
		m_cube = _wcsdup( FROM_STRING( a_row.CUBE_USCORENAME, CP_UTF8 ) );
		m_dimension_name = _wcsdup( FROM_STRING( a_row.DIMENSION_USCORENAME, CP_UTF8 ) );
		m_dimension_unique_name = _wcsdup( FROM_STRING( a_row.DIMENSION_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		m_dimension_caption = _wcsdup( FROM_STRING( a_row.DIMENSION_USCORECAPTION, CP_UTF8 ) );
		m_default_hierarcy = _wcsdup( FROM_STRING( a_row.DEFAULT_USCOREHIERARCHY, CP_UTF8 ) );
		m_description = _wcsdup( FROM_STRING( a_row.DESCRIPTION, CP_UTF8 ) );

		m_ordinal = atoi(a_row.DIMENSION_USCOREORDINAL);
		m_type = get_int( a_row.DIMENSION_USCORETYPE );
		m_cardinality = get_int( a_row.DIMENSION_USCORECARDINALITY );
	}

	dimension_row( const dimension_row& other )
	{
		copy( other );
	}

	dimension_row& operator=( const dimension_row& other )
	{
		if ( this != &other )  {  copy( other ); }
		return *this;
	}

	~dimension_row()
	{
		delete[] m_catalog;
		delete[] m_cube;
		delete[] m_dimension_name;
		delete[] m_dimension_unique_name;
		delete[] m_dimension_caption;
		delete[] m_default_hierarcy;
		delete[] m_description;
	}

	EMPTY_CONSTRUCTOR(dimension_row);
	static char* schema_name() { return "MDSCHEMA_DIMENSIONS"; }

	BEGIN_PROVIDER_COLUMN_MAP( dimension_row )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CATALOG_NAME", 1, MAX_BUF_SIZE, m_catalog )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "SCHEMA_NAME", 2, MAX_BUF_SIZE, m_schema )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CUBE_NAME", 3, MAX_BUF_SIZE, m_cube )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DIMENSION_NAME", 4, MAX_BUF_SIZE, m_dimension_name )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DIMENSION_UNIQUE_NAME", 5, MAX_BUF_SIZE, m_dimension_unique_name )
	PROVIDER_COLUMN_ENTRY_TYPE( "DIMENSION_GUID", 6, VT_CLSID, m_dimension_guid )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DIMENSION_CAPTION", 7, MAX_BUF_SIZE, m_dimension_caption )
	PROVIDER_COLUMN_ENTRY_TYPE( "DIMENSION_ORDINAL", 8, VT_UI4, m_ordinal )
	PROVIDER_COLUMN_ENTRY_TYPE( "DIMENSION_TYPE", 9, VT_I2, m_type )
	PROVIDER_COLUMN_ENTRY_TYPE( "DIMENSION_CARDINALITY", 10, VT_UI4, m_cardinality )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DEFAULT_HIERARCHY", 11, MAX_BUF_SIZE, m_default_hierarcy )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DESCRIPTION", 12, MAX_BUF_SIZE, m_description )
	END_PROVIDER_COLUMN_MAP()
};
