/**
 *    fmi.h
 *    Copyright (C) 2011  Francisco Claude F.
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


#ifndef fmi_WORDS_H
#define fmi_WORDS_H

#include <sequence/SequenceBuilder.h>
#include <sequence/Sequence.h>
#include <bitsequence/BitSequenceBuilder.h>
#include <bitsequence/BitSequence.h>

#include <mapper/Mapper.h>
#include <algorithm>

using namespace std;

#define fmi_HDR 30
namespace cds_static
{
    class fmi
    {
      public:
        fmi(uchar *data, uint n, SequenceBuilder *seq_builder, bool free_text=false);
        fmi(uint *data, uint n, SequenceBuilder *seq_builder, bool free_text=false);
        fmi(ifstream & fp);
        ~fmi();

        fmi* set_samplepos(uint sample);
        fmi* set_samplesuff(uint sample);

        bool build_index(Array * a=NULL, BitSequence * b=NULL);

        uint size();
        void print_stats();
        uint length();
        uint getLength();
        
        uint locate(uchar * pattern, uint m, uint ** occ);
        uint count(uchar * pattern, uint m);
        uint count(uint * pattern, uint m);
        void BWS(uint sp, uint ep, uint pi, uint &nsp, uint &nep);
        uchar * extract(uint i, uint j);
        uchar extract_pos(uint i);
        uint getBWTSize();
        void print_bwt();
        /*returns the whole SA vector*/
        uint *getSA();
        /*returns the invSA*/
        int *getInvSA();
        /*Remove some data structres that are left accesible after building
         (A,invA, ...)*/
        // void freeSpace();
        uchar * rebuild();
        void save(ofstream & fp);


      protected:
        uint n,n1;
        uint sigma;
        Sequence * bwt;
        BitSequence * sampled;
        uint samplepos;
        uint samplesuff;
        uint * pos_sample;
        uint * suff_sample;
        uchar * sbuff;
        uint spos;
        uint * occ;
        uint maxV;

        bool built;
        bool free_text;
        uchar * _seq;
        uint * _sequ;
        uint * _bwt;
        uint * _sa;
        uint *A;
        int *invA;
        
        SequenceBuilder *seq_builder; 

        void build_bwt();
        void build_sa();
        void sort_sa(uint ini, uint fin);
        int cmp(uint i, uint j);
        void swap(uint i, uint j);
        uint pivot(uint ini, uint fin, uint piv);
        void fill_buffer(uint i, uint j);
        uint extractSegmentA(uint sp, uint ep, uint **positions);

    };
};
#endif
