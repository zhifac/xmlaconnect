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
					interprets proxy configuration scripts
*/
#pragma once


#include <activscp.h>
#pragma warning (push)
#pragma warning (disable: 4995)
#pragma warning (pop)

#include <fstream>
#include <deque>
#include <Windns.h>

class language_upgrade : public IDispatch
{
private:
	static const int GET_IP_DISP_ID = 1;
	static const int RESOLVE_HOST_DISP_ID = 2;
	static const int IS_RESOLVABLE_DISP_ID = 3;
public:
	// IUnknown
	STDMETHOD_(ULONG, AddRef)(){ return 1; }//not quite a COM object. This is maintained here. Does not live and die by IUnknown ref counting.
	STDMETHOD_(ULONG, Release)() { return 1; }//not quite a COM object. This is maintained here. Does not live and die by IUnknown ref counting.
	STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject)
	{
		if (riid == IID_IUnknown || riid == IID_IDispatch )
		{
			*ppvObject = this;
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	//IDispatch
	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
	{
		HRESULT hr = S_OK;
		for ( UINT i = 0; i < cNames; ++i )
		{
			if ( 0 == wcscmp( L"dnsResolve", rgszNames[i] ) )
			{
				rgDispId[i] = RESOLVE_HOST_DISP_ID;
			} else if ( 0 == wcscmp( L"myIpAddress", rgszNames[i] ) ) {
				rgDispId[i] = GET_IP_DISP_ID;
			} else if ( 0 == wcscmp( L"isResolvable", rgszNames[i] ) ) {
				rgDispId[i] = IS_RESOLVABLE_DISP_ID;
			} else {
				rgDispId[i] = DISPID_UNKNOWN;
				hr = DISP_E_UNKNOWNNAME;
			}
		}
		return hr;
	}

	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
	{
		return E_NOTIMPL;
	}

	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo)
	{
		if ( nullptr == pctinfo ) {
			return E_INVALIDARG;
		}
		*pctinfo = 0;
		return S_OK;
	}

	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid,  LCID lcid, WORD wFlags, DISPPARAMS *pDispParams,  VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr )
	{
		switch( dispIdMember )
		{
		case RESOLVE_HOST_DISP_ID:
			if ( nullptr == pDispParams ) { return E_INVALIDARG; }
			if ( 1 != pDispParams->cArgs ) { return E_INVALIDARG; }
			if ( pDispParams[0].rgvarg[0].vt != VT_BSTR ){ return E_INVALIDARG; }
			if ( nullptr == pVarResult )  { return E_INVALIDARG; }
			{
				PDNS_RECORD dns_data = nullptr;			
				DNS_STATUS result = DnsQuery( pDispParams[0].rgvarg[0].bstrVal, DNS_TYPE_A, DNS_QUERY_STANDARD, nullptr, &dns_data, nullptr );
				if ( 0 == result )
				{
					struct in_addr addr;
					addr.s_addr = (long)dns_data->Data.A.IpAddress;
					std::string address_s( inet_ntoa(addr) );
					DnsRecordListFree( dns_data, DNS_FREE_TYPE );
					pVarResult->vt = VT_BSTR;
					pVarResult->bstrVal = SysAllocString( ATL::CA2W( address_s.c_str() ) );
				}
			}
			break;
		case GET_IP_DISP_ID:
			if ( nullptr == pVarResult )  { return E_INVALIDARG; }
			{
				struct hostent *phe = gethostbyname(nullptr);
				if ( nullptr == phe ) { return E_UNEXPECTED; }
				if ( nullptr == phe->h_addr_list ) { return E_UNEXPECTED; }
				struct in_addr addr;
				addr.s_addr = *(u_long *) phe->h_addr_list[0];
				std::string address_s( inet_ntoa(addr) );
				pVarResult->vt = VT_BSTR;
				pVarResult->bstrVal = SysAllocString( ATL::CA2W( address_s.c_str() ) );
			}
			break;
		case IS_RESOLVABLE_DISP_ID:
			if ( nullptr == pVarResult )  { return E_INVALIDARG; }
			{
				PDNS_RECORD dns_data = nullptr;			
				DNS_STATUS result = DnsQuery( pDispParams[0].rgvarg[0].bstrVal, DNS_TYPE_A, DNS_QUERY_STANDARD, nullptr, &dns_data, nullptr );
				pVarResult->vt = VT_BOOL;
				pVarResult->boolVal = 0 == result;
				if ( 0 == result )
				{
					DnsRecordListFree( dns_data, DNS_FREE_TYPE );
				}
			}
			break;
		default:
			return E_INVALIDARG;
		}
		return S_OK;
	}
};

