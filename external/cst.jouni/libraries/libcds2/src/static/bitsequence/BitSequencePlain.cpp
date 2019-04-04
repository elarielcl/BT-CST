/* BitSequencePlain.cpp
 * Copyright (C) 2012, Alberto Ordóñez, all rights reserved.
 *
 * plain Bitsequence - don't use, only for testing
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

#include <bitsequence/BitSequencePlain.h>
namespace cds_static
{


uint BitSequencePlain::popcount (const uint x) const{ 
  uint val =  popc[x&0xFF] + popc[(x>>8)&0xFF] + popc[(x>>16)&0xFF] + popc[x>>24];
  return val; 
}
  

BitSequencePlain::BitSequencePlain(uint * string, uint n, uint samplingSelect) {
     this->fastSelect0=false;
     this->fastSelect1=false; 
     build(string,n,samplingSelect);
}

BitSequencePlain::BitSequencePlain(uint * string, uint n, BuilderSequencePlain &b){
  this->fastSelect0=b.fastselect0_;
  this->fastSelect1=b.fastselect1_;
  this->select1sampling=b.sonesamp_;
  this->select0sampling=b.szerosamp_;
  
  build(string,n,b.samplingRate);
}


BitSequencePlain::BitSequencePlain(BuilderSequencePlain &b) {
  this->fastSelect0=b.fastselect0_;
  this->fastSelect1=b.fastselect1_;
  this->select1sampling=b.sonesamp_;
  this->select0sampling=b.szerosamp_;
  build(b.bs_,b.len_,b.samplingRate);
}

void BitSequencePlain::build(uint * string, uint _n, uint samplingSelect){
    
    uint i,j,pop,bpop,pos;
     uint words;
     this->length = _n;
     this->n = _n; 

     words = (n+W-1)/W;
     integers = n/W+1;
     b=32;
     factor = 256;
     //~ ns = (n+factor-1)/factor; nb = factor/W; // adjustments
     ns = n/factor+1; nb = factor/W; // adjustments

     //~ cout << "total uints: " << uint_len(n,1) << endl; 
     data = new uint[uint_len(n,1)]; 
     for (uint i=0;i<uint_len(n,1);i++)data[i]=string[i];
     //~ data = string;


     bdata = new uchar [ns*nb];
     //~ cout << "ns*nb: " << ns*nb << endl; 
     sdata = new uint[ns];
   //~ cout << "ns: " << ns << endl; 
     pop = 0; pos = 0;
     this->ones = 0; 
     uint pointer=0;
     for (i=0;i<ns;i++)
  { bpop = 0;
    sdata[i] = pop;
    for (j=0;j<nb;j++)
       { if (pos == words) break;
         bdata[pos++] = bpop;
         bpop += popcount(*string++);
         pointer++;
       }
    pop += bpop;
  }
  for (;j<nb;j++){
    bdata[pos++]=bpop;
  }
  //~ cout << "pointer: " << pointer << endl; 
  this->ones = pop; 

  //if fast0select or fast1select
  if (fastSelect1) {
      // cout << "entered here" << endl;
      this->Stones = new uint[ones/select1sampling+1];
      Stones[0] = 0;
      for (size_t i = 0 ; i <= ones;i+=select1sampling) {
        if (i==0) continue;
        Stones[i/select1sampling] = this->select1_s(i);
      }
    }
    if (fastSelect0) {
      this->Stzero = new uint[(n-ones)/select0sampling+1];
      Stzero[0] = 0;
      for (size_t i = 0 ; i <= (n-ones);i+=select0sampling) {
        if (i==0) continue;
        Stzero[i/select0sampling] = this->select0_s((i));
      }
    }  
}

BitSequencePlain::BitSequencePlain(const BitString & bs) {
 
   n = bs.getLength();
     uint i,j=0,pop,bpop=0,pos;
     uint words;
     uint *dataBS = bs.getData();
     //~ cout << "total uints: " << uint_len(n,1) << endl; 
     data = new uint[uint_len(n,1)]; 
     for (uint i=0;i<uint_len(n,1);i++)data[i]=dataBS[i];
     //~ data = string;
     this->length = n; 
     words = (n+W-1)/W;
     integers = n/W+1;
     b=32;
     
     factor = 256;// bits(n-1);
     //~ ns = (n+factor-1)/factor; nb = factor/W; // adjustments
     ns = n/factor+1; nb = factor/W; // adjustments

     bdata = new uchar [ns*nb];
     //~ cout << "ns*nb: " << ns*nb << endl; 
     sdata = new uint[ns];
   //~ cout << "ns: " << ns << endl; 
     pop = 0; pos = 0;
     this->ones = 0; 
     uint pointer=0;
     for (i=0;i<ns;i++)
  { bpop = 0;
    sdata[i] = pop;
    for (j=0;j<nb;j++)
       { if (pos == words) break;
         bdata[pos++] = bpop;
         bpop += popcount(*dataBS++);
         pointer++;
       }
    pop += bpop;
  }
  for (;j<nb;j++){
    bdata[pos++]=bpop;
  }
  //~ cout << "ns*nb: " << ns*nb << endl;
  //~ cout << "pos: " << pos << endl;
  //~ cout << "pointer: " << pointer << endl; 
  this->ones = pop; 

}
BitSequencePlain::BitSequencePlain(){

}

BitSequencePlain::~BitSequencePlain() {
  delete [] data; 
  delete [] sdata; 
  delete [] bdata; 
}


size_t BitSequencePlain::rank1(const size_t i) const {
  uint ii=i+1; 
  if (ii==this->length) return this->ones;
  return (size_t)sdata[ii>>8] + 
  bdata[ii>>5] + 
  popcount (data[ii>>5] & ((1<<(ii&0x1F))-1));
 
}

size_t BitSequencePlain::rank0(const size_t i) const {
  return i - rank1(i) +1; 
}

size_t BitSequencePlain::select0(size_t i) const {
   if (fastSelect0){
     return select0_t(i);
   }else{
    return select0_s(i);
   }
}

size_t BitSequencePlain::select1(size_t i) const{
     if (fastSelect1){
       return select1_t(i);
     }else{
      return select1_s(i);
     }
}

size_t BitSequencePlain::select1_t(size_t x) const{
  if(x>ones) return (uint)(-1);
  // cout << "select1sampling = " << select1sampling << endl;
  uint pos = (x/select1sampling);
  register int bits_restantes = 0;
  uint s = factor;
  uint r = Stones[pos]/s;
  // cout << "pos = " << pos << endl;
  uint ini = 0;
  if (s < select1sampling) {
    size_t incr = 1;
    while(bdata[r+incr] < x) {
      incr = incr*2;
      if (r+incr > n/s)
        break;
    }
    r = r+incr/2-1;
    // cout << "r = " << r << endl;
    while(bdata[r+1] < x) {
      r = r+1;
      if (r >= n/s) {
        r=r-1;
        break; 
      }
    }
  }
  if (pos !=0) {
    if (Stones[pos] >= r*s) {
      ini = Stones[pos]+1;
      bits_restantes = x - pos*select1sampling;
    } else {
      // cout << "here2!" << endl;
      ini = r*factor*W;
      bits_restantes = x-bdata[r];
    }
  } else{ 
    bits_restantes = x;
    ini = 0;
  }
  uint posini = ini/32;
  // cout << "posini = " << posini << endl;
  uint dataj = data[posini];
  dataj = dataj >> ini%32;
    uint ones = __builtin_popcount(dataj);
  while (ones < bits_restantes) {
    bits_restantes-=ones;
    posini++;
    if (posini > integers) return n;
    dataj = data[posini];     
    ones = __builtin_popcount(dataj);
    ini = posini*32;
  }
  uint rankmid = __builtin_popcount(dataj&0xff);
  if (rankmid < bits_restantes) {
    dataj=dataj>>8;
    bits_restantes-=rankmid;
    ini+=8;
    rankmid = __builtin_popcount(dataj&0xff);
    if (rankmid < bits_restantes) {
      dataj=dataj>>8;
      bits_restantes-=rankmid;
      ini+=8;
      rankmid = __builtin_popcount(dataj&0xff);
      if (rankmid < bits_restantes) {
        dataj=dataj>>8;
        bits_restantes-=rankmid;
        ini+=8;
      }
    }
  }
  while (bits_restantes>0) {
    if (dataj&1) bits_restantes--;
    dataj=dataj>>1;
    ini++;
  }
  return ini-1;
}

size_t BitSequencePlain::select1_s(size_t i) const {
if (i==0) return 0;
  if(i>this->ones) return (size_t)(-1);
  
  // returns i such that x=rank(i) && rank(i-1)<x or n if that i not exist
  // first binary search over first level rank structure
  // then sequential search using popcount over a int
  // then sequential search using popcount over a char
  // then sequential search bit a bit
      //~ for (uint bla=0;bla<ns;bla++) cout << "sb["<<bla<<"]: " << sdata[bla] << endl; 
      //~ cout << "sb["<<ns-1<<"]: " << sdata[ns-1] << endl; 
    //~ for (uint bla=(ns-1)*nb;bla<ns*nb;bla++){
     //~ cout << "bdata[" << bla << "]: " << (uint)bdata[bla] << endl; 
//~ 
    //~ }

  uint l=0, r=ns-1;
  uint mid=(l+r)/2;
  uint rankmid = sdata[mid];
  while (l<=r) {
    if (rankmid<i)
      l = mid+1;
    else
      r = mid-1;
    mid = (l+r)/2;
    rankmid = sdata[mid];
  }
  //~ if (mid==ns){ mid--; rankmid=sdata[mid];}
  l=mid*nb, r=(mid+1)*nb-1; 
  i-=rankmid; 
    mid=(l+r)/2;
    rankmid = bdata[mid];
  while (l<=r) {
    if (rankmid<i)
      l = mid+1;
    else
      r = mid-1;
    mid = (l+r)/2;
    rankmid = bdata[mid];
  }
  //~ if (mid==rr){ rankmid=bdata[rr-1];mid--;}
  //sequential search using popcount over a int
  uint left;
  left=mid;
  i-=rankmid;
  uint j=data[left];
  uint ones = popcount(j);
  while (ones < i) {
    i-=ones;left++;
    if (left > integers)return this->length;
    j = data[left];
    ones = popcount(j);
  }
  //sequential search using popcount over a char
  left=left*b;
  rankmid = popcount8(j);
  if (rankmid < i) {
    j=j>>8;
    i-=rankmid;
    left+=8;
    rankmid = popcount8(j);
    if (rankmid < i) {
      j=j>>8;
      i-=rankmid;
      left+=8;
      rankmid = popcount8(j);
      if (rankmid < i) {
        j=j>>8;
        i-=rankmid;
        left+=8;
      }
    }
  }

  // then sequential search bit a bit
  while (i>0) {
    if  (j&1) i--;
    j=j>>1;
    left++;
  }
  return left-1;
}

size_t BitSequencePlain::select0_t(size_t x) const {
  
  if(x>n-ones) return (uint)(-1);
  uint pos = (x/select0sampling);
  int bits_restantes = 0;
  uint ini = 0;
  uint s = factor;//TODO check this
  uint r = Stzero[pos]/s;
  // cout << "=================" << endl;
  // cout << "x \t = " << x << endl;
  // cout << "pos \t = " << pos << endl;

  if (s < select0sampling) {
    size_t incr = 1;
    while(((r+incr)*s-bdata[r+incr]) < x) {
      incr = incr*2;
      if (r+incr > n/s)
        break;
    }
    r = r+incr/2-1;
    // cout << "r = \t" << r << endl;
    while((((r+1)*s)-bdata[r+1]) < x) {
      r = r+1;
      if (r >= n/s) {
        r=r-1;
        break; 
      }
    }
  }
  if (pos !=0) {
    if (Stzero[pos] >= r*s) {
      ini = Stzero[pos]+1;
      bits_restantes = x - pos*select0sampling;
    } else {
      // cout << "aca!" << endl;
      // cout << "St \t = " << Stzero[pos] << endl;
      // cout << "r*s \t = " << r << endl;
      ini = r*factor*W;
      bits_restantes = x-(r*s-bdata[r]);
    }
  } else{ 
    ini = 0;
    bits_restantes = x;
  }
  // cout << "ini \t = " << ini << endl;

  uint posini = ini/32;
  uint dataj = data[posini];
  uint mod = ini%32;
  dataj = dataj >> mod;
    uint zeros = 32 - __builtin_popcount(dataj) - mod ;
  // cout << "posini \t  = " << posini << endl;

  while (zeros < bits_restantes) {
    bits_restantes-=zeros;
    posini++;
    if (posini > integers) return n;
    dataj = data[posini];     
    zeros = W - __builtin_popcount(dataj);
    ini = posini*32;
  }

  uint rankmid = 8 - popcount8(dataj);
  if (rankmid < bits_restantes) {
    dataj=dataj>>8;
    bits_restantes-=rankmid;
    ini+=8;
    rankmid = 8 - popcount8(dataj);
    if (rankmid < bits_restantes) {
      // cout << "here3" << endl;
      dataj=dataj>>8;
      bits_restantes-=rankmid;
      ini+=8;
      rankmid = 8 - popcount8(dataj);
      if (rankmid < bits_restantes) {
        // cout << "here4" << endl;
        dataj=dataj>>8;
        bits_restantes-=rankmid;
        ini+=8;
      }
    }
  }
  while (bits_restantes>0) {
    if  (dataj%2 == 0 ) bits_restantes--;
    dataj=dataj>>1;
    ini++;
  }
  ini--;
  // cout << "ini \t  = " << ini << endl;
  if (ini > n+1)  return n;
  else  return ini;
}

size_t BitSequencePlain::select0_s(size_t i) const {
  if(i==0) return 0;
  // returns i such that x=rank(i) && rank(i-1)<x or n if that i not exist
  // first binary search over first level rank structure
  // then sequential search using popcount over a int
  // then sequential search using popcount over a char
  // then sequential search bit a bit
      //~ for (uint bla=0;bla<ns;bla++) cout << "sb["<<bla<<"]: " << bla*nb*W-sdata[bla] << endl; 
    //~ for (uint bla=0;bla<ns*nb;bla++){
     //~ cout <<"bdata[" << bla << "]: " << (uint)bdata[bla] << ", bdata[" << bla << "]: " << (uint)bla*W-bdata[bla] << endl; 
//~ 
    //~ }

  uint l=0, r=ns-1;
  uint mid=(l+r)/2;
  uint rankmid = mid*nb*W - sdata[mid];
  while (l<=r) {
    if (rankmid<i)
      l = mid+1;
    else
      r = mid-1;
    mid = (l+r)/2;
    rankmid = mid*nb*W - sdata[mid];
  }
  //~ if (mid==ns){ mid--; rankmid=sdata[mid];}
  l=mid*nb, r=(mid+1)*nb-1; 
  i-=rankmid; 
    mid=(l+r)/2;
    rankmid = (mid%nb)*W - bdata[mid];
  while (l<=r) {
    if (rankmid<i)
      l = mid+1;
    else
      r = mid-1;
    mid = (l+r)/2;
    rankmid = (mid%nb)*W - bdata[mid];
  }
  //~ if (mid==rr){ rankmid=bdata[rr-1];mid--;}
  //sequential search using popcount over a int
  uint left;
  left=mid;
  i-=rankmid;
  uint j=data[left];
  uint ones = W-popcount(j);
  while (ones < i) {
    i-=ones;left++;
    if (left > integers) return this->length;
    j = data[left];
    ones = W-popcount(j);
  }
  //sequential search using popcount over a char
  left=left*b;
  rankmid = 8-popcount8(j);
  if (rankmid < i) {
    j=j>>8;
    i-=rankmid;
    left+=8;
    rankmid = 8-popcount8(j);
    if (rankmid < i) {
      j=j>>8;
      i-=rankmid;
      left+=8;
      rankmid = 8-popcount8(j);
      if (rankmid < i) {
        j=j>>8;
        i-=rankmid;
        left+=8;
      }
    }
  }

  // then sequential search bit a bit
  while (i>0) {
    if  ((j&1)==0) i--;
    j=j>>1;
    left++;
  }
  left--;
  if (left > this->length)  return this->length;
  else return left;
}

bool BitSequencePlain::access(const size_t i) const {
  return ((1u << (i % W)) & data[i/W]) !=0;
}

BitSequencePlain *  BitSequencePlain::load(ifstream & fp){
  if (&fp == NULL) return NULL;
  BitSequencePlain *br = new BitSequencePlain();
  uint type;
  type = loadValue<uint>(fp); 
  if(type!=PLAIN_HDR) return NULL; 
  br->length = loadValue<size_t>(fp);              
  br->n = br->length;
  br->factor = loadValue<uint>(fp);
  br->integers = loadValue<uint>(fp);
  br->b=loadValue<uint>(fp);
  br->ns = loadValue<uint>(fp);
  br->nb = loadValue<uint>(fp);
  br->data  = loadValue<uint>(fp,br->length/W+1); 
  br->bdata = loadValue<uchar>(fp,br->ns*br->nb); 
  br->sdata = loadValue<uint>(fp,br->ns);
  br->ones  = loadValue<size_t>(fp); 
  br->fastSelect0  = loadValue<bool>(fp); 
  if (br->fastSelect0){
    br->select0sampling = loadValue<uint>(fp);
    br->Stzero = loadValue<uint>(fp,(br->n-br->ones)/br->select0sampling+1);
  }else{
    br->select0sampling=0;
    br->Stzero=nullptr;
  }
  br->fastSelect1  = loadValue<bool>(fp); 
  if (br->fastSelect1){
    br->select1sampling = loadValue<uint>(fp);
    br->Stones = loadValue<uint>(fp,br->ones/br->select1sampling+1);
  }else{
    br->select1sampling=0;
    br->Stones=nullptr;
  }
  return br;
 }

void BitSequencePlain::save(ofstream  & fp) const{ 
  if (&fp == NULL) return;

  uint wr = PLAIN_HDR;
  assert(n==length);
  saveValue(fp,wr); 
  saveValue(fp,length);
  saveValue(fp,factor);
  saveValue(fp,integers);
  saveValue(fp,b);
  saveValue(fp,ns);
  saveValue(fp,nb);
  saveValue(fp,data,length/W+1);
  saveValue(fp,bdata,ns*nb);
  saveValue(fp,sdata, ns);
  saveValue(fp,ones);
  saveValue(fp, fastSelect0);
  if (fastSelect0){
    saveValue(fp, select0sampling);
    saveValue(fp,Stzero,(n-ones)/select0sampling+1);
  }
  saveValue(fp, fastSelect1);
  if (fastSelect1){
    saveValue(fp, select1sampling);
    saveValue(fp,Stones,ones/select1sampling+1);
  }
}


size_t BitSequencePlain::getSize() const{
   size_t total =uint_len(this->length,1)+(ns*nb)*sizeof(uint)+ns*sizeof(uint)+sizeof(BitSequencePlain);

    if (fastSelect0) {        // select0 table
      total += ((n-ones)/select0sampling)*4;
    }
    if (fastSelect1) {        // select1 table
      total += (ones/select1sampling)*4;
    } 
   return total; 
}




};
