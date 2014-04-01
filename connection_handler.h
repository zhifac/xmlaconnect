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
					wraps a connection to the XMXLA server.
					calls Discover/Execute.
					intended for one single call to the server.
*/

#pragma once

#define ALLOW_TRANSLATIONS

#include <vector>
#include <unordered_map>
#include "soapXMLAConnectionProxy.h"
#ifdef ALLOW_TRANSLATIONS
#include "query_translator.h"
#endif

class connection_handler
{
public:
	class out_of_bound : public std::runtime_error 
	{
	public:
		out_of_bound() : std::runtime_error("index out of bounds"){}
	};
private:
	int m_session_id;
	cxmla__DiscoverResponse m_d_response;
	cxmla__ExecuteResponse m_e_response;
	session* m_session;
	XMLAConnectionProxy m_proxy;
	std::string m_location;
	std::string m_user;
	std::string m_pass;
	std::string m_catalog;
	std::vector<int> m_cell_data;
private:

	typedef std::unordered_map< soap*, session* > indirection_table_type;
	static indirection_table_type& soap_2_session()
	{
		static indirection_table_type result;
		return result;
	}
	static int (*fparsehdr)(struct soap*, const char*, const char*);//this is defined in command.cpp

	static int http_post_parse_header(struct soap *soap, const char* key, const char* val)
	{
		if ( !soap_tag_cmp(key, "Server") )
		{
			::session* match = soap_2_session()[soap];
			if ( nullptr != match ) 
			{
				session::session_table()[ match ].register_server( val );
			}
		}
		return fparsehdr( soap, key, val );
	}

	HRESULT get_connection_data()
	{
		HRESULT					hr;
		IGetDataSource*			pDataSource = NULL;
		IDBProperties*			pProperties = NULL;
		IMalloc*				pIMalloc = NULL;

		ULONG propCount;
		DBPROPSET* props;

		if FAILED( hr = CoGetMalloc( 1, &pIMalloc ) ) {
			return hr;
		}

		if FAILED( hr = m_session->QueryInterface(__uuidof(IGetDataSource),(void**)&pDataSource) ) {
			pIMalloc->Release();
			return hr;
		}

		if FAILED( hr = pDataSource->GetDataSource( __uuidof(IDBProperties), ( IUnknown** ) &pProperties ) )
		{
			pIMalloc->Release();
			pDataSource->Release();
			return hr;
		}

		pProperties->GetProperties( 0, NULL, &propCount, &props );


		for ( ULONG i = 0; i < propCount; i++ )
		{
			for ( ULONG j =0; j < props[i].cProperties; j++ )
			{
				if ( IsEqualGUID( props[i].guidPropertySet,DBPROPSET_DBINIT ) )
				{
					switch ( props[i].rgProperties[j].dwPropertyID )
					{
					case DBPROP_INIT_LOCATION:
						m_location = CT2A(props[i].rgProperties[j].vValue.bstrVal);
						break;
					case DBPROP_AUTH_USERID:
						m_user = CT2A(props[i].rgProperties[j].vValue.bstrVal);
						break;
					case DBPROP_AUTH_PASSWORD:
						m_pass = CT2A(props[i].rgProperties[j].vValue.bstrVal);
						break;
					case DBPROP_INIT_CATALOG:
						if ( props[i].rgProperties[j].vValue.bstrVal )  m_catalog = CT2A(props[i].rgProperties[j].vValue.bstrVal);
						break;
					}
				}
				VariantClear( &(props[i].rgProperties[j].vValue) );
			}
			pIMalloc->Free( props[i].rgProperties );
		}
		pIMalloc->Free( props );

		//Seesion catalog has higher precendence than db catalog
		ISessionProperties* pISessionProperties = NULL;
		if SUCCEEDED( m_session->QueryInterface(__uuidof(ISessionProperties),(void**)&pISessionProperties) ) {
			pISessionProperties->GetProperties( 0, NULL, &propCount, &props );

			for ( ULONG i = 0; i < propCount; i++ )
			{
				for ( ULONG j =0; j < props[i].cProperties; j++ )
				{
					if ( IsEqualGUID( props[i].guidPropertySet, DBPROPSET_SESSION ) ) {
						if ( DBPROP_CURRENTCATALOG == props[i].rgProperties[j].dwPropertyID ) {
							std::string buf = CT2A(props[i].rgProperties[j].vValue.bstrVal);
							if ( !buf.empty() )  {
								std::swap( m_catalog, buf );
							}
						}
					}
					VariantClear( &(props[i].rgProperties[j].vValue) );
				}
				pIMalloc->Free( props[i].rgProperties );
			}
			pIMalloc->Free( props );

			pISessionProperties->Release();
		}

		pIMalloc->Release();
		pProperties->Release();
		pDataSource->Release();
		return S_OK;
	}

