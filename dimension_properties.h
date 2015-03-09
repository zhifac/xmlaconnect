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
					dimension properties fix for excel compatibility
*/

#pragma once

class dim_properties 
{
public:
	typedef std::pair<std::string,std::string>						sgPairType; //used for <Catalog+User,Cube> or <hierachy_prop,level_prop>
	typedef std::vector<sgPairType>									propertiesType;
	typedef std::pair<sgPairType,propertiesType>					containerValueType;
	typedef std::vector<containerValueType>							propertiesContainerType;
	typedef propertiesContainerType::iterator						itType;

private:

	propertiesContainerType		mProperties;

	dim_properties(){}
public:
	static dim_properties& instance()
	{
		static dim_properties instance;
		return instance;
	}

	itType find( sgPairType& what  ) {
		
		for( itType i = mProperties.begin(), e = mProperties.end(); i != e; ++i ) {
			if ( what == i->first ) { return i; }
		}
		return mProperties.end();
	}

	itType find( const std::string& aCatalogKey, const std::string& aCube  ) {
		
		sgPairType what = sgPairType(aCatalogKey, aCube);
		for( itType i = mProperties.begin(), e = mProperties.end(); i != e; ++i ) {
			if ( what == i->first ) { return i; }
		}
		return mProperties.end();
	}

	itType end() {
		return mProperties.end();
	}

	void addProperty( const std::string& aCatalogKey, const std::string& aCube, const std::string& aProp, const std::string& aHier, const std::string& aLevel ) {
		sgPairType key = sgPairType(aCatalogKey, aCube);
		sgPairType val = sgPairType(aHier+".["+aProp+"]", aLevel+".["+aProp+"]");
		
		itType match = find( key );
		if( mProperties.end() == match ) {
			mProperties.push_back( containerValueType(key,propertiesType()));
			match = mProperties.end() - 1;
		}
		match->second.push_back(val);
	}

	bool noDimProps( const std::string& aCatalogKey, const std::string& aCube ) {
		sgPairType key = sgPairType(aCatalogKey, aCube);

		itType match = find( key );
		if( mProperties.end() == match ) {
			return true;
		}
		return false;
	}

};