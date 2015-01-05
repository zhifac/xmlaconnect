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
					main module instantiation
*/

#include "stdafx.h"
#include "config_data.h"

bool config_data::m_skip_ssl_host_check = false;
std::wstring config_data::m_location = TEXT("");
bool config_data::m_use_proxy = false;
std::string config_data::m_bypass;
std::string config_data::m_proxy;
int config_data::m_proxy_port;
std::string config_data::m_proxy_ssl;
int config_data::m_proxy_port_ssl;
bool config_data::m_do_init = true;
CSimpleScriptSite* config_data::m_auto_proxy_handler = nullptr;
