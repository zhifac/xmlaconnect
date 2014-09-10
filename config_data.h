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


#define ARQUERY_KEY_PATH "Software\\Arquery\\ODBO"



class config_data
{
public:
	config_data() 
	{
		HKEY key;
		if ( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER, TEXT( ARQUERY_KEY_PATH ), 0,  KEY_SET_VALUE + KEY_READ, &key ) )
		{
			DWORD	len = STR_LEN;
			TCHAR	location[STR_LEN];
			DWORD	skip_host_check = 0;
			ZeroMemory( location, STR_LEN * sizeof( TCHAR ) );
			RegGetValue( key, nullptr, TEXT("location"), RRF_RT_REG_SZ, nullptr, location, &len );
			m_location = location;
			len = sizeof(skip_host_check);
			DWORD err = RegGetValue( key, nullptr, TEXT("SOAP_SSL_SKIP_HOST_CHECK"), RRF_RT_REG_DWORD, nullptr, &skip_host_check, &len );
			m_skip_ssl_host_check = skip_host_check != 0;
			
			CloseHandle( key );
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
private:
	static bool m_skip_ssl_host_check;
	static std::wstring m_location;
};