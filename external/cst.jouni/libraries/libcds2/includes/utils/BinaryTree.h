/*  
 * Copyright (C) 2013-current_year, Alberto Ordóñez, all rights reserved.
 *
 * Alberto Ordóñez <alberto.ordonez@udc.es>
 *
 * Implementation of a binary tree
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

#ifndef _BINARYTREE_H
#define	_BINARYTREE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <assert.h>
#include <utils/libcdsBasics.h>

using namespace std;

namespace cds_utils{
	
template <typename T>class BinaryTree{               
		public:
				BinaryTree(T v);
				BinaryTree();
				~BinaryTree();
				BinaryTree<T>* getLeft();
				BinaryTree<T>* getRight();
				BinaryTree* getParent();
				BinaryTree* getRoot();
				uint getDepth();
				T getValue();
				void setLeft(BinaryTree<T> *);
				void setRight(BinaryTree<T> *);
				void setParent(BinaryTree<T> *);
				void setValue(T value);
				/**
				* bmp has to be initiallized with enough space
				**/
				size_t obtainParenthesesEstructure(uint *bmp);
				size_t obtainParenthesesEstructure(uint *bmp, uint depth);
				
		protected: 
			T value;
			BinaryTree *parent;
			BinaryTree<T>* left;
			BinaryTree<T>* right;
			void obtainParEstructureRec(BinaryTree<T> *t, uint *bmp, size_t &p);
    };
    
    
template <typename T> BinaryTree<T>::BinaryTree(){
	parent = NULL;
	left   = NULL;
	right  = NULL;
}
    
template <typename T> BinaryTree<T>::BinaryTree(T v){
	value=v;
	parent = NULL;
	left   = NULL;
	right  = NULL;
}

template <typename T> BinaryTree<T>::~BinaryTree(){
	if (left)
		delete left;
	if (right)
		delete right;
}

template <typename T> BinaryTree<T>* BinaryTree<T>::getLeft(){
	return left;
}

template <typename T> BinaryTree<T>* BinaryTree<T>::getRight(){
	return right;
}

template <typename T> BinaryTree<T>* BinaryTree<T>::getParent(){
	return parent;	
}

template <typename T> BinaryTree<T>* BinaryTree<T>::getRoot(){
	if (this->getParent())
		return this->getParent()->getRoot();
	else
		return this;	
}
template <typename T> uint BinaryTree<T>::getDepth(){
	if (!(left || right))	
		return 1;
	uint ml,mr;
	ml = (left)?left->getDepth()+1:0;
	mr = (right)?right->getDepth()+1:0;
	return max(ml,mr);
}

template <typename T> void BinaryTree<T>::setLeft(BinaryTree<T> *t){
	if (t) t->parent = this;
	left = t;
}

template <typename T> void BinaryTree<T>::setRight(BinaryTree<T> *t){
	if (t) t->parent = this;
	right = t;
}

template <typename T> void BinaryTree<T>::setParent(BinaryTree<T> *t){
	assert(t);
	parent = t;	
}

template <typename T> T BinaryTree<T>::getValue(){
	return value;
}

template <typename T> void BinaryTree<T>::setValue(T val){
	this->value = val;
}

template <typename T> void BinaryTree<T>::obtainParEstructureRec(BinaryTree<T> *t, uint *bmp, size_t &p){
	if (t){
		bitset(bmp,p++);
		obtainParEstructureRec(t->getLeft(),bmp,p);
		obtainParEstructureRec(t->getRight(),bmp,p);
		p++;
	}
}

template <typename T> size_t BinaryTree<T>::obtainParenthesesEstructure(uint *bmp){
	size_t length=0;
	obtainParEstructureRec(this,bmp,length);
	return length;
}

template <typename T> size_t BinaryTree<T>::obtainParenthesesEstructure(uint *bmp, uint depth){
	
	depth++;
	depth*=2;
	BinaryTree<T> **nodes = new BinaryTree<T>*[depth];
	bool *l = new bool[depth];
	bool *r = new bool[depth];
	for (uint i=0;i<depth;i++)
		l[i]=r[i]=false;
	int pointerNodes=0;
	size_t length=1;
	nodes[0]=this;
	bitset(bmp,0);
	while(pointerNodes>=0){
		assert(pointerNodes<depth);
		if (!nodes[pointerNodes]){
			pointerNodes--;
			//length++;
		}else{
			if (!l[pointerNodes]){
				//descend througout the left child
				l[pointerNodes]=true;
				if (nodes[pointerNodes]->getLeft()){
					bitset(bmp,length++);					
					if (!(pointerNodes+1<depth)){
						cerr << endl;
					}
					assert(pointerNodes+1<depth);
					nodes[pointerNodes+1]=nodes[pointerNodes]->getLeft();	
					pointerNodes++;
				}
			}else if (!r[pointerNodes] ){
				r[pointerNodes]=true;
				if (nodes[pointerNodes]->getRight()){
					bitset(bmp,length++);
					if (!(pointerNodes+1<depth)){
						cerr << endl;
					}
					assert(pointerNodes+1<depth);
					nodes[pointerNodes+1]=nodes[pointerNodes]->getRight();	
					pointerNodes++;
				}
			}else{
				length++;
				l[pointerNodes]=r[pointerNodes]=false;
				pointerNodes--;
			}	
		}
	}
	delete [] nodes;
	delete [] l;
	delete [] r;
	return length;
}
};

#endif	

