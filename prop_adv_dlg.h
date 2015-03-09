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
					tab for advanced properties in the provider configuration dialog
					although this is empty atm it should be declared otherwise the
					feature does not work
*/

#pragma once
#include "resource.h"       // main symbols


#include "XMLAProvider_i.h"

#include <Strsafe.h>

using namespace ATL;

class ATL_NO_VTABLE prop_adv_dlg:
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<prop_adv_dlg, &CLSID_XMLAAdvancedDlg>,
	public IPropertyPageImpl<prop_adv_dlg>,
	public IPersistPropertyBagImpl<prop_adv_dlg>,
	public ATL::CDialogImpl<prop_adv_dlg>
{
private:
	HWND m_web;
	HWND m_mail;

	HFONT  m_font_ori;
	HFONT  m_font_under;

	bool m_web_active;
	bool m_mail_active;

	HCURSOR m_hand_cursor;
	HCURSOR m_initial_cursor;

	static const unsigned int VERSION_LEN = 256;
	TCHAR m_file_version[VERSION_LEN];

private:
	void rect_to_screen( RECT& what )
	{
		POINT point;

		point.x = what.left;
		point.y = what.top;
		::ScreenToClient( m_hWnd, &point );
		what.left = point.x;
		what.top = point.y;

		point.x = what.right;
		point.y = what.bottom;
		::ScreenToClient( m_hWnd, &point );
		what.right = point.x;
		what.bottom = point.y;
	}


	void get_file_version()
	{
		TCHAR file_name[MAX_PATH];
		if ( 0 == GetModuleFileName( _AtlBaseModule.GetModuleInstance(), file_name, MAX_PATH ) )
		{
			return;
		}
		DWORD dummy;
		DWORD vi_size = GetFileVersionInfoSize( file_name, &dummy );
		if ( 0 == vi_size ) { return; };

		BYTE* vi_data = new BYTE[vi_size];

		if ( 0 != GetFileVersionInfo( file_name, 0, vi_size, static_cast<LPVOID>(vi_data) ) )
		{
			VS_FIXEDFILEINFO* file_info;
			UINT size;
			if ( 0 != VerQueryValue( static_cast<LPVOID>(vi_data), TEXT("\\"), (LPVOID*)(&file_info), &size ) )
			{
				StringCbPrintf( m_file_version, VERSION_LEN, TEXT("v %d.%d.%d.%d")
					, HIWORD( file_info->dwFileVersionMS )
					, LOWORD( file_info->dwFileVersionMS )
					, HIWORD( file_info->dwFileVersionLS )
					, LOWORD( file_info->dwFileVersionLS )
				);
			}
			SetDlgItemText(IDC_VERSION_INFO, m_file_version );
		}
		delete[] vi_data;
	}
public:
	prop_adv_dlg()
	{
		m_dwTitleID = IDS_TITLEXMLAADVANCEDDLG;
		m_dwHelpFileID = IDS_HELPFILEXMLAADVANCEDDLG;
		m_dwDocStringID = IDS_DOCSTRINGXMLAADVANCEDDLG;
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		m_bRequiresSave = false;
		m_web_active = m_mail_active = false;
		m_file_version[0]=0;
		return S_OK;
	}

	void FinalRelease()
	{
	}

	enum {IDD = IDD_XMLAADVANCEDDLG};

DECLARE_REGISTRY_RESOURCEID(IDR_XMLAADVANCEDDLG)


BEGIN_COM_MAP(prop_adv_dlg)
	COM_INTERFACE_ENTRY(IPropertyPage)
	COM_INTERFACE_ENTRY(IPersistPropertyBag)
END_COM_MAP()

BEGIN_MSG_MAP(prop_adv_dlg)
	CHAIN_MSG_MAP(IPropertyPageImpl<prop_adv_dlg>)
	COMMAND_HANDLER(IDC_STATIC_WEB, BN_CLICKED, OnBnClickedWeb)
	COMMAND_HANDLER(IDC_STATIC_MAIL, BN_CLICKED, OnBnClickedMail)
	MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnStaticColor)
END_MSG_MAP()

	STDMETHOD(Apply)(void)
	{
		for (UINT i = 0; i < m_nObjects; i++)
		{
		}
		m_bDirty = FALSE;
		return S_OK;
	}


BEGIN_PROP_MAP(prop_adv_dlg)
END_PROP_MAP()

public:
	BOOL			m_bRequiresSave;
	LRESULT OnBnClickedWeb(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ShellExecute(NULL, TEXT("open"), TEXT("http://www.arquery.com"), NULL, NULL, SW_SHOWNORMAL);
		return 0;
	}
	LRESULT OnBnClickedMail(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ShellExecute(NULL, TEXT("open"), TEXT("mailto:support@arquery.com"), NULL, NULL, SW_SHOWNORMAL);
		return 0;
	}
	LRESULT OnInitCursor(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return 0;
	}
	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if ( !m_web_active && wParam == (WPARAM)m_web )
		{
			SendMessage(m_web, WM_SETFONT, (WPARAM) m_font_under, TRUE);
			m_web_active = true;
		}

		if ( !m_mail_active && wParam == (WPARAM)m_mail )
		{
			SendMessage(m_mail, WM_SETFONT, (WPARAM) m_font_under, TRUE);
			m_mail_active = true;
		}

		if ( wParam == (WPARAM)m_web || wParam == (WPARAM)m_mail)
		{
			SetCursor(m_hand_cursor);
		} else 
		{
			SetCursor(m_initial_cursor);
		}

		return TRUE;
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		m_web = GetDlgItem( IDC_STATIC_WEB );
		m_mail = GetDlgItem( IDC_STATIC_MAIL );

		m_initial_cursor = GetCursor();
		m_hand_cursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND));
		if ( nullptr == m_hand_cursor )
		{
			m_hand_cursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
		}

		m_font_ori = (HFONT) SendMessage(m_web, WM_GETFONT, 0, 0);

		LOGFONT lf;
		GetObject(m_font_ori, sizeof(lf), &lf);
		lf.lfUnderline = TRUE;

		m_font_under = CreateFontIndirect(&lf);

		get_file_version();
		return 0;
	}
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if ( m_web_active )
		{
			SendMessage(m_web, WM_SETFONT, (WPARAM) m_font_ori, TRUE);
			m_web_active = false;
		}
		if ( m_mail_active )
		{
			SendMessage(m_mail, WM_SETFONT, (WPARAM) m_font_ori, TRUE);
			m_mail_active = false;
		}

		return 0;
	}

	LRESULT OnStaticColor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		if ( m_web == (HWND) lParam || m_mail == (HWND) lParam )
		{
			SetBkColor( (HDC) wParam, GetSysColor(COLOR_BTNFACE));//RGB( 0, 0, 192 ) );
			SetTextColor( (HDC) wParam, RGB( 0, 0, 192 ) );
			return TRUE;
		}
		return 0;
	}
};


OBJECT_ENTRY_AUTO(__uuidof(XMLAAdvancedDlg), prop_adv_dlg)
