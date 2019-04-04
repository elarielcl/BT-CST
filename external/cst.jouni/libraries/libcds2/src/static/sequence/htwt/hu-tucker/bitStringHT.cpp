

#include <sequence/htwt/hu-tucker/bitStringHT.h>


    BitStringHT* load_BitStringHT(FILE *f) {
        BitStringHT * bsf = (BitStringHT *) malloc(sizeof(BitStringHT)); 
        if (fread((char*)&(bsf->length),sizeof(size_t),1 ,f) == 0) return NULL;
        if (fread((char*)&(bsf->uintLength),sizeof(size_t),1 ,f) == 0) return NULL; 
        
        bsf->data = (uint *) malloc(sizeof(uint)* bsf->uintLength);  
        if (fread((char*)(bsf->data),bsf->uintLength*sizeof(uint),1 ,f) == 0) return NULL; 
        return bsf;
    }

    void initDataBitStringHT(BitStringHT *bsf, const size_t len) {
		
        bsf->length = len;
        bsf->uintLength = bsf->length/W_F+1;
        bsf->data = (uint *) malloc(sizeof(uint) * len); ;
        uint i; 
        for(i=0;i<bsf->uintLength;i++)
            bsf->data[i] = 0;
    }

    BitStringHT* create_BitStringHT(const size_t len) {
		BitStringHT * bsf = (BitStringHT *) malloc(sizeof(BitStringHT)); 
        initDataBitStringHT(bsf, len);
        return bsf; 
    }

    BitStringHT* create_BitStringHT_2(const uint * fields, const size_t len) {
		BitStringHT * bsf = (BitStringHT *) malloc(sizeof(BitStringHT)); 
		
        initDataBitStringHT(bsf, len);
        size_t i; 
        for(i=0; i<bsf->uintLength; i++)
            bsf->data[i] = fields[i];
        return bsf;
    }

    void save_BitStringHT(FILE *f, BitStringHT bsf) 
    {
   
        fwrite((char*)&bsf.length,sizeof(size_t), 1, f);
        fwrite((char*)&bsf.uintLength,sizeof(size_t), 1, f);
        fwrite((char*)bsf.data,bsf.uintLength*sizeof(uint), 1, f);
    }


/* ********************************************************************* */


            /** Sets the pos-th bit
             * @param pos position
             * @param bit value [0-1]
             */
             void setBit_BitStringHT(BitStringHT *bsf, const size_t pos, int bit) {
                if(bit) bitset_F(bsf->data,pos);
                else bitclean_F(bsf->data,pos);
            }

            /** Gets the pos-th bit
             * @param pos position
             */
             uint getBit_BitStringHT(BitStringHT bsf, const size_t pos) 
            {
                return bitget_F(bsf.data,pos);
            }




            /** Returns the size in bytes of the BitStringHT */
             size_t getSizeBitStringHT(BitStringHT bsf) 
            {
                return bsf.uintLength*sizeof(uint)+sizeof(size_t)*2 + sizeof(uint *);
            }

            /** Returns the length in bits of the BitStringHT */
             size_t getLengthBitStringHT(BitStringHT bsf) 
            {
                return bsf.length;
            }

            /** Returns a pointer to the buffer storing the values 
             */
             uint * getDataBitStringHT(BitStringHT bsf) 
            {
                return bsf.data;
            }

            /** Initializes the class fields */
            void initDataBitStringHT(BitStringHT *bsf, const size_t len);
