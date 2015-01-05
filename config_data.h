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
					uniform access to configuration data
*/
#pragma once

#include <Wininet.h>
#include <vector>
#include <algorithm>

#include "soapXMLAConnectionProxy.h"

#include "pac_interpreter.h"

#define ARQUERY_KEY_PATH "Software\\Arquery\\ODBO"


class config_data
{
public:
	config_data() 
	{
		init_module_path();
		if ( !m_do_init ) { return; }
		m_do_init = false;
		soap_ssl_init();
		load_registry_values();
		if ( !m_use_proxy )
		{
			check_for_proxy();
		}
	}
public:
	static const bool skip_ssl_host_check() { return m_skip_ssl_host_check; }
	
	static const std::wstring& location() { return m_location; }
	static void location( const std::wstring& loc )
	{ 
		HKEY key;
		if ( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER, TEXT( ARQUERY_KEY_PATH ), 0,  KEY_SET_VALUE, &key ) )
		{
			RegSetValueEx( key, TEXT("location"), 0, REG_SZ, (BYTE*)loc.c_str(), (DWORD)_tcslen( loc.c_str() ) * sizeof(wchar_t) );
			CloseHandle( key );
		}
	}
	void static get_proxy( const char* uri, const char*& proxy, int& port )
	{
		if ( !m_use_proxy ) { return; }
		if ( nullptr != m_auto_proxy_handler )
		{
			const wchar_t* resolved_proxy = m_auto_proxy_handler->get_proxy_for( ATL::CA2W( uri ) );
			
			if ( nullptr == resolved_proxy ) { proxy=nullptr;return; }

			static std::string last_proxy_str;
			
			if ( check_port( std::string( ATL::CW2A( resolved_proxy, CP_UTF8 ) ), last_proxy_str, port ) )
			{
				proxy = last_proxy_str.c_str();
			}
			return;
		}

		std::string local_uri( uri );
		std::transform(local_uri.begin(), local_uri.end(),local_uri.begin(), ::toupper);

		//TODO: bypass

		if ( 0 == local_uri.substr( 0, 5 ).compare("HTTPS") && 0 != m_proxy_port_ssl )
		{
			proxy = m_proxy_ssl.c_str();
			port = m_proxy_port_ssl;
			return;
		}
		if ( 0 == local_uri.substr( 0, 4 ).compare("HTTP") && 0 != m_proxy_port )
		{
			proxy = m_proxy.c_str();
			port = m_proxy_port;
			return;
		}
	}

	static char* get_module_path()
	{
		static char MODULE_PATH[MAX_PATH];
		return MODULE_PATH;
	}

	static void ssl_init( soap* connection )
	{
		if ( skip_ssl_host_check() )
		{
			soap_ssl_client_context( connection, SOAP_SSL_SKIP_HOST_CHECK, nullptr, nullptr, nullptr, nullptr, nullptr );
		} else 
		{
			//std::string cacerts_file( get_module_path() );
			//cacerts_file.append("cacerts.pem");
			soap_ssl_client_context( connection, SOAP_SSL_DEFAULT, nullptr, nullptr, /*cacerts_file.c_str()*/nullptr, nullptr, nullptr );
			import_certificates( connection->ctx, TEXT("My") );
			import_certificates( connection->ctx, TEXT("CA") );
			import_certificates( connection->ctx, TEXT("ROOT") );
		}
	}
