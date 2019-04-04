/* ArrayDA.cpp
 * Copyright (C) 2009, Francisco Claude, all rights reserved.
 *
 * ArrayDA interface
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <direct_access/ArrayDA.h>

namespace cds_static
{

	ArrayDA::ArrayDA(size_t n, uint _maxValue) : DirectAccess(n){
		if (len==0) return;
		maxValue = _maxValue;
		initData();
	}

	ArrayDA::ArrayDA(uint * A, size_t n, uint bpe) : DirectAccess(n) {
		if (len==0) return;
		maxValue = 0;
		if(bpe==0) {
			for(size_t k=0;k<n;k++)
				maxValue = max(maxValue,A[k]);
		}
		else {
			maxValue = (1<<(bpe))-1;
		}
		
		initData();
		assert(bpe==0 || bitsPerItem==bpe);
		for(size_t k=0;k<n;k++) {
			assert(A[k] <= maxValue);
			setField(k, A[k]);
		}
	}

	ArrayDA::~ArrayDA() {
		if (len!=0)
		delete [] data;
	}


	ArrayDA::ArrayDA(ifstream & input) {
		uint rw = loadValue<uint>(input);
		if (rw!=ARRAY_DA_HDR) return;
		len = loadValue<size_t>(input);
		if (len==0)	return;
		maxValue = loadValue<uint>(input);
		bitsPerItem = loadValue<uint>(input);
		uintLength = loadValue<uint>(input);
		data = loadValue<uint>(input,uintLength);
	}

	
	void ArrayDA::save(ofstream & out) const
	{
		uint rw = ARRAY_DA_HDR;
		saveValue(out, rw);
		saveValue(out,len);
		if (len==0)	return;
		saveValue(out,maxValue);
		saveValue(out,bitsPerItem);
		saveValue(out,uintLength);
		saveValue(out,data,uintLength);
	}

	void ArrayDA::initData() {
		bitsPerItem = bits(maxValue);
		uintLength = uint_len(len,bitsPerItem);
		data = new uint[uintLength];
		for(size_t i=0;i<uintLength;i++)
			data[i] = 0;
	}

	inline size_t ArrayDA::getSize() const
	{
		if (len==0) return sizeof(this);
		return sizeof(uint)*uintLength+sizeof(this);
	}
};
