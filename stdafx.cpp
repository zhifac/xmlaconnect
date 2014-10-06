// stdafx.cpp : source file that includes just the standard includes
// XMLAProvider.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
void make_error( LPOLESTR  aDescription  )
{
	ICreateErrorInfo	*pcerrinfo;
	IErrorInfo			*perrinfo;
	HRESULT				hr;
	OLECHAR				error_txt[2*1024];		

	hr = CreateErrorInfo(&pcerrinfo);

	if ( FAILED( hr ) ) return;

	hr = pcerrinfo->QueryInterface(IID_IErrorInfo, (LPVOID FAR*) &perrinfo);

	if ( FAILED( hr ) )
	{
		pcerrinfo->Release();
		return;
	}
	ZeroMemory( error_txt, 2*1024*sizeof(OLECHAR) );
	wcscat_s( error_txt, 2*1024, L"There was an error communicating with the server:\n\n" );
	wcscat_s( error_txt, 2*1024, aDescription );
	wcscat_s( error_txt, 2*1024, L".\n\nPlease refer to the server’s logs for more details." );

	pcerrinfo->SetDescription( error_txt );
	pcerrinfo->SetGUID(  __uuidof(IDBSchemaRowset) );
	SetErrorInfo(0, perrinfo);

	perrinfo->Release();
	pcerrinfo->Release();
}