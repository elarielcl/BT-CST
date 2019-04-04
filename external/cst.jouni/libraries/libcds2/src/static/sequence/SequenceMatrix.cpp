/* SequenceMatrix.cpp
 * Copyright (C) 2014, Alberto Ordóñez
 *
 * SequenceMatrix definition
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

#include <sequence/SequenceMatrix.h>
#include <assert.h>

namespace cds_static
{

	void SequenceMatrix::build(uint *data, size_t _n, BitSequenceBuilder * bmb, Mapper * am, bool deleteSymbols){
		
		bmb->use();
		this->n = _n;
		this->length = _n;
		this->am = am;
		am->use();
		max_v = 0;		
		for(size_t i = 0; i < n; i++){
			data[i] = am->map(data[i]);
			max_v = max(max_v,data[i]);
		}

		uint **_bm; 
		_bm = new uint*[max_v+1];
		for (uint i=0;i<=max_v;i++)
			createEmptyBitmap(_bm+i,this->n);
		for (size_t i=0;i<this->n;i++){
			bitset(_bm[data[i]],i);
		}

		bitstring = new BitSequence*[max_v+1];
		for (uint i=0;i<=max_v;i++){
			bitstring[i] = bmb->build(_bm[i],this->n);
			delete [] _bm[i];
		}
		delete [] _bm;

		if (!deleteSymbols){
			for(size_t i = 0; i < n; i++){
				data[i] = am->unmap(data[i]);
			}			
		}
		bmb->unuse();
	}
	
	SequenceMatrix::SequenceMatrix(const Array & a, BitSequenceBuilder * bmb, Mapper * am) : Sequence(a.getLength()) {
		uint *symbols = new uint[a.getLength()];
		for (size_t i=0;i<a.getLength();i++)
			symbols[i] = a[i];
		build(symbols,a.getLength(),bmb,am,true);
		delete [] symbols;
	}

	SequenceMatrix::SequenceMatrix(uint * symbols, size_t n, BitSequenceBuilder * bmb, Mapper * am, bool deleteSymbols) : Sequence(n) {
		build(symbols,n,bmb,am,deleteSymbols);
	}
	
	SequenceMatrix::SequenceMatrix():Sequence(0) {
		bitstring = NULL;
		am = NULL;
	}

	SequenceMatrix::~SequenceMatrix() {
		if (bitstring) {
			for (uint i = 0; i < max_v; i++)
				if (bitstring[i])
					delete bitstring[i];
			delete [] bitstring;
		}
		if (am)
			am->unuse();
	}

	void SequenceMatrix::save(ofstream & fp) const
	{
		uint wr = SEQUENCE_MATRIX_HDR;
		saveValue(fp, wr);
		saveValue<size_t>(fp,n);
		saveValue(fp, max_v);
		am->save(fp);
		for (uint i = 0; i <= max_v; i++)
			bitstring[i]->save(fp);
	}

	SequenceMatrix * SequenceMatrix::load(ifstream & fp) {
		uint rd = loadValue<uint>(fp);
		if (rd != SEQUENCE_MATRIX_HDR) return NULL;
		SequenceMatrix * ret = new SequenceMatrix();
		ret->n = loadValue<size_t>(fp);
		ret->max_v = loadValue<uint>(fp);
		ret->am = Mapper::load(fp);
		if (ret->am == NULL) {
			delete ret;
			return NULL;
		}
		ret->am->use();
		ret->bitstring = new BitSequence*[ret->max_v+1];
		for(uint i = 0; i <=ret->max_v; i++)
			ret->bitstring[i] = NULL;
		for(uint i = 0; i <= ret->max_v; i++) {
			ret->bitstring[i] = BitSequence::load(fp);
			if (ret->bitstring[i] == NULL) {
				delete ret;
				return NULL;
			}
		}
		return ret;
	}
	
	uint SequenceMatrix::access(size_t pos) const
	{
		uint ret=0;
		while(!bitstring[ret]->access(pos))ret++;
		return am->unmap(ret);
	}

	uint SequenceMatrix::access(size_t pos, size_t &r) const
	{
		//cerr << "Matrix access" << endl;
		uint ret=0;
		while(!bitstring[ret]->access(pos))ret++;
		r = bitstring[ret]->rank1(pos);
		return am->unmap(ret);
		
	}

	size_t SequenceMatrix::rank(uint symbol, size_t pos) const
	{
		//cerr << "Matrix rank" << endl;
		symbol = am->map(symbol);;
		assert(symbol<=max_v);
		size_t ret = bitstring[symbol]->rank1(pos);
		//cerr << "rank("<<symbol<<","<<pos<<"): "<<ret << endl;
		return ret;
	}

	size_t SequenceMatrix::select(uint symbol, size_t j) const
	{
		//cerr << "Matrix select" << endl;
		symbol = am->map(symbol);
		assert(symbol<=max_v);
		size_t ret = bitstring[symbol]->select1(j);	
		//cerr << "select("<<symbol<<","<<j<<"): "<<ret << endl;
		return ret;
	}

	size_t SequenceMatrix::getSize() const
	{
		size_t ptrs = sizeof(SequenceMatrix) + (max_v+1) * sizeof(BitSequence*);
		size_t bytesBitstrings = 0;
		for(uint i = 0; i <=max_v; i++)
			bytesBitstrings += bitstring[i]->getSize();
		
		return bytesBitstrings + ptrs + am->getSize();
	}

	bool SequenceMatrix::selfTest(){
		for (size_t i=0;i<this->n;i++){
			uint j;
			for (j=0;j<=max_v;j++){
				if (bitstring[j]->access(i))
					break;
			}
			assert(j<=max_v);
		}
		return true;
	}
	
};
