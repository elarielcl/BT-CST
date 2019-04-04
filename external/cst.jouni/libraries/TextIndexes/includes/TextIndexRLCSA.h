/**
*    TextIndex.h
*    Copyright (C) 2014 Alberto Ordóñez
*
*    Wrapper for the RLCSA implementation of Jouni Siren.
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


#ifndef TEXT_INDEX_RLCSA_H_
#define TEXT_INDEX_RLCSA_H_

#include <TextIndex.h>
#include <sequence/Sequence.h>
#include <rlcsa/rlcsa.h>
#include <utils/libcdsBasics.h>

using namespace cds_utils;

namespace TextIndexes
{
    class TextIndexRLCSA : public TextIndex
    {
    public:

        TextIndexRLCSA(unsigned char *data, ulong bytes, ulong block_size, ulong sa_sample_rate, bool multiple_sequences, bool delete_data);
        TextIndexRLCSA();
        ~TextIndexRLCSA();
        RLCSA *rlcsa;
        virtual ulong getSize();
        virtual ulong getSizeCount();
        virtual uint locate(uchar * pattern, uint m, uint ** occ);
        virtual ulong count(uchar * pattern, ulong m);
        virtual uchar * extract(ulong i, ulong j);
        virtual uchar getT   (ulong i);
        virtual ulong getPsi (ulong pos);
        virtual ulong getPsi (ulong pos, ulong k);
        virtual uchar getBWT (ulong pos);
        virtual ulong getSA  (ulong pos);
        virtual ulong getISA  (ulong pos);
        RLCSA *getRLCSA();
        virtual void save(char *filename)const;
        static TextIndex *load(const char *filename);

    protected:

        bool build_index_internal();

        //tmp data
        uchar* data;
        ulong bytes;
        ulong block_size;
        ulong sa_sample_rate;
        bool multiple_sequences;
        bool delete_data;
    };

};
#endif
