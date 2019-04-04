/* SparseSuffixSort.h
 * Copyright (C) 2015, Alberto Ordóñez, all rights reserved.
 *
 * Set of algorithms to sort sparse suffixes
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
#ifndef SPARSE_SUFFIX_SORT_H_
#define SPARSE_SUFFIX_SORT_H_

#include <bits/stl_vector.h>
#include <utils/IfstreamBuffer.h>
#include <text_indexes/IntSuffixSort.h>
#include <algorithm>
#include <utils/DisjointSet.h>

using namespace std;

namespace cds_utils {


    struct CellSort{
        CellSort(ulong ini, ulong end, long rank=0):ini(ini),end(end),rank(rank),id(rank){}
        CellSort():ini(0UL),end(0UL),rank(0){}
        ~CellSort(){}
        ulong ini;
        ulong end;
        long rank;
        long id;
    };

    template<typename T, typename I>
    class SparseSuffixSort {

    protected:
        I input;
        ulong length;
        DisjointSet<long> *ds;
         virtual bool sortFunction(CellSort &a, CellSort &b){
            //dir tells in which direction move the cursor along the input stream
            int dira,dirb;
            //len tells the length of the segment covered by each par
            long lena, lenb;
            dira=-1;dirb=-1;
            lena = a.ini-a.end;lenb = b.ini-b.end;
            if (a.ini<a.end){
                dira=1;
                lena= -lena;
            }
            if (b.ini<b.end){
                dirb=1;
                lenb= -lenb;
            }
            lenb++;lena++;
            assert(lenb>0);
            assert(lena>0);

            auto len = min(lenb,lena);
            ulong cursora, cursorb;
            cursora = a.ini;cursorb=b.ini;
            while(len>0){
                if (input[cursora]!=input[cursorb]){
                    return input[cursora]<input[cursorb];
                }else{
                    cursora+=dira;
                    cursorb+=dirb;
                    len--;
                }
            }
            if (lenb==lena){
                //this means that both suffixes are identical --> carry out a union
                //of both "sets" to remember that
               ds->unionSet(a.id, b.id);
            }
            return (cursora==length);
        }

    public:
        /**
        * input: contains the elements we want to sort. It must implement opreator[]
        * length: is the length of the input. If not provided, the input must have a
        *         getLength() method that returns that info.
        */
        SparseSuffixSort(I &input, ulong length=0):input(input),length(length),ds(nullptr){
            if (length==0){
                length = input.getLength();
                assert(length);
            }
        }

        /**
        * Does not free the input. Just self destroys.
        */
        virtual ~SparseSuffixSort(){}
        /**
        * Sorts the suffixes starting at positions[i].ini and ending at positions[i].end
        * using the function sortFunction. Note that if positions[i].ini > positions[i].end,
        * it considers the suffix starting at positions[i].ini (read the input backwards).
        */
        void sparseStringSort(vector<CellSort> &positions){
            ds = new DisjointSet<long>(positions.size());
            for (auto &x: positions){
                ds->makeSet(x.id, x.rank);
            }
            auto f = [&](CellSort a, CellSort b){
                return this->sortFunction(a, b);
            };
            sort(positions.begin(),positions.end(),f);
            for (auto &x: positions){
                assert(ds->findSet(x.id)!=-1U);
            }
        }

        /**
        * Receives the positions of the strings sorted (the result of applying sparseStringSort)
        */
        void sparseSuffixSort(vector<CellSort> &pSSorted, vector<long> &SA){
            //the suffixSort library only sorts ints
            vector<long> suffixes(pSSorted.size()+1);
            suffixes.reserve(pSSorted.size());
            int sigma = 1;
            ulong upperLimit = pSSorted.size()-1;
            for (ulong i=0;i<upperLimit;i++){
                suffixes[pSSorted[i].rank] = sigma;
                if (ds->findSet(pSSorted[i].id)!=ds->findSet(pSSorted[i+1].id)){
                    sigma++;
                }
            }
            suffixes[pSSorted[pSSorted.size()-1].rank] = sigma;
            suffixes[pSSorted.size()] = 0UL;

            vector<long> sa(pSSorted.size()+1);
            SA = move(sa);
            integer_suffix_sort::IntSuffixSort<long>::suffixsort(&suffixes[0],&SA[0], pSSorted.size(), sigma+1, 1);
        }

    };
};
#endif