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
					schema rowset
*/

#include "connection_handler.h"

#pragma once


template <class T, class Storage, class CreatorClass >
class base_rowset :
	public CSchemaRowsetImpl< T, Storage, CreatorClass>,
	public IErrorRecordsImpl< base_rowset<T, Storage, CreatorClass>>
{
public:

	//IDBSchemaRowset implementation
	HRESULT Execute( DBROWCOUNT* pcRowsAffected, ULONG cRestrictions, const VARIANT* rgRestrictions ) 
	{
		if ( 0 == strcmp( "UNSUPORTED", Storage::schema_name() ) ) {
			pcRowsAffected = 0;
			return S_OK;
		}

		connection_handler handler( m_spUnkSite );

		int result = handler.discover( Storage::schema_name(), cRestrictions, rgRestrictions);
		if ( handler.no_session() ) {
			result = handler.discover( Storage::schema_name(), cRestrictions, rgRestrictions);
		}
		if ( S_OK != result ) {
			make_error( FROM_STRING( handler.fault_string(), CP_UTF8 ) );
			return E_FAIL;
		}

		for ( int i = 0, e = handler.discover_response().cxmla__return__.root.__rows.__size; i < e; ++i ) {			
			if ( 0 == strcmp( "MDSCHEMA_LEVELS", Storage::schema_name() ) ) {
				if ( 0 == strcmp( "[Measures]", handler.discover_response().cxmla__return__.root.__rows.row[i].DIMENSION_USCOREUNIQUE_USCORENAME ) )
				{
					//Somehow we will crash under excel without this hack.
					row dummy_level;
					dummy_level.CATALOG_USCORENAME = handler.discover_response().cxmla__return__.root.__rows.row[i].CATALOG_USCORENAME;
					dummy_level.CUBE_USCORENAME = handler.discover_response().cxmla__return__.root.__rows.row[i].CUBE_USCORENAME;
					dummy_level.DIMENSION_USCOREUNIQUE_USCORENAME = handler.discover_response().cxmla__return__.root.__rows.row[i].DIMENSION_USCOREUNIQUE_USCORENAME;
					dummy_level.HIERARCHY_USCOREUNIQUE_USCORENAME = handler.discover_response().cxmla__return__.root.__rows.row[i].HIERARCHY_USCOREUNIQUE_USCORENAME;
					dummy_level.LEVEL_USCORECARDINALITY = handler.discover_response().cxmla__return__.root.__rows.row[i].LEVEL_USCORECARDINALITY;
					dummy_level.LEVEL_USCORETYPE = handler.discover_response().cxmla__return__.root.__rows.row[i].LEVEL_USCORETYPE;
					dummy_level.LEVEL_USCORENAME = "Level00";
					dummy_level.LEVEL_USCOREUNIQUE_USCORENAME = "[Measures].[Level00]";
					dummy_level.LEVEL_USCORECAPTION = "Level00";
					dummy_level.LEVEL_USCORENUMBER = "-1";
					dummy_level.DESCRIPTION = "Level00";
					m_rgRowData.Add( Storage( dummy_level ) );
				}
			}
			m_rgRowData.Add( Storage( handler.discover_response().cxmla__return__.root.__rows.row[i] ) );
		}


		*pcRowsAffected = (LONG) m_rgRowData.GetCount();
		return S_OK;
	}

public:
	BEGIN_PROPSET_MAP( T )
	BEGIN_PROPERTY_SET( DBPROPSET_ROWSET )
	PROPERTY_INFO_ENTRY( IAccessor )
	PROPERTY_INFO_ENTRY( IColumnsInfo )
	PROPERTY_INFO_ENTRY( IConvertType )
	PROPERTY_INFO_ENTRY( IRowset )
	PROPERTY_INFO_ENTRY( IRowsetIdentity )
	PROPERTY_INFO_ENTRY( IRowsetInfo )
	PROPERTY_INFO_ENTRY( ISupportErrorInfo )
	PROPERTY_INFO_ENTRY( CANFETCHBACKWARDS )
	PROPERTY_INFO_ENTRY( CANHOLDROWS )
	PROPERTY_INFO_ENTRY( CANSCROLLBACKWARDS )
	PROPERTY_INFO_ENTRY_VALUE( MAXOPENROWS, 0 )
	PROPERTY_INFO_ENTRY_VALUE( MAXROWS, 0 )
	END_PROPERTY_SET( DBPROPSET_ROWSET )
	END_PROPSET_MAP()
};
