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
					a row in a value possible rowset of valuea
	@observation
					this is declared for compatibility. ODBO providers
					access data through different interfaces (IMDDataset for example)
*/

#pragma once

//no instance of this class should be created in this project
class row_data
{
public:
	static ATL::ATLCOLUMNINFO* GetColumnInfo( void* pv, DBORDINAL* pcInfo );

	static const size_t CELL_ORDINAL_POS = 1;

};