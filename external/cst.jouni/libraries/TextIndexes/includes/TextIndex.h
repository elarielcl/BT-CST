/**
*    TextIndex.h
*    Copyright (C) 2014 Alberto Ordóñez
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


#ifndef TEXT_INDEX_H_
#define TEXT_INDEX_H_

#define RLCSA_HDR 2
#include <utils/libcdsBasics.h>

using namespace cds_utils;
namespace TextIndexes
{

    enum typeSelfIndex{trlcsa=0,tfmi=1,trlfmi=2};

    class TextIndex
    {
    public:
        TextIndex(ulong len);
        virtual ~TextIndex();
        //bool build_index();

        /**
        * Returs the total size of the index in bytes (with that amount of
        * space you can run count,locate, and extract).
        */
        virtual ulong getSize()=0;

        /**
        * Returns the size of the index in bytes necessary to carry out count queries.
        */
        virtual ulong getSizeCount()=0;
        virtual uint locate(uchar * pattern, uint m, uint ** occ)=0;
        virtual ulong count(uchar * pattern, ulong m)=0;
        virtual uchar * extract(ulong i, ulong j)=0;

        /**
        * Returns the first letter of the suffix starting at position i.
        */
        virtual uchar getT   (ulong i)=0;

        /**
        * Returns psi(i)
        */
        virtual ulong getPsi (ulong pos)=0;

        /**
        * Iterates on psi k times
        */
        virtual ulong getPsi (ulong pos, ulong k)=0;

        /**
        * Returns bwt(i)
        */
        virtual uchar getBWT (ulong pos)=0;

        /**
        * Retursn SA[i]
        */
        virtual ulong getSA  (ulong pos)=0;
        /**
        * Retursn invSA[i]
        */
        virtual ulong getISA  (ulong pos)=0;

        /**
        * Tells the constructor if i want to keep the uncompressed versions of A and invA.
        */
        virtual TextIndex *keepUncompressedData(bool keep);

        //if i cannot store the invA and A, this function must always return false
        //f.i., the TextIndexRLCSA returns always false
        virtual bool haveUncompressedData();

        /**
        * Deleetes A and invA if they have been stored
        */
        virtual void deleteUncompressedData();

        bool isBuilt();

        /**
        * Before calling each of the following functions *Uncomp, the user must ensure he can
        * be calling first to haveUncompressedData()
        */
        virtual ulong getSAUncomp(ulong pos);
        virtual ulong getISAUncomp(ulong pos);

        virtual void save(char *filename)const=0;
        static TextIndex *load(char *filename);
        ulong getLength();

    protected:
        ulong len;
        bool built;//tells if the index has been built

    };
};

#include <TextIndexRLCSA.h>
#endif