class CSimpleScriptSite :
    public IActiveScriptSite,
    public IActiveScriptSiteWindow
{
private:

	language_upgrade m_lang;
private:
	typedef std::pair<std::wstring, std::wstring> url_cache_entry_type;
	typedef std::deque< url_cache_entry_type > url_cache_type;

	static const size_t URL_CACHE_LIMIT = 10;
private:
	wchar_t* get_helper_script();
	wchar_t* get_script( LPCTSTR address );
public:
    CSimpleScriptSite( LPCTSTR address ) 
		: m_cRefCount(1)
		, m_hWnd(NULL) 
	{ 
		m_script = get_script( address );
		m_helper_script = get_helper_script();
		prepare_script();
	}
	
	~CSimpleScriptSite()
	{
		if ( nullptr != m_script ) { delete[] m_script; }
		if ( nullptr != m_helper_script ) { delete[] m_helper_script; }		
		m_jscript = nullptr;
		m_jscript_parse = nullptr;
	}

    // IUnknown

    STDMETHOD_(ULONG, AddRef)()
	{
		return InterlockedIncrement(&m_cRefCount);
	}
    STDMETHOD_(ULONG, Release)()
	{
		if (!InterlockedDecrement(&m_cRefCount))
		{
			delete this;
			return 0;
		}
		return m_cRefCount;
	}
    STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject)
	{
		if (riid == IID_IUnknown || riid == IID_IActiveScriptSiteWindow)
		{
			*ppvObject = (IActiveScriptSiteWindow *) this;
			AddRef();
			return NOERROR;
		}
		if (riid == IID_IActiveScriptSite)
		{
			*ppvObject = (IActiveScriptSite *) this;
			AddRef();
			return NOERROR;
		}
		return E_NOINTERFACE;
	}

    // IActiveScriptSite

    STDMETHOD(GetLCID)(LCID *plcid){ *plcid = 0; return S_OK; }
    STDMETHODIMP GetItemInfo(LPCOLESTR pstrName, DWORD dwReturnMask, IUnknown **ppiunkItem, ITypeInfo **ppti) 
	{ 
		if ( 0 != wcscmp( L"execution_helper", pstrName ) ) { return TYPE_E_ELEMENTNOTFOUND; }
		if ( SCRIPTINFO_IUNKNOWN != dwReturnMask ) { return E_INVALIDARG; }
		*ppiunkItem = (IUnknown*)(&m_lang);
		return S_OK;
	}
    STDMETHOD(GetDocVersionString)(BSTR *pbstrVersion) { *pbstrVersion = SysAllocString(L"1.0"); return S_OK; }
    STDMETHOD(OnScriptTerminate)(const VARIANT *pvarResult, const EXCEPINFO *pexcepinfo) { return S_OK; }
    STDMETHOD(OnStateChange)(SCRIPTSTATE ssScriptState) { return S_OK; }
    STDMETHOD(OnScriptError)(IActiveScriptError *pIActiveScriptError) { return S_OK; }
    STDMETHOD(OnEnterScript)(void) { return S_OK; }
    STDMETHOD(OnLeaveScript)(void) { return S_OK; }

    // IActiveScriptSiteWindow

    STDMETHOD(GetWindow)(HWND *phWnd) { *phWnd = m_hWnd; return S_OK; }
    STDMETHOD(EnableModeless)(BOOL fEnable) { return S_OK; }

    // Miscellaneous

    HRESULT SetWindow(HWND hWnd) { m_hWnd = hWnd; }

	void prepare_script()
	{
		HRESULT hr;
		hr = m_jscript.CoCreateInstance(OLESTR("JScript"));
		hr = m_jscript->SetScriptSite(this);
		hr = m_jscript->QueryInterface(&m_jscript_parse);
		hr = m_jscript_parse->InitNew();
		hr = m_jscript->AddNamedItem(L"execution_helper", SCRIPTITEM_GLOBALMEMBERS + SCRIPTITEM_ISVISIBLE);

		EXCEPINFO ei = { };
		
		if ( nullptr != m_helper_script )
		{
			hr = m_jscript_parse->ParseScriptText(m_helper_script, NULL, NULL, NULL, 0, 0, SCRIPTTEXT_ISEXPRESSION, &m_execution_result, &ei);
		}
		if ( nullptr != m_script )
		{
			hr = m_jscript_parse->ParseScriptText(m_script, NULL, NULL, NULL, 0, 0, SCRIPTTEXT_ISEXPRESSION, &m_execution_result, &ei);
		}
	}

	const wchar_t* get_proxy_for( const wchar_t* url )
	{
		url_cache_type::const_iterator match = std::find_if( m_url_cache.begin(), m_url_cache.end(), [&](url_cache_entry_type& param ){ return param.first == url;} );
		if ( m_url_cache.end() == match ) 
		{
			if ( URL_CACHE_LIMIT == m_url_cache.size() ) { m_url_cache.pop_front(); }
			
			m_url_cache.push_back( std::make_pair( nullptr == url ? L"" : url,  interpret_proxy_script(url) ) );
		}

		return std::find_if( m_url_cache.begin(), m_url_cache.end(), [&](url_cache_entry_type& param ){ return param.first == url;} )->second.c_str();
	}

private:

	std::wstring interpret_proxy_script( const wchar_t* url )
	{
		std::wstring local_url(url);
		std::wstring query(L"getProxy(\"");
		query.append( url );
		query.append(L"\")");
		EXCEPINFO ei = { };
		HRESULT hr = m_jscript_parse->ParseScriptText(query.c_str(), NULL, NULL, NULL, 0, 0, SCRIPTTEXT_ISEXPRESSION, &m_execution_result, &ei);
		
		if FAILED( hr ) { return L""; }

		//only use the first. no fallback
		std::wstring proxy_candidate(m_execution_result.bstrVal);		
		std::size_t divider = proxy_candidate.find_first_of(';');
		if ( std::string::npos != divider ) 
		{ 
			proxy_candidate = proxy_candidate.substr( 0, divider ); 
		}

		divider = proxy_candidate.find( L"PROXY " );
		if ( std::string::npos == divider ) 
		{
			//anything else than PROXY is DIRECT
			return L"";
		}

		proxy_candidate = proxy_candidate.substr( divider + wcslen( L"PROXY " ), proxy_candidate.length() );
		std::transform(local_url.begin(), local_url.end(),local_url.begin(), ::toupper);	
		return proxy_candidate;
	}

public:
    LONG                m_cRefCount;
    HWND                m_hWnd;
private:
	wchar_t*			m_script;
	wchar_t*			m_helper_script;
	ATL::CComPtr<IActiveScript>		m_jscript;
	ATL::CComPtr<IActiveScriptParse> m_jscript_parse;
	ATL::CComVariant m_execution_result;

	url_cache_type m_url_cache;
};