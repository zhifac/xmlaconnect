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
					interprets proxy configuration scripts
*/

#include "stdafx.h"
#include "pac_interpreter.h"
#include "config_data.h"
#include "atlhttp.h"

wchar_t* CSimpleScriptSite::get_helper_script()
{
	std::string script_content;
	std::string file_name(config_data::get_module_path());
	file_name.append("pac_functions.js");

	std::ifstream pac_file(file_name);
	std::string line;
	while ( std::getline(pac_file, line, '\n') )
	{
		script_content.append( line );
		script_content.append( "\n" );
	}
	if ( script_content.empty() ) { return nullptr; }
	wchar_t* script_data = new wchar_t[script_content.size()+1];
	wcscpy_s( script_data, script_content.size()+1,  ATL::CA2W( (char*) script_content.c_str() ) );
	return script_data;
}

wchar_t* CSimpleScriptSite::get_script( LPCTSTR address )
{
	ATL::CAtlHttpClient http_client;
	ATL::CAtlNavigateData navData;
	navData.SetSocketTimeout( 6000 );
	navData.SetMethod( ATL_HTTP_METHOD_GET );
	http_client.Navigate( address, &navData );
	const BYTE* response = http_client.GetBody();
	const DWORD len =http_client.GetBodyLength();
	if ( len < 3 ) {return nullptr;}//too short
		
	wchar_t* script_data = new wchar_t[len+1];

	if ( (0xef == response[0]) && (0xbb == response[1]) && (0xbf == response[2]) )
	{
		wcscpy_s( script_data, len+1,  ATL::CA2W( (char*) response+3, CP_UTF8 ) );
	} else
	{
		wcscpy_s( script_data, len+1,  ATL::CA2W( (char*) response ) );
	}

	return script_data;
}
