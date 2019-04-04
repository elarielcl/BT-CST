/*  
 * Copyright (C) 2014-current_year, Alberto Ordonez, all rights reserved.
 *
   Alberto Ordonez <alberto.ordonez@udc.es>
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

#ifndef _HEADER_BPNAIVE_H
#define _HEADER_BPNAIVE_H

#include <trees/Parentheses.h>
#include <bitsequence/BitSequence.h>

namespace cds_static{

	class BPNaive : public Parentheses{
	  public:
	  	BPNaive(uint *input, ulong len);
		virtual ~BPNaive();
		virtual ulong fwdExcess(uint s, uint e);
		virtual ulong bwdExcess( uint s, int e);
		virtual ulong rmq( uint s, int t, int &m);
		virtual ulong depth( uint s);
		virtual ulong rankOpen( uint s);
		virtual ulong rankClose( uint s);
		virtual ulong selectOpen( uint s);
		virtual ulong selectClose( uint s);
		virtual ulong root_node();
		virtual ulong inspect(uint s);
		virtual ulong findClose(uint s);
		virtual ulong findOpen(uint s);
		virtual ulong parent(uint s);
		virtual ulong parentClose(uint s);
		virtual ulong enclose(uint s);
		virtual ulong levelAncestor(uint s, uint d);
		virtual ulong lca(uint a, uint b);
		virtual ulong preorderRank(uint s);
		virtual ulong preorderSelect(uint s);
		virtual ulong postorderRank(uint s);
		virtual ulong isLeaf(uint s);
		virtual ulong subtreeSize(uint s);
		virtual ulong firstChild(uint s);
	  	virtual ulong nextSibling(uint s);
	  	virtual ulong isAncestor(uint a, uint b);
	  	virtual ulong prevSibling(uint x);
	  	virtual void save(ofstream &fp)const;
	  	static BPNaive *load(ifstream &fp);
	  	virtual ulong getSize() const;
	  	
	  protected:
	  	BPNaive();
	  	BPNaive(ulong n);
	  	ulong getExcess(uint pos);
	  	BitSequence *bs;
	  };
};

#endif
