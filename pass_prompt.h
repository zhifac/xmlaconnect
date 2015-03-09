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
					prompter for user/password
*/

#pragma once

#include "resource.h"       // main symbols
#include <atlhost.h>

using namespace ATL;

class pass_prompt_ui :  public CDialogImpl<pass_prompt_ui>
{
public:
	TCHAR* m_user;
	TCHAR* m_pass;
public:
	pass_prompt_ui(){}

	~pass_prompt_ui(){}

	enum { IDD = IDD_PASSPROMPT };

BEGIN_MSG_MAP(pass_prompt_ui)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ATLVERIFY(CenterWindow());
		bHandled = TRUE;
		SetDlgItemText(IDC_EDIT_USER_L, m_user );
		SetDlgItemText(IDC_EDIT_PASS_L, m_pass );
		GotoDlgCtrl(GetDlgItem(IDC_EDIT_PASS_L)); 
		return 0;  // Manual focus selection
	}

	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		GetDlgItemText(IDC_EDIT_USER_L, m_user, 255 );
		GetDlgItemText(IDC_EDIT_PASS_L, m_pass, 255 );

		EndDialog(wID);
		return 0;
	}

	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}
	LRESULT OnEnChangeEdit1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};


