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

#ifndef hu_tucker_node_h
#define hu_tucker_node_h


#include <sequence/htwt/basics.h>
#include <cassert>
#include <vector>
#include <bitsequence/BitSequence.h>
#include <bitsequence/BitsequenceBuilder.h>


/** Class for representing nodes in a HuTucker shaped WT using pointers 
 * 
 *  @author Alberto Ordóñez
 */
 
#define HU_TUCKER_NODE_NULL 0
#define HU_TUCKER_NODE_NOT_NULL 1

using namespace std;

namespace cds_static
{
class huTuckerInternalNode: public huTuckerNode{
	public:
		huTuckerInternalNode(uint *bitmap, uint n, BitSequenceBuilder *bsb);
		huTuckerInternalNode(uint *bitmap, uint n, BitSequenceBuilder *bsb, huTuckerNode *l, huTuckerNode *r);
		~huTuckerInternalNode();
		uint rank(uint symbol, uint pos, uint l);
		uint select(uint symbol, uint pos, uint l);
		uint access(uint pos);
		uint size();
		void save(FILE *fp);
		uint printNode(); 
		uint bitmapLength(); 
		static huTuckerInternalNode * load(FILE *fp);
		
		void  setLeftChild(huTuckerNode *l){
			leftChild = l; 
		}
		void  setRightChild(huTuckerNode *r){
			rightChild = r; 
		}
		huTuckerNode *  getLeftChild(){
			return leftChild; 
		}
		huTuckerNode *  getRightChild(){
			return rightChild; 
		}
		static_bitsequence * getBitmap(){
			return bitmap; 
		}
		uint getLeaf(){ return leaf;}
		void setLeaf(uint l){ leaf = l; }
		
	protected:
		
		huTuckerNode *leftChild, *rightChild; 
		BitSequence *bitmap; 
};
};

#endif
