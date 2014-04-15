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
					data source object
*/

#pragma once

#include "resource.h"       // main symbols
#include "rowset.h"
#include "session.h"
#include "pass_prompt.h"
#include "comutil.h"

//from msmd.h
#define MDPROP_MDX_SUBQUERIES				0x00000140L
#define IDS_MDPROP_MDX_SUBQUERIES  MDPROP_MDX_SUBQUERIES
#define MDX_SUBQUERIES_Type VT_I4
#define MDX_SUBQUERIES_Flags ( DBPROPFLAGS_DATASOURCEINFO | DBPROPFLAGS_READ )


using namespace ATL;

class ATL_NO_VTABLE data_source : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<data_source, &CLSID_XMLAData>,
	public IDBCreateSessionImpl<data_source, session>,
	public IDBInitializeImpl<data_source>,
	public IDBPropertiesImpl<data_source>,
	public IPersistImpl<data_source>,
	public IInternalConnectionImpl<data_source>,
	public IServiceProviderImpl<data_source>,
	public ISpecifyPropertyPagesImpl<data_source>,
	public IGetSelf
{
public:
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return FInit();
	}
	
	void FinalRelease() 
	{
	}

	STDMETHOD(CreateSession)(
		_In_opt_ IUnknown *pUnkOuter,
		_In_ REFIID riid,
		_Deref_out_ IUnknown **ppDBSession)
	{
		return IDBCreateSessionImpl<data_source, session>::CreateSession(pUnkOuter, riid, ppDBSession);
	}


	STDMETHODIMP GetProperties (
	   ULONG               cPropertyIDSets,
	   const DBPROPIDSET   rgPropertyIDSets[],
	   ULONG              *pcPropertySets,
	   DBPROPSET         **prgPropertySets)
	{
		return IDBPropertiesImpl<data_source>::GetProperties( cPropertyIDSets, rgPropertyIDSets, pcPropertySets, prgPropertySets);
	}

	STDMETHODIMP GetPropertyInfo(
	   ULONG               cPropertyIDSets,
	   const DBPROPIDSET   rgPropertyIDSets[],
	   ULONG              *pcPropertyInfoSets,
	   DBPROPINFOSET     **prgPropertyInfoSets,
	   OLECHAR           **ppDescBuffer)
	{
		return IDBPropertiesImpl<data_source>::GetPropertyInfo( cPropertyIDSets, rgPropertyIDSets, pcPropertyInfoSets, prgPropertyInfoSets, ppDescBuffer);
	}

	STDMETHODIMP SetProperties (
	   ULONG       cPropertySets,
	   DBPROPSET   rgPropertySets[])
	{
		return IDBPropertiesImpl<data_source>::SetProperties( cPropertySets, rgPropertySets );
	}
	
	//IDBPropertiesImpl specializations

	HRESULT OnPropertyChanged( ULONG  iCurSet , DBPROP* pDBProp )
	{
		switch ( pDBProp->dwPropertyID )
		{
		case DBPROP_CURRENTCATALOG:
		case DBPROP_INIT_CATALOG:
			{
				ISessionProperties*						pProps = NULL;
				DBPROPSET								propSets[1];

				propSets[0].cProperties				= 1;
				propSets[0].guidPropertySet			= DBPROPSET_SESSION;
				propSets[0].rgProperties			= pDBProp;
				
				pDBProp->dwPropertyID = DBPROP_CURRENTCATALOG;

				for ( session::session_table_type::const_iterator i = session::session_table().begin(), e = session::session_table().end(); i != e; ++i )
				{
					i->first->QueryInterface( __uuidof( ISessionProperties ), (void**)&pProps );
					pProps->SetProperties( 1, propSets );
					pProps->Release();
				}				
			}
			break;
		default:
			break;
		}
		return IDBPropertiesImpl<data_source>::OnPropertyChanged( iCurSet, pDBProp );
	}

private:

	HWND m_parent_window_handle;
	TCHAR m_user[256];
	TCHAR m_pass[256];

