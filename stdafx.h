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
					precompiled header
*/

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atldb.h>

#include <map>
#include <algorithm>
#include <sstream>

//atldb.h line 656 defines this as VT_I4. 
//64 bits of Excel try to set this as VT_I8
#ifdef _WIN64 
#ifdef INIT_HWND_Type 
#undef INIT_HWND_Type
#endif
#define INIT_HWND_Type VT_I8
#endif

inline int get_int( const char* from ) {
	return from ? atoi(from) : -1;
}

#define FROM_STRING( x, y ) (x) ? CA2W( (x), y ) : L""
#define EMPTY_CONSTRUCTOR(class_name) class_name##(){}