	void load_restrictions( ULONG cRestrictions, const VARIANT* rgRestrictions, xmlns__Restrictions& where )
	{
		//TODO: validate memory consumption due to the strdup here

		where.RestrictionList.PropertyName = NULL;
		where.RestrictionList.CATALOG_USCORENAME = NULL;
		where.RestrictionList.CUBE_USCORENAME = NULL;
		where.RestrictionList.HIERARCHY_USCOREUNIQUE_USCORENAME = NULL;
		where.RestrictionList.MEMBER_USCOREUNIQUE_USCORENAME = NULL;
		where.RestrictionList.LEVEL_USCOREUNIQUE_USCORENAME = NULL;
		where.RestrictionList.TREE_USCOREOP = NULL;


		if ( !m_catalog.empty() ) {
			where.RestrictionList.CATALOG_USCORENAME = _strdup(m_catalog.c_str());//Allocates for consistence (all the other do strdup)
		}

		//only handle what we know
		for( ULONG i = 0; i < cRestrictions; i++ ) 
		{
			switch (i) 
			{
			case 0://CATALOG_NAME
				if ( VT_BSTR == rgRestrictions[i].vt  ) {
					where.RestrictionList.CATALOG_USCORENAME = _strdup(CT2A(rgRestrictions[i].bstrVal));
				}
				break;
			case 2://CUBE_NAME
				if ( VT_BSTR == rgRestrictions[i].vt  ) {
					where.RestrictionList.CUBE_USCORENAME = _strdup(CT2A(rgRestrictions[i].bstrVal));
				}
				break;
			case 4://HIERARCHY_UNIQUE_NAME
				if ( VT_BSTR == rgRestrictions[i].vt  ) {
					where.RestrictionList.HIERARCHY_USCOREUNIQUE_USCORENAME = _strdup(CT2A(rgRestrictions[i].bstrVal));
				}
				break;
			case 5://LEVEL_UNIQUE_NAME
				if ( VT_BSTR == rgRestrictions[i].vt  ) {
					where.RestrictionList.LEVEL_USCOREUNIQUE_USCORENAME = _strdup(CT2A(rgRestrictions[i].bstrVal));
				}
				break;
			case 8://MEMBER_UNIQUE_NAME
				if ( VT_BSTR == rgRestrictions[i].vt  ) {
					where.RestrictionList.MEMBER_USCOREUNIQUE_USCORENAME = _strdup(CT2A(rgRestrictions[i].bstrVal));
				}
				break;
			case 11://TREE_OP
				if ( VT_UI4 == rgRestrictions[i].vt  ) {
					char buf[20];
					_itoa_s( rgRestrictions[i].uiVal, buf, 20, 10 );
					where.RestrictionList.TREE_USCOREOP = _strdup( buf );
				} else if ( VT_EMPTY == rgRestrictions[i].vt  ) {
					where.RestrictionList.TREE_USCOREOP = "0";
				}
				break;
			}
		}
	}

