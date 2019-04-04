/*  
 * Copyright (C) 2013-current_year, Alberto Ordóñez, all rights reserved.
 *
 * Alberto Ordóñez <alberto.ordonez@udc.es>
 * 
 * Implements the idea of Tabei et al.[32] (check out the paper "Faster
 * Compressed Suffix Trees for Repeititve Text Collections". At Section 3
 * we breafly describe it). 
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

#ifndef _DICIONARYRP_H
#define	_DICIONARYRP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <utils/TreePointers.h>
#include <bitsequence/BitSequence.h>
#include <permutation/Permutation.h>
#include <grammar/Repair.h>
#include <utils/cppUtils.h>

namespace cds_static{
	
	class DictionaryRP{               
		public:
				DictionaryRP(uint *LEFT, uint *RIGHT, uint alph, int R, uint *&,PermutationBuilder *pmb);
				DictionaryRP(uint *LEFT, uint *RIGHT, uint alph, int R, uint *&);
				DictionaryRP(Repair *rp,uint *&PERM,PermutationBuilder *pmb);
				~DictionaryRP();
				uint getRight(size_t rule);
				uint getLeft(size_t rule);
				uint getAlph();
				uint getNRules();
				size_t getSize();
				size_t getSizeVerbose();
				void save(ofstream &of);
				static DictionaryRP *load(ifstream &in);
		protected: 
			DictionaryRP();
			void buildTrees(uint *LEFT, uint *RIGHT, uint ALPH, int R, uint *&PERM, PermutationBuilder *pmb);
			void printTree(TreePointers<int> *t);
			void getLOUDS(TreePointers<int> **queue,uint &head,uint &pq, uint &p, uint *louds);
			void getPERM(TreePointers<int> *t, uint &p, uint *perm);
			size_t getParentLOUDS(uint r, BitSequence *louds);
			void deleteStructures(TreePointers<int> *t);
			BitSequence *LT_LOUDS;
			BitSequence *RT_LOUDS;
			uint invertUnary(uint val, uint ini, uint len);
			Permutation *permutation;
			uint R;
			uint alpha;
			uint alphaPlus1;
			uint *PERM_tmp;
			uint *INVPERM_tmp;
			uint PERM_SIZE;
			uint RSIZE;
			bool testDictionaryRP(uint *LEFT, uint *RIGHT, uint ALPH, int R,uint *PERM);
			
    };

};

#endif	

