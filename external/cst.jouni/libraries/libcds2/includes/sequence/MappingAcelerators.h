/* MappingAcelerators.h
 * Copyright (C) 2014-current_year, Alberto Ordóñez, all rights reserved.
 *
 * Strategies to speed up the decompression of Huffman codes.
 * References:
 * [1]: Liddell, M., Moffat, A.,"Decoding prefix codes", SPE 2006, vol 36, pg. 1687-1710.
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

#ifndef _MAPPING_ACELERATORS_H_
#define _MAPPING_ACELERATORS_H_

#include <bits/stl_bvector.h>
#include <utils/GenericArray.h>
#include <unordered_map>
#include <limits.h>

using namespace std;

namespace cds_static
{
    //None aceleartor is ussed. Just sequenctial search the cannonical structures
    class AceleratorNone
    {

    public:
        AceleratorNone(){}
        AceleratorNone(uint _firstLevelWithTerminals, uint _maxLevel,
                uint *_nCodesAtLevel, uint *_minCodesPerLevel):
                firstLevelWithTerminals(_firstLevelWithTerminals),maxLevel(_maxLevel){

            nCodesAtLevel.assign(_nCodesAtLevel, _nCodesAtLevel+maxLevel+1);
            minCodePerLevel.assign(_minCodesPerLevel, _minCodesPerLevel+maxLevel+1);
        }
        ~AceleratorNone(){}

        size_t getSize(){
            return sizeof(this)+sizeof(nCodesAtLevel)+sizeof(minCodePerLevel);
        }

        //the most signigicant bit in the rightmost position!!!
        uint getCodeLen(uint code, uint &ret_code){
            uint len=-1;
            ret_code = invertWord(code)>>(W-firstLevelWithTerminals);
            for (uint i=firstLevelWithTerminals;i<=maxLevel;i++){
                uint nCodes = nCodesAtLevel[i];
                if (nCodes>0) {
                    if (ret_code < (nCodes + minCodePerLevel[i])) {
                        len = i;
                        break;
                    }
                }
                ret_code = (ret_code<<1)|((code>>i)&1);
            }
            return len;
        }

    private:
        uint firstLevelWithTerminals;
        uint maxLevel;
        vector<uint> nCodesAtLevel;
        vector<uint> minCodePerLevel;
    };

    //Acelerator start found at Section 4.1. of [1].
    //Uses a table of 2^b bits which stores, at each position, which is the
    //shortests codeword that starts by that prefix.
    class AceleratorStart
    {

    public:
        AceleratorStart(){}
        AceleratorStart(uint _firstLevelWithTerminals, uint _maxLevel,
                uint *_nCodesLevel, uint *_minCodesPerLevel, uint _b):
            firstLevelWithTerminals(_firstLevelWithTerminals),maxLevel(_maxLevel),b(_b){
            nCodesAtLevel.assign(_nCodesLevel, _nCodesLevel+maxLevel+1);
            minCodePerLevel.assign(_minCodesPerLevel, _minCodesPerLevel+maxLevel+1);
            assert(b<32);
            buildTable();
        }

        ~AceleratorStart(){}

        AceleratorStart& operator=(AceleratorStart &&a) = default;

        size_t getSize(){
            return sizeof(this)+sizeof(nCodesAtLevel)+sizeof(minCodePerLevel)+start.getSize();
        }

//        uint findCode(const vector<pair<uint,uint>> &codes, uint b, uint value){
//            uint smallest = INT_MAX;
//            for (const auto &it: codes){
//                if ((it.first >> (max(0,static_cast<int>(it.second-b)))&mask_b_bits) == value)
//                smallest = smallest;//it.second is the length of the code
//            }
//        }
        void buildTable(){
            vector<pair<uint,uint>> codes;
            for (uint i=firstLevelWithTerminals;i<=maxLevel;i++){
                uint code = minCodePerLevel[i];
                for (uint j=0;j<nCodesAtLevel[i];j++){
                    codes.push_back(make_pair(code+j,i));
                }
            }
            for (uint i=1;i<codes.size();i++) {
                assert(codes[i].first > codes[i - 1].first);
                assert(codes[i].second >= codes[i - 1].second);
            }

            //codes are sorted
            //creates a table of b fiels with b bits each
            mask_b_bits = (1u << b)-1;
            start = GenericArray<uint>(mask_b_bits+1,maxLevel);
            for (uint i=0;i<=mask_b_bits;i++){
                start.setField(i,firstLevelWithTerminals);
            }
            for (uint i=0;i<=mask_b_bits;i++){
                uint smallest = firstLevelWithTerminals;
                for (const auto &it: codes){
                    uint code_shifted = (it.first >> (max(0,static_cast<int>(it.second-b)))&mask_b_bits);
                    if ( code_shifted >= i){
                        if (code_shifted == i)
                            smallest = it.second;
                        uint pos = (LibcdsBasicsGeneric::invertWord(it.first)>>(32-it.second)) & mask_b_bits;
                        start.setField(pos, smallest);
                        break;
                    }
                    smallest = it.second;//it.second is the length of the code
                }

            }
        }

//        //THE MOST SIGNIFICANT BIT SHOULD BE AT THE RIGHTMOST POSSITION (BIGENDIAN)
//        uint getCodeLen(uint code,uint reverted_code){
//            uint len=-1;
//            uint i = start[reverted_code & mask_b_bits];
//            for (;i<=maxLevel;i++){
//                if (nCodesAtLevel[i]==0) continue;
//                if (code <(nCodesAtLevel[i] + minCodePerLevel[i])){
//                    len = i;
//                    break;
//                }
//            }
//            return len;
//        }
        //THE MOST SIGNIFICANT BIT SHOULD BE AT THE RIGHTMOST POSSITION (BIGENDIAN)
        uint getCodeLen(uint code, uint &ret_code){
            uint len=-1;
            uint i = start[code & mask_b_bits];
            ret_code = invertWord(code) >> (W-i);
            for (;i<=maxLevel;i++){
                if (nCodesAtLevel[i]>0) {
                    if (ret_code < (nCodesAtLevel[i] + minCodePerLevel[i])) {
                        len = i;
                        break;
                    }
                }
                ret_code = (ret_code<<1)|((code>>i)&1);
            }
            return len;
        }
    private:
        uint firstLevelWithTerminals;
        uint maxLevel;
        uint b;
        GenericArray<uint> start;
        uint mask_b_bits;
        vector<uint> nCodesAtLevel;
        vector<uint> minCodePerLevel;
    };

    //Acelerator start found at Section 4.2. of [1].
    //Uses a table of 2^b bits which stores, at each position, which is the
    //shortests codeword that starts by that prefix.
    class AceleratorExtended
    {

    public:
        AceleratorExtended(){}
        //The vector symbols is sorted in order of increasing code
        AceleratorExtended(uint _firstLevelWithTerminals, uint _maxLevel,
                uint *_nCodesLevel, uint *_minCodesPerLevel, uint _x, const vector<uint> &symbols):
                firstLevelWithTerminals(_firstLevelWithTerminals),maxLevel(_maxLevel),x(_x){
            nCodesAtLevel.assign(_nCodesLevel, _nCodesLevel+maxLevel+1);
            minCodePerLevel.assign(_minCodesPerLevel, _minCodesPerLevel+maxLevel+1);
            buildTable(symbols);
        }

        ~AceleratorExtended(){}

        AceleratorExtended& operator=(AceleratorExtended &&a) = default;

        size_t getSize(){
            return sizeof(this)+sizeof(nCodesAtLevel)+sizeof(minCodePerLevel)+decodingTable.getSize()+
                    nsyms.getSize()+nbits.getSize()+offset.getSize();
        }

        void buildTable(const vector<uint> &symbols){
            L = (1U<<x)-1;
            vector<uint> decoding_vector;
            nsyms = GenericArray<uint>(L+1,x/firstLevelWithTerminals+(((x%firstLevelWithTerminals)!=0)?1:0));
            nbits = GenericArray<uint>(L+1,L);
            vector<uint> _offset;
            unordered_map<uint,pair<uint,uint>> code_symbol;

            auto it = symbols.cbegin();
            for (uint i=firstLevelWithTerminals;i<=maxLevel;i++){
                uint code = minCodePerLevel[i];
                for (uint j=0;j<nCodesAtLevel[i];j++){
                    code_symbol[code+j] ={*it,i};
                    ++it;
                };
            }
            _offset.push_back(0);
            for (uint i=0;i<=L;i++){
                uint number_of_bits=0;
                uint code_len = 0;
                uint code = 0;
                uint count_syms = 0;
                bool first = true;
                for (uint j=0;j<x;j++){

                     code = (code<<1)|((i>>j)&1U);
                     code_len++;
                    const auto found = code_symbol.find(code);
                    if (found !=code_symbol.end() && code_len==(*found).second.second){
                        if (first){
                            _offset.push_back(decoding_vector.size());
                        }
                        number_of_bits+=code_len;
                        code_len=0;
                        count_syms++;
                        decoding_vector.push_back((*found).second.first);
                        first = false;
                        code=0;
                    }
                }
                if (first){
                    _offset.push_back(_offset.back());
                }
                nbits.setField(i, number_of_bits);
                nsyms.setField(i, count_syms);
            }
            assert(_offset.size()==L+2);

            decodingTable = GenericArray<uint>(decoding_vector.size(),symbols.size());
            offset = GenericArray<uint>(_offset.size(),decoding_vector.size());
            for (ulong i=0UL;i<_offset.size();i++)
                offset.setField(i,_offset[i]);
            for (uint i=0;i<decoding_vector.size();i++){
                decodingTable.setField(i, decoding_vector[i]);
            }
        }

        //THE MOST SIGNIFICANT BIT SHOULD BE AT THE RIGHTMOST POSITION (BIGENDIAN)
        uint getCodeLen(uint code, uint &nsymbols, vector<uint> &symbols){
            uint index = code & L;
            nsymbols = 0;
            uint nb;
            if ((nb=nbits[index])==0) return 0;
            nsymbols = nsyms[index];
            uint pointer = offset[index];
            for (uint i=0;i<nsymbols;i++){
                symbols[i] = decodingTable[pointer+i];
            }
            return nb;
        }


    private:
        uint firstLevelWithTerminals;
        uint maxLevel;
        uint x;
        uint L;
        vector<uint> nCodesAtLevel;
        vector<uint> minCodePerLevel;
        GenericArray<uint> decodingTable;
        GenericArray<uint> nsyms;
        GenericArray<uint> nbits;
        GenericArray<uint> offset;

    };
};

#endif