	void unload_restrictions( xmlns__Restrictions& where )
	{
		if ( NULL != where.RestrictionList.PropertyName ) 
		{
			free( where.RestrictionList.PropertyName );
			where.RestrictionList.PropertyName = NULL;
		}
		if ( NULL != where.RestrictionList.CATALOG_USCORENAME )
		{
			free( where.RestrictionList.CATALOG_USCORENAME );
			where.RestrictionList.CATALOG_USCORENAME = NULL;
		}
		if ( NULL != where.RestrictionList.CUBE_USCORENAME )
		{
			free( where.RestrictionList.CUBE_USCORENAME );
			where.RestrictionList.CUBE_USCORENAME = NULL;
		}
		if ( NULL != where.RestrictionList.HIERARCHY_USCOREUNIQUE_USCORENAME )
		{
			free( where.RestrictionList.HIERARCHY_USCOREUNIQUE_USCORENAME );
			where.RestrictionList.HIERARCHY_USCOREUNIQUE_USCORENAME = NULL;
		}
		if ( NULL != where.RestrictionList.MEMBER_USCOREUNIQUE_USCORENAME )
		{
			free( where.RestrictionList.MEMBER_USCOREUNIQUE_USCORENAME );
			where.RestrictionList.MEMBER_USCOREUNIQUE_USCORENAME = NULL;
		}
		if ( NULL != where.RestrictionList.LEVEL_USCOREUNIQUE_USCORENAME )
		{
			free( where.RestrictionList.LEVEL_USCOREUNIQUE_USCORENAME );
			where.RestrictionList.LEVEL_USCOREUNIQUE_USCORENAME = NULL;
		}
		if ( NULL != where.RestrictionList.TREE_USCOREOP )
		{
			free( where.RestrictionList.TREE_USCOREOP );
			where.RestrictionList.TREE_USCOREOP = NULL; 
		}
	}

private:
	void begin_session()
	{
		m_proxy.header = new SOAP_ENV__Header();
		m_proxy.header->BeginSession = new BSessionType();
		m_proxy.header->BeginSession->element = NULL;
		m_proxy.header->BeginSession->xmlns = NULL;
		m_proxy.header->EndSession = NULL;
		m_proxy.header->Session = NULL;
	}

	void session()
	{
		char sessid[20];
		_itoa_s( m_session_id, sessid, 20, 10 );
		m_proxy.header = new SOAP_ENV__Header();
		m_proxy.header->Session = new SessionType();
		m_proxy.header->Session->element = NULL;
		m_proxy.header->Session->xmlns = NULL;
		m_proxy.header->Session->SessionId = _strdup( sessid );
		m_proxy.header->EndSession = NULL;
		m_proxy.header->BeginSession = NULL;
	}
public:
	connection_handler( IUnknown* aSession ) : m_session( nullptr )
	{
		IGetSelf* pGetSelf;
		aSession->QueryInterface( __uuidof( IGetSelf ) , ( void** ) &pGetSelf );
		pGetSelf->GetSelf( (void**)&m_session );
		pGetSelf->Release();

		m_session_id = session::session_table()[ m_session ].id;

		get_connection_data();

		m_proxy.header = NULL;

		if ( nullptr == fparsehdr ) 
		{
			fparsehdr = m_proxy.fparsehdr;
		}
		m_proxy.fparsehdr = http_post_parse_header;

		soap_2_session()[ &m_proxy ] = m_session;
	}

	connection_handler( const std::string& location, const std::string& user, const std::string& pass, const std::string& catalog )
		: m_session( nullptr )
		, m_location(location)
		, m_user(user)
		, m_pass(pass)
		, m_catalog( catalog )
		, m_session_id( -1 )
	{
		if ( nullptr == fparsehdr ) 
		{
			fparsehdr = m_proxy.fparsehdr;
		}
		m_proxy.fparsehdr = http_post_parse_header;
	}

	virtual ~connection_handler()
	{
		soap_2_session().erase( &m_proxy );
	}

