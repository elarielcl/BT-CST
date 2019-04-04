/* BitSequenceBRW32.cpp
   Copyright (C) 2005, Rodrigo Gonzalez, all rights reserved.

   New RANK, SELECT, SELECT-NEXT and SPARSE RANK implementations.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include <bitsequence/BitSequenceBRW32.h>
#include <cassert>
#include <cmath>
// #include <sys/types.h>


/////////////
//Rank(B,i)// 
/////////////
//_factor = 0  => s=W*lgn
//_factor = P  => s=W*P
//Is interesting to notice
//factor=2 => overhead 50%
//factor=3 => overhead 33%
//factor=4 => overhead 25%
//factor=20=> overhead 5%

namespace cds_static{

BitSequenceBRW32::BitSequenceBRW32(){
  data=NULL;
//  this->owner = true;
  this->n=0;
  this->factor=0;
}

BitSequenceBRW32::BitSequenceBRW32( uint *bitarray, uint _n, uint _factor){
  /*cout << "*****" << endl;
  cout << bitarray << endl;
  cout << _n << endl;
  cout << _factor << endl; */
  if(_factor==0) exit(-1);
  data=new uint[_n/W+1];
  for(uint i=0;i<uint_len(_n,1);i++)
    data[i] = bitarray[i];
  for(uint i=uint_len(_n,1);i<_n/W+1;i++)
    data[i] = 0;
  //this->owner = true;
  this->n=_n;
  uint lgn=bits(n-1);
  this->factor=_factor;
  if (_factor==0) this->factor=lgn;
  else this->factor=_factor;
  b=32;
  s=b*this->factor;
  integers = n/W+1;
  BuildRank();
  //~ this->len = n;
  this->ones = rank1(n-1);
}

BitSequenceBRW32::~BitSequenceBRW32() {
  delete [] Rs;
  delete [] data;
}

//Metodo que realiza la busqueda d
void BitSequenceBRW32::BuildRank(){
  uint num_sblock = n/s;
  Rs = new uint[num_sblock+5];// +1 pues sumo la pos cero
  for(uint i=0;i<num_sblock+5;i++)
    Rs[i]=0;
  uint j;
  Rs[0]=0;
  for (j=1;j<=num_sblock;j++) {
    Rs[j]=Rs[j-1];
    Rs[j]+=BuildRankSub((j-1)*factor,factor);
  }
}

uint BitSequenceBRW32::BuildRankSub(uint ini,uint bloques){
  uint rank=0,aux;
  for(uint i=ini;i<ini+bloques;i++) {
    if (i < integers) {
      aux=data[i];
      rank+=popcount(aux);
    }
  }
  return rank; //retorna el numero de 1's del intervalo

}

size_t BitSequenceBRW32::rank0(size_t i) const{
  return i-rank1(i)+1;
}

size_t BitSequenceBRW32::rank1(size_t i) const{
  ++i;
  size_t resp=Rs[i/s];
  uint aux=(i/s)*factor;
  for (uint a=aux;a<i/W;a++)
    resp+=popcount(data[a]);
  resp+=popcount(data[i/W]  & ((1<<(i & mask31))-1));
  return resp;
}

bool BitSequenceBRW32::access(const size_t i) const{
  return (1u << (i % W)) & data[i/W];
}

bool BitSequenceBRW32::access(const size_t i,size_t &r) const {
	cerr << "Not implemented yet"<<endl;
	r=-1;
	return (1u << (i % W)) & data[i/W];
}

void BitSequenceBRW32::save(ofstream & f) const {
  uint wr = BRW32_HDR;
  saveValue(f,wr);
  saveValue(f,n);
  saveValue(f,factor);
  saveValue(f,data,n/W+1);
  saveValue(f,Rs,n/s+1);
  //TODO
  //~ if (f == NULL) return 20;
  //~ if( fwrite(&wr,sizeof(uint),1,f) != 1 ) return -1;
  //~ if (fwrite (&n,sizeof(uint),1,f) != 1) return 21;
  //~ if (fwrite (&factor,sizeof(uint),1,f) != 1) return 21;
  //~ if (fwrite (data,sizeof(uint),n/W+1,f) != n/W+1) return 21;
  //~ if (fwrite (Rs,sizeof(uint),n/s+1,f) != n/s+1) return 21;
}

