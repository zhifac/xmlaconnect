/*
	ODBO provider for XMLA m_data_exchange stores
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
					a row in the axis rowset
*/

#pragma once

class axis_row
{
private:
	static const size_t MAX_BUF_SIZE = 10 * 1024 * sizeof( wchar_t );
private:
	char*					m_data_exchange;
	xmlns__Axis*			m_axis;
	xmlns__AxisInfo*		m_axis_info;
	ATLCOLUMNINFO*			m_col_info;
	unsigned int			m_col_info_cnt;
	DBLENGTH				m_buf_size;
	typedef 				std::map<unsigned short, unsigned short> m_indirection_type;
	m_indirection_type		m_indirection;
	std::vector<size_t>		m_boundaries;
public:
	axis_row()
	{
		m_data_exchange = nullptr;
		m_axis = nullptr;
		m_col_info = nullptr;
	}

	~axis_row()
	{
		clear();
	}

	void setup_data( DBCOUNTITEM idx, connection_handler* handler ) {

		clear();

		handler->get_axis( idx, m_axis, m_axis_info );
		
		m_col_info_cnt = 1;//TUPLE_ORDINAL;
		for ( unsigned int i = 0, e = m_axis_info->__size; i < e; ++i ) {
			m_col_info_cnt += 5;//required columns;
			xmlns__HierarchyInfo hInfo = m_axis_info->HierarchyInfo[i];
			if ( nullptr != hInfo.PARENT_USCOREUNIQUE_USCORENAME ) {
				++m_col_info_cnt;
			}
			if ( nullptr != hInfo.MEMBER_USCORENAME ) {
				++m_col_info_cnt;
			}
			if ( nullptr != hInfo.MEMBER_USCORETYPE ) {
				++m_col_info_cnt;
			}
			m_col_info_cnt += hInfo.__userProp.__size;
		}
		m_buf_size = 0;

		m_col_info = new ATLCOLUMNINFO[ m_col_info_cnt ];

		m_col_info[0].pwszName = _wcsdup( L"TUPLE_ORDINAL" );
		m_col_info[0].pTypeInfo = (ITypeInfo*)nullptr;
		m_col_info[0].iOrdinal = 1;
		m_col_info[0].dwFlags = DBCOLUMNFLAGS_ISFIXEDLENGTH;
		m_col_info[0].ulColumnSize = sizeof(unsigned long);
		m_col_info[0].wType = DBTYPE_UI4;
		m_col_info[0].bPrecision = 10;
		m_col_info[0].bScale = 0xFF;
		m_col_info[0].cbOffset = m_buf_size;
		memset( &( m_col_info[0].columnid ), 0, sizeof( DBID ));
		m_buf_size += m_col_info[0].ulColumnSize;

		unsigned int crtColInfo = 1;

		for ( unsigned int i = 0, e = m_axis_info->__size; i < e; ++i ) {
			xmlns__HierarchyInfo hInfo = m_axis_info->HierarchyInfo[i];
			m_boundaries.push_back(crtColInfo);
			m_col_info[crtColInfo].pwszName = _wcsdup( CA2W( hInfo.UName.name, CP_UTF8 ) );
			m_col_info[crtColInfo].pTypeInfo = (ITypeInfo*)nullptr;
			m_col_info[crtColInfo].iOrdinal = crtColInfo+1;
			m_col_info[crtColInfo].dwFlags = DBCOLUMNFLAGS_ISFIXEDLENGTH;
			m_col_info[crtColInfo].ulColumnSize = MAX_BUF_SIZE;
			m_col_info[crtColInfo].wType = DBTYPE_WSTR;
			m_col_info[crtColInfo].bPrecision = 0xFF;
			m_col_info[crtColInfo].bScale = 0xFF;
			m_col_info[crtColInfo].cbOffset = m_buf_size;
			memset( &( m_col_info[crtColInfo].columnid ), 0, sizeof( DBID ));
			m_buf_size += m_col_info[crtColInfo].ulColumnSize;
			++crtColInfo;

			m_col_info[crtColInfo].pwszName = _wcsdup( CA2W( hInfo.Caption.name, CP_UTF8 ) );
			m_col_info[crtColInfo].pTypeInfo = (ITypeInfo*)nullptr;
			m_col_info[crtColInfo].iOrdinal = crtColInfo+1;
			m_col_info[crtColInfo].dwFlags = DBCOLUMNFLAGS_ISFIXEDLENGTH;
			m_col_info[crtColInfo].ulColumnSize = MAX_BUF_SIZE;
			m_col_info[crtColInfo].wType = DBTYPE_WSTR;
			m_col_info[crtColInfo].bPrecision = 0xFF;
			m_col_info[crtColInfo].bScale = 0xFF;
			m_col_info[crtColInfo].cbOffset = m_buf_size;
			memset( &( m_col_info[crtColInfo].columnid ), 0, sizeof( DBID ));
			m_buf_size += m_col_info[crtColInfo].ulColumnSize;
			++crtColInfo;

			m_col_info[crtColInfo].pwszName = _wcsdup( CA2W( hInfo.LName.name, CP_UTF8 ) );
			m_col_info[crtColInfo].pTypeInfo = (ITypeInfo*)nullptr;
			m_col_info[crtColInfo].iOrdinal = crtColInfo+1;
			m_col_info[crtColInfo].dwFlags = DBCOLUMNFLAGS_ISFIXEDLENGTH;
			m_col_info[crtColInfo].ulColumnSize = MAX_BUF_SIZE;
			m_col_info[crtColInfo].wType = DBTYPE_WSTR;
			m_col_info[crtColInfo].bPrecision = 0xFF;
			m_col_info[crtColInfo].bScale = 0xFF;
			m_col_info[crtColInfo].cbOffset = m_buf_size;
			memset( &( m_col_info[crtColInfo].columnid ), 0, sizeof( DBID ));
			m_buf_size += m_col_info[crtColInfo].ulColumnSize;
			++crtColInfo;

			m_col_info[crtColInfo].pwszName = _wcsdup( CA2W( hInfo.LNum.name, CP_UTF8 ) );
			m_col_info[crtColInfo].pTypeInfo = (ITypeInfo*)nullptr;
			m_col_info[crtColInfo].iOrdinal = crtColInfo+1;
			m_col_info[crtColInfo].dwFlags = DBCOLUMNFLAGS_ISFIXEDLENGTH;
			m_col_info[crtColInfo].ulColumnSize = sizeof(long);
			m_col_info[crtColInfo].wType = DBTYPE_I4;
			m_col_info[crtColInfo].bPrecision = 10;
			m_col_info[crtColInfo].bScale = 0xFF;
			m_col_info[crtColInfo].cbOffset = m_buf_size;
			memset( &( m_col_info[crtColInfo].columnid ), 0, sizeof( DBID ));
			m_buf_size += m_col_info[crtColInfo].ulColumnSize;
			++crtColInfo;

			m_col_info[crtColInfo].pwszName = _wcsdup( CA2W( hInfo.DisplayInfo.name, CP_UTF8 ) );
			m_col_info[crtColInfo].pTypeInfo = (ITypeInfo*)nullptr;
			m_col_info[crtColInfo].iOrdinal = crtColInfo+1;
			m_col_info[crtColInfo].dwFlags = DBCOLUMNFLAGS_ISFIXEDLENGTH;
			m_col_info[crtColInfo].ulColumnSize = sizeof(unsigned long);
			m_col_info[crtColInfo].wType = DBTYPE_UI4;
			m_col_info[crtColInfo].bPrecision = 10;
			m_col_info[crtColInfo].bScale = 0xFF;
			m_col_info[crtColInfo].cbOffset = m_buf_size;
			memset( &( m_col_info[crtColInfo].columnid ), 0, sizeof( DBID ));
			m_buf_size += m_col_info[crtColInfo].ulColumnSize;
			++crtColInfo;

			if ( nullptr != hInfo.PARENT_USCOREUNIQUE_USCORENAME ) {
				m_col_info[crtColInfo].pwszName = _wcsdup( CA2W( hInfo.PARENT_USCOREUNIQUE_USCORENAME->name, CP_UTF8 ) );
				m_col_info[crtColInfo].pTypeInfo = (ITypeInfo*)nullptr;
				m_col_info[crtColInfo].iOrdinal = crtColInfo+1;
				m_col_info[crtColInfo].dwFlags = DBCOLUMNFLAGS_ISFIXEDLENGTH;
				m_col_info[crtColInfo].ulColumnSize = MAX_BUF_SIZE;
				m_col_info[crtColInfo].wType = DBTYPE_WSTR;
				m_col_info[crtColInfo].bPrecision = 0xFF;
				m_col_info[crtColInfo].bScale = 0xFF;
				m_col_info[crtColInfo].cbOffset = m_buf_size;
				memset( &( m_col_info[crtColInfo].columnid ), 0, sizeof( DBID ));
				m_buf_size += m_col_info[crtColInfo].ulColumnSize;
				m_indirection[crtColInfo-1] = 0;
				++crtColInfo;
			}
			if ( nullptr != hInfo.MEMBER_USCORENAME ) {
				m_col_info[crtColInfo].pwszName = _wcsdup( CA2W( hInfo.MEMBER_USCORENAME->name, CP_UTF8 ) );
				m_col_info[crtColInfo].pTypeInfo = (ITypeInfo*)nullptr;
				m_col_info[crtColInfo].iOrdinal = crtColInfo+1;
				m_col_info[crtColInfo].dwFlags = DBCOLUMNFLAGS_ISFIXEDLENGTH;
				m_col_info[crtColInfo].ulColumnSize = MAX_BUF_SIZE;
				m_col_info[crtColInfo].wType = DBTYPE_WSTR;
				m_col_info[crtColInfo].bPrecision = 0xFF;
				m_col_info[crtColInfo].bScale = 0xFF;
				m_col_info[crtColInfo].cbOffset = m_buf_size;
				memset( &( m_col_info[crtColInfo].columnid ), 0, sizeof( DBID ));
				m_buf_size += m_col_info[crtColInfo].ulColumnSize;
				m_indirection[crtColInfo-1] = 1;
				++crtColInfo;
			}
			if ( nullptr != hInfo.MEMBER_USCORETYPE ) {
				m_col_info[crtColInfo].pwszName = _wcsdup( CA2W( hInfo.MEMBER_USCORETYPE->name, CP_UTF8 ) );
				m_col_info[crtColInfo].pTypeInfo = (ITypeInfo*)nullptr;
				m_col_info[crtColInfo].iOrdinal = crtColInfo+1;
				m_col_info[crtColInfo].dwFlags = DBCOLUMNFLAGS_ISFIXEDLENGTH;
				m_col_info[crtColInfo].ulColumnSize = sizeof(long);
				m_col_info[crtColInfo].wType = DBTYPE_I4;
				m_col_info[crtColInfo].bPrecision = 10;
				m_col_info[crtColInfo].bScale = 0xFF;
				m_col_info[crtColInfo].cbOffset = m_buf_size;
				memset( &( m_col_info[crtColInfo].columnid ), 0, sizeof( DBID ));
				m_buf_size += m_col_info[crtColInfo].ulColumnSize;
				m_indirection[crtColInfo-1] = 2;
				++crtColInfo;
			}

			for ( unsigned int j = 0, je = hInfo.__userProp.__size; j < je; ++j ) {
				m_col_info[crtColInfo].pwszName = _wcsdup( CA2W( hInfo.__userProp.__array[j].elementName, CP_UTF8 ) );
				m_col_info[crtColInfo].pTypeInfo = (ITypeInfo*)nullptr;
				m_col_info[crtColInfo].iOrdinal = crtColInfo+1;
				m_col_info[crtColInfo].dwFlags = DBCOLUMNFLAGS_ISFIXEDLENGTH;
				m_col_info[crtColInfo].ulColumnSize = MAX_BUF_SIZE;
				m_col_info[crtColInfo].wType = DBTYPE_WSTR;
				m_col_info[crtColInfo].bPrecision = 0xFF;
				m_col_info[crtColInfo].bScale = 0xFF;
				m_col_info[crtColInfo].cbOffset = m_buf_size;
				memset( &( m_col_info[crtColInfo].columnid ), 0, sizeof( DBID ));
				m_buf_size += m_col_info[crtColInfo].ulColumnSize;
				m_indirection[crtColInfo-1] = 3 + j;
				++crtColInfo;
			}
		}

		m_data_exchange = new char[ m_buf_size ];
	}

