/*  
 * Copyright (C) 2013-current-year, Alberto Ordóñez, all rights reserved.
 *
 * Alberto Ordóñez <alberto.ordonez@udc.es>
 * 
 * DirectAccess interface. It offers that, sequences with capability of 
 * retriving what is stored in a given position.
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
#ifndef DIRECTACCESS_H
#define DIRECTACCESS_H
#include <utils/libcdsBasics.h>
#include <utils/cppUtils.h>

#define DAC_HDR 0
#define DAC_E_HDR 1
#define DAC_INC_HDR 2
#define ARRAY_DA_HDR 3

using namespace cds_utils;
using namespace std;

namespace cds_static{

	class DirectAccess{
		public:
		    DirectAccess(uint _len){len=_len;}
			virtual ~DirectAccess(){}
			virtual uint access(size_t param) const = 0;
			virtual size_t getSize() const = 0;
			virtual void save(ofstream &of) const = 0;
			uint getLength() const;
			static DirectAccess *load(ifstream &in);
						/** operator [] for getField
			 */
			inline uint operator[](const size_t pos) const
			{
				return access(pos);
			}
			
		protected:
			DirectAccess();
			size_t len;
			uint * optimizationk(uint * list,int listLength, int * nkvalues);
	};
};
#include <direct_access/DAC.h>
#include <direct_access/DACe.h>
#include <direct_access/DACInc.h>
#include <direct_access/ArrayDA.h>
#endif
