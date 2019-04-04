/* GenericMapperCont.h
 * Copyright (C) 2015, Alberto Ordóñez, all rights reserved.
 *
 * Generic mapperCont
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

#ifndef _GENERIC_MAPPERCONT_H
#define _GENERIC_MAPPERCONT_H

#include <utils/libcdsBasics.h>
#include <iostream>
#include <bitsequence/BitSequence.h>
#include <bitsequence/BitSequenceBuilder.h>
#define GENERIC_MAPPER 0
#define GENERIC_MAPPER_CONT 1
using namespace std;
using namespace cds_utils;

namespace cds_static
{

    template<typename T> class GenericMapper{
    public:
        GenericMapper(){}
        virtual ~GenericMapper(){}
        virtual T map(T s) const{return s;}
        virtual T unmap(T s) const{return s;}
        virtual size_t getSize() const{return sizeof(this);}
        virtual void save(ofstream & out) const{
            uint wr = GENERIC_MAPPER;
            saveValue(out, wr);
        }
        static GenericMapper<T> * load(ifstream & input){
            uint wr;
            loadValue<uint>(input);
            auto pos = input.tellg();
            input.seekg(pos-sizeof(uint),ios_base::beg);
            switch(wr){
                case GENERIC_MAPPER:
                    return new GenericMapper<T>();
                case GENERIC_MAPPER_CONT:
                    return nullptr;
            };
            return nullptr;
        }

    };
    /** Mapper that makes the values in the set contiguous*/
    template<typename T> class GenericMapperCont: public GenericMapper<T>
    {
    public:

        GenericMapperCont(const T * A, const size_t len, const BitSequenceBuilder & bsb){
            if (len==0){
                throw new runtime_error("In GenericMapperCont the size of the input is 0");
            }
            T max_v = A[0];
            for (ulong i=1;i<len;i++){
                max_v = max(max_v,A[i]);
            }
            uint *bmp;
            createEmptyBitmap(&bmp, max_v+1);
            for (ulong i=0;i<len;i++)
                bitset(bmp,A[i]);
            m = bsb.build(bmp, max_v+1);
            delete bmp;
        }

        virtual ~GenericMapperCont(){
            if (m) delete m;
        }

        virtual T map(T s) const{
            return m->rank1(s);
        }
        virtual T unmap(T s) const{
            return m->select1(s);
        }
        virtual size_t getSize() const{
            return sizeof(this)+m->getSize();
        }

        virtual void save(ofstream & out) const{
            uint wr = GENERIC_MAPPER_CONT;
            saveValue(out, wr);
            m->save(out);
        }

        static GenericMapperCont<T> * load(ifstream & input){
            uint wr;
            loadValue<uint>(input);
            if (wr!= GENERIC_MAPPER_CONT){
                return nullptr;
            }
            GenericMapperCont<T> *ret = new GenericMapperCont<T>();
            ret->m = BitSequence::load(input);
            return ret;
        }

    protected:
        GenericMapperCont():GenericMapper<T>(){m=nullptr;}
        BitSequence * m;

    };

};
#endif
