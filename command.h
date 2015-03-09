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
					a command (MDX query) that can generate a rowset
*/

#pragma once

using namespace ATL;

class connection_handler;

class ATL_NO_VTABLE command : 
	public CComObjectRootEx<CComObjectThreadModel>,
	public IAccessorImpl<command>,
	public ICommandTextImpl<command>,
	public ICommandPropertiesImpl<command>,
	public IObjectWithSiteImpl<command>,
	public IConvertTypeImpl<command>,
	public IColumnsInfoImpl<command>,
	public IInternalCommandConnectionImpl<command>,
	public IExtendCommand,
	public ISupportErrorInfo
{
public:

BEGIN_COM_MAP(command)
	COM_INTERFACE_ENTRY(ICommand)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IAccessor)
	COM_INTERFACE_ENTRY(ICommandProperties)
	COM_INTERFACE_ENTRY2(ICommandText, ICommand)
	COM_INTERFACE_ENTRY(IColumnsInfo)
	COM_INTERFACE_ENTRY(IConvertType)
	COM_INTERFACE_ENTRY(IInternalConnection)
	COM_INTERFACE_ENTRY(IExtendCommand)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
private:
	std::unique_ptr< connection_handler > mConnectionHandler;
public:
	HRESULT FinalConstruct()
	{
		mConnectionHandler.reset( NULL );
		HRESULT hr = CConvertHelper::FinalConstruct();
		if (FAILED (hr))
			return hr;
		hr = IAccessorImpl<command>::FinalConstruct();
		if (FAILED(hr))
			return hr;
		return CUtlProps<command>::FInit();
	}
	void FinalRelease()
	{
		IAccessorImpl<command>::FinalRelease();
	}

	static ATLCOLUMNINFO* GetColumnInfo(command* pv, DBORDINAL* pcInfo)
	{
		return row_data::GetColumnInfo(pv,pcInfo);
	}
	
	STDMETHOD ( Execute )(IUnknown * pUnkOuter, REFIID riid, DBPARAMS * pParams, DBROWCOUNT * pcRowsAffected, IUnknown ** ppRowset);

//IExtendCommand
	STDMETHOD ( GetAxisRowset ) (IUnknown * pUnkOuter, REFIID riid, void * pParams, DBROWCOUNT * pcRowsAffected, IUnknown ** ppRowset);
	STDMETHOD ( GetConnectionHandler )( void** connection );

//ISupportErrorInfo
	HRESULT STDMETHODCALLTYPE InterfaceSupportsErrorInfo (REFIID riid)
	{
		return S_OK;
	}
BEGIN_PROPSET_MAP(command)
	BEGIN_PROPERTY_SET(DBPROPSET_ROWSET)
		PROPERTY_INFO_ENTRY(IAccessor)
		PROPERTY_INFO_ENTRY(IColumnsInfo)
		PROPERTY_INFO_ENTRY(IConvertType)
		PROPERTY_INFO_ENTRY(IRowset)
		PROPERTY_INFO_ENTRY(IRowsetIdentity)
		PROPERTY_INFO_ENTRY(IRowsetInfo)
		PROPERTY_INFO_ENTRY(IRowsetLocate)
		PROPERTY_INFO_ENTRY(BOOKMARKS)
		PROPERTY_INFO_ENTRY(BOOKMARKSKIPPED)
		PROPERTY_INFO_ENTRY(BOOKMARKTYPE)
		PROPERTY_INFO_ENTRY(CANFETCHBACKWARDS)
		PROPERTY_INFO_ENTRY(CANHOLDROWS)
		PROPERTY_INFO_ENTRY(CANSCROLLBACKWARDS)
		PROPERTY_INFO_ENTRY(LITERALBOOKMARKS)
		PROPERTY_INFO_ENTRY(ORDEREDBOOKMARKS)
	END_PROPERTY_SET(DBPROPSET_ROWSET)
END_PROPSET_MAP()

};
