/*  MappingBuilder.h
 * Copyright (C) 2012, Alberto Ordóñez, all rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */



#ifndef MAPPINGBUILDER_H
#define MAPPINGBUILDER_H


#include <utils/libcdsBasics.h>
#include <utils/Tuples.h>
#include <sequence/MappingSymbols.h>

using namespace cds_utils;

namespace cds_static
{

	class MappingBuilder
	{
		public:
			MappingBuilder() { userCount=0; }
			virtual ~MappingBuilder() {}
			virtual  MappingSymbols * build(tuple * list, size_t sigma)=0;
			virtual void use() { userCount++; }
			virtual void unuse() { userCount--; assert(userCount>=0); if(userCount==0) delete this; }

		protected:
			int userCount;
	};

	class MappingTableBuilder : public MappingBuilder
	{
	public:
		MappingTableBuilder(){use();}
		virtual ~MappingTableBuilder(){unuse();}
		virtual  MappingSymbols * build(tuple * list, size_t sigma){
			return new MappingTable(list,sigma);
		}
	};

	class MappingTableBuilderStart : public MappingBuilder
	{
	public:
		MappingTableBuilderStart(uint _b):b(_b){use();}
		virtual ~MappingTableBuilderStart(){unuse();}
		virtual  MappingSymbols * build(tuple * list, size_t sigma){
			return new MappingTableAceleartorStart(list,sigma,b);
		}
	protected:
		uint b;
	};

	class MappingTableBuilderExtended : public MappingBuilder
	{
	public:
		MappingTableBuilderExtended(uint _x):x(_x){use();}
		virtual ~MappingTableBuilderExtended(){unuse();}
		virtual  MappingSymbols * build(tuple * list, size_t sigma){
			return new MappingTableAceleratorExtended(list,sigma,x);
		}
	protected:
		uint x;
	};

	class MappingPermutationsBuilder : public MappingBuilder
	{
	public:
		MappingPermutationsBuilder(BitSequenceBuilder *_bsb){
			this->bsb = _bsb;
			use();
		}
		virtual ~MappingPermutationsBuilder(){
			this->bsb->unuse();
			unuse();
		}
		virtual  MappingSymbols * build(tuple * list, size_t sigma){
			return new MappingPermutations(list,sigma, this->bsb);
		}

	protected:
		BitSequenceBuilder *bsb;
	};
};

#endif