	int discover( char* endpoint, ULONG cRestrictions, const VARIANT* rgRestrictions)
	{
		m_proxy.soap_endpoint = m_location.c_str();

		soap_omode(&m_proxy, SOAP_XML_DEFAULTNS);
		if ( -1 == m_session_id ) {
			begin_session();
			m_proxy.userid = m_user.c_str();
			m_proxy.passwd = m_pass.c_str();
		} else {
			//palo requires credentials inside the session
			m_proxy.userid = m_user.c_str();
			m_proxy.passwd = m_pass.c_str();
			session();
		}	

		xmlns__Restrictions restrictions;
		load_restrictions( cRestrictions, rgRestrictions, restrictions );
		xmlns__Properties props;
		props.PropertyList.Catalog = const_cast<char*>(m_catalog.c_str());//make Palo happy		
		props.PropertyList.LocaleIdentifier = 1048;
		int result = m_proxy.Discover( endpoint, restrictions, props, m_d_response );
		if ( NULL != m_session && NULL != m_proxy.header && NULL != m_proxy.header->Session && NULL != m_proxy.header->Session->SessionId ) {
			session::session_table()[ m_session ].id = atoi( m_proxy.header->Session->SessionId );
		}
		unload_restrictions( restrictions );
		return result;
	}

	void get_cell_data( cxmla__ExecuteResponse&  aResponse ) 
	{
		if ( NULL == aResponse.cxmla__return__.root.CellData ) return;
		if ( NULL == aResponse.cxmla__return__.root.Axes ) return; 

		int size = 1;
		//get cellData size with empty values
		for( int i = 0; i < aResponse.cxmla__return__.root.Axes->__size; ++i ) {
			if (  strcmp (aResponse.cxmla__return__.root.Axes->Axis[i].name,"SlicerAxis") != 0) {
				size *= aResponse.cxmla__return__.root.Axes->Axis[i].Tuples.__size;
			}
		}

		m_cell_data.assign (size,-1);

		for( int i = 0; i < aResponse.cxmla__return__.root.CellData->__size; ++i ) {
			m_cell_data[ atoi(aResponse.cxmla__return__.root.CellData->Cell[i].CellOrdinal) ] = i;
		}
	}

	int execute ( char* statement )
	{
		m_proxy.soap_endpoint = m_location.c_str();
		soap_omode(&m_proxy, SOAP_XML_DEFAULTNS);
		//soap_omode(&m_proxy,SOAP_XML_INDENT);
		if ( -1 == m_session_id ) {
			begin_session();
			m_proxy.userid = m_user.c_str();
			m_proxy.passwd = m_pass.c_str();
		} else {
			//palo requires credentials inside the session
			m_proxy.userid = m_user.c_str();
			m_proxy.passwd = m_pass.c_str();
			session();
		}	

		xmlns__Command command;

#ifdef ALLOW_TRANSLATIONS
		std::string translation( statement );
		if ( nullptr !=  m_session )
		{
			query_translator::translator().translate( translation, session::session_table()[ m_session ].server );
		}
		statement = const_cast<char*>( translation.c_str() );
#endif
		command.Statement = statement;
		xmlns__Properties Properties;
		Properties.PropertyList.LocaleIdentifier = 1048;
		Properties.PropertyList.Content = "Data";
		Properties.PropertyList.AxisFormat = "TupleFormat";
		Properties.PropertyList.Format = "Multidimensional";
		Properties.PropertyList.Catalog = const_cast<char*>(m_catalog.c_str());
		//clear cell data
		m_cell_data.clear();
		int result = m_proxy.Execute( NULL, command, Properties, m_e_response );
		//add cell data
		get_cell_data( m_e_response );
		if ( NULL != m_session && NULL != m_proxy.header && NULL != m_proxy.header->Session && NULL != m_proxy.header->Session->SessionId ) {
			session::session_table()[ m_session ].id = atoi( m_proxy.header->Session->SessionId );
		}

		return result;
	}
	
	bool no_session() 
	{
		bool result = m_proxy.fault && 0 == strcmp(m_proxy.fault->faultstring,"Invalid Session id");
		if ( result ) {
			m_session_id = -1;
		}
		return result;
	}

	const cxmla__DiscoverResponse& discover_response() const
	{
		return m_d_response;
	}

	const cxmla__ExecuteResponse& execute_response() const
	{
		return m_e_response;
	}

	const char* fault_string()
	{
		if  ( NULL == m_proxy.fault ) {
			static const char* noInfo = "No further information.";
			return noInfo;
		}
		return m_proxy.fault->faultstring;
	}

