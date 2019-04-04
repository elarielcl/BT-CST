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

#ifndef perm_wt_h
#define perm_wt_h


#include <utils/libcdsBasics.h>
#include <cassert>
#include <vector>
#include <bitsequence/BitSequence.h>
#include <bitsequence/BitSequenceBuilder.h>


/** Class for representing nodes in a HuTucker shaped WT using pointers 
 * 
 *  @author Alberto Ordóñez
 */
#define HUTUCKERPERM 1
#define HUTUCKERPERMPTRS 2
#define HUFFMANPERM 3

using namespace std;

namespace cds_static
{
	class PermWT{
		public:
			//~ PermWT(uint *bitmap, uint n, static_bitsequence_builder *bsb);
			PermWT();
			
			virtual ~PermWT();
			
			virtual void destroy(); 

			virtual uint directApplication( uint i);

			virtual uint inverseApplication( uint i, uint &leaf);

			virtual int rank(uint pos, uint limInf, uint &leaf); 

			virtual void save(ofstream & f); 

			virtual uint size(); 

			virtual void printInfo();
				
			static PermWT * load(ifstream & fp);

	};
};
#include <sequence/htwt/huTuckerPerm.h>
#include <sequence/htwt/HuTuckerPermPtrs.h>

#endif
