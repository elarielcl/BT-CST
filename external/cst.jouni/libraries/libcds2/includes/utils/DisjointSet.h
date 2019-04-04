/*  
 * Copyright (C) 2014-current_year, Alberto Ordóñez, all rights reserved.
 *
 * Alberto Ordóñez <alberto.ordonez@udc.es>
 *
 * Implementation of a union-find data structure. It follows the heuristic
 * union-by-rank [CLRS] Cormen,Leiserson,Rivest,Stein., Introduction to Algorithms. 3rd Edition, pg. 568].
 * 
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

#ifndef _DISJOINTSET_H
#define	_DISJOINTSET_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <assert.h>
#include <utility>
#include <utils/TreePointers.h>


using namespace std;

namespace cds_utils{
	
/**
 * Implementation of a Disjoint-Set algorithm. The elementary component is 
 * the struct node_ds. It contains the id of the set, its rank (the max. depth 
 * of any of its children), and a type T that contains the data of interest
 * (that is, the data you want to store, the id and rank are info. only needed 
 * by the algorithms). Following the ideas of [CLRS], each set has a list of 
 * children. This is implemented using a template already contained in this library: 
 * TreePointer.h. TreePointers.h is a template that represents a tree node with k-children
 * (k is unbounded so you can add as many children as you need). 
 * 
 * 
 * */
template <typename T>class DisjointSet{               

			
		public:
		
			struct node_ds{
				uint id; //id of the set
				uint rank;//max. depth of any of the children of the set
				T content;//data of interest (associated with the sets)
			};
			
			TreePointers<node_ds> **sets;
			uint sigma;
			
				DisjointSet(uint sigma);
				
				~DisjointSet();
				/**
				 * Creates a new set with value as it representative. 
				 * Returns 0 on failure, whatever otherwise. 
				 * */
				uint makeSet(uint value, T content);
				/**
				 * Additionally adds child as a child of the new set
				 * */
				uint makeSet(uint value, T content, TreePointers<node_ds> *child);
				/**
				 * Carries out the union of sets x and y and stores
				 * the representative of the result set on res. 
				 * Returns 0 on failure, whatever on success
				 * */
				uint unionSet(uint x, uint y, uint &res);

                uint unionSet(uint x, uint y);
				/**
				 * Returns the representative of set x
				 * */
				uint findSet(uint x);
				
				/**
				 * Access the i-th set
				 * */
				TreePointers<node_ds> *operator[](const uint i) const;
				
				TreePointers<typename DisjointSet<T>::node_ds> *accessSet(uint pos);
				
			protected:
				uint reallocSets();

			
    };
    
    
template <typename T> DisjointSet<T>::DisjointSet(uint _sigma){
	sigma = _sigma;
	sets = new TreePointers<node_ds > *[sigma];
	//~ bmpSets = new uint*[sigma];
	//~ bmpLen = new uint[sigma];
	
	if (!sets){
		cerr << "Error allocating memory for DisjointSet ("<<sigma<<" elements)"<<endl;
	}
	for (uint i=0;i<sigma;i++){
		sets[i]=NULL;
		//~ bmpLen[i]=0;
		//~ bmpSets[i]=0;
	}
}

template <typename T> DisjointSet<T>::~DisjointSet(){
	//TODO
	if (sets){	
		delete [] sets;
	}
}

template <typename T> uint DisjointSet<T>::makeSet(uint value, T content){
	node_ds d;
	d.id   	  = value;
	d.rank 	  = 0;
	d.content = content;
	TreePointers<node_ds> *t = new TreePointers<node_ds>(d);
	if (value>=sigma){
		//TODO give a parameter to realloc to know how to expand (value could be > 2*sigma)
		if (!reallocSets()){
			cerr << "Error:(DisjointSet::makeSet): Unable to allocate new memory for sets" << endl;
		}
	}
	assert(value<sigma);
	sets[value] = t;
	//~ safeSetBit(value,value);
	return 0;
}


template <typename T> uint DisjointSet<T>::makeSet(uint value, T content, TreePointers<node_ds> *child){
	node_ds d;
	d.id   	  = value;
	d.rank 	  = child->getChild(0)->getValue().rank+1;
	d.content = content;
	TreePointers<node_ds> *t = new TreePointers<node_ds>(d);
	if (value>=sigma){
		if (!reallocSets()){
			cerr << "Error:(DisjointSet::makeSet): Unable to allocate new memory for sets" << endl;
		}
	}
	assert(value<sigma);
	sets[value] = t;
	
	sets[value]->addChild(child);
	return 0;
}
template <typename T> uint DisjointSet<T>::unionSet(uint x1, uint y1){
    uint res;
    unionSet(x1, y1, res);
    return res;
}

template <typename T> uint DisjointSet<T>::unionSet(uint x1, uint y1, uint &res){
	
	//Find the set representatives of x1 and y1
	const uint x = findSet(x1);
	const uint y = findSet(y1);
	if (x==y) return 0;//already belong to the same set
	
	//LINK SET-x and SET-y. We paste the shallow tree above the root of the 
	//deepest one. node_ds.rank stores the rank of the tree (in this case its hight)
	const uint rankx = sets[x]->getValue().rank;
	const uint ranky = sets[y]->getValue().rank;
	if ( rankx > ranky ){
		sets[x]->addChild(sets[y]);
		res = sets[x]->getValue().id;
	}
	else if (rankx < ranky){
		sets[y]->addChild(sets[x]);
		res = sets[y]->getValue().id;
	}else{
		node_ds p;
		p.id = sets[x]->getValue().id;
		p.rank = rankx+1;
		p.content = sets[x]->getValue().content;
		sets[x]->setValue(p);
		sets[x]->addChild(sets[y]);
		res = sets[x]->getValue().id;
	}
	return 0;
}

template <typename T> uint DisjointSet<T>::findSet(uint x){
	uint res;
	if (x>sigma) return -1;
	if (!sets[x]) return -1;
	if (sets[x]->getParent()){
		res = findSet(sets[x]->getParent()->getValue().id);
        if (sets[res]!=sets[x]->getParent()){
            sets[x]->getParent()->removeChild(sets[x]);
            sets[res]->addChild(sets[x]);
        }
	}else{
		res = x;
	}
	return res;
}


template <typename T> TreePointers<typename DisjointSet<T>::node_ds> *DisjointSet<T>::operator[](const uint pos) const{
	if (pos>=sigma) return NULL;
	return sets[pos];
}

template <typename T> TreePointers<typename DisjointSet<T>::node_ds> *DisjointSet<T>::accessSet(uint pos){
	if (pos>=sigma) return NULL;
	return sets[pos];
}

template <typename T> uint DisjointSet<T>::reallocSets(){
	TreePointers<node_ds> **new_sets = new TreePointers<node_ds>*[sigma*2];
	if (!new_sets) return 0;
	for (uint i=0;i<sigma;i++){
		new_sets[i] = sets[i];
	}
	sigma*=2;
	sets = new_sets;
	return 1;
}

};

#endif	


