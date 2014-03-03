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
					main module declaration
*/

class XMLA_provider_module : public ATL::CAtlDllModuleT< XMLA_provider_module >
{
public :
	DECLARE_LIBID(LIBID_XMLAProviderLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_XMLAPROVIDER, "{A4355FD6-8F4C-4721-B34D-5C10FFF0EFF4}")
};

extern class XMLA_provider_module _AtlModule;
