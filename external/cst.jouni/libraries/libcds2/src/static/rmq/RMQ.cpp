/*  Sequence.cpp
 * Sebastian Kreft
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <rmq/RMQ.h>
#include <iostream>
#include <cassert>

namespace cds_static
{

RMQ::RMQ(int* _array, uint length, rmq_type r){
    assert(_array);
    assert(length>0);
    int max_v=0;
    this->array = new int[length];
    for (uint i=0;i<length;i++){
        this->array[i]=_array[i];
    }
    if (r==RMQ_MAX){
        for (uint i=0;i<length;i++){
            max_v=max(max_v,array[i]);
        }
        for (uint i=0;i<length;i++){
            array[i]=max_v-array[i];  
        }
    }

    this->RMQ_s = NULL;
    if(length>=200){
        this->RMQ_s = new RMQ_succinct(array,length);
    }
    this->t = r;
    this->length = length;
    
}


RMQ::RMQ(){
    this->RMQ_s = NULL;
    this->array=NULL;
    this->length=0;
    this->t = RMQ_MIN;
}

RMQ::~RMQ(){
    if(RMQ_s!=NULL)delete RMQ_s;
    if (array) delete [] array;
}

rmq_type RMQ::getType(){
    return t;
}

uint RMQ::getLength(){
    return length;
}

unsigned int RMQ::getPos(int start, unsigned int end){
    unsigned int minpos;
    // start++;
    // end++;
    if(end>=this->length || start<0)return ~0;
    if(this->RMQ_s!=NULL && end-start>=50)
        return this->RMQ_s->query(start, end);
    minpos = start;
    for(unsigned i=start+1;i<=end;i++){
        if(this->array[i]<this->array[minpos])minpos=i;
    }    
    return minpos;
}
	
};
