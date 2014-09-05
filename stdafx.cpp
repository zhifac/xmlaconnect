// stdafx.cpp : source file that includes just the standard includes
// XMLAProvider.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
void make_error( LPOLESTR  aDescription  )
{
	ICreateErrorInfo	*pcerrinfo;
	IErrorInfo			*perrinfo;
	HRESULT				hr;

	hr = CreateErrorInfo(&pcerrinfo);

	if ( FAILED( hr ) ) return;

	hr = pcerrinfo->QueryInterface(IID_IErrorInfo, (LPVOID FAR*) &perrinfo);

	if ( FAILED( hr ) )
	{
		pcerrinfo->Release();
		return;
	}

	pcerrinfo->SetDescription( aDescription );
	pcerrinfo->SetGUID(  __uuidof(IDBSchemaRowset) );
	SetErrorInfo(0, perrinfo);

	perrinfo->Release();
	pcerrinfo->Release();
}