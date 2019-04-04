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

#ifndef _HEAP_UTILS_CDS_H
#define	_HEAP_UTILS_CDS_H

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
	Implementation of a heap
*/
    template <typename T>class Heap{
    public:

        Heap();
        ~Heap();

        unsigned long long insert(unsigned long long k, T value);
        T getTop();
        T popTop();


    protected:
        vector<T> *heap;
        void popTopRec();
    };

    template <typename T> Heap<T>::Heap(){
        heap = new vector<T>();
    }

    template <typename T> Heap<T>::~Heap(){
        delete heap;
    };

    template <typename T> T Heap<T>::getTop(){
        if (heap->size()==0) return 0;
        return heap[0];
    }

    template <typename T> T Heap<T>::popTop(){
        if (heap->size()==0) return 0;
        T value = (*heap)[0];
        return (*heap)[0];
    }

    template <typename T> void Heap<T>::popTopRec(size_t pos){
        uint largest;
        if (2*pos<heap->size() && (*heap)[2*pos]>(*heap)[pos]){
            largest = 2*pos;
        }else{
            largest=pos;
        }
        if ((2*pos+1)<heap->size() && (*heap)[2*pos+1]>(*heap)[largest]){
            largest = 2*pos+1;
        }
        if (largest!=pos){
            T v = (*heap)[pos];
            (*heap)[pos] = (*heap)[largest];
            (*heap)[largest] = v;
            popTopRec(largest);
        }
    }


};

#endif	

