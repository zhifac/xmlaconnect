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
					implementation/instantiation for all ui functions and classes
*/

#include "stdafx.h"
#include "prop_conn_dlg.h"
#include "prop_adv_dlg.h"
#include "pass_prompt.h"
#include "soapXMLAConnectionProxy.h"


LRESULT prop_conn_dlg::OnEnChangeEdit1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	GetDlgItemText( IDC_EDIT1, mHost, 255 );
	m_bRequiresSave = TRUE;
	m_bDirty = TRUE;

	return 0;
}

LRESULT prop_conn_dlg::OnEnChangeEdit3(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	GetDlgItemText( IDC_EDIT3, mUser, 255 );
	m_bRequiresSave = TRUE;
	m_bDirty = TRUE;

	return 0;
}

LRESULT prop_conn_dlg::OnEnChangeEdit4(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	GetDlgItemText( IDC_EDIT4, mPass, 255 );
	m_bRequiresSave = TRUE;
	m_bDirty = TRUE;

	return 0;
}

LRESULT prop_conn_dlg::OnBnClickedButton1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	TCHAR		loc[2*STR_LEN+11];
	TCHAR*		ploc = loc;
	MakeLocation( &ploc );
	std::string location(CW2A( loc, CP_UTF8 ));
	std::string user(CW2A( mUser, CP_UTF8 ));
	std::string pass(CW2A( mPass, CP_UTF8 ));
	std::string db(CW2A( mInitialCatalog, CP_UTF8 ));

	XMLAConnectionProxy proxy( location.c_str() );
	soap_omode(&proxy, SOAP_XML_DEFAULTNS);
	//soap_omode(&proxy,SOAP_XML_INDENT);
	cxmla__DiscoverResponse response;

	xmlns__Restrictions restrictions;

	restrictions.RestrictionList.CATALOG_USCORENAME = (char*)db.c_str();

//	BSessionType begin_session;

//	SOAP_ENV__Header header1;
//	header1.BeginSession = &begin_session;

	proxy.header = new SOAP_ENV__Header();
	proxy.header->BeginSession = new BSessionType();
	proxy.header->BeginSession->element = NULL;
	proxy.header->BeginSession->xmlns = NULL;
	proxy.header->EndSession = NULL;
	proxy.header->Session = NULL;

	proxy.userid = user.c_str();
	proxy.passwd = pass.c_str();

	xmlns__Properties props;
	props.PropertyList.LocaleIdentifier = CP_UTF8;
	int ret = proxy.Discover( "MDSCHEMA_CUBES", restrictions, props, response );

	if ( S_OK == ret )
	{
		MessageBox( TEXT( "Connection Succesfull." ), TEXT( "Info" ), MB_ICONINFORMATION );

		HKEY key;
		if ( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER, TEXT("Software\\Arquery\\ODBO" ), 0,  KEY_SET_VALUE, &key ) )
		{
			RegSetValueEx( key, TEXT("location"), 0, REG_SZ, (BYTE*)loc, (DWORD)_tcslen( loc ) * 2 );
			CloseHandle( key );
		}

	}
	else
	{
		MessageBox( CA2W( proxy.fault->faultstring, CP_UTF8) , TEXT( "Error" ), MB_ICONERROR );
	}
	
	return 0;
}

LRESULT prop_conn_dlg::OnCbnSelchangeCombo1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& /*bHandled*/)
{
	LRESULT		idx = SendMessage( hWndCtl, CB_GETCURSEL, 0, 0 );
	if ( LRESULT( -1 ) == idx ) return 0;

	SendMessage( hWndCtl, CB_GETLBTEXT, idx, (LPARAM)mInitialCatalog );
	GetDlgItem(IDC_BUTTON1).EnableWindow( true );
	
	m_bRequiresSave = TRUE;
	m_bDirty = TRUE;

	return 0;
}

LRESULT prop_conn_dlg::OnCbnEditchangeCombo1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	TCHAR		textBuf[256];

	GetDlgItemText( IDC_COMBO1, textBuf, 255 );

	_tcscpy_s( mInitialCatalog, STR_LEN, textBuf );

	m_bRequiresSave = TRUE;
	m_bDirty = TRUE;

	return 0;
}

LRESULT prop_conn_dlg::OnCbnDropdownCombo1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//populate database list
	HWND hWnd = GetDlgItem(IDC_COMBO1).m_hWnd;
	SendMessage( hWnd, CB_RESETCONTENT, 0, 0 );


	TCHAR		loc[2*STR_LEN+11];
	TCHAR*		ploc = loc;
	MakeLocation( &ploc );
	std::string location(CW2A( loc, CP_UTF8 ));
	std::string user(CW2A( mUser, CP_UTF8 ));
	std::string pass(CW2A( mPass, CP_UTF8 ));

	XMLAConnectionProxy proxy( location.c_str() );
	soap_omode(&proxy, SOAP_XML_DEFAULTNS);
	//soap_omode(&proxy,SOAP_XML_INDENT);
	cxmla__DiscoverResponse response;

	//restrictions here
	xmlns__Restrictions restrictions;

	BSessionType begin_session;

	SOAP_ENV__Header header;
	header.BeginSession = &begin_session;

	proxy.header = new SOAP_ENV__Header();
	proxy.header->BeginSession = new BSessionType();
	proxy.header->BeginSession->element = NULL;
	proxy.header->BeginSession->xmlns = NULL;
	proxy.header->EndSession = NULL;
	proxy.header->Session = NULL;

	proxy.userid = user.c_str();
	proxy.passwd = pass.c_str();
	xmlns__Properties props;
	props.PropertyList.LocaleIdentifier = CP_UTF8;
	proxy.Discover( "DBSCHEMA_CATALOGS", restrictions, props, response );

	if ( SOAP_FAULT == proxy.error ) {
		MessageBox( CA2W( proxy.fault->faultstring, CP_UTF8) , TEXT( "Error" ), MB_ICONERROR );
		return 1;
	}

	for ( int i = 0, e = response.cxmla__return__.root.__rows.__size; i < e; ++i )
	{
		SendMessage( hWnd, CB_ADDSTRING, 0, (LPARAM)CA2T( response.cxmla__return__.root.__rows.row[i].CATALOG_USCORENAME, CP_UTF8 ).m_psz );
	}

	m_bRequiresSave = TRUE;
	m_bDirty = TRUE;

	return 0;
}
