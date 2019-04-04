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

#ifndef _HEADER_PARENTHESES_H
#define _HEADER_PARENTHESES_H

#define BP_HDR 0
#define BPNAIVE_HDR 2

#include <stdlib.h>
#include <iostream>
#include <cstdio>
#include <stdexcept>
 
using namespace std;

namespace cds_static{

	class Parentheses{
	  public:
		virtual ~Parentheses(){};
		virtual ulong fwdExcess(uint s, uint e)=0;
		virtual ulong bwdExcess( uint s, int e)=0;
		virtual ulong rmq( uint s, int t, int &m)=0;
		virtual ulong depth( uint s)=0;
		virtual ulong rankOpen( uint s)=0;
		virtual ulong rankClose( uint s)=0;
		virtual ulong selectOpen( uint s)=0;
		virtual ulong selectClose( uint s)=0;
		virtual ulong root_node()=0; 
		virtual ulong inspect(uint s)=0;
		virtual ulong findClose(uint s)=0;
		virtual ulong findOpen(uint s)=0;
		virtual ulong parent(uint s)=0;
		virtual ulong parentClose(uint s)=0;
		virtual ulong enclose(uint s)=0;
		virtual ulong levelAncestor(uint s, uint d)=0;
		virtual ulong lca(uint a, uint b)=0;
		virtual ulong preorderRank(uint s)=0;
		virtual ulong preorderSelect(uint s)=0;
		virtual ulong postorderRank(uint s)=0;
		virtual ulong isLeaf(uint s)=0;
		virtual ulong subtreeSize(uint s)=0;
		virtual ulong firstChild(uint s)=0;
	  	virtual ulong nextSibling(uint s)=0;
	  	virtual ulong isAncestor(uint a, uint b)=0;


		virtual ulong id(ulong pos){
			throw runtime_error("id not implemented by default");
		}

		virtual ulong leftmostLeaf(ulong pos){
			throw runtime_error("leftmost_leaf not implemented by default");
		}
		//left limit of the interval of leaves covered by the node (it is an index--> starts in 0)
		virtual ulong lb(ulong pos){
			throw runtime_error("lb not implemented by default");
		}
		//right limit of the interval of leaves covered by the node (it is an index--> starts in 0)
		virtual ulong rb(ulong pos){
			throw runtime_error("rb not implemented by default");
		}

		ulong getInitialValueNextNodeIterator(){
			return 1;
		}
		/**
		* Given the ith-opened-parentheses, it returns the position of the ith+1
		*/
	  	virtual ulong nextNode(ulong ith){
			throw runtime_error("nextNode not implemented by default");
		}

		ulong getInitialValueNextNodeBottomUpIterator(){
			return 1;
		}

		/**
		* Performs a bottom-up trasversal of the tree.
		* @param pos: position in the BP of the tree.
		* @return the position in the BP of the next node in Bottom-up trasveral.
		*/
		virtual ulong nextNodeBottomUp(ulong pos){
			throw runtime_error("nextNodeBottomUp not implemented by default");
		}

	  	virtual ulong access(uint s){
	  		throw runtime_error("access not implemented by default");
	  	}
	  	virtual ulong excessAt(uint s){
	  		throw runtime_error("excessAt not implemented by default");
	  	}
	  	
	  	virtual ulong bwd_search(uint v, int e){
	  		throw runtime_error("bwd_search not implemented by default");
	  	}
	  	virtual ulong fwd_search(uint v, int e){
	  		throw runtime_error("fwd_search not implemented by default");
	  	}
		virtual bool isLeaf(uint s, uint &leaf_rank){
			throw runtime_error("isLeaf(s,leaf_rank) not implemented by default");
		}
	  	virtual ulong inorder(uint v){
	  		throw runtime_error("inorder not implemented by default");
	  	}

	  	virtual ulong brLeafSelect(uint v){
	  		throw runtime_error("brLeafSelect not implemented by default");
	  	}

	  	virtual ulong brLeafRank(uint v){
	  		throw runtime_error("brLeafRank not implemented by default");
	  	}
	  	//TODO
	  	virtual ulong degree(uint x){
	  		throw runtime_error("degree not implemented");
	  	}
	  	virtual ulong childRank(uint x){
	  		throw runtime_error("childRank not implemented");
	  	}
	  	virtual ulong child(uint x, uint i){
	  		throw runtime_error("child not implemented");
	  	}
		virtual ulong prevSibling(uint x){
			throw runtime_error("child not implemented");	
		}
	  	
	  	virtual void save(ofstream &fp)const=0;
	  	static Parentheses *load(ifstream &fp);

	  	void save(FILE &out)const;
	  	static Parentheses *load(FILE &in);
	  	
	  	static bool isATree(uint *par, ulong len);

	  	ulong getLength(){return n;}
	  	virtual ulong getSize() const=0;
	  protected:
	  	Parentheses();
	  	Parentheses(ulong n);
	
	  	ulong n;
	  };
};

#include <trees/BP.h>
#include <trees/BPNaive.h>

#endif