private:
	void load_registry_values()
	{
		HKEY key;
		LSTATUS open_stat = RegOpenKeyEx( HKEY_CURRENT_USER, TEXT( ARQUERY_KEY_PATH ), 0,  KEY_READ, &key );

		if ( ERROR_FILE_NOT_FOUND == open_stat )
		{
			//load defaults
			open_stat = RegCreateKeyEx( HKEY_CURRENT_USER, TEXT( ARQUERY_KEY_PATH ), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, nullptr, &key, nullptr );
			if ( ERROR_SUCCESS == open_stat )
			{
				DWORD skip = 0;
				RegSetValueEx( key, TEXT("SOAP_SSL_SKIP_HOST_CHECK"), 0, REG_DWORD, (const BYTE*) &skip, sizeof(skip) );
				m_skip_ssl_host_check = true;
				m_location.clear();
				CloseHandle( key );
			}
		} else if ( ERROR_SUCCESS == open_stat )
		{
			DWORD	len = STR_LEN;
			TCHAR	location[STR_LEN];
			DWORD	skip_host_check = 0;
			ZeroMemory( location, STR_LEN * sizeof( TCHAR ) );
			RegGetValue( key, nullptr, TEXT("location"), RRF_RT_REG_SZ, nullptr, location, &len );
			m_location = location;
			len = sizeof(skip_host_check);
			RegGetValue( key, nullptr, TEXT("SOAP_SSL_SKIP_HOST_CHECK"), RRF_RT_REG_DWORD, nullptr, &skip_host_check, &len );
			m_skip_ssl_host_check = skip_host_check != 0;
			ZeroMemory(location, STR_LEN);
			len = STR_LEN;
			if ( ERROR_SUCCESS == RegGetValue( key, nullptr, TEXT("AUTHORITATIVE_PROXY"), RRF_RT_REG_SZ, nullptr, location, &len ) )
			{
				std::string ascii_proxy( ATL::CW2A(location, CP_UTF8) );
				load_proxies( ascii_proxy );
			}			
			CloseHandle( key );
		}
	}



	static void import_certificates( SSL_CTX* ctx, LPCTSTR store )
	{
		bool should_free = false;
		HCERTSTORE  win_store = CertOpenSystemStore( NULL, store );
		if ( NULL == win_store ) { return; }

		if ( nullptr == ctx ) 
		{ 
			ctx = SSL_CTX_new(SSLv23_client_method()); 
			should_free = true;
		}

		X509_STORE* ossl_store = SSL_CTX_get_cert_store( ctx );
		PCCERT_CONTEXT certificate_context = nullptr;
		while ( true )
		{
			certificate_context = CertEnumCertificatesInStore( win_store, certificate_context );
			if ( NULL == certificate_context ) { break; }

			if ( certificate_context->dwCertEncodingType & X509_ASN_ENCODING ) 
			{ 
				const unsigned char * buf = (unsigned char*) certificate_context->pbCertEncoded;
				X509* ossl_cert = d2i_X509( nullptr, &buf, certificate_context->cbCertEncoded );
				if ( nullptr != ossl_cert )
				{
					X509_STORE_add_cert( ossl_store, ossl_cert );
				}
			}
		}
		if ( nullptr != certificate_context ) { CertFreeCertificateContext( certificate_context ); }
		if ( should_free && nullptr != ctx ) { SSL_CTX_free( ctx ); }
		CertCloseStore( win_store, 0 );
	}
	
	void check_for_proxy()
	{
		//http://support.microsoft.com/kb/226473
		INTERNET_PER_CONN_OPTION_LIST    List;
		INTERNET_PER_CONN_OPTION         Option[4];
		unsigned long                    nSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);


		Option[0].dwOption = INTERNET_PER_CONN_FLAGS;
		Option[1].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
		Option[2].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
		Option[3].dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL;

		List.dwSize = sizeof(INTERNET_PER_CONN_OPTION_LIST);
		List.pszConnection = NULL;
		List.dwOptionCount = 4;
		List.dwOptionError = 0;
		List.pOptions = Option;

		m_use_proxy = false;
		m_proxy_port_ssl = m_proxy_port = 0;
		
		if( !InternetQueryOption(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &List, &nSize) ){return;}

		if ( PROXY_TYPE_PROXY == ( Option[0].Value.dwValue & PROXY_TYPE_PROXY ) )
		{
OutputDebugString( L"Using proxy: ");
			if ( nullptr != Option[1].Value.pszValue ) { m_bypass = std::string( ATL::CW2A(Option[1].Value.pszValue, CP_UTF8) ); }

			if ( nullptr == Option[2].Value.pszValue ){ goto CLEANUP; }
OutputDebugString( Option[2].Value.pszValue);
			std::string ascii_proxy( ATL::CW2A(Option[2].Value.pszValue, CP_UTF8) );
			load_proxies( ascii_proxy );

		} else if ( PROXY_TYPE_AUTO_PROXY_URL == ( Option[0].Value.dwValue & PROXY_TYPE_AUTO_PROXY_URL ) )
		{
OutputDebugString( L"Using proxy autoconfig script.\n");
			if ( nullptr != Option[3].Value.pszValue ) 
			{
OutputDebugString( L"Script location: ");
OutputDebugString( Option[3].Value.pszValue);
				m_auto_proxy_handler = new CSimpleScriptSite( Option[3].Value.pszValue ); 
				m_use_proxy = true;
			}

		}