private:

	HRESULT DataLinkInitialize(void)
	{
		HRESULT					hr						= S_OK;
		IDBPromptInitialize*	pIDBPromptInitialize	= NULL;
		IDBInitialize*			pIDBInitialize			= NULL;

		
		if FAILED(
					hr = QueryInterface(IID_IDBInitialize, (void**)&pIDBInitialize)
					) goto CLEANUP;


		if FAILED( 
					hr = CoCreateInstance(
										CLSID_DataLinks, 
										NULL, 
										CLSCTX_ALL,
										IID_IDBPromptInitialize, 
										(void**)&pIDBPromptInitialize)
					) goto CLEANUP;


		if FAILED( 
					hr = pIDBPromptInitialize->PromptDataSource(
										NULL, 
										NULL, 
										DBPROMPTOPTIONS_PROPERTYSHEET + DBPROMPTOPTIONS_DISABLE_PROVIDER_SELECTION, 
										0, 
										NULL, 
										NULL, 
										IID_IDBInitialize, 
										(IUnknown**)&pIDBInitialize)
					) goto CLEANUP;
CLEANUP:
		if ( NULL != pIDBPromptInitialize )
		{
			pIDBPromptInitialize->Release();
		}

		if ( NULL != pIDBInitialize )
		{
			pIDBInitialize->Release();
		}
		return hr;
	}

	HRESULT PromptInitialize(void)
	{
		pass_prompt_ui login_prompt;
		login_prompt.m_user = m_user;
		login_prompt.m_pass = m_pass;
		if ( IDOK == login_prompt.DoModal( m_parent_window_handle ) ) {
			_tcscpy_s( m_user, 256, login_prompt.m_user );
			_tcscpy_s( m_pass, 256, login_prompt.m_pass );

			DBPROPSET   propset;
			DBPROP		props[2];
			_variant_t usr(login_prompt.m_user);
			_variant_t auth(login_prompt.m_pass);

			ZeroMemory( props, 2 * sizeof( DBPROP ) );
			propset.guidPropertySet = DBPROPSET_DBINIT;
			propset.cProperties = 2;
			propset.rgProperties = props;
			
			props[0].dwPropertyID = DBPROP_AUTH_USERID;		
			props[0].vValue = usr;
			props[1].dwPropertyID = DBPROP_AUTH_PASSWORD;
			props[1].vValue = auth;
			return SetProperties( 1, &propset );
		}

		return S_OK;
	}

	void read_props( std::string& location, std::string& catalog, ULONG propCount, DBPROPSET* pProperties )
	{
		for ( ULONG i = 0; i < propCount; i++ )
		{
			if ( IsEqualGUID( pProperties[i].guidPropertySet,DBPROPSET_DBINIT ) )
			{
				for ( ULONG j =0; j < pProperties[i].cProperties; j++ )
				{
					DBPROP  crtProp = pProperties[i].rgProperties[j];
					switch ( crtProp.dwPropertyID )
					{
					case DBPROP_INIT_LOCATION:
						location = CT2A(crtProp.vValue.bstrVal, CP_UTF8);
						break;
					case DBPROP_INIT_CATALOG:
						if ( crtProp.vValue.bstrVal )  catalog = CT2A(crtProp.vValue.bstrVal, CP_UTF8);
						break;
					case DBPROP_AUTH_USERID:
						_tcscpy_s( m_user, 256, crtProp.vValue.bstrVal );
						break;
					case DBPROP_AUTH_PASSWORD:
						_tcscpy_s( m_pass, 256, crtProp.vValue.bstrVal );
						break;
					case DBPROP_INIT_HWND:
#ifdef _WIN64 
						//(HWND)crtProp.vValue.llVal; 64 bit does not set the handle right.
						//Sometimes I see here uninitialized data in the high order long.
						//I quess the trick below will handle :) it as the documentation hints it is only the low order 32 bits that matter
						m_parent_window_handle = IsWindow( (HWND)crtProp.vValue.llVal ) ? (HWND)crtProp.vValue.llVal : (HWND)( (LONG)crtProp.vValue.llVal );
#else
						m_parent_window_handle = (HWND)crtProp.vValue.lVal;
#endif

						break;
					}
				}
			}
		}
	}
public:

	//IDBInitializeImpl specializations
	STDMETHOD(Initialize)(void)
	{
		HRESULT					hr;
		ULONG					propCount;
		DBPROPSET*				pProperties;
		unsigned char			assets = 0;
		std::string				location;
		std::string				catalog;
		
		if ( FAILED( hr = GetProperties( 0, NULL, &propCount, &pProperties ) ) ) return hr;

		read_props( location, catalog, propCount, pProperties );

		if ( location.empty() || catalog.empty() )
		{
			if FAILED( hr = DataLinkInitialize() ) 
			{ 
				return hr; 
			}
			else
			{
				read_props( location, catalog, propCount, pProperties );
			}
		} 
		
		
		for (  int i = 0; i < 3; ++i ){
			connection_handler handler( location, std::string(CT2A( m_user, CP_UTF8 )), std::string(CT2A( m_pass, CP_UTF8 )), catalog );
			if ( S_OK != handler.execute("") && !handler.valid_credentials() ) {
				PromptInitialize();
			} else {
				break;
			}
		}

		return IDBInitializeImpl<data_source>::Initialize();
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

DECLARE_REGISTRY_RESOURCEID(IDR_XMLADATA)
BEGIN_COM_MAP(data_source)
	COM_INTERFACE_ENTRY(IDBCreateSession)
	COM_INTERFACE_ENTRY(IDBInitialize)
	COM_INTERFACE_ENTRY(IDBProperties)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IInternalConnection)
	COM_INTERFACE_ENTRY(IServiceProvider)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IGetSelf)
END_COM_MAP()

BEGIN_PROP_MAP(data_source)
    PROP_PAGE(CLSID_XMLAConnectionDlg)
    PROP_PAGE(CLSID_XMLAAdvancedDlg)
END_PROP_MAP()

