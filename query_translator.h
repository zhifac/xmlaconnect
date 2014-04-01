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
					maintains subtle differences from server to server.
*/

#pragma once

#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <cctype>

class query_translator
{
private:
	struct server_specifics
	{
		enum caps_type
		{
			CAPS_ANY = 0,		//this is the default (no transformation on the string)
			CAPS_UPPER = 1,
			CAPS_LOWER = 2
		};
		typedef std::unordered_map< std::string, std::string > alias_map_type;

		caps_type		caps;
		alias_map_type	alias_map;

		server_specifics() : caps(CAPS_ANY) {}

		server_specifics( const server_specifics& other )
		{
			if ( this == &other ) { return; }
			caps = other.caps;
			alias_map = other.alias_map;
		}

		server_specifics( session::session_data::server_type server )
			 : caps(CAPS_ANY) 
		{
			std::string file_name(query_translator::get_module_path());

			switch ( server )
			{
			case session::session_data::ORACLE:
				//ORACLE is the norm. we have full control on the server and adjust there. so, leave it empty
				break;
			case session::session_data::MONDRIAN:
				file_name.append("mondrian_dialect.txt");
				load_from_file(file_name.c_str());
				break;
			case session::session_data::JEDOX:
				file_name.append("jedox_dialect.txt");
				load_from_file(file_name.c_str());
				break;
			case session::session_data::UNDEFINED:
				//anything else still get a chance to adjust here
				file_name.append("undefined_dialect.txt");
				load_from_file(file_name.c_str());
				break;
			}
		}

		std::string& translate( std::string& what ) const
		{
			if ( caps == CAPS_UPPER )
			{
				std::transform( what.begin(), what.end(), what.begin(), std::toupper );
			} else if ( caps == CAPS_LOWER )
			{
				std::transform( what.begin(), what.end(), what.begin(), std::tolower );
			}
			if ( alias_map.empty() ) { return what; }
			for ( alias_map_type::const_iterator i = alias_map.begin(), e = alias_map.end(); i != e; ++i )
			{
				size_t start_pos = 0;
				while((start_pos = what.find(i->first, start_pos)) != std::string::npos) {
					what.replace(start_pos, i->first.length(), i->second);
					start_pos += i->second.length(); // ...
				}
			}
			return what;
		}

	private:
		void load_from_file( const char* file_name )
		{
			std::ifstream cfg_file(file_name);
			std::string line;
			while ( std::getline(cfg_file, line, '\n') )
			{				
				if ( line.size() > 0 && line[0]=='#' ) { continue; }
				std::string::size_type pos = line.find(" ");
				if ( std::string::npos != pos ) {
					std::string key = line.substr( 0, pos );
					std::string val = line.substr( pos + 1, line.size() );
					std::transform( key.begin(), key.end(), key.begin(), std::toupper );
					if ( "CASE" == key ) {
						load_caps( val );
					} else if ("ALIAS" == key )
					{
						load_alias( val );
					}
				}
			}
		}

		void load_alias( const std::string& val )
		{
			std::string::size_type pos = val.find("=");
			if ( std::string::npos == pos ) { return; }
			alias_map[ val.substr( 0, pos ) ] = val.substr( pos + 1, val.size() );
		}

		void load_caps( std::string& val )
		{
			std::transform( val.begin(), val.end(), val.begin(), std::toupper );
			if ( "UPPER" == val )
			{
				caps = CAPS_UPPER;
			} else if ( "LOWER" == val) 
			{
				caps = CAPS_LOWER;
			}
		}
	};

	typedef std::unordered_map< session::session_data::server_type, server_specifics > translation_table_type;
	translation_table_type m_translation_table;
	const server_specifics& get( session::session_data::server_type server )
	{
		translation_table_type::const_iterator match = m_translation_table.find( server );
		if ( m_translation_table.end() == match ) 
		{
			m_translation_table[ server ] = server_specifics( server );
			match = m_translation_table.find( server );
		}
		
		return match->second;
	}
private:
	query_translator()
	{
		char * path = get_module_path();
		GetModuleFileNameA( _AtlBaseModule.GetModuleInstance(), path, MAX_PATH );
		path+= strlen(path);
		while( *path != '\\' ) { *path-- = 0; }
	}

	static char* get_module_path()
	{
		static char MODULE_PATH[MAX_PATH];
		return MODULE_PATH;
	}
public:
	static query_translator& translator()
	{
		static query_translator instance;
		return instance;
	}

	const std::string& translate( std::string& src, session::session_data::server_type server )
	{
		const server_specifics& translator = get( server );
		return translator.translate( src );
	}
};
