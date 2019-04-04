/*  
 * Copyright (C) 2013-current-year, Alberto Ordóñez, all rights reserved.
 *
 * Alberto Ordóñez <alberto.ordonez@udc.es>
 * 
 * Translation to c++ of the Direct Addresable Codes (http://lbd.udc.es/research/DACS/)
 * originally implemented by Susana Ladra (sladra@udc.es).
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
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <malloc.h>
#include <direct_access/DirectAccess.h>

#include <bitsequence/BitSequence.h>
#include <utils/libcdsBasics.h>
#include <utils/libcdsTrees.h>

#ifndef DAC_e_H
#define DAC_e_H

using namespace cds_utils;

namespace cds_static{

	class DACe : public DirectAccess {
		public:
			DACe(uint *list,uint listLength,bool del=false);
			~DACe();
			virtual uint access(uint param) const ;
			virtual size_t getSize() const;
			virtual void save(ofstream &of) const;
			static DACe* load(ifstream &fp);
		protected:
		  
		  uint tamCode;
		  uint * levels;
		  uint * levelsIndex;
		  uint * iniLevel;
		  uint * rankLevels;
		  BitSequence * bS;	
		  uint * base;
		  ushort * base_bits;
		  uint * tablebase;
		  uint tamtablebase;
		  
		  /**
		   * Number of levels
		   * */
		  uint nLevels;
		  
		  uint nLevelsMinus1;
		  /**
		   * A sequence for all the codes of the same length new int*[nLevels]
		   * */
		  uint **seqs;
		  /**
		   * Rank of 0's at the beggining of each level (rank0Level[0]=0);
		   * */
		   uint *rank0Level;
		  /**
		   * Rank of 1's of each level. yeah!rank1 can be computed from rank0 but i 
		   * don't want to do such a computation on query time(the added space overhead
		   * is absolutely negligigle).
		   * */
		   uint *rank1Level;
		   /**
		    * Bitmap of all levels concatenated (that is why we need rankLevel)
		    * */
			BitSequence *bitmap;
			
			uint *acumPosLevel;
			uint *acumLen;
			/**
			 * Code length of each level
			 * */
		    uint *lenChunkLevel;
		    
		    uint *acumPosMinusRank1;
		   
	};	
};
#endif
