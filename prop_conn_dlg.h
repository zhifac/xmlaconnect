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
					tab for connection properties in the provider configuration dialog
*/

#pragma once
#include "resource.h"       // main symbols

#include "XMLAProvider_i.h"

#include "comutil.h"

using namespace ATL;

// prop_conn_dlg

class ATL_NO_VTABLE prop_conn_dlg :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<prop_conn_dlg, &CLSID_XMLAConnectionDlg>,
	public IPropertyPageImpl<prop_conn_dlg>,
	public IPersistPropertyBagImpl<prop_conn_dlg>,
	public ATL::CDialogImpl<prop_conn_dlg>
{
private:
	static const int STR_LEN = 256;

	TCHAR				mHost[STR_LEN];

	TCHAR				mUser[STR_LEN];
	TCHAR				mPass[STR_LEN];

	TCHAR				mInitialCatalog[STR_LEN];

private:
	void MakeLocation( TCHAR** loc )
	{
		(*loc)[0]=0;
		if ( ( mHost[0] != 'h' && mHost[0] != 'H' ) || ( mHost[1] != 't' && mHost[1] != 'T' )  || ( mHost[2] != 't' && mHost[2] != 'T' ) || ( mHost[3] != 'p' && mHost[3] != 'P' ) )
		{
 			wcscat_s( *loc, 2*STR_LEN+11, L"http://" );
		}
		wcscat_s( *loc, 2*STR_LEN+11, mHost );
	}
public:
	prop_conn_dlg()
	{
		m_dwTitleID = IDS_TITLEXMLACONNECTIONDLG;
		m_dwHelpFileID = IDS_HELPFILEXMLACONNECTIONDLG;
		m_dwDocStringID = IDS_DOCSTRINGXMLACONNECTIONDLG;
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		mHost[0]=0;
		mUser[0]=0;
		mPass[0]=0;
		mInitialCatalog[0]=0;
		m_bRequiresSave = false;

		HKEY key;

		if ( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER, TEXT("Software\\Arquery\\ODBO" ), 0,  KEY_READ, &key ) )
		{
			DWORD len = STR_LEN;
			RegGetValue( key, nullptr, TEXT("location"), REG_SZ, nullptr, mHost, &len );
			CloseHandle( key );
		}

		return S_OK;
	}

	void FinalRelease()
	{
	}

	enum {IDD = IDD_XMLACONNECTIONDLG};

DECLARE_REGISTRY_RESOURCEID(IDR_XMLACONNECTIONDLG)


BEGIN_COM_MAP(prop_conn_dlg)
	COM_INTERFACE_ENTRY(IPropertyPage)
	COM_INTERFACE_ENTRY(IPersistPropertyBag)
END_COM_MAP()

BEGIN_MSG_MAP(prop_conn_dlg)
	CHAIN_MSG_MAP(IPropertyPageImpl<prop_conn_dlg>)
	COMMAND_HANDLER(IDC_EDIT1, EN_CHANGE, OnEnChangeEdit1)
	COMMAND_HANDLER(IDC_EDIT3, EN_CHANGE, OnEnChangeEdit3)
	COMMAND_HANDLER(IDC_EDIT4, EN_CHANGE, OnEnChangeEdit4)
	COMMAND_HANDLER(IDC_COMBO1, CBN_SELCHANGE, OnCbnSelchangeCombo1)
	COMMAND_HANDLER(IDC_BUTTON1, BN_CLICKED, OnBnClickedButton1)
	COMMAND_HANDLER(IDC_COMBO1, CBN_DROPDOWN, OnCbnDropdownCombo1)
	COMMAND_HANDLER(IDC_COMBO1, CBN_EDITCHANGE, OnCbnEditchangeCombo1)
END_MSG_MAP()

	STDMETHOD(Apply)(void)
	{
		for (UINT i = 0; i < m_nObjects; i++)
		{
		}
		m_bDirty = FALSE;
		return S_OK;
	}


	STDMETHOD(Load)(LPPROPERTYBAG pPropBag, LPERRORLOG pErrorLog)
	{
		HRESULT		hr = IPersistPropertyBagImpl<prop_conn_dlg>::Load( pPropBag, pErrorLog );
		if SUCCEEDED( hr ) InitDialogControls();

		return hr;
	}

	STDMETHOD(Save)(LPPROPERTYBAG pPropBag, BOOL fClearDirty, BOOL fSaveAllProperties)
	{
		HRESULT		hr;

		_variant_t	buf( mInitialCatalog );
		if FAILED( hr = pPropBag->Write( OLESTR("Initial Catalog"), &buf ) ) return hr;

		TCHAR		loc[2*STR_LEN+11];
		TCHAR*		ploc = loc;
		MakeLocation( &ploc );
		
		buf = loc;
		if FAILED( hr = pPropBag->Write( OLESTR("Location"), &buf ) ) return hr;

		buf = mUser;
		if FAILED( hr = pPropBag->Write( OLESTR("User ID"), &buf ) ) return hr;

		buf = mPass;
		if FAILED( hr = pPropBag->Write( OLESTR("Password"), &buf ) ) return hr;

		buf = true;
		if FAILED( hr = pPropBag->Write( OLESTR("Persist Security Info"), &buf ) ) return hr;

		return IPersistPropertyBagImpl<prop_conn_dlg>::Save( pPropBag, fClearDirty, fSaveAllProperties );
	}

	void InitDialogControls()
	{		
		GotoDlgCtrl(GetDlgItem(IDC_EDIT3)); 

		SetDlgItemText( IDC_EDIT1, mHost );	
		SetDlgItemText( IDC_EDIT3, mUser );
		SetDlgItemText( IDC_EDIT4, mPass );
		SetDlgItemText( IDC_COMBO1, mInitialCatalog );
		if ( 0 != mInitialCatalog[0] ) {
			GetDlgItem(IDC_BUTTON1).EnableWindow( true );
		}
	}

BEGIN_PROP_MAP(prop_conn_dlg)
	PROP_DATA_ENTRY( "Initial Catalog", mInitialCatalog, VT_LPWSTR )
	PROP_DATA_ENTRY( "Host", mHost, VT_LPWSTR )
	PROP_DATA_ENTRY( "User", mUser, VT_LPWSTR )
	PROP_DATA_ENTRY( "Password", mPass, VT_LPWSTR )
END_PROP_MAP()

public:
	BOOL			m_bRequiresSave;
	LRESULT OnEnChangeEdit1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEnChangeEdit3(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEnChangeEdit4(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCbnSelchangeCombo1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND hWndCtl, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButton1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCbnDropdownCombo1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCbnEditchangeCombo1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};


OBJECT_ENTRY_AUTO(__uuidof(XMLAConnectionDlg), prop_conn_dlg)
