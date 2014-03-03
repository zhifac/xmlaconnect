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
					tab for advanced properties in the provider configuration dialog
					although this is empty atm it should be declared otherwise the
					feature does not work
*/

#pragma once
#include "resource.h"       // main symbols


#include "XMLAProvider_i.h"

using namespace ATL;

class ATL_NO_VTABLE prop_adv_dlg:
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<prop_adv_dlg, &CLSID_XMLAAdvancedDlg>,
	public IPropertyPageImpl<prop_adv_dlg>,
	public IPersistPropertyBagImpl<prop_adv_dlg>,
	public ATL::CDialogImpl<prop_adv_dlg>
{
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
};


OBJECT_ENTRY_AUTO(__uuidof(XMLAAdvancedDlg), prop_adv_dlg)
