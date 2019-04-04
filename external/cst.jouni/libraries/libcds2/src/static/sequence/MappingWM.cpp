/* Sequence.h
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
 

#include <sequence/MappingWM.h>
#include <sequence/MappingWMBlocks.h>
#include <sequence/MappingWMBMP.h>
#include <sequence/MappingWMBS.h>
#include <sequence/MappingWMBSC.h>

namespace cds_static
{
	
		MappingWM::MappingWM(uint _sigma, uint _maxLevel, uint _minLevel){
			sigma=sigma;
			maxLevel=_maxLevel;
			minLevel=_minLevel;
		}
		
		MappingWM::MappingWM(){}
	
		MappingWM::~MappingWM(){}
		
		 uint MappingWM::getSymbol(uint code, uint len) {return -1;}
			
		 void MappingWM::getCode(uint symbol, uint &code, uint &len) {} 
	
		 bool MappingWM::finishCode(uint code, uint len) {return false;}
		
		 size_t MappingWM::getSize(){return -1;}
		 
		 void MappingWM::save(ofstream & fp){}
		 
		MappingWM * MappingWM::load(ifstream & fp){
			uint type = loadValue<uint>(fp);
			size_t pos = fp.tellg();
			fp.seekg(pos-sizeof(uint),ios::beg);
			switch(type) {
				case MAPPING_WM_BMP: return MappingWMBMP::load(fp);;
				case MAPPING_WM_BS: return MappingWMBS::load(fp);
				case MAPPING_WM_BSC: return MappingWMBSC::load(fp);
				case MAPPING_WM_BLOCKS: return MappingWMBlocks::load(fp);
			}
			return NULL;
		}

};

					
