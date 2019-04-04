/* huffman_codes.h
   Copyright (C) 2008, Francisco Claude, all rights reserved.

   Wrapper for huff written by Gonzalo Navarro

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

#ifndef HUTUCKERPERMPTRS_H
#define HUTUCKERPERMPTRS_H

#include <sequence/htwt/PermWT.h>
#include <sequence/htwt/hu-tucker/hutucker.h>
#include <sequence/htwt/hu-tucker/binarynode.h>
#include <sequence/htwt/huTuckerNode.h>
#include <bitsequence/BitSequenceBuilder.h>

/** Wrapper for the canonical huffman implementation of Gonzalo Navarro. 
 * 
 *  @author Alberto Ordóñez
 */
 
 using namespace std;

namespace cds_static
{
class HuTuckerPermPtrs : public PermWT {

  public:
    /** Creates the codes for the sequence seq of length n */
	HuTuckerPermPtrs(int *weights, uint weightsSize, uint *psi, uint n, BitSequenceBuilder * bmb);
	
	virtual void destroy(); 

	virtual uint directApplication( uint i);

	virtual uint inverseApplication( uint i, uint &leaf);

	virtual int rank(uint pos, uint limInf, uint &leaf); 

	virtual void save(ofstream  & f); 

	virtual uint size(); 

	static HuTuckerPermPtrs* load( ifstream & f); 

	virtual void printInfo();
	 
    virtual ~HuTuckerPermPtrs();
    
  protected:
  
	HuTuckerPermPtrs();
  
	uint nleafs; 
	huTuckerNode *root; 
	huTuckerNode **leafs; 
	uint *C; 
	void printTreeStructure(huTuckerNode *node, uint level);
	huTuckerNode * crearHT(bNode *node, BitSequenceBuilder * bmb);
    void preMergePsiHTWT(bNode *f, bNode *l, bNode *r, uint **bitmap); 
    void mergePsiHTWT(uint *psiL, uint sizePsiL, uint *psiR, uint sizePsiR, uint **psiN, uint *sizePsiN, uint *bitmap); 
    uint recursiveInverseApplication(huTuckerNode *node, uint i, uint p, uint s, uint &leaf);
    uint recursiveDirectApplication(huTuckerNode *node,  uint s, uint j);
    uint recursiveRankHTWT(huTuckerNode *node, uint pos, uint s, uint end, uint intinf, uint &leaf);
    void asignLeafNumber(huTuckerNode *node, uint &leafNo);
    void loadLeafVector(huTuckerNode *node);
    uint isLeaf(uint posInTree);

	uint getLeftChild(uint posInTree, uint prevPosInTree);

	uint getRightChild( uint posInTree, uint prevPosInTree);
	uint isLeafBnode(bNode *node);
    //~ uint recursiveDirectApplication(uint i);
};
};

#endif
