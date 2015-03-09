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
					a rowset generated by a MDX query
*/

#pragma once

#include "resource.h"
#include "XMLAProvider_i.h"
#include "row_data.h"
#include "command.h"

using namespace ATL;

class rowset: 
	public CRowsetImpl< rowset, row_data, command>,
	public IErrorRecordsImpl< rowset>,
	public IMDDataset
{
	friend class row_data;
private:
	connection_handler* mConnectionHandler;
public:
BEGIN_COM_MAP(rowset)
	COM_INTERFACE_ENTRY(IMDDataset)
	COM_INTERFACE_ENTRY_CHAIN(CRowsetImpl)
END_COM_MAP()

	HRESULT Execute(DBPARAMS * /*pParams*/, DBROWCOUNT* pcRowsAffected);

	//IMDDataSet
	STDMETHOD ( GetAxisInfo )( DBCOUNTITEM   *pcAxes, MDAXISINFO   **prgAxisInfo );
	STDMETHOD ( FreeAxisInfo )( DBCOUNTITEM cAxes, MDAXISINFO *rgAxisInfo );
	STDMETHOD ( GetAxisRowset )(
									IUnknown     *pUnkOuter,
									DBCOUNTITEM   iAxis,
									REFIID        riid,
									ULONG         cPropertySets,
									DBPROPSET     rgPropertySets[],
									IUnknown      **ppRowset );
	STDMETHOD ( GetCellData )(
									HACCESSOR   hAccessor,
									DBORDINAL   ulStartCell,
									DBORDINAL   ulEndCell,
									VOID       *pData );

	STDMETHODIMP GetSpecification( REFIID riid, IUnknown** ppSpecification ) 
	{
		return E_NOTIMPL; 
	} 

	//IAccessor with row size

	//refines the IAccessor implementation to handle cbRowSize
	STDMETHOD(CreateAccessor)(DBACCESSORFLAGS dwAccessorFlags,
							  DBCOUNTITEM cBindings,
							  const DBBINDING rgBindings[],
							  DBLENGTH cbRowSize,
							  HACCESSOR *phAccessor,
							  DBBINDSTATUS rgStatus[])
	{

	
		HRESULT hr =  CRowsetImpl< rowset, row_data, command>::CreateAccessor( dwAccessorFlags, cBindings, rgBindings, cbRowSize, phAccessor, rgStatus);
	
		if SUCCEEDED( hr )
		{
			m_accessor_row_sizes[ *phAccessor ] = cbRowSize;
		}

		return hr;
	}

	STDMETHOD(ReleaseAccessor)( HACCESSOR hAccessor, DBREFCOUNT *pcRefCount )
	{

		HRESULT hr =  CRowsetImpl< rowset, row_data, command>::ReleaseAccessor( hAccessor, pcRefCount);
		
		if ( SUCCEEDED( hr ) && (0 == pcRefCount) )
		{
			m_accessor_row_sizes.erase( hAccessor );
		}

		return hr;
	}
private:

	DBLENGTH GetRowSize( HACCESSOR   hAccessor )
	{
		std::map<HACCESSOR, DBLENGTH>::const_iterator	row_size;
		if ( m_accessor_row_sizes.end() != ( row_size = m_accessor_row_sizes.find( hAccessor ) ) )
		{
			return row_size->second;
		}
		return 1;
	}

	std::map<HACCESSOR, DBLENGTH>	m_accessor_row_sizes;
};
