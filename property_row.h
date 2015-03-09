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
					a row in the propery rowset
*/

#pragma once

#include "soapH.h"

class property_row
{
private:
	static const size_t MAX_BUF_SIZE = 10 * 1024 * sizeof( wchar_t );
private:
	void copy( const property_row& other )
	{
		m_schema = nullptr;
		m_member_unique_name = nullptr;
		m_prop_content_type = 0;
		m_prop_origin = 1;
		m_prop_is_visible = true;
		m_description = nullptr;
		
		m_prop_cardinality = _wcsdup( FROM_STRING( "MANY", CP_UTF8 ));

		m_catalog = _wcsdup( other.m_catalog  );
		m_cube = _wcsdup( other.m_cube );
		m_dimension_unique_name = _wcsdup( other.m_dimension_unique_name );
		m_hierarchy_unique_name = _wcsdup( other.m_hierarchy_unique_name );
		m_level_unique_name = _wcsdup( other.m_level_unique_name );
		m_property_type = other.m_property_type;
		m_property_name = _wcsdup( other.m_property_name );
		m_property_caption = _wcsdup( other.m_property_caption );		
		m_attrib_hier = _wcsdup( other.m_attrib_hier );
		m_data_type = other.m_data_type;
		
	}
public:
	wchar_t*				m_catalog;
	wchar_t*				m_schema;
	wchar_t*				m_cube;
	wchar_t*				m_dimension_unique_name;
	wchar_t*				m_hierarchy_unique_name;
	wchar_t*				m_level_unique_name;
	wchar_t*				m_member_unique_name;
	short					m_property_type;
	wchar_t*				m_property_name;
	wchar_t*				m_property_caption;
	unsigned short			m_data_type;
	wchar_t*				m_description;
	short					m_prop_content_type;
	unsigned short			m_prop_origin;
	wchar_t*				m_attrib_hier;
	wchar_t*				m_prop_cardinality;
	bool					m_prop_is_visible;

	property_row( row& a_row )
	{
		m_schema = nullptr;
		m_member_unique_name = nullptr;
		m_prop_content_type = 0;
		m_prop_origin = 1;
		m_prop_is_visible = true;
		m_description = nullptr;
		
		m_prop_cardinality = _wcsdup( FROM_STRING( "MANY", CP_UTF8 ));

		m_catalog = _wcsdup( FROM_STRING( a_row.CATALOG_USCORENAME, CP_UTF8 ) );
		m_cube = _wcsdup( FROM_STRING( a_row.CUBE_USCORENAME, CP_UTF8 ) );
		m_dimension_unique_name = _wcsdup( FROM_STRING( a_row.DIMENSION_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		m_hierarchy_unique_name = _wcsdup( FROM_STRING( a_row.HIERARCHY_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		m_level_unique_name = _wcsdup( FROM_STRING( a_row.LEVEL_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
		m_property_type = get_int(a_row.PROPERTY_USCORETYPE);
		m_property_name = _wcsdup( FROM_STRING( a_row.PROPERTY_USCORENAME, CP_UTF8 ) );
		m_property_caption = _wcsdup( FROM_STRING( a_row.PROPERTY_USCORECAPTION, CP_UTF8 ) );		
		m_attrib_hier = _wcsdup( FROM_STRING( a_row.PROPERTY_USCOREATTRIBUTE_USCOREHIERARCHY_USCORENAME, CP_UTF8 ));
		m_data_type = get_int(a_row.DATA_USCORETYPE);		
	}

	property_row( const property_row& other )
	{
		copy( other );
	}

	property_row& operator=( const property_row& other )
	{
		if ( this != &other )  {  copy( other ); }
		return *this;
	}

	~property_row()
	{
		delete[] m_catalog;
		delete[] m_cube;
		delete[] m_dimension_unique_name;
		delete[] m_hierarchy_unique_name;
		delete[] m_level_unique_name;
		delete[] m_property_name;
		delete[] m_property_caption;
		delete[] m_attrib_hier;
	}


	EMPTY_CONSTRUCTOR(property_row);

	static char* schema_name() { return "MDSCHEMA_PROPERTIES"; }

	BEGIN_PROVIDER_COLUMN_MAP( property_row )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CATALOG_NAME", 1, MAX_BUF_SIZE, m_catalog )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "SCHEMA_NAME", 2, MAX_BUF_SIZE, m_schema )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "CUBE_NAME", 3, MAX_BUF_SIZE, m_cube )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DIMENSION_UNIQUE_NAME", 4, MAX_BUF_SIZE, m_dimension_unique_name )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "HIERARCHY_UNIQUE_NAME", 5, MAX_BUF_SIZE, m_hierarchy_unique_name )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "LEVEL_UNIQUE_NAME", 6, MAX_BUF_SIZE, m_level_unique_name )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "MEMBER_UNIQUE_NAME", 7, MAX_BUF_SIZE, m_member_unique_name )
	PROVIDER_COLUMN_ENTRY_TYPE( "PROPERTY_TYPE", 8, VT_I2, m_property_type )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "PROPERTY_NAME", 9, MAX_BUF_SIZE, m_property_name )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "PROPERTY_CAPTION", 10, MAX_BUF_SIZE, m_property_caption )
	PROVIDER_COLUMN_ENTRY_TYPE( "DATA_TYPE", 11, VT_UI2, m_data_type )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "DESCRIPTION", 12, MAX_BUF_SIZE, m_description )
	PROVIDER_COLUMN_ENTRY_TYPE( "PROPEPERTY_CONTENT_TYPE", 13, VT_I2, m_prop_content_type )
	PROVIDER_COLUMN_ENTRY_TYPE( "PROPERTY_ORIGIN", 14, VT_UI2, m_prop_origin )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "PROPERTY_ATTRIBUTE_HIERARCHY_NAME", 15, MAX_BUF_SIZE, m_attrib_hier )
	PROVIDER_COLUMN_ENTRY_VAR_WSTR( "PROPERTY_CARDINALITY", 16, MAX_BUF_SIZE, m_prop_cardinality )
	PROVIDER_COLUMN_ENTRY_TYPE( "PROPERTY_IS_VISIBLE", 17, VT_BOOL, m_prop_is_visible )
	END_PROVIDER_COLUMN_MAP()
};