BEGIN_PROPSET_MAP(data_source)
	BEGIN_PROPERTY_SET(DBPROPSET_DATASOURCEINFO)
		PROPERTY_INFO_ENTRY( ACTIVESESSIONS )
		PROPERTY_INFO_ENTRY( DATASOURCEREADONLY )
		PROPERTY_INFO_ENTRY( BYREFACCESSORS )
		PROPERTY_INFO_ENTRY( OUTPUTPARAMETERAVAILABILITY )
		PROPERTY_INFO_ENTRY( PROVIDEROLEDBVER )
		PROPERTY_INFO_ENTRY_VALUE( DSOTHREADMODEL, DBPROPVAL_RT_FREETHREAD )
		PROPERTY_INFO_ENTRY( SUPPORTEDTXNISOLEVELS )
		PROPERTY_INFO_ENTRY( USERNAME )

//OLAP
		MDPROPERTY_INFO_ENTRY_VALUE( MDX_FORMULAS, MDPROPVAL_MF_SCOPE_SESSION | MDPROPVAL_MF_CREATE_CALCMEMBERS | MDPROPVAL_MF_CREATE_NAMEDSETS | MDPROPVAL_MF_WITH_CALCMEMBERS | MDPROPVAL_MF_WITH_NAMEDSETS )
		MDPROPERTY_INFO_ENTRY_VALUE( FLATTENING_SUPPORT, MDPROPVAL_FS_FULL_SUPPORT )
		MDPROPERTY_INFO_ENTRY_VALUE( NAMED_LEVELS, MDPROPVAL_NL_NAMEDLEVELS )
		MDPROPERTY_INFO_ENTRY( MDX_SET_FUNCTIONS )
		MDPROPERTY_INFO_ENTRY_VALUE( AXES, 7 )
		MDPROPERTY_INFO_ENTRY_VALUE( MDX_OBJQUALIFICATION, MDPROPVAL_MOQ_DIMHIER_LEVEL | MDPROP_NAMED_LEVELS | MDPROPVAL_MOQ_DIMHIER_MEMBER )
		//MDPROPERTY_INFO_ENTRY_VALUE( MDX_SUBQUERIES, 0x03)
		PROPERTY_INFO_ENTRY( DBMSVER )
		PROPERTY_INFO_ENTRY_EX( DATASOURCE_TYPE, VT_I4, DBPROPFLAGS_DATASOURCEINFO | DBPROPFLAGS_READ, DBPROPVAL_DST_MDP, 0 )
		PROPERTY_INFO_ENTRY( ROWSET_ASYNCH )

//Identification
		PROPERTY_INFO_ENTRY_VALUE( PROVIDERNAME, TEXT("XMLAProvider.dll") )
		PROPERTY_INFO_ENTRY_VALUE( PROVIDERFRIENDLYNAME, TEXT("XMLA Data Source") )
		PROPERTY_INFO_ENTRY_VALUE( PROVIDERVER, TEXT("01.00.0000") )
		PROPERTY_INFO_ENTRY_VALUE( DBMSVER, TEXT("01.00.0000") )

	END_PROPERTY_SET(DBPROPSET_DATASOURCEINFO)
	
	BEGIN_PROPERTY_SET(DBPROPSET_DBINIT)
		PROPERTY_INFO_ENTRY( AUTH_PASSWORD )
		PROPERTY_INFO_ENTRY( AUTH_USERID )
		PROPERTY_INFO_ENTRY( INIT_DATASOURCE )
		PROPERTY_INFO_ENTRY( INIT_HWND )
		PROPERTY_INFO_ENTRY( INIT_LOCATION )
		PROPERTY_INFO_ENTRY( INIT_CATALOG )		
		PROPERTY_INFO_ENTRY_VALUE( DBMSNAME, TEXT("XMLA") )

		PROPERTY_INFO_ENTRY( INIT_LCID )
		PROPERTY_INFO_ENTRY( INIT_MODE )
		PROPERTY_INFO_ENTRY( INIT_TIMEOUT )
		PROPERTY_INFO_ENTRY( INIT_PROMPT )
		PROPERTY_INFO_ENTRY( AUTH_PERSIST_SENSITIVE_AUTHINFO )
		PROPERTY_INFO_ENTRY( INIT_PROVIDERSTRING )
		PROPERTY_INFO_ENTRY_VALUE( INIT_OLEDBSERVICES, DBPROPVAL_OS_DISABLEALL )
	END_PROPERTY_SET(DBPROPSET_DBINIT)

	BEGIN_PROPERTY_SET( DBPROPSET_DATASOURCE )
	PROPERTY_INFO_ENTRY( CURRENTCATALOG )
	END_PROPERTY_SET( DBPROPSET_DATASOURCE )

	CHAIN_PROPERTY_SET(session)
	CHAIN_PROPERTY_SET(command)
END_PROPSET_MAP()

BEGIN_SERVICE_MAP(data_source)
   SERVICE_ENTRY( OLEDB_SVC_DSLPropertyPages ) 
END_SERVICE_MAP()
};

OBJECT_ENTRY_AUTO(__uuidof(XMLAData), data_source)
