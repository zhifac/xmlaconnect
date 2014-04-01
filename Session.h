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
					session object
*/

#pragma once

#include <strsafe.h>

class catalog_rowset;
class cube_rowset;
class dimension_rowset;
class hierarchy_rowset;
class level_rowset;
class member_rowset;
class measure_rowset;
class property_rowset;
class set_rowset;

using namespace ATL;

class ATL_NO_VTABLE session : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IGetDataSourceImpl<session>,
	public IOpenRowsetImpl<session>,
	public ISessionPropertiesImpl<session>,
	public IObjectWithSiteSessionImpl<session>,
	public IDBSchemaRowsetImpl<session>,
	public IDBCreateCommandImpl<session, command>,
	public IGetSelf	, 
	public ISupportErrorInfo
{
public:
	struct session_data
	{
		enum server_type
		{
			UNDEFINED = 0,
			ORACLE = 1,
			MONDRIAN = 2,
			JEDOX = 3
		};

		int id;
		server_type server;

		session_data() : id(-1), server( UNDEFINED ){}

		void register_server( const char* server_data )
		{
			if ( server !=  UNDEFINED  ) { return; }
			if ( str_match( server_data, "Apache" ) ) { server = MONDRIAN; return; }
			if ( str_match( server_data, "gSOAP" ) ) { server = ORACLE; return; }
			if ( str_match( server_data, "Palo" ) ) { server = JEDOX; return; }
		}

	private:
		bool str_match( const char* src, const char* match )
		{
			while ( *src++ == *match++ )
			{
				if ( 0 == *src ) { return true; }
			}
			return 0 == *match;
		}
	};
	typedef std::map< session*, session_data > session_table_type;
	static  session_table_type& session_table(){
		static session_table_type result;
		return result;
	}
public:

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		session_table()[ this ] = session_data();
		return FInit();
	}
	
	void FinalRelease() 
	{
		session_table().erase( session_table().find( this ) );
	}

	STDMETHOD(OpenRowset)(IUnknown *pUnk, DBID *pTID, DBID *pInID, REFIID riid,
					   ULONG cSets, DBPROPSET rgSets[], IUnknown **ppRowset)
	{
		rowset* pRowset;
		return CreateRowset(pUnk, pTID, pInID, riid, cSets, rgSets, ppRowset, pRowset);
	}

	void SetRestrictions(ULONG cRestrictions, GUID* rguidSchema, ULONG* rgRestrictions)
	{
		for (ULONG l=0; l<cRestrictions; l++)
		{
			if (InlineIsEqualGUID(rguidSchema[l], DBSCHEMA_TABLES))
				rgRestrictions[l] = 0x04;
			else if (InlineIsEqualGUID(rguidSchema[l], DBSCHEMA_COLUMNS))
				rgRestrictions[l] = 0x04;
			else if (InlineIsEqualGUID(rguidSchema[l], DBSCHEMA_PROVIDER_TYPES))
				rgRestrictions[l] = 0x00;
		}
	}

//IGetSelf
	STDMETHODIMP GetSelf( void** pSelf )
	{
		if ( NULL == pSelf ) {
			return E_INVALIDARG;
		}
		*pSelf = this;
		return S_OK;
	}

//ISupportErrorInfo
	HRESULT STDMETHODCALLTYPE InterfaceSupportsErrorInfo (REFIID riid)
	{
		return S_OK;
	}

BEGIN_PROPSET_MAP(session)
	BEGIN_PROPERTY_SET(DBPROPSET_SESSION)
		PROPERTY_INFO_ENTRY( CURRENTCATALOG )
	END_PROPERTY_SET(DBPROPSET_SESSION)
END_PROPSET_MAP()

BEGIN_COM_MAP(session)
	COM_INTERFACE_ENTRY(IGetDataSource)
	COM_INTERFACE_ENTRY(IOpenRowset)
	COM_INTERFACE_ENTRY(ISessionProperties)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IDBCreateCommand)
	COM_INTERFACE_ENTRY(IDBSchemaRowset)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IGetSelf)
END_COM_MAP()

BEGIN_SCHEMA_MAP(session)
	SCHEMA_ENTRY( DBSCHEMA_CATALOGS , catalog_rowset )
	SCHEMA_ENTRY( MDSCHEMA_CUBES , cube_rowset )
	SCHEMA_ENTRY( MDSCHEMA_DIMENSIONS, dimension_rowset )
	SCHEMA_ENTRY( MDSCHEMA_HIERARCHIES, hierarchy_rowset )
	SCHEMA_ENTRY( MDSCHEMA_LEVELS, level_rowset )
	SCHEMA_ENTRY( MDSCHEMA_MEMBERS, member_rowset )
	SCHEMA_ENTRY( MDSCHEMA_MEASURES, measure_rowset )
	SCHEMA_ENTRY( MDSCHEMA_PROPERTIES, property_rowset )
	SCHEMA_ENTRY( MDSCHEMA_SETS, set_rowset )
END_SCHEMA_MAP()

};


#include "catalog_row.h"
#include "cube_row.h"
#include "dimension_row.h"
#include "hierarchy_row.h"
#include "level_row.h"
#include "measure_row.h"
#include "member_row.h"
#include "property_row.h"
#include "set_row.h"
#include "base_rowset.h"
class catalog_rowset : public base_rowset< catalog_rowset, catalog_row, session >{};
class cube_rowset : public base_rowset< cube_rowset, cube_row, session >{};
class dimension_rowset : public base_rowset< dimension_rowset, dimension_row, session >{};
class hierarchy_rowset : public base_rowset< hierarchy_rowset, hierarchy_row, session >{};
class level_rowset : public base_rowset< level_rowset, level_row, session >{};
class member_rowset : public base_rowset< member_rowset, member_row, session >{};
class measure_rowset : public base_rowset< measure_rowset, measure_row, session >{};
class property_rowset : public base_rowset< property_rowset, property_row, session >{};
class set_rowset : public base_rowset< set_rowset, set_row, session >{};
