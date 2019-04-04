/**
*    TextIndexHelper.h
*    Copyright (C) 2014 Alberto Ordóñez
*
*    Helper class for TextIndex.
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef TEXT_INDEX_HELPER_H_
#define TEXT_INDEX_HELPER_H_

namespace TextIndexes
{

    class TextIndexHelper
    {
    public:
        static void mapToCont(ulong n, uint max_sigma, uint *input, vector<uint> &dic, vector<uchar> &inv_dic){

            for (uint i=0;i<=max_sigma;i++){
                dic.push_back(0);
                inv_dic.push_back('0');
            }
            for (ulong i=0;i<n;i++){
                dic[input[i]]=1;
            }
            uint prev=0;
            for (uint i=0;i<max_sigma;i++){
                if (dic[i]){
                    dic[i]=prev;
                    inv_dic[prev]=(uchar)i;
                    prev++;
                }
            }
            for (ulong i=0;i<n;i++){
                input[i] = dic[input[i]];
            }
//            input[n]=0;
        }
    };


};
#endif
