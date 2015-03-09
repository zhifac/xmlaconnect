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
					maintains subtle differences from server to server.
*/

#pragma once

#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>
#include "config_data.h"
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
		alias_map_type	dim_props_map;
		alias_map_type	regex_map;

		server_specifics() : caps(CAPS_ANY) {}

		server_specifics( const server_specifics& other )
		{
			if ( this == &other ) { return; }
			caps = other.caps;
			alias_map = other.alias_map;
			dim_props_map = other.dim_props_map;
			regex_map = other.regex_map;
		}

		server_specifics( session::session_data::server_type server )
			 : caps(CAPS_ANY) 
		{
			std::string file_name(config_data::get_module_path());

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

			if ( !regex_map.empty() ) {
				for ( alias_map_type::const_iterator i = regex_map.begin(), e = regex_map.end(); i != e; ++i ) {
					std::stringstream buff;
					buff<<std::regex_replace (what,std::regex(i->first),i->second);
					what = buff.str();
				}
				
			}

			if ( alias_map.empty() && dim_props_map.empty()) { return what; }
			for ( alias_map_type::const_iterator i = alias_map.begin(), e = alias_map.end(); i != e; ++i )
			{
				size_t start_pos = 0;
				while((start_pos = what.find(i->first, start_pos)) != std::string::npos) {
					what.replace(start_pos, i->first.length(), i->second);
					start_pos += i->second.length(); // ...
				}
			}
			//just for properties
			for ( alias_map_type::const_iterator i = dim_props_map.begin(), e = dim_props_map.end(); i != e; ++i )
			{
				size_t start_pos = 0;
				while((start_pos = what.find(i->first, start_pos)) != std::string::npos) {
					what.replace(start_pos, i->first.length(), i->second);
					start_pos += i->second.length(); // ...
				}
			}

			if ( substitutions.empty() ) { return what; }
			for ( substitution_vector::const_iterator i = substitutions.begin(), e = substitutions.end(); i != e; ++i )
			{
				while ( i->evaluate_once( what ) ){}
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
					} else if ( "SUBST" == key )
					{
						substitution subst;
						if ( subst.load( val ) )
						{
							substitutions.push_back( subst );
						}
					} else if ("REGEX" == key ) 
					{
						load_regex( val ); 
					}
				}
			}
		}

		void load_regex( const std::string& val )
		{
			std::string::size_type pos = val.find("==");
			if ( std::string::npos == pos ) { return; }

			std::string key = val.substr( 0, pos );
			const std::string subst = val.substr( pos + 2, val.size() ); 

			regex_map[ key ] = subst;
		}

		void load_alias( const std::string& val )
		{
			std::string::size_type pos = val.find("=");
			if ( std::string::npos == pos ) { return; }

			std::string key = val.substr( 0, pos );
			const std::string subst = val.substr( pos + 1, val.size() ); 

			while ( std::string::npos != (pos = key.find("0x") ) )
			{
				const char char_val = (char) strtol( key.substr( pos+2, 2 ).c_str(), nullptr, 16 );
				key.erase( pos+1, 3 );
				key[pos] = char_val;
			}

			alias_map[ key ] = subst;
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
	public:
		void load_prop_alias( std::string& key, std::string& subst ) 
		{
			dim_props_map[ key ] = subst;
		}

		void remove_prop_alias( std::vector<std::string>& aliases )
		{
			for(std::vector<std::string>::iterator i = aliases.begin(), e = aliases.end(); i != e; ++i) {
				dim_props_map.erase( *i );
			}
		}

	private:
		struct substitution
		{
			std::string function;
			std::vector<std::string> parameters;
			std::string expansion;
			mutable std::vector<std::string> actual_parameters;

			bool advance_to( std::string::size_type& what, const std::string& where, char mark ) const
			{
				bool in_lit = false;
				int parent_cnt = 0; //used for (
				int parent1_cnt = 0; //used fot {
				while( true )
				{
					if ( where.size() == what ) { return false; }
					++what;
					const char crt = where[what];
					if ( in_lit )
					{
						in_lit = ']' != crt;
						continue;
					}
					if ( '['== crt )
					{
						in_lit = true;
						continue;
					}
					if ( crt == mark && 0 == parent_cnt && 0 == parent1_cnt ) { return true; }

					switch (crt ) {
						case '(':
							++parent_cnt;
							break;
						case '{':
							++parent1_cnt;
							break;
						case ')':
							--parent_cnt;
							break;
						case'}':
							--parent1_cnt;
							break;
						default:
							break;
					}
				
					if ( parent_cnt < 0 ) { return false; } //some syntax error here
					if ( parent1_cnt < 0 ) { return false; } //some syntax error here
					//TODO: add more, like string literals and so on
				}
			}

			bool evaluate_once(std::string& what) const
			{
				std::string::size_type base_offset = 0;

				while ( true )
				{
					size_t param_pos = 0;
					std::string::size_type start;
					std::string::size_type offset;
					std::string::size_type pos = what.find(function, base_offset);
					if ( std::string::npos == pos ) { return false; }

					base_offset = pos + function.size(); 

					actual_parameters.clear();

					start = pos;
					pos+= function.size();
					offset = pos;
					++pos;
					bool have_match = true;

					while ( param_pos < parameters.size() - 1 )
					{
						if ( !advance_to( offset, what, ',' ) )
						{
							have_match = false;
							break;
						}
						actual_parameters.push_back( what.substr( pos, offset-pos ) );
						pos = offset+1;
						offset = pos;
						++param_pos;
					}

					if ( !have_match ) { continue; }

					if ( !advance_to( offset, what, ')' ) )
					{ 
						continue; 
					}
					actual_parameters.push_back( what.substr( pos, offset-pos ) );
					pos = offset+1;
					offset = pos;
					++param_pos;

					std::string crt_expansion(expansion);
					for ( size_t i = 0; i < parameters.size(); ++i )
					{
						std::string::size_type crt_pos;
						while ( std::string::npos != ( crt_pos = crt_expansion.find( parameters[i] ) ) )
						{
							crt_expansion.replace(crt_pos, parameters[i].size(), actual_parameters[i] );
						}
					}

					what.replace( start, pos - start, crt_expansion );

					return true;
				}
			}

			bool load( const std::string& val )
			{
				parameters.clear();
				std::string::size_type pos = val.find("=");
				if ( std::string::npos == pos ) { return false; }

				const std::string left = val.substr( 0, pos );
				const std::string right = val.substr( pos + 1, val.size() );

				pos = left.find("(");
				if ( std::string::npos == pos ) { return false; }

				function = left.substr( 0, pos );

				std::string::size_type offset = pos+1;
				while ( std::string::npos != ( pos = left.find( "%", offset ) ) )
				{
					offset = pos + 1;
					while ( offset < left.size() && ','!=left[offset] && ')'!=left[offset] ){ ++offset; }
					if ( offset == left.size() ) { return false; }
					parameters.push_back( left.substr( pos, offset - pos ) );
				}
				expansion = right;
				return true;
			}
		};
		typedef std::vector<substitution> substitution_vector;
		substitution_vector substitutions;
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

	void release( session::session_data::server_type server )
	{
		m_translation_table.erase( server );
	}
private:
	query_translator() {}

	std::string		m_cube_key;
	
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
	
	void load_alias( std::string& key, std::string& subst, session::session_data::server_type server ) 
	{
		const server_specifics& translator = get( server );
		(const_cast<server_specifics&>(translator)).load_prop_alias( key, subst);
	}

	void clear_alias( session::session_data::server_type server )
	{
		const server_specifics& translator = get( server );
		//(const_cast<server_specifics&>(translator))
	}

	void setKey( std::string& key ) {
		m_cube_key = key;
	}

	bool same_cube( std::string& key ) {
		return 0 == key.compare( m_cube_key);
	}
};