/* static_sequence_canonical_wt.h
 * Copyright (C) 2012, Alberto Ordóñez, all rights reserved.
 *
 * static_sequence_canonical_wt definition
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

#ifndef _TUPLE_H
#define _TUPLE_H

#include <sys/types.h>

using namespace std;

namespace cds_static{
	
class Tuple {
	
  public:
	//~ static int cmpSymbols(const void *s1, const void *s2);
	//~ static bool cmpTupleByCode(const void *t1, const void *t2);
	//~ static int cmpTupleBySymbol(const void *s1, const void *s2);	
	//~ static int cmpTupleByLength(const void *t1, const void *t2);
	
	Tuple(uint symbol, uint code, uint len, uint occs); 
	Tuple();
	
	uint getSymbol(){return symbol;} 
	uint getCode(){return code;}
	uint getLen(){return len;}
	uint getOccs(){return occs;}; 
	
	void setSymbol(uint s){symbol=s;} 
	void setCode(uint c){code=c;}
	void setLen(uint l){len=l;}
	void setOccs(uint o){occs=o;}; 
  protected:
    uint symbol;
	uint code;
	uint len;
	uint occs;
};
};
#endif


