/*  
 * Copyright (C) 2013-current_year, Alberto Ordóñez, all rights reserved.
 *
 * Alberto Ordóñez <alberto.ordonez@udc.es>
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

#ifndef _TREEPOINTERS_H
#define	_TREEPOINTERS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <assert.h>

using namespace std;

namespace cds_utils{
	
template <typename T>class TreePointers{               
		public:
				TreePointers(T v);
				TreePointers();
				~TreePointers();
				TreePointers<T>* getChild(uint i);
				TreePointers<T>* popLastChild();
				T getValue();
				void addChild(TreePointers<T> *);
				void addChildHead(TreePointers<T> *);
				void addChildTail(TreePointers<T> *);
				void addChildPos(TreePointers<T> *,uint);
				void removeChild(TreePointers<T>*c);
				void replaceChild(TreePointers<T> *,TreePointers<T> *);
				int getNChildren();
				TreePointers* getParent();
				void setParent(TreePointers<T>* p);
				void setValue(T value);
				void resize();
				void resize(uint);
				
		protected: 
			T value;
			TreePointers *parent;
			TreePointers<T>** children;
//            vector<TreePointers<T>*> children;
			uint nchildren;
			uint max_cap;
    };
    
    
template <typename T> TreePointers<T>::TreePointers(){
	max_cap=10;
	nchildren=0;
	children = new TreePointers<T> *[max_cap];
	parent=NULL;
	//~ children=NULL;
	//~ children = new vector<TreePointers<T>*>();
	//~ if (!children)
		//~ cerr << "Error: cannot allocate memory for children vector at TreePointers.h" << endl;
}
    
template <typename T> TreePointers<T>::TreePointers(T v){
	value=v;
	max_cap=10;
	nchildren=0;
	parent = NULL;
	children = new TreePointers<T> *[max_cap];
}

template <typename T> TreePointers<T>::~TreePointers(){
	//~ if (children!=NULL){
		//~ for (uint i=0;i<children->size();i++)
			//~ delete children->at(i);
		//~ delete children;
	//~ }
}

template <typename T> TreePointers<T>* TreePointers<T>::getChild(uint i){
	//~ assert(i<children->size());
	//~ return children->at(i);	
	assert(i<nchildren);
	return children[i];
}

template <typename T> TreePointers<T>* TreePointers<T>::popLastChild(){
	//~ TreePointers<T> *tmp = children->at(children->size()-1);
	//~ children->pop_back();	
	//~ return tmp;
	return children[--nchildren];
}

template <typename T> void TreePointers<T>::removeChild(TreePointers<T>*c){
	uint p=0;
	for (uint i=0;i<nchildren;i++){
		if (children[i]!=c){
			children[p++]=children[i];
		}else{
			children[i]->parent=NULL;
		}
	}
	nchildren=p;
}

template <typename T> void TreePointers<T>::replaceChild(TreePointers<T> *replaced,TreePointers<T> *new_element){
	for (uint i=0;i<nchildren;i++){
		if (children[i]==replaced){
			children[i] = new_element;
			if (new_element->getParent()){
				new_element->getParent()->removeChild(new_element);
			}
			new_element->parent=this;
		}
	}
}

template <typename T> TreePointers<T>* TreePointers<T>::getParent(){
	return parent;	
}

template <typename T> void TreePointers<T>::setParent(TreePointers<T> *p){
	if (parent){
		removeChild(this);
	}
	if (p){
		p->addChild(this);	
	}
}

template <typename T> void TreePointers<T>::addChild(TreePointers<T> *t){
	assert(t);
	assert(children);
	if (t->getParent()==this) return;
	if (t->getParent())
		t->getParent()->removeChild(t);
	t->parent = this;
	if (nchildren>=max_cap)
		resize();
	children[nchildren++]=t;
}

template <typename T> void TreePointers<T>::addChildPos(TreePointers<T> *t, uint pos){
	assert(t);
	assert(children);
	if (t->getParent()==this) return;
	if (t->getParent())
		t->getParent()->removeChild(t);
	t->parent = this;
	if (nchildren>=max_cap)
		resize(pos);
	for (int i=nchildren;i>(int)pos;i--){
		children[i]=children[i-1];
	}
	children[pos]=t;
	nchildren++;
}

template <typename T> void TreePointers<T>::addChildTail(TreePointers<T> *t){
	addChild(t);
}

template <typename T> void TreePointers<T>::addChildHead(TreePointers<T> *t){
	assert(t);
	assert(children);
	if (t->getParent()==this) return;
	if (t->getParent())
		t->getParent()->removeChild(t);
	t->parent = this;
	if (nchildren>=max_cap)
		resize();
	
	for (int i=nchildren;i>0;i--){
		children[i]=children[i-1];
	}
	children[0]=t;
	nchildren++;
}

template <typename T> T TreePointers<T>::getValue(){
	return value;
}

template <typename T> void TreePointers<T>::setValue(T val){
	this->value = val;
}

template <typename T> int TreePointers<T>::getNChildren(){
	//~ return (children!=NULL)? children->size():0;
	return nchildren;
}

template <typename T> void TreePointers<T>::resize(){
	max_cap = max_cap*2;
	TreePointers<T>** children2 = new TreePointers<T>*[max_cap];
	for (uint i=0;i<nchildren;i++)
		children2 [i] = children[i];
	children = children2;
}

template <typename T> void TreePointers<T>::resize(uint k){
	max_cap = k*2;
	TreePointers<T>** children2 = new TreePointers<T>*[max_cap];
	for (uint i=0;i<nchildren;i++)
		children2 [i] = children[i];
	children = children2;
}
};

#endif	

