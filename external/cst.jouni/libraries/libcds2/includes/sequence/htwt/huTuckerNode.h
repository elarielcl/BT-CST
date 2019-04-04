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


#include <utils/libcdsBasics.h>
#include <cassert>
#include <vector>
#include <bitsequence/BitSequence.h>
#include <bitsequence/BitSequenceBuilder.h>


/** Class for representing nodes in a HuTucker shaped WT using pointers 
 * 
 *  @author Alberto Ordóñez
 */
 
#define HU_TUCKER_LEAF 0
#define HU_TUCKER_INTERNAL 1
using namespace std;

namespace cds_static
{
	
class huTuckerNode{
	public:
		huTuckerNode(uint *bitmap, uint n, BitSequenceBuilder *bsb);
		huTuckerNode(uint *bitmap, uint n, BitSequenceBuilder *bsb, huTuckerNode *l, huTuckerNode *r);
		huTuckerNode(uint *bitmap, uint n, BitSequenceBuilder *bsb, huTuckerNode *l, huTuckerNode *r, uint leaf);
		huTuckerNode();
		~huTuckerNode();
		uint rank(uint symbol, uint pos, uint l);
		uint select(uint symbol, uint pos, uint l);
		uint access(uint pos);
		uint size();
		void save(ofstream & fp);
		uint printNode(); 
		uint bitmapLength(); 
		
		static huTuckerNode * load(ifstream & fp);
		
		bool isLeaf(){
			return (leftChild==NULL && rightChild==NULL); 
		}

		void  setLeftChild(huTuckerNode *l){
			leftChild = l; 
			leftChild->setParent(this); 
		}
		void  setRightChild(huTuckerNode *r){
			rightChild = r; 
			rightChild->setParent(this); 
		}
		huTuckerNode *  getLeftChild(){
			return leftChild; 
		}
		huTuckerNode *  getRightChild(){
			return rightChild; 
		}
		BitSequence * getBitmap(){
			return bitmap; 
		}
		
		huTuckerNode *  getParent(){
			return parent; 
		}
		void setParent(huTuckerNode *p){
			parent = p; 
		}
		uint getLeaf(){ return leaf;}
		void setLeaf(uint l){ leaf = l; }
		
	protected:
		
		huTuckerNode *leftChild, *rightChild, *parent; 
		BitSequence *bitmap; 
		uint leaf; 
};
};

#endif
