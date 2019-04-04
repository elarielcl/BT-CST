/*
This class extends the RePair algorithm implemented by Gonzalo Navarro. It carries out a disk-oriented
version until the resulting data structures fit in memory. From that point, it runs the
regular RePair.

Copyright (C) 2014-current_year Alberto Ordóñez

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Author's contact: Gonzalo Navarro, Dept. of Computer Science, University of
Chile. Blanco Encalada 2120, Santiago, Chile. gnavarro@dcc.uchile.cl

Alberto Ordóñez, alberto.ordonez@udc.es

*/

#ifndef REPAIR_DN_H
#define REPAIR_DN_H

#include <grammar/Repair.h>

#ifndef MB2B
#define MB2B(x) x*1048576
#endif

namespace cds_static{

    class RepairDN: public Repair{

    public:
        /**
        * Builds a grammar considering the input a sequence of chars
        * */
        RepairDN(char *input, long len, uint MB);
        /**
        * Builds a grammar considering the input a sequence of ints
        * */
        RepairDN(int *input, long len, uint MB);
        /**
        * Builds a grammar from a bitmap input of length len (len bits in total)
        * */
        static Repair *instanceFromBitmap(uint *input, long len, uint MB);

        virtual void save(ofstream &fp);

        static Repair *load(ifstream &fp);

    protected:
        RepairDN();
        virtual void runRepair(char *input, long len, uint MB);
        virtual void runRepair(int *input, long len, uint MB);
        void prepare0(long len, char *f1);
        long repair0(long len, char *f1, char *f2);
        virtual void prepare(long len);
        virtual void repair();

        int MB;

        int MBB; //MB in bytes

        int BSB; //disk block size in bytes

        int BSI; //disk block in ints (it must be a pair number of ints)

        int did0; // did prepare0 or not

        uint *buffer; //read buffer of BS bytes

        uint *bufferWrite; //necessary for repair0

        long diskRead; // disk read/writes
        long diskWrite; // disk read/writes

        long readBlock(FILE *in, uint *b);
        long writeBlock(FILE *in, uint *b);
        long rewindFile(FILE *in);
        //bs is the block size in bytes
        long nblocks(unsigned long long len, unsigned long long bs);
        //writes at pos%capacity of the buffer the value w. If pos is at the end of the

        long writeInBuffer(FILE *f, uint *buffer, size_t capacity, size_t pos, uint w);
        long readInBuffer(FILE *f, uint *buffer, uint *tailPrev, size_t capacity, size_t pos, uint *valueCpos, uint *valueCpos1, uint *valueCpos2);
    };

};
#endif
