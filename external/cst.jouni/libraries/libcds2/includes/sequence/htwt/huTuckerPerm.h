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

#ifndef HUTUCKERPERM_H
#define HUTUCKERPERM_H

//~ #include <sequence/htwt/basics.h>

//~ #include <static_bitsequence.h>

#include <utils/libcdsBasics.h>
#include <sequence/htwt/PermWT.h>
#include <sequence/htwt/htwt.h>
#include <bitsequence/BitSequence.h>
#include <bitsequence/BitSequenceBuilder.h>

/** Wrapper for the canonical huffman implementation of Gonzalo Navarro. 
 * 
 *  @author Alberto Ordóñez
 */
 
using namespace std;
using namespace cds_utils; 
namespace cds_static
{
	
class huTuckerPerm : public PermWT {

  public:
    /** Creates the codes for the sequence seq of length n */
	huTuckerPerm(int *weights, uint weightsSize, uint *psi, uint n, uint sampling_rate, BitSequenceBuilder * bmb);
	
	virtual void destroy(); 

	virtual uint directApplication( uint i);

	virtual uint inverseApplication( uint i, uint &leaf);

	virtual int rank(uint pos, uint limInf, uint &leaf); 

	virtual void save(ofstream & f); 

	virtual uint size(); 

	static huTuckerPerm* load(ifstream & f); 

	virtual void printInfo();
	 
	void printTreeStructure(uint posInTree, uint prevPosInTree, uint level); 

    virtual ~huTuckerPerm();
    
  protected:
	huTuckerPerm();
   
    BitSequence *bitmap; 
	uint *children; 
	uint *starts; 
	uint n;//size of psi
	uint nleafs; 
	uint minBits; //se podría almacenar en la primera posición de childre que no se usa
	uint minBitsStarts; 
	uint vocSize; 
	uint numberOfBitsTree; 
    
    uint recursiveInverseApplication(uint posInTree, uint prevPosInTree, uint i, uint p, uint s, uint &leaf);
    uint recursiveDirectApplication(uint posInTree, uint prevPosInTree, uint s, uint j);
    uint recursiveRankHTWT(uint posInTree, uint pos, uint prevPosInTree, uint s, uint end, uint intinf, uint &leaf);
    
    uint isLeaf(uint posInTree);

	uint getLeftChild(uint posInTree, uint prevPosInTree);

	uint getRightChild( uint posInTree, uint prevPosInTree);
	
  private: 
  
  int createHTWT(htwtTree **tree, int *weights, uint weightsSize, uint *psi, uint n, uint samplingRate);
  uint isLeafBnode(bNode *node);
  void crearHT(htwtTree *tree, bNode *node, uint *bitmap, uint *pb, uint nleafs, uint i, uint *libre, uint minBits);
  void preMergePsiHTWT(bNode *f, bNode *l, bNode *r, uint *bitmap, uint *pb);
  void mergePsiHTWT(uint *psiL, uint sizePsiL, uint *psiR, uint sizePsiR, uint **psiN, uint *sizePsiN, uint *bitmap, uint pb);
  uint isLeaf(htwtTree *tree, uint posInTree);
  uint getLeftChild(htwtTree *tree, uint posInTree);
  uint getRightChild(htwtTree *tree, uint posInTree);
  uint my_log2HTWT(uint value);
    //~ uint recursiveDirectApplication(uint i);
};
};

#endif
