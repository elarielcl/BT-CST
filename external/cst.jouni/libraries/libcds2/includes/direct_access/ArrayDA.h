/* array.h
 * Copyright (C) 2009, Francisco Claude, all rights reserved.
 *
 * Array interface
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


#ifndef _ARRAY_DA_H
#define _ARRAY_DA_H

#include <fstream>
#include <vector>
#include <set>

#include <utils/libcdsBasics.h>
#include <direct_access/DirectAccess.h>


using namespace std;

namespace cds_static
{
	
	class ArrayDA :  public DirectAccess {
		public:
			/** Reads and Array from a file stream
			 * @param input input file stream
			 */
			ArrayDA(ifstream & input);

			/** Creates an arrayDA copying part of a previously existing arrayDA
			 * @param A source arrayDA
			 * @param n length of the source arrayDA
			 * @param bpe bits per element (0 uses the max in A[i,j]
			 */
			ArrayDA(uint * A, size_t n, uint bpe = 0);

			/** Creates and arrayDA with n elements that can store elements between 0 and _maxValue
			 * @param n length (in elements) of the arrayDA
			 * @param _maxValue maximum value that could be stored in the arrayDA
			 */
			ArrayDA(size_t n, uint _maxValue);

			/** Destroys the arrayDA */
			~ArrayDA();

			/** Retrieves ArrayDA[pos]
			 * @paran pos position
			 * @return ArrayDA[pos]
			 */
			inline uint getField(const size_t pos) const
			{
				assert(pos<len);
				return get_field(data, bitsPerItem, pos);
			}

			/** Assigns v to ArrayDA[pos]
			 * @paran pos position
			 * @param v value
			 * @return ArrayDA[pos]
			 */
			inline uint setField(const size_t pos, const uint v) {
				assert(pos<len); assert(v<=maxValue);
				set_field(data, bitsPerItem, pos, v);
				return v;
			}


			inline uint access(size_t pos) const{
				return get_field(data, bitsPerItem, pos);
			}

			/** Saves the arrayDA into a file */
			void save(ofstream & out) const;
			
			/** Returns the size of the arrayDA in bytes
			 */
			size_t getSize() const;

			/** Returns the maximum value in the arrayDA
			 */
			inline uint getMax() const
			{
				uint maxV = 0;	 // default max
				for(size_t i=0; i<len; i++)
					maxV = max(maxV,getField(i));
				return maxV;
			}

		protected:
			/** ArrayDA where the data is stored */
			uint * data;
			/** Length in bits of each field */
			uint bitsPerItem;
			/** Maximum value stored in the array */
			uint maxValue;
			/** Length of the array in number of uints */
			uint uintLength;

			/** Initializes the array, all the values to 0
			 */
			void initData();
	};
}
#endif
