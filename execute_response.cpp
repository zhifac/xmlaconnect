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
	soap_element_begin_out(soap, a->elementName , id, a->__xsi__type); // print XML beginning tag
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
	if ( NULL != soap->type )
	{
		a->__xsi__type = soap_strdup( soap, soap->type );
	} else
	{
		a->__xsi__type = NULL;
	}
	soap_in_xsd__string( soap, soap->tag, const_cast<char**>(&(a->value)), "xsd:string" );
	return a; 
}

void soap_serialize_arrayUserProp(struct soap *soap, struct arrayUserPropStruct const *a)
{
	if (a->arrayUserPropStruct::__array)
	{	int i;
		for (i = 0; i < a->arrayUserPropStruct::__size; i++)
		{
			soap_serialize_UserDataProp(soap, a->arrayUserPropStruct::__array + i);
		}
	}
}

void soap_default_arrayUserProp(struct soap *soap, struct arrayUserPropStruct *a)
{	
	a->arrayUserPropStruct::__size = 0;
	a->arrayUserPropStruct::__array = NULL;	
}


struct arrayUserPropStruct * soap_in_arrayUserProp(struct soap *soap, const char *tag, arrayUserPropStruct *a, const char *type)
{
	(void)type; /* appease -Wall -Werror */
	if (soap_element_begin_in(soap, tag, 0, NULL))
		return NULL;
	a = (arrayUserPropStruct *)soap_class_id_enter(soap, soap->id, a, SOAP_TYPE_arrayUserPropStruct, sizeof(arrayUserPropStruct), soap->type, soap->arrayType);
	if (!a)
		return NULL;
	if (soap->alloced)
	{	soap_default_arrayUserProp(soap, a);//a->soap_default(soap);
		if (soap->clist->type != SOAP_TYPE_arrayUserPropStruct)
		{	soap_revert(soap);
			*soap->id = '\0';
			return soap_in_arrayUserProp(soap, tag, a, type);//(arrayUserPropStruct *)a->soap_in(soap, tag, type);
		}
	}
	struct soap_blist *soap_blist___array1 = NULL;

		for (;;)
		{	soap->error = SOAP_TAG_MISMATCH;
			if (soap->error == SOAP_TAG_MISMATCH && !soap_peek_element(soap))
			{	if (a->arrayUserPropStruct::__array == NULL)
				{	if (soap_blist___array1 == NULL)
						soap_blist___array1 = soap_new_block(soap);
					a->arrayUserPropStruct::__array = (struct UserPropStruct *)soap_push_block(soap, soap_blist___array1, sizeof(struct UserPropStruct));
					if (a->arrayUserPropStruct::__array == NULL)
						return NULL;
					soap_default_UserDataProp(soap, a->arrayUserPropStruct::__array);
				}
				if (soap_in_UserDataProp(soap, "-array", a->arrayUserPropStruct::__array, "UserDataProp"))
				{	a->arrayUserPropStruct::__size++;
					a->arrayUserPropStruct::__array = NULL;
					continue;
				}
			}
			if (soap->error == SOAP_TAG_MISMATCH)
				soap->error = soap_ignore_element(soap);
			if (soap->error == SOAP_NO_TAG)
				break;
			if (soap->error)
				return NULL;
		}
		if (a->arrayUserPropStruct::__array)
			soap_pop_block(soap, soap_blist___array1);
		if (a->arrayUserPropStruct::__size)
			a->arrayUserPropStruct::__array = (struct UserPropStruct *)soap_save_block(soap, soap_blist___array1, NULL, 1);
		else
		{	a->arrayUserPropStruct::__array = NULL;
			if (soap_blist___array1)
				soap_end_block(soap, soap_blist___array1);
		}
		if (soap_element_end_in(soap, tag))
			return NULL;

	return a;
}

int soap_out_arrayUserProp(struct soap *soap, const char *tag, int id, const arrayUserPropStruct *a, const char *type)
{
	
	if (soap_element_begin_out(soap, tag, soap_embedded_id(soap, id, a, SOAP_TYPE_arrayUserPropStruct), type))
		return soap->error;
	if (a->arrayUserPropStruct::__array)
	{	int i;
		for (i = 0; i < a->arrayUserPropStruct::__size; i++)
			if (soap_out_UserDataProp(soap, "-array", -1, a->arrayUserPropStruct::__array + i, ""))
				return soap->error;
	}
	return soap_element_end_out(soap, tag);
}


void soap_serialize_CellDataProp(struct soap *soap,  const struct CellPropStruct *a)
{ 
} 

void soap_default_CellDataProp(struct soap* soap,  struct CellPropStruct *a)
{ 
	a = NULL; 
}

int soap_out_CellDataProp(struct soap *soap, const char *tag, int id, const struct CellPropStruct *a, const char *type)
{
	//not really needed
   return SOAP_OK;
}


