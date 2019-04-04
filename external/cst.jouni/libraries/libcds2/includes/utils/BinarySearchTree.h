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

#ifndef _BINARY_SEARCH_TREE_H
#define	_BINARY_SEARCH_TREE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <assert.h>
#include <utils/libcdsBasics.h>
#include <climits>
using namespace std;

namespace cds_utils{
	
template <typename T>class BinarySearchTree{               
		public:
				BinarySearchTree(T v);
				BinarySearchTree();
				~BinarySearchTree();
				BinarySearchTree* searchKey(T);
				BinarySearchTree* insertKey(T);
				/*Finds the smallest value >=T*/
				BinarySearchTree* successor(T);
				BinarySearchTree* getRoot();
				BinarySearchTree* getLeft();
				BinarySearchTree* getRight();
				bool isInitialized();
				/*It is a not safe operation. If the node is not initialized, 
				it will return something anyway. It is on the user to check 
				if the node is initialized or not.*/
				T getValue();
		protected: 
			T value;
			bool initialized;
			BinarySearchTree *parent;
			BinarySearchTree<T>* left;
			BinarySearchTree<T>* right;
			void recSuccessor(T key, T m, BinarySearchTree<T> *&f);
    };

template <typename T> BinarySearchTree<T>::BinarySearchTree(){
	initialized=false;
	parent = NULL;
	left   = NULL;
	right  = NULL;
}

template <typename T> BinarySearchTree<T>::BinarySearchTree(T v){
	value=v;
	initialized=true;
	parent = NULL;
	left   = NULL;
	right  = NULL;
}

template <typename T> BinarySearchTree<T>::~BinarySearchTree(){
	if (left)
		delete left;
	if (right)
		delete right;
}

template <typename T> BinarySearchTree<T>* BinarySearchTree<T>::getRoot(){
	if (this->parent)
		return this->parent->getRoot();
	else
		return this;	
}

template <typename T> BinarySearchTree<T>* BinarySearchTree<T>::getLeft(){
	return left;
}

template <typename T> BinarySearchTree<T>* BinarySearchTree<T>::getRight(){
	return right;
}

template <typename T> bool BinarySearchTree<T>::isInitialized(){
	return initialized;
}

template <typename T> BinarySearchTree<T>* BinarySearchTree<T>::insertKey(T key){
	if (!initialized){
		value = key;
		initialized=true;
		return this;
	}
	if (this->getValue()==key){
		return this;
	}
	if (this->getValue()>key){
		if (this->left){
			return this->left->insertKey(key);
		}else{
			BinarySearchTree<T> *bt = new BinarySearchTree(key);
			bt->parent=this;
			this->left=bt;
			return bt;
		}
	}else{
		if (this->right){
			return this->right->insertKey(key);
		}else{
			BinarySearchTree<T> *bt = new BinarySearchTree(key);
			bt->parent=this;
			this->right=bt;
			return bt;
		}
	}
}

template <typename T> BinarySearchTree<T>* BinarySearchTree<T>::searchKey(T key){
	if (!initialized){
		return nullptr;
	}
	if (this->getValue() == key){
		return this;
	}
	if (this->getValue()>key){
		if (this->left){
			return this->left->searchKey(key);
		}else{
			return nullptr;
		}
	}else{
		if (this->right){
			return this->right->searchKey(key);
		}else{
			return nullptr;
		}
	}
}

template <typename T> BinarySearchTree<T>* BinarySearchTree<T>::successor(T key){
	if (!initialized){
		return nullptr;
	}
	BinarySearchTree<T> *f=nullptr;
	T m=INT_MAX;//you do not even need to initialize this variable
	this->recSuccessor(key,m,f);
	return f;
}

template <typename T> void BinarySearchTree<T>::recSuccessor(T key, T m, BinarySearchTree<T> *&f){
	if (this->value == key){
		f = this;
	}else if (this->value > key){
		if (this->value > key && (this->value < m || f==nullptr)){
			f = this;
			m = this->value;
		}
		if (this->left){
			this->left->recSuccessor(key,m,f);
		}
	}else{
		if (this->right){
			this->right->recSuccessor(key,m,f);
		}
	}
	
}

template <typename T> T BinarySearchTree<T>::getValue(){
	return value;
}

};

#endif	