	inline const char& operator[]( size_t idx ) const
	{
		if ( idx >= (size_t)m_axis->Tuples.__size ) {
			throw std::runtime_error( "index out of bounds" );
		}

		const unsigned short member_size = ( m_col_info_cnt - 1 ) / m_axis_info->__size;
		xmlns__Tuple& crtTuple = m_axis->Tuples.Tuple[idx];
		DBLENGTH offset = 0;

		*( ( DBLENGTH *) m_data_exchange ) = idx;
		offset += m_col_info[0].ulColumnSize;

		size_t pos = 0;
		size_t length = m_boundaries.size() - 1;
		for ( unsigned short i = 1; i < m_col_info_cnt; ++i ) {
			if ( (pos < length) && (i == m_boundaries[pos+1] ) ) {
				++pos;
			}
			unsigned short idx = i- m_boundaries[pos];//( i-1 ) % member_size;
			unsigned short memberIdx = pos;//(i - 1 ) / member_size;
			switch ( idx ) {
			case 0://UName
				wcscpy_s(  ( wchar_t* )( m_data_exchange + offset ), m_col_info[i].ulColumnSize / 2, CA2W( crtTuple.Member[ memberIdx ].UName, CP_UTF8 ) );
				offset += m_col_info[i].ulColumnSize;
				break;
			case 1://Caption
				wcscpy_s(  ( wchar_t* )( m_data_exchange + offset ), m_col_info[i].ulColumnSize / 2, CA2W( crtTuple.Member[ memberIdx ].Caption, CP_UTF8 ) );
				offset += m_col_info[i].ulColumnSize;
				break;
			case 2://LName
				wcscpy_s(  ( wchar_t* )( m_data_exchange + offset ), m_col_info[i].ulColumnSize / 2, CA2W( crtTuple.Member[ memberIdx ].LName, CP_UTF8 ) );
				offset += m_col_info[i].ulColumnSize;
				break;
			case 3://LNum
				*( ( long* )( m_data_exchange + offset ) ) = atol( crtTuple.Member[ memberIdx ].LNum );
				offset += m_col_info[i].ulColumnSize;
				break;
			case 4://DisplayInfo
				*( ( unsigned long* )( m_data_exchange + offset ) ) = atol( crtTuple.Member[ memberIdx ].DisplayInfo );
				offset += m_col_info[i].ulColumnSize;
				break;
			default://optional
				{
					//unsigned short customIdx = m_indirection.at(idx);
					switch ( m_indirection.at( i - 1 ) ) {
					case 0://ParentUniqueName
						if ( nullptr != crtTuple.Member[ memberIdx ].PARENT_USCOREUNIQUE_USCORENAME ) {
							wcscpy_s(  ( wchar_t* )( m_data_exchange + offset ), m_col_info[i].ulColumnSize / 2, CA2W( crtTuple.Member[ memberIdx ].PARENT_USCOREUNIQUE_USCORENAME, CP_UTF8 ) );
						} else {
							*( ( wchar_t* )( m_data_exchange + offset ) ) = 0;
						}
						offset += m_col_info[i].ulColumnSize;
						break;
					case 1://MEMBER_USCORENAME
						if ( nullptr != crtTuple.Member[ memberIdx ].MEMBER_USCORENAME ) {
							wcscpy_s(  ( wchar_t* )( m_data_exchange + offset ), m_col_info[i].ulColumnSize / 2, CA2W( crtTuple.Member[ memberIdx ].MEMBER_USCORENAME, CP_UTF8 ) );
						}  else {
							*( ( wchar_t* )( m_data_exchange + offset ) ) = 0;
						}
						offset += m_col_info[i].ulColumnSize;
						break;
					case 2://MEMBER_USCORETYPE
						{
							static const std::string regular("MDMEMBER_TYPE_REGULAR");
							static const std::string all("MDMEMBER_TYPE_ALL");
							static const std::string measure("MDMEMBER_TYPE_MEASURE");
							static const std::string formula("MDMEMBER_TYPE_FORMULA");
							const char* type = crtTuple.Member[ memberIdx ].MEMBER_USCORETYPE;
							if ( regular == type ) {
								*( ( long* )( m_data_exchange + offset ) ) = MDMEMBER_TYPE_REGULAR;
							} else if ( all == type ){
								*( ( long* )( m_data_exchange + offset ) ) = MDMEMBER_TYPE_ALL;
							} else if ( measure == type ) {
								*( ( long* )( m_data_exchange + offset ) ) = MDMEMBER_TYPE_MEASURE;
							} else if ( formula == type ) {
								*( ( long* )( m_data_exchange + offset ) ) = MDMEMBER_TYPE_FORMULA;
							} else {//UNKNOWN
								*( ( long* )( m_data_exchange + offset ) ) = MDMEMBER_TYPE_UNKNOWN;
							}
							offset += m_col_info[i].ulColumnSize;
						}
						break;
					default://custom props
						{
							size_t crt_idx = m_indirection.at( i - 1 ) - 3;
							if ( crtTuple.Member[ memberIdx ].__userProp.__size <= crt_idx )
							{
								*( ( wchar_t* )( m_data_exchange + offset ) ) = 0;
							} else
							{
								wcscpy_s(  ( wchar_t* )( m_data_exchange + offset ), m_col_info[i].ulColumnSize / 2, CA2W( crtTuple.Member[ memberIdx ].__userProp.__array[crt_idx].value, CP_UTF8 ) );
							}
							offset += m_col_info[i].ulColumnSize;
						}
						break;
					};
				}
				break;
			}
		}
		return *m_data_exchange;
	}

//prerequisites for a row in the rowset

	inline size_t GetCount()
	{
		return m_axis ? m_axis->Tuples.__size : 0;
	}

	void RemoveAll()
	{
		clear();
	}

	ATLCOLUMNINFO* GetColumnInfo( DBORDINAL* pcInfo )
	{
		*pcInfo = m_col_info_cnt;
		return m_col_info;
	}

private:
	void clear()
	{
		if ( nullptr != m_col_info ) {
			for ( unsigned int i = 0; i < m_col_info_cnt; ++i ) {
				delete[] m_col_info[i].pwszName;
			}
			delete[] m_col_info;
		}

		if ( nullptr != m_data_exchange ) {
			delete[] m_data_exchange;
		}

		m_col_info = nullptr;
		m_data_exchange = nullptr;
		m_axis = nullptr;
		m_boundaries.clear();
	}
};


