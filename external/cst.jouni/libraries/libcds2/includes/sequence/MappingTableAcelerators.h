//
// Created by alberto on 3/16/15.
//

#ifndef _ALL_MAPPINGTABLEACELERATORS_H_
#define _ALL_MAPPINGTABLEACELERATORS_H_
#include <sequence/MappingTable.h>

namespace cds_static {
    class MappingTableNoAcelerator : public MappingTable {
    public:
        MappingTableNoAcelerator(tuple *list, size_t sigma):MappingTable(list,sigma){
            acelerator_none = AceleratorNone(this->firstLevelWithTerminals,this->maxLevel,&nCodesAtlevel[0],this->minCodePerLevel);
        }

        ~MappingTableNoAcelerator(){}

        size_t getSize(){
            return sizeof(this)+MappingTable::getSize()+acelerator_none.getSize();
        }
        //returns the number of read bits, the number of read symbols and the symbols read
        uint getSymbols(uint code, uint &nSymbols, vector<uint> &symbols){
            uint rev_code;
            uint len   = acelerator_none.getCodeLen(code, rev_code);
            symbols[0] = getSymbol(rev_code,len);
            nSymbols   = 1;
            return len;
        }

    private:
        AceleratorNone acelerator_none;
    };

    class MappingTableAceleartorStart: public MappingTable {
    public:
        MappingTableAceleartorStart(tuple *list, size_t sigma, uint b):MappingTable(list,sigma){
            acelerator_start = AceleratorStart(this->firstLevelWithTerminals,this->maxLevel,&nCodesAtlevel[0],this->minCodePerLevel,b);
        }

        ~MappingTableAceleartorStart(){}

        size_t getSize(){
            return sizeof(this)+MappingTable::getSize()+acelerator_start.getSize();
        }
        //returns the number of read bits, the number of read symbols and the symbols read
        uint getSymbols(uint code, uint &nSymbols, vector<uint> &symbols){
            uint rev_code;
            uint len   = acelerator_start.getCodeLen(code,rev_code);
            symbols[0] = getSymbol(rev_code,len);
            nSymbols   = 1;
            return len;
        }
    private:
        AceleratorStart acelerator_start;
    };

    class MappingTableAceleratorExtended : public MappingTable {
    public:
        MappingTableAceleratorExtended(tuple *list, size_t sigma,uint x):MappingTable(list,sigma){
            vector<uint> symbols;
            symbols.reserve(sigma);
            for (uint i=0;i<sigma;i++){
                symbols.push_back(list[i].symbol);
            }
            acelerator_extended = AceleratorExtended(this->firstLevelWithTerminals,this->maxLevel,&(this->nCodesAtlevel[0]),this->minCodePerLevel,x,symbols);
            acelerator_start = AceleratorStart(this->firstLevelWithTerminals,this->maxLevel,&(this->nCodesAtlevel[0]),this->minCodePerLevel,x);
        }

        ~MappingTableAceleratorExtended(){}

        size_t getSize(){
            return sizeof(this)+MappingTable::getSize()+acelerator_start.getSize()+acelerator_extended.getSize();
        }
        //returns the number of read bits, the number of read symbols and the symbols read
        uint getSymbols(uint code, uint &nSymbols, vector<uint> &symbols){
            uint nbits = acelerator_extended.getCodeLen(code,nSymbols,symbols);
            if (nbits==0){
                uint rev_code;
                nbits   = acelerator_start.getCodeLen(code,rev_code);
                symbols[0] = getSymbol(rev_code,nbits);
                nSymbols   = 1;
            }
            return nbits;
        }
    private:
        AceleratorExtended acelerator_extended;
        AceleratorStart acelerator_start;
    };

}
#endif //_ALL_MAPPINGTABLEACELERATORS_H_
