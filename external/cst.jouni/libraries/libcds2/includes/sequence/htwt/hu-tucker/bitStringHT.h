#ifndef _BIT_STRINGHT_INCLUDED
#define _BIT_STRINGHT_INCLUDED

/* 
 * Copyright (C) 2012, Alberto Ordóñez, all rights reserved.

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
 * Contacting the author:
 *   Alberto Ordóñez:  alberto.ordonez@udc.es
 *
 */



#include <sequence/htwt/hu-tucker/basicsHT.h>

typedef struct sBitString{
            size_t length;
            size_t uintLength;
            uint * data;
            
}BitStringHT; 

            /** Reads a BitStringHT from a file stream
             * @param input input file stream
             */
           BitStringHT* load_BitStringHT(FILE *f);

            /** Creates a BitStringHT with len bits of space */
           BitStringHT* create_BitStringHT(const size_t len);
           
           
            /** Creates a bitmap from an array (len bits) */
           BitStringHT* create_BitStringHT_2(const uint * array, const size_t len);



            /** Sets the pos-th bit
             * @param pos position
             * @param bit value [0-1]
             */
              void setBit_BitStringHT(BitStringHT *bsf, const size_t pos, int bit);

            /** Gets the pos-th bit
             * @param pos position
             */
              uint getBit_BitStringHT(BitStringHT bsf, const size_t pos) ;



            /** operator [] for getBit
             */
            //~ inline uint operator[](BitStringHT bsf,const size_t pos) 
            //~ {
                //~ return bitget_F(bsf.data,pos);
            //~ }

            /** Saves the bitmap to a file
             * @param out file stream
             */
            void save_BitStringHT(FILE *f, BitStringHT bsf) ;

            /** Returns the size in bytes of the BitStringHT */
              size_t getSizeBitStringHT(BitStringHT bsf) ;

            /** Returns the length in bits of the BitStringHT */
              size_t getLengthBitStringHT(BitStringHT bsf);

            /** Returns a pointer to the buffer storing the values 
             */
              uint * getDataBitStringHT(BitStringHT bsf);

            /** Initializes the class fields */
            void initDataBitStringHT(BitStringHT *bsf, const size_t len);


#endif