CLEANUP:
		if ( nullptr != Option[1].Value.pszValue ) { GlobalFree(Option[1].Value.pszValue); }
		if ( nullptr != Option[2].Value.pszValue ) { GlobalFree(Option[2].Value.pszValue); }
	}

	void load_proxies( const std::string& proxy_string )
	{
		std::vector< std::string > list;						
		break_list( proxy_string, list );

		if ( 1 == list.size() )
		{
			std::string buf( list[0] );
			std::transform(buf.begin(), buf.end(),buf.begin(), ::toupper);
			if ( 0 != buf.substr(0, 5 ).compare("HTTP=") && 0 != buf.substr(0, 6 ).compare("HTTPS=") && 0 != buf.substr(0, 4 ).compare("FTP=") && 0 != buf.substr(0, 6 ).compare("SOCKS=") )
			{
				const std::string proxy(list[0]);
				list[0] = std::string("http=").append( proxy );
				list.push_back( std::string("https=").append( proxy ) );
			}
		}
			
		for ( std::vector< std::string >::const_iterator i = list.begin(), e = list.end(); i != e; ++i )
		{
			std::size_t divider = i->find_first_of('=');
			if ( std::string::npos == divider ) { continue; }
				
			std::string key( i->substr( 0, divider ) );
			std::transform(key.begin(), key.end(),key.begin(), ::toupper);

			if ( 0 == key.compare( "HTTP" ) )
			{
				m_use_proxy = check_port(i->substr( divider+1, i->length() ), m_proxy, m_proxy_port ) || m_use_proxy;
			} else if ( 0 == key.compare( "HTTPS" ) )
			{
				m_use_proxy = check_port(i->substr( divider+1, i->length() ), m_proxy_ssl, m_proxy_port_ssl ) || m_use_proxy;
			} 
		}
	}

	void break_list( const std::string& str_list, std::vector< std::string >& list )
	{
		size_t offset = 0;
		size_t pos = 0;
		while ( std::string::npos != ( pos = str_list.find( ';', offset ) ) )
		{
			list.push_back( str_list.substr( offset, pos-offset ) );
			offset = pos + 1;
		}
		if ( offset < str_list.size() )
		{
			list.push_back( str_list.substr( offset, str_list.size() ) );
		}
	}	

	static bool check_port( const std::string& def, std::string& proxy, int& port )
	{
		std::size_t divider = def.find_first_of(':');
		if ( std::string::npos == divider ) { return false; }
		proxy = def.substr( 0, divider );
		port = atoi( def.c_str() + divider + 1 );
		return 0 != port;
	}

	void init_module_path()
	{
		char * path = get_module_path();
		GetModuleFileNameA( ATL::_AtlBaseModule.GetModuleInstance(), path, MAX_PATH );
		path+= strlen(path);
		while( *path != '\\' ) { *path-- = 0; }
	}
private:
	static bool m_skip_ssl_host_check;
	static std::wstring m_location;

	//proxy conf
	static bool m_use_proxy;
	static std::string m_bypass;
	
	static std::string m_proxy;
	static int m_proxy_port;
	static std::string m_proxy_ssl;
	static int m_proxy_port_ssl;

	static bool m_do_init;

	static CSimpleScriptSite* m_auto_proxy_handler;
};