	const bool valid_credentials()
	{
		if  ( NULL == m_proxy.fault ) { return true; }
		if  ( 401 ==  m_proxy.error ) { return false; }
		return ( NULL == strstr( m_proxy.fault->faultstring, "ORA-01005" )  && NULL == strstr( m_proxy.fault->faultstring, "ORA-01017" ) );
	}

	VARIANT at( DBORDINAL index )
	{
		VARIANT result;
		result.vt = VT_NULL;

		if ( NULL == m_e_response.cxmla__return__.root.CellData ) {
			throw std::runtime_error( "no query response");
		}

		if ( index >= m_cell_data.size() ) {
			//empty response
			result.bstrVal = NULL;
			result.vt = VT_BSTR;
			return result;
		}

		if ( -1 == m_cell_data[index] ) {
			//empty response
			result.bstrVal = NULL;
			result.vt = VT_BSTR;
			return result;
		}

		_Value& val = m_e_response.cxmla__return__.root.CellData->Cell[m_cell_data[index]].Value;
		
		if ( 0 == strcmp( val.xsi__type, "xsd:double" ) ) {
			result.vt = VT_R8;
			if ( NULL == val.__v ) {
				result.bstrVal = NULL;
				result.vt = VT_BSTR;
			} else {
				result.dblVal = atof( val.__v );
			}
		} else if ( 0 == strcmp( val.xsi__type, "xsd:string" ) ) {
			result.vt = VT_BSTR;
			if ( NULL != val.__v ) {
				result.bstrVal = SysAllocString( CA2W( val.__v, CP_UTF8 ) );
			} else {
				result.bstrVal = NULL;
			}
		} else if ( 0 == strcmp( val.xsi__type, "xsd:empty" ) ) {
			result.bstrVal = NULL;
			result.vt = VT_BSTR;
		} else if ( 0 == strcmp( val.xsi__type, "xsd:int" ) ) {
			result.vt = VT_I4;
			if ( NULL == val.__v ) {
				result.bstrVal = NULL;
				result.vt = VT_BSTR;
			} else {
				result.intVal = atoi( val.__v );
			}
		} else {
			//handle unknown as string
			result.vt = VT_BSTR;
			if ( NULL != val.__v ) {
				result.bstrVal = SysAllocString( CA2W( val.__v, CP_UTF8 ) );
			} else {
				result.bstrVal = NULL;
			}
		}

		return result;
	}

