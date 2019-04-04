/* RMQ.h
 * Author: Sebastian Kreft (lz77index)
 *
 * RMQ declaration (Range Minimum/Maximum Query)
 * 
 * Adapted to libcds: Alberto Ordóñez
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

#ifndef _RMQ_
#define _RMQ_

#include <iostream>
#include <cstdio>
#include <RMQ_succinct.h> 

#ifndef rmq_type
 enum rmq_type{RMQ_MIN, RMQ_MAX};
 #endif

namespace cds_static
{

	/** RMQ implementation  
	 *
	 *  @author Sebastian Kreft
	 */
	class RMQ
	{

		public:
            /* It is implemented as a Range Minimum Query. Thus, if you want a 
               Range Maximum Query, you should set r=RMQ_MAX. The RMQ will make a copy
               the input array but taking the maximum value
               in that input and set array[i] = max_v-array[i]. By doing so, 
               a maximum is converted in a minimum, which is exactly the funcionality
               which is implemented. 
            */
        RMQ(int* array, uint length, rmq_type r=RMQ_MIN);
        ~RMQ();
        
        unsigned int getPos(int start, unsigned int end);
        /*Returns the type of the RMQ*/
        rmq_type getType();
        /*Returns the length of the array*/
        uint getLength();
        /*The class hass stored the input array, so we can access ramdonly 
          to any position*/
        // uint getValue(uint pos);
        
    private: 
        RMQ();
        RMQ_succinct *RMQ_s;
        rmq_type t;
        int* array;
        unsigned int length;

	};

};

#endif							 /* _RMQ_H */
