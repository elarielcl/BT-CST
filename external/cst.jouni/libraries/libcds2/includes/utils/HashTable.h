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

#ifndef _HASH_TABLE_H
#define	_HASH_TABLE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <assert.h>
#include <cmath>
#include <utils/cppUtils.h>

using namespace std;

namespace cds_utils{
	
/*
	Implementation of a hash table with open addressing and double hashing. 
	The T type is the satellite
	information associated with each key. If the satellite info is an unsigned long long with the same 
	value than the key, it is like a regular hash table. 
*/
template <typename T>class HashTable{               
	public:
		/*
			Requires: a>0, b>=0, m>0
			m is the table size
			a,b,p are the parameters of the hash funciton: 
			h(k) = (a*k+b) mod unsigned long long
		*/
		// HashTable(unsigned long long a, unsigned long long b, unsigned long long m, unsigned long long prime);
		/*
			If p is not given, it is computed by nearestPrime(m) function.
		*/
		HashTable(unsigned long long a, unsigned long long b, unsigned long long m);

		~HashTable();
		/*
			Inserts value in the table hash. Value has key k associated. 
			On success returns the position in which value was inserted. 
			On failure (tabler overflow), returns -1. 
		*/
		unsigned long long insert(unsigned long long k, T value);

		unsigned long long insert(unsigned long long precomputedHash, unsigned long long k, T value);

		T search(unsigned long long key);
		unsigned long long search(unsigned long long precomputedHash, unsigned long long key);
		T getAt(unsigned long long pos);
		uint getSize();
		uint getNEmptyCells();
		unsigned long long getModulo();
		//returns the position of the next value found in the table starting at pos (included)
		unsigned long long getNext(unsigned long long pos) const;
		void save(ofstream &of);
		static HashTable<T> load(ifstream &in);
		static size_t nearestPrime(size_t n);
		inline T operator[](const unsigned long long v) {
						assert(v<m);
						return table[v];
					};
	protected: 
		HashTable();
		unsigned long long a;
		unsigned long long b;
		unsigned long long m;
		unsigned long long mprime;
		unsigned long long p;
		T *table;
		bool *empty;
		unsigned long long *keys;
		unsigned long long h(unsigned long long i, unsigned long long iter);
		unsigned long long h(unsigned long long precomputedHash, unsigned long long i, unsigned long long iter);
		unsigned long long h1(unsigned long long i);
		unsigned long long h2(unsigned long long i);
		
		//unsigned long long hcolision(unsigned long long i, unsigned long long iter);
};

template <typename T> HashTable<T>::HashTable(){
	a = 0;
	b = 0;
	p = m = 0;
	mprime = 0;
}
    
template <typename T> HashTable<T>::HashTable(unsigned long long _a, unsigned long long _b, unsigned long long _m){
	
	a = _a;
	b = _b;
	p = m = nearestPrime(_m);
	mprime = m-1;
	assert(mprime>1);
	assert(a>0);
	assert(b>=0);
	assert(m>0);
	assert(p>=m && p>1);
	table = new T[m];
	for (unsigned long int i=0;i<m;i++)
		table[i]=NULL;
	keys = new unsigned long long[m];
	empty = new bool[m];
	for (unsigned long long i=0;i<m;i++){
		empty[i]=true;
	}
}
template <typename T> HashTable<T>::~HashTable(){
	delete [] table;
};

template <typename T> unsigned long long HashTable<T>::h(unsigned long long precomputedHash, unsigned long long k, unsigned long long iter){
	return (precomputedHash+iter*h2(k)) % m;
}

template <typename T> unsigned long long HashTable<T>::h(unsigned long long k, unsigned long long iter){
	return (h1(k)+iter*h2(k)) % m;
}

template <typename T> unsigned long long HashTable<T>::h1(unsigned long long k){
	return (a*k +b)%p;
}

template <typename T> unsigned long long HashTable<T>::h2(unsigned long long k){
	return 1+k%mprime;
}

template <typename T> unsigned long long HashTable<T>::getModulo(){
	return m;
}

template<typename T> unsigned long long HashTable<T>::insert(unsigned long long k, T value){
	unsigned long long iter=0;
	unsigned long long i;
	do{
		i = h(k,iter);
		if (empty[i]){
			table[i]=value;
			empty[i]=false;
			keys[i] = k;
			return i;
		}
		iter++;
	}while(iter<m);

	return -1;
}

template<typename T> unsigned long long HashTable<T>::insert(unsigned long long precomputedHash, unsigned long long k, T value){
	unsigned long long iter=0;
	unsigned long long i;
	do{
		i = h(precomputedHash,k,iter);
		if (empty[i]){
			table[i]=value;
			empty[i]=false;
			keys[i] = k;
			return i;
		}
		iter++;

	}while(iter<m);

	return -1;
}

template<typename T> uint HashTable<T>::getNEmptyCells(){
	uint count = 0;
	for (uint i=0;i<m;i++){
		if (empty[i])count++;
	}
	return count;
}

template<typename T> T HashTable<T>::getAt(unsigned long long pos){
	return table[pos];
}

template<typename T> T HashTable<T>::search(unsigned long long key){
	unsigned long long iter=0;
	unsigned long long i;
	do{
		i = h(key,iter);
		if (empty[i]) return -1;
		if (keys[i]==key){
			return i;
		}
		assert(i<m);
		iter++;
	}while(iter<m);

	return -1;
}

template<typename T> unsigned long long HashTable<T>::search(unsigned long long precomputedHash, unsigned long long key){
	unsigned long long iter=0;
	unsigned long long i;
	do{
		i = h(precomputedHash,key,iter);
		if (empty[i]) return -1;
		if (keys[i]==key){
			return i;
		}
		assert(i<m);
		iter++;
	}while(iter<m);

	return -1;
}

template<typename T> uint HashTable<T>::getSize(){
	return m*(sizeof(T)+sizeof(uint));
}

template<typename T> unsigned long long HashTable<T>::getNext(unsigned long long pos) const{
	while(pos<m){
		if (empty[pos]){
			pos++;	
		}else{ 
			return pos;
		}
	}
	return -1;
}

template<typename T> size_t HashTable<T>::nearestPrime(size_t n)
{
    long position;  /* the prime number being sought */
    long index;

    for (position = n; ; position++)
    {
        // checks if those values from 2 to $\sqrt{m}$ can be factors of $m$ */
        for (index = 2; index <= (long) sqrt((double) position) && position % index != 0; index++) ;

        if (position % index != 0)  /* No factors in that range, therefore a prime number was found */
        {
            break;
        }
    }
    return position;
}


template<typename T> void HashTable<T>::save(ofstream &of){
	saveValue(of, a);
	saveValue(of, b);
	saveValue(of, m);
	saveValue(of, mprime);
	saveValue(of, p);
	saveValue(of,table,m);
	saveValue(of,empty,m);
	saveValue(of,keys,m);
}

template<typename T> HashTable<T> HashTable<T>::load(ifstream &in){
	HashTable<T> *t = new HashTable<T>();
	t->a = loadValue<unsigned long long>(in);
	t->b = loadValue<unsigned long long>(in);
	t->m = loadValue<unsigned long long>(in);
	t->mprime = loadValue<unsigned long long>(in);
	t->p = loadValue<unsigned long long>(in);
	t->table = loadValue<T>(in,t->m);
	t->empty = loadValue<bool>(in,t->m);
	t->keys = loadValue<unsigned long long>(in,t->m);
	return t;
}

};

#endif	

