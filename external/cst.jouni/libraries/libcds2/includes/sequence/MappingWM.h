/* MappingWM.h
 * Copyright (C) 2012, Alberto Ordóñez, all rights reserved.
 *
 * Sequence definition
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

#ifndef _MAPPING_WM_H
#define _MAPPING_WM_H


#include <utils/cppUtils.h>
#include <utils/libcdsBasics.h>

using namespace std;
using namespace cds_utils; 

namespace cds_static
{
		#define MAPPING_WM_BMP 1
		#define MAPPING_WM_BS 2
		#define MAPPING_WM_BSC 3
		#define MAPPING_WM_BLOCKS 4
	class MappingWM 
	{

			public:
				MappingWM(uint sigma, uint maxLevel, uint minLevel); 
				
				virtual ~MappingWM();

				virtual uint getSymbol(uint code, uint len) ; 
			
				virtual void getCode(uint symbol, uint &code, uint &len) ; 
			
				virtual bool finishCode(uint code, uint len) ;
				
				virtual size_t getSize(); 
				
				virtual void save(ofstream & fp);

				static MappingWM * load(ifstream & fp);

			protected:
				MappingWM();
				/**Number of disctinct symbols**/
				uint sigma; 
				/**Max length of a code**/
				uint maxLevel;
				/**Min length of a code**/
				uint minLevel; 
				/**Biggest code of each length. If we invert the codes, we obtain that code[i] < code[i+1]. Remark that the codes
				 do not have to be consecutives at each length, since, commonly, we have to jump codes that are already closed (we do 
				 not extend closed branches.**/
		};

};
#include <sequence/MappingWMBlocks.h>
#include <sequence/MappingWMBSC.h>
#include <sequence/MappingWMBS.h>
#include <sequence/MappingWMBMP.h>
#endif						