	void get_axis_info( DBCOUNTITEM   *pcAxes, MDAXISINFO   **prgAxisInfo )
	{
		if ( nullptr == m_e_response.cxmla__return__.root.OlapInfo ) {
			throw std::runtime_error( "no query response");
		}

		if ( nullptr == m_e_response.cxmla__return__.root.OlapInfo ){
			throw std::runtime_error( "the server returned an invalid answer");
		}

		if ( nullptr == m_e_response.cxmla__return__.root.Axes ){
			throw std::runtime_error( "the server returned an invalid answer");
		}

		*pcAxes			= ( DBCOUNTITEM ) m_e_response.cxmla__return__.root.OlapInfo->AxesInfo.__size;
		if ( 0 == *pcAxes ) {
			return;
		}

		//Mondrian gives an empty slicer
		DBCOUNTITEM idx = 0;

		MDAXISINFO* axisInfo = new MDAXISINFO[*pcAxes];
		for ( DBCOUNTITEM i = 0; i < *pcAxes; ++i ) {
			axisInfo[idx].cbSize = sizeof( MDAXISINFO );
			axisInfo[idx].cCoordinates = m_e_response.cxmla__return__.root.Axes->Axis[idx].Tuples.__size;//count on the same order
			axisInfo[idx].cDimensions = m_e_response.cxmla__return__.root.OlapInfo->AxesInfo.AxisInfo[idx].__size;
			std::string name( m_e_response.cxmla__return__.root.OlapInfo->AxesInfo.AxisInfo[idx].name );
			if ( name.substr( 0, 4 ) == "axis" ) {
				axisInfo[idx].iAxis = atoi( name.substr(4, name.size() ).c_str() );
			} else {
				axisInfo[idx].iAxis = MDAXIS_SLICERS;
				if ( 0 == m_e_response.cxmla__return__.root.OlapInfo->AxesInfo.AxisInfo[idx].__size ) {
					//slicer was present but it was empty
					(*pcAxes)--;
					continue;
				}
			}
			axisInfo[idx].rgcColumns = new DBORDINAL[ axisInfo[idx].cDimensions ];
			axisInfo[idx].rgpwszDimensionNames = new LPOLESTR[ axisInfo[idx].cDimensions ];
			for ( DBCOUNTITEM j = 0; j < axisInfo[idx].cDimensions; ++j ) {
				xmlns__HierarchyInfo hInfo = m_e_response.cxmla__return__.root.OlapInfo->AxesInfo.AxisInfo[idx].HierarchyInfo[j];
				DBORDINAL col_count = 5;//required columns;
				if ( NULL != hInfo.PARENT_USCOREUNIQUE_USCORENAME ) {
					++col_count;
				}
				if ( NULL != hInfo.MEMBER_USCORENAME ) {
					++col_count;
				}
				if ( NULL != hInfo.MEMBER_USCORETYPE ) {
					++col_count;
				}
				col_count += hInfo.__userProp.__size;
				axisInfo[idx].rgcColumns[j] = col_count;
				axisInfo[idx].rgpwszDimensionNames[j] =  _wcsdup( CA2W( m_e_response.cxmla__return__.root.OlapInfo->AxesInfo.AxisInfo[idx].HierarchyInfo[j].name, CP_UTF8 ) );
			}
			++idx;
		}

		*prgAxisInfo	= axisInfo;
	}

	void free_axis_info( DBCOUNTITEM   cAxes, MDAXISINFO   *rgAxisInfo )
	{
		for ( DBCOUNTITEM i = 0; i < cAxes; ++i ) {
			delete[] rgAxisInfo[i].rgcColumns;
			for ( DBCOUNTITEM j = 0; j < rgAxisInfo[i].cDimensions; ++j ) {
				delete[] rgAxisInfo[i].rgpwszDimensionNames[j];
			}
			delete[] rgAxisInfo[i].rgpwszDimensionNames;
		}
		delete[] rgAxisInfo;
	}

	unsigned int row_count()
	{
		if ( NULL == m_e_response.cxmla__return__.root.CellData ) {
			return 0;
		}
		return m_e_response.cxmla__return__.root.CellData->__size;
	}

	void get_axis( DBCOUNTITEM idx, xmlns__Axis*& axisData, xmlns__AxisInfo*& axisInfo )
	{
		if ( NULL == m_e_response.cxmla__return__.root.Axes ) {
			throw std::runtime_error( "no query response");
		}

		if ( NULL == m_e_response.cxmla__return__.root.OlapInfo ) {
			throw std::runtime_error( "no query response");
		}

		std::string axisName;
		if ( MDAXIS_SLICERS == idx ) {
			axisName = "SlicerAxis";
		} else {
			std::stringstream buf;
			buf << "Axis";
			buf << idx;
			axisName = buf.str();
		}
		
		for ( int i = 0, e = m_e_response.cxmla__return__.root.Axes->__size; i < e; ++i ) {
			if ( axisName == m_e_response.cxmla__return__.root.Axes->Axis[i].name ) {
				axisData = &( m_e_response.cxmla__return__.root.Axes->Axis[i] );
			}
		}

		for ( DBCOUNTITEM i = 0, e = ( DBCOUNTITEM ) m_e_response.cxmla__return__.root.OlapInfo->AxesInfo.__size; i < e; ++i ) {
			if ( axisName == m_e_response.cxmla__return__.root.OlapInfo->AxesInfo.AxisInfo[i].name ) {
				axisInfo = &(m_e_response.cxmla__return__.root.OlapInfo->AxesInfo.AxisInfo[i] );
			}
		}
	}
};
