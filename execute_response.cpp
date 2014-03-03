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
					some gsoap related functions
*/
#include "stdafx.h"
#include "soapH.h"



xmlns__UName::xmlns__UName()
{
	name = NULL;
	UName = NULL;
}

xmlns__Caption::xmlns__Caption()
{
	name= NULL;
	Caption= NULL;
}

xmlns__LName::xmlns__LName() 
{
	name= NULL;
	LName= NULL;
}

xmlns__LNum::xmlns__LNum() 
{
	name= NULL;
	LNum= NULL;
}

xmlns__DisplayInfo::xmlns__DisplayInfo()
{
	name= NULL;
	DisplayInfo= NULL;
}

xmlns__ParentUniqueName::xmlns__ParentUniqueName()
{
	name= NULL;
	ParentUniqueName= NULL;
}

xmlns__MemberType::xmlns__MemberType()
{
	name = NULL;
	MEMBER_USCORETYPE = NULL;
}

xmlns__Member::xmlns__Member()
{
	Hierarchy= NULL;
	UName= NULL;
	Caption= NULL;
	LName= NULL;
	LNum= NULL;
	DisplayInfo= NULL;
	PARENT_USCOREUNIQUE_USCORENAME= NULL;	
	MEMBER_USCORENAME = NULL;
	MEMBER_USCORETYPE = NULL;
}
///////////
//for dimension properties, user defined properties

void soap_serialize_UserDataProp(struct soap *soap,  const struct UserPropStruct *a)
{ 
} 

void soap_default_UserDataProp(struct soap* soap,  struct UserPropStruct *a)
{ 
	a = NULL; 
}


int soap_out_UserDataProp(struct soap *soap, const char *tag, int id, const struct UserPropStruct *a, const char *type)
{
	if ( a == NULL ) return SOAP_OK;
  soap_element_begin_out(soap, a->elementName , id, type); // print XML beginning tag
  soap_send(soap, a->value); // just print the string (no XML conversion)
  soap_element_end_out(soap, a->elementName); // print XML ending tag
   return SOAP_OK;
}


struct UserPropStruct* soap_in_UserDataProp(struct soap *soap, const char *tag, struct UserPropStruct *a, const char *type)
{	
	if ( SOAP_OK != soap_peek_element( soap ) ) {
		return NULL;
	}
	a->elementName = soap_strdup( soap, soap->tag );
	soap_in_xsd__string( soap, soap->tag, const_cast<char**>(&(a->value)), "xsd:string" );
	return a; 
}