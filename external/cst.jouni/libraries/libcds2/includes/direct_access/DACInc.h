/*  
 * Copyright (C) 2014-current-year, Alberto Ordóñez, all rights reserved.
 *
 * Alberto Ordóñez <alberto.ordonez@udc.es>
 * 
 * Implements a DAC for incremental values. Basically is adding a super Sampling layer to induce
 * smaller numbers. 
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

#ifndef DAC_INC_H
#define DAC_INC_H

using namespace cds_utils;

namespace cds_static{

	class DACInc : public DirectAccess {
		public:
			DACInc(uint *list,uint listLength,uint ss, bool del=false);
			~DACInc();
			virtual uint access(size_t param) const ;
			virtual size_t getSize() const;
			virtual void save(ofstream &fp) const;
			static DACInc *load(ifstream &in);
		protected:
		  DACInc();
		 DAC *base;
		 Array *superSampling;
		 uint ss;
	};
};
#endif
