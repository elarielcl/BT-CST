/* huTuckerNode.h
 * Copyright (C) 2012, Alberto Ordóñez, all rights reserved.
 *
 * huTuckerNode
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


#include <PermWT.h>

/** Class for representing nodes in a HuTucker shaped WT using pointers 
 * 
 *  @author Alberto Ordóñez
 */
namespace cds_static
{
		PermWT::PermWT(){}
		
		PermWT::~PermWT(){}
		 
		 void PermWT::destroy(){}

		 uint PermWT::directApplication( uint i){return -1;}

		 uint PermWT::inverseApplication( uint i, uint &leaf){return -1; }

		 int PermWT::rank(uint pos, uint limInf, uint &leaf){return -1;} 

		 void PermWT::save(ofstream & f){} 

		 uint PermWT::size(){
			 return -1;
		} 

		 void PermWT::printInfo(){}
		
		
		PermWT * PermWT::load(ifstream & fp){
			uint type = loadValue<uint>(fp);
			size_t pos = fp.tellg();
			fp.seekg(pos-sizeof(uint),ios::beg);
			  switch(type) {
				case HUTUCKERPERM: return huTuckerPerm::load(fp);
				case HUTUCKERPERMPTRS: return HuTuckerPermPtrs::load(fp);
				//~ case HUFFMANPERM: return HuffmanPerm::load(fp);
			  }
			  return NULL;
		}

};