BitSequence * BitSequenceBRW32::load(ifstream  &f) {
  BitSequenceBRW32 *ret = NULL;
  try
	{
		ret = new BitSequenceBRW32();
		uint type = loadValue<uint>(f);
		if(type!=BRW32_HDR) {
			abort();
		}
		ret->n= loadValue<uint>(f);
		ret->b=32;
		ret->factor=loadValue<uint>(f);
		ret->s=ret->b*ret->factor;
		uint aux=(ret->n+1)%W;
		if (aux != 0)
			ret->integers = (ret->n+1)/W+1;
		else
			ret->integers = (ret->n+1)/W;
		ret->data = new uint[ret->n/W+1];
		if (!ret->data) return NULL;
		ret->data = loadValue<uint>(f,ret->n/W+1);
		ret->Rs= new uint[ret->n/ret->s+1];
		if (!ret->Rs) return NULL;
		ret->Rs=loadValue<uint>(f,ret->n/ret->s+1);
		ret->ones = ret->rank1(ret->n-1);
		return ret;
	}
	catch(exception e) {
		delete ret;
	}
	return NULL;
}

uint BitSequenceBRW32::SpaceRequirementInBits() const{
  return uint_len(n,1)*sizeof(uint)*8+(n/s)*sizeof(uint)*8 +sizeof(BitSequenceBRW32)*8; 
}

size_t BitSequenceBRW32::getSize()  const{
  return sizeof(BitSequenceBRW32)+SpaceRequirementInBits()/8;
}

uint BitSequenceBRW32::SpaceRequirement() const{
  return n/8+(n/s)*sizeof(uint)+sizeof(BitSequenceBRW32); 
}

uint BitSequenceBRW32::prev2(uint start) {
      // returns the position of the previous 1 bit before and including start.
      // tuned to 32 bit machine

      uint i = start >> 5;
      int offset = (start % W);
      uint answer = start;
      uint val = data[i] << (Wminusone-offset);

      if (!val) { val = data[--i]; answer -= 1+offset; }

      while (!val) { val = data[--i]; answer -= W; }

      if (!(val & 0xFFFF0000)) { val <<= 16; answer -= 16; }
      if (!(val & 0xFF000000)) { val <<= 8; answer -= 8; }

      while (!(val & 0x80000000)) { val <<= 1; answer--; }
      return answer;
}

uint BitSequenceBRW32::prev(uint start) {
      // returns the position of the previous 1 bit before and including start.
      // tuned to 32 bit machine
  
      uint i = start >> 5;
      int offset = (start % W);
      uint aux2 = data[i] & (-1u >> (31-offset));
  
      if (aux2 > 0) {
                if ((aux2&0xFF000000) > 0) return i*W+23+prev_tab[(aux2>>24)&0xFF];
                else if ((aux2&0xFF0000) > 0) return i*W+15+prev_tab[(aux2>>16)&0xFF];
                else if ((aux2&0xFF00) > 0) return i*W+7+prev_tab[(aux2>>8)&0xFF];
                else  return i*W+prev_tab[aux2&0xFF]-1;
      }
      for (uint k=i-1;;k--) {
         aux2=data[k];
         if (aux2 > 0) {
                if ((aux2&0xFF000000) > 0) return k*W+23+prev_tab[(aux2>>24)&0xFF];
                else if ((aux2&0xFF0000) > 0) return k*W+15+prev_tab[(aux2>>16)&0xFF];
                else if ((aux2&0xFF00) > 0) return k*W+7+prev_tab[(aux2>>8)&0xFF];
                else  return k*W+prev_tab[aux2&0xFF]-1;
         }
      }
      return 0;
} 

