/* GenericArray.h
 * Copyright (C) 2015, Alberto Ordóñez, all rights reserved.
 *
 * Generic GenericArray
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



#include <fstream>
#include <vector>
#include <set>

#ifndef _GENERIC_GenericArray_H
#define _GENERIC_GenericArray_H

#include <utils/libcdsBasicsGeneric.h>

using namespace std;

namespace cds_utils {
    /** Class for GenericArrays with variable bit-width
    * @author Alberto Ordóñez
    */
    template<typename T>
    class GenericArray {
    protected:
        /** GenericArray where the data is stored */
        size_t *data;
        /** Length of the GenericArray in number of elements */
        size_t length;
        /** Length in bits of each field */
        uint bitsPerItem;
        /** Maximum value stored in the GenericArray */
        T maxValue;
        /** Length of the GenericArray in number of uints */
        size_t sizetLength;

        /** Initializes the GenericArray, all the values to 0
        */
        void initData();

        GenericArray& copy(const GenericArray &a);

    public:
        typedef typename std::vector<T> input_vector;
        typedef const typename std::vector<T> const_input_vector;
        typedef typename std::vector<T>::iterator input_iterator;
        typedef const typename std::vector<T>::iterator const_input_iterator;

        GenericArray(const GenericArray &&a){
            *this = copy(a);
        }
        /** Reads and GenericArray from a file stream
        * @param input input file stream
        */
        GenericArray(ifstream &input);

        /** Creates an GenericArray from a vector
        * @param A vector with the elements
        * @bpe bits per element
        */
        GenericArray(const_input_vector &A, uint bpe = 0);

        /** Creates an GenericArray from a vector iterator
        * @param ini initial position
        * @param fin final position
        * @bpe bits per element
        */
        GenericArray(const_input_iterator &ini, const_input_iterator &fin, uint bpe = 0);

        /** Creates an GenericArray copying part of a previously existing GenericArray
        * @param A source GenericArray
        * @param n length of the source GenericArray
        * @param bpe bits per element (0 uses the max in A[i,j]
        */
        GenericArray(T *A, size_t n, uint bpe = 0);

        /** Creates an GenericArray copying part of a previously existing GenericArray
        * @param A source GenericArray
        * @param i initial position
        * @param j final position
        * @param bpe bits per element (0 uses the max in A[i,j]
        */
        GenericArray(T *A, size_t i, size_t j, uint bpe = 0);

        /** Creates and GenericArray with n elements that can store elements between 0 and _maxValue
        * @param n length (in elements) of the GenericArray
        * @param _maxValue maximum value that could be stored in the GenericArray
        */
        GenericArray(size_t n, T _maxValue);

        /** Destroys the GenericArray */
        ~GenericArray();

        GenericArray();


        /** Retrieves GenericArray[pos]
        * @paran pos position
        * @return GenericArray[pos]
        */
        inline T getField(const size_t pos) const {
            assert(pos < length);
            return (T) LibcdsBasicsGeneric::get_field(data, bitsPerItem, pos);
        }

        /** Assigns v to GenericArray[pos]
        * @paran pos position
        * @param v value
        * @return GenericArray[pos]
        */
        inline T setField(const size_t pos, const T v) {
            assert(pos < length);
            assert(v <= maxValue);
            LibcdsBasicsGeneric::set_field(data, bitsPerItem, pos, (size_t) v);
            return v;
        }

        /**
        * Asignment operator (move semantics)
        */
        inline GenericArray<T>& operator=(GenericArray<T> &&a){
            this->length = a.length;
            this->maxValue = a.maxValue;
            this->bitsPerItem = a.bitsPerItem;
            this->sizetLength = a.sizetLength;
            this->data = a.data;
            a.data = nullptr;
            return *this;
        }

        /*destroys the access :-(
        inline GenericArrayModifier operator[](const uint pos) {
          return GenericArrayModifier(data,length,bitsPerItem,pos);
        }*/

        /** operator [] for getField
        */
        inline T operator[](const size_t pos) const {
            assert(pos < length);
            return (T) LibcdsBasicsGeneric::get_field(data, bitsPerItem, pos);
        }

        /** Saves the GenericArray into a file */
        void save(ofstream &out) const;

        /** Returns the size of the GenericArray in bytes
        */
        inline size_t getSize() const {
            return sizeof(ulong) * sizetLength + sizeof(this);
        }

        /** Returns the length of the GenericArray
        */
        inline size_t getLength() const {
            return length;
        }

        /** Returns the maximum value in the GenericArray
        */
        inline uint getMax() const {
            T maxV = (T) 0;     // default max
            for (size_t i = 0; i < length; i++)
                maxV = std::max(maxV, (T) getField(i));
            return maxV;
        }

        //shrinks the GenericArray to newLen
        //requires newLen<length
        //we keep the data as originally is. We only change the length
        inline bool resize(ulong newLen) {
            if (newLen <= length) {
                length = newLen;
                return true;
            }
            return false;
        }

    };

    template<typename T> GenericArray<T>& GenericArray<T>::copy(const GenericArray &a){
        this->bitsPerItem = a.bitsPerItem;
        this->length = a.length;
        this->maxValue = a.maxValue;
        this->sizetLength = a.sizetLength;
        this->data = new size_t[this->sizetLength];
        for(size_t i=0;i<this->sizetLength;i++)
            this->data[i] = 0ULL;
        return *this;
    }

    template<typename T> GenericArray<T>::~GenericArray(){
        delete [] data;
    }

    template<typename T> GenericArray<T>::GenericArray(){
        length = 0ULL;
        maxValue = 0ULL;
        bitsPerItem=0;
        sizetLength=0;
        data=nullptr;
    }

    template<typename T> GenericArray<T>::GenericArray(ifstream &input){
        length = loadValue<size_t>(input);
        maxValue = loadValue<size_t>(input);
        bitsPerItem = loadValue<uint>(input);
        sizetLength = loadValue<size_t>(input);
        data = loadValue<size_t>(input,sizetLength);
    }

    template<typename T> GenericArray<T>::GenericArray(const_input_vector &A, uint bpe){
        size_t n = A.size();
        maxValue = (T)0;
        if(bpe==0) {
            for(size_t k=0;k<n;k++)
                maxValue = std::max(maxValue,A[k]);
        }
        else {
            maxValue = (T)(1ULL<<(bpe))-1;
        }
        length = n;
        initData();
        assert(bpe==0 || bitsPerItem==bpe);
        for(size_t k=0;k<n;k++) {
            assert(A[k] <= maxValue);
            setField(k, A[k]);
        }
    }

    template<typename T> GenericArray<T>::GenericArray(const_input_iterator &ini, const_input_iterator &fin, uint bpe){
        size_t n = 0;
        maxValue = (T)0;
        for(auto it = ini; it!=fin ;++it) {
            maxValue = std::max(maxValue,*it);
            n++;
        }
        if(bpe!=0) {
            maxValue = (T)(1ULL<<(bpe))-1;
        }
        length = n;
        initData();
        assert(bpe==0 || bitsPerItem==bpe);
        n = 0;
        for(auto it=ini;it!=fin;++it) {
            assert(*it <= maxValue);
            setField(n,*it);
            n++;
        }
    }

    template<typename T> GenericArray<T>::GenericArray(T *A, size_t n, uint bpe){
        maxValue = (T)0;
        if(bpe==0) {
            for(size_t k=0;k<n;k++)
                maxValue = std::max(maxValue,A[k]);
        }
        else {
            maxValue = (T)(1ULL<<(bpe))-1;
        }
        length = n;
        initData();
        assert(bpe==0 || bitsPerItem==bpe);
        for(size_t k=0;k<n;k++) {
            assert(A[k] <= maxValue);
            setField(k, A[k]);
        }
    }

    template<typename T> GenericArray<T>::GenericArray(T *A, size_t i, size_t j, uint bpe){
        maxValue = (T)0;
        if(bpe==0) {
            for(size_t k=i;k<=j;k++)
                maxValue = std::max(maxValue,A[k]);
        }
        else {
            maxValue = (T)(1ULL<<(bpe))-1;
        }
        length = j-i+1;
        initData();
        assert(bpe==0 || bitsPerItem==bpe);
        for(size_t k=i;k<=j;k++) {
            assert(A[k] <= maxValue);
            setField(k-i, A[k]);
        }
    }

    template<typename T> GenericArray<T>::GenericArray(size_t n, T _maxValue){
        length = n;
        maxValue = _maxValue;
        initData();
    }

    template<typename T> void GenericArray<T>::initData() {
        bitsPerItem = LibcdsBasicsGeneric::bits((ulong)maxValue);
        sizetLength = LibcdsBasicsGeneric::sizetLen(length,bitsPerItem);
        data = new size_t[sizetLength];
        for(size_t i=0;i<sizetLength;i++)
            data[i] = 0ULL;
    }

    template<typename T> void  GenericArray<T>::save(ofstream & out) const
    {
        saveValue(out,(size_t)length);
        saveValue(out,(size_t)maxValue);
        saveValue(out,(uint)bitsPerItem);
        saveValue(out,(size_t)sizetLength);
        saveValue(out,data,sizetLength);
    }


};
#endif