struct CellPropStruct* soap_in_CellDataProp(struct soap *soap, const char *tag, struct CellPropStruct *a, const char *type)
{	
	if ( SOAP_OK != soap_peek_element( soap ) ) {
		return NULL;
	}
	a->elementName = soap_strdup( soap, soap->tag );
	a->name = nullptr;
	a->type = nullptr;
	soap_s2string(soap, soap_attr_value(soap, "name", 0), const_cast<char**>(&(a->name)), 0, -1);
	soap_s2string(soap, soap_attr_value(soap, "type", 0), const_cast<char**>(&(a->type)), 0, -1);
	//soap_in_xsd__string( soap, soap_attr_value(soap, "name", 0), const_cast<char**>(&(a->name)), "xsd:string" );
	soap_in_xsd__string( soap, soap->tag, const_cast<char**>(&(a->value)), "xsd:string" );
	return a; 
}


//mark

void soap_serialize_arrayCellProp(struct soap *soap, struct arrayCellPropStruct const *a)
{
 if (a->arrayCellPropStruct::__array)
 { int i;
  for (i = 0; i < a->arrayCellPropStruct::__size; i++)
  {
   soap_serialize_CellDataProp(soap, a->arrayCellPropStruct::__array + i);
  }
 }
}


void soap_default_arrayCellProp(struct soap *soap, struct arrayCellPropStruct *a)
{ 
 a->arrayCellPropStruct::__size = 0;
 a->arrayCellPropStruct::__array = NULL; 
}

struct arrayCellPropStruct * soap_in_arrayCellProp(struct soap *soap, const char *tag, arrayCellPropStruct *a, const char *type)
{
 (void)type; /* appease -Wall -Werror */
 if (soap_element_begin_in(soap, tag, 0, NULL))
  return NULL;
 a = (arrayCellPropStruct *)soap_class_id_enter(soap, soap->id, a, SOAP_TYPE_arrayCellPropStruct, sizeof(arrayCellPropStruct), soap->type, soap->arrayType);
 if (!a)
  return NULL;
 if (soap->alloced)
 { soap_default_arrayCellProp(soap, a);//a->soap_default(soap);
  if (soap->clist->type != SOAP_TYPE_arrayCellPropStruct)
  { soap_revert(soap);
   *soap->id = '\0';
   return soap_in_arrayCellProp(soap, tag, a, type);//(arrayCellPropStruct *)a->soap_in(soap, tag, type);
  }
 }
 struct soap_blist *soap_blist___array1 = NULL;
  for (;;)
  { soap->error = SOAP_TAG_MISMATCH;
   if (soap->error == SOAP_TAG_MISMATCH && !soap_peek_element(soap))
   { if (a->arrayCellPropStruct::__array == NULL)
    { if (soap_blist___array1 == NULL)
      soap_blist___array1 = soap_new_block(soap);
     a->arrayCellPropStruct::__array = (struct CellPropStruct *)soap_push_block(soap, soap_blist___array1, sizeof(struct CellPropStruct));
     if (a->arrayCellPropStruct::__array == NULL)
      return NULL;
     soap_default_CellDataProp(soap, a->arrayCellPropStruct::__array);
    }
    if (soap_in_CellDataProp(soap, "-array", a->arrayCellPropStruct::__array, "CellDataProp"))
    { a->arrayCellPropStruct::__size++;
     a->arrayCellPropStruct::__array = NULL;
     continue;
    }
   }
   if (soap->error == SOAP_TAG_MISMATCH)
    soap->error = soap_ignore_element(soap);
   if (soap->error == SOAP_NO_TAG)
    break;
   if (soap->error)
    return NULL;
  }
  if (a->arrayCellPropStruct::__array)
   soap_pop_block(soap, soap_blist___array1);
  if (a->arrayCellPropStruct::__size)
   a->arrayCellPropStruct::__array = (struct CellPropStruct *)soap_save_block(soap, soap_blist___array1, NULL, 1);
  else
  { a->arrayCellPropStruct::__array = NULL;
   if (soap_blist___array1)
    soap_end_block(soap, soap_blist___array1);
  }
  if (soap_element_end_in(soap, tag))
   return NULL;

 return a;
}

int soap_out_arrayCellProp(struct soap *soap, const char *tag, int id, const arrayCellPropStruct *a, const char *type)
{
 
 if (soap_element_begin_out(soap, tag, soap_embedded_id(soap, id, a, SOAP_TYPE_arrayCellPropStruct), type))
  return soap->error;
 if (a->arrayCellPropStruct::__array)
 { int i;
  for (i = 0; i < a->arrayCellPropStruct::__size; i++)
   if (soap_out_CellDataProp(soap, "-array", -1, a->arrayCellPropStruct::__array + i, ""))
    return soap->error;
 }
 return soap_element_end_out(soap, tag);
}