uint BitSequenceBRW32::next(uint k) {
        uint count = k;
        uint des,aux2;
        des=count%W; 
        aux2= data[count/W] >> des;
        if (aux2 > 0) {
                if ((aux2&0xff) > 0) return count+select_tab[aux2&0xff]-1;
                else if ((aux2&0xff00) > 0) return count+8+select_tab[(aux2>>8)&0xff]-1;
                else if ((aux2&0xff0000) > 0) return count+16+select_tab[(aux2>>16)&0xff]-1;
                else {return count+24+select_tab[(aux2>>24)&0xff]-1;}
        }

        for (uint i=count/W+1;i<integers;i++) {
                aux2=data[i];
                if (aux2 > 0) {
                        if ((aux2&0xff) > 0) return i*W+select_tab[aux2&0xff]-1;
                        else if ((aux2&0xff00) > 0) return i*W+8+select_tab[(aux2>>8)&0xff]-1;
                        else if ((aux2&0xff0000) > 0) return i*W+16+select_tab[(aux2>>16)&0xff]-1;
                        else {return i*W+24+select_tab[(aux2>>24)&0xff]-1;}
                }
        }
        return n;
} 

size_t BitSequenceBRW32::select1(const size_t x1) const {
  // returns i such that x=rank(i) && rank(i-1)<x or n if that i not exist
  // first binary search over first level rank structure
  // then sequential search using popcount over a int
  // then sequential search using popcount over a char
  // then sequential search bit a bit
 size_t x = x1;
  //binary search over first level rank structure
  uint l=0, r=n/s;
  uint mid=(l+r)/2;
  uint rankmid = Rs[mid];
  while (l<=r) {
    if (rankmid<x)
      l = mid+1;
    else
      r = mid-1;
    mid = (l+r)/2;
    rankmid = Rs[mid];
  }
  //sequential search using popcount over a int
  uint left;
  left=mid*factor;
  x-=rankmid;
        uint j=data[left];
        uint ones = popcount(j);
        while (ones < x) {
    x-=ones;left++;
    if (left > integers) return n;
          j = data[left];
      ones = popcount(j);
        }
  //sequential search using popcount over a char
  left=left*b;
  rankmid = popcount8(j);
  if (rankmid < x) {
    j=j>>8;
    x-=rankmid;
    left+=8;
    rankmid = popcount8(j);
    if (rankmid < x) {
      j=j>>8;
      x-=rankmid;
      left+=8;
      rankmid = popcount8(j);
      if (rankmid < x) {
        j=j>>8;
        x-=rankmid;
        left+=8;
      }
    }
  }

  // then sequential search bit a bit
        while (x>0) {
    if  (j&1) x--;
    j=j>>1;
    left++;
  }
  return left-1;
}

size_t BitSequenceBRW32::select0(const size_t x1) const{
  // returns i such that x=rank_0(i) && rank_0(i-1)<x or n if that i not exist
  // first binary search over first level rank structure
  // then sequential search using popcount over a int
  // then sequential search using popcount over a char
  // then sequential search bit a bit
	size_t x = x1;
  //binary search over first level rank structure
  if(x==0) return 0;
  uint l=0, r=n/s;
  uint mid=(l+r)/2;
  uint rankmid = mid*factor*W-Rs[mid];
  while (l<=r) {
    if (rankmid<x)
      l = mid+1;
    else
      r = mid-1;
    mid = (l+r)/2;
    rankmid = mid*factor*W-Rs[mid];
  }
  //sequential search using popcount over a int
  uint left;
  left=mid*factor;
  x-=rankmid;
  uint j=data[left];
  uint zeros = W-popcount(j);
  while (zeros < x) {
    x-=zeros;left++;
    if (left > integers) return n;
    j = data[left];
    zeros = W-popcount(j);
  }
  //sequential search using popcount over a char
  left=left*b;
  rankmid = 8-popcount8(j);
  if (rankmid < x) {
    j=j>>8;
    x-=rankmid;
    left+=8;
    rankmid = 8-popcount8(j);
    if (rankmid < x) {
      j=j>>8;
      x-=rankmid;
      left+=8;
      rankmid = 8-popcount8(j);
      if (rankmid < x) {
        j=j>>8;
        x-=rankmid;
        left+=8;
      }
    }
  }

  // then sequential search bit a bit
  while (x>0) {
    if  (j%2 == 0 ) x--;
    j=j>>1;
    left++;
  }
  left--;
  if (left > n)  return n;
  else return left;
}

}
