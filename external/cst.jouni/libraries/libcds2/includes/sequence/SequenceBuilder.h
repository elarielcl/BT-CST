/*  SequenceBuilder.h
 * Copyright (C) 2010, Francisco Claude, all rights reserved.
 *
 * Francisco Claude <fclaude@cs.uwaterloo.ca>
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

#include <utils/libcdsBasics.h>
#include <utils/StringUtils.h>
#include <sequence/Sequence.h>

#ifndef SEQUENCEBUILDER_H
#define SEQUENCEBUILDER_H

namespace cds_static
{

	class SequenceBuilder
	{
		public:
			SequenceBuilder() { userCount=0; }
			virtual ~SequenceBuilder() {}
			virtual Sequence * build(uint * seq, size_t len)=0;
			virtual Sequence * build(const Array & seq)=0;
			//input is the name of the file that contains a uint[len]
			virtual Sequence * build(string &input){
				uint *seq;
				long len;
				loadFile<uint>(input,seq,len);
				return build(seq,static_cast<size_t>(len));
			}
			virtual void use() { userCount++; }
			virtual void unuse() { userCount--; assert(userCount>=0); if(userCount==0) delete this; }

		protected:
			int userCount;
	};
};

#include <sequence/SequenceBuilderWaveletTreeNoptrs.h>
#include <sequence/SequenceBuilderGMRChunk.h>
#include <sequence/SequenceBuilderGMR.h>
#include <sequence/SequenceBuilderAlphPart.h>
#include <sequence/SequenceBuilderWaveletTree.h>
#include <sequence/SequenceBuilderWaveletMatrix.h>
#include <sequence/SequenceBuilderStr.h>
#include <sequence/SequenceBuilderCanonicalWTNoptrs.h>
#include <sequence/SequenceBuilderWaveletMatrix.h>
#include <sequence/SequenceBuilderWTNoptrsHybrid.h>
#include <sequence/SequenceBuilderSequenceMatrix.h>
//#include <sequence/SequenceBuilderLZBlock.h>
#include <sequence/SequenceBuilderBinary.h>
#endif
