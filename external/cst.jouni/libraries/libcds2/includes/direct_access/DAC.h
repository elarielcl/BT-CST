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

#ifndef DACS_H
#define DACS_H

using namespace cds_utils;

namespace cds_static{

	class DAC : public DirectAccess {
		public:
			DAC(uint *list,uint listLength,bool del=false);
			~DAC();
			virtual uint access(size_t param) const ;
			virtual size_t getSize() const;
			virtual void save(ofstream &fp) const;
			static DAC *load(ifstream &in);
		protected:
		  DAC();
		  unsigned char nLevels;
		  uint tamCode;
		  uint tamtablebase;
 		  uint * tablebase;
 		  ushort * base_bits;
		  uint * base;
		  uint * levelsIndex;
		  uint * iniLevel;
		  uint * rankLevels;
		  uint * levels;
		  BitSequence * bS;	
	};
};
#endif
