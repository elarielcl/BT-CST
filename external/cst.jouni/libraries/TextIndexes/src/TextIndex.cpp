#include <TextIndex.h>
#include <utils/cppUtils.h>
using namespace cds_utils;

namespace TextIndexes{

    TextIndex::TextIndex(ulong len) {
        this->len = len;
        this->built = false;
    }
    TextIndex::~TextIndex(){}

    ulong TextIndex::getLength(){
        return len;
    }


    TextIndex *TextIndex::load(char *filename){

        ifstream fp(filename);
        uint w = loadValue<uint>(fp);
        fp.close();
        switch (w){
            case RLCSA_HDR: return TextIndexRLCSA::load(filename);
            default: return nullptr;
        }
    }


    TextIndex *TextIndex::keepUncompressedData(bool keep) {
        return this;
    }

    bool TextIndex::haveUncompressedData() {
        return false;
    }

    void TextIndex::deleteUncompressedData() {}

    ulong TextIndex::getSAUncomp(ulong pos){
        return (ulong)-1;
    }

    ulong TextIndex::getISAUncomp(ulong pos){
        return (ulong)-1;
    }

    bool TextIndex::isBuilt(){
        return built;
    }

//    bool TextIndex::build_index(){
//        built = true;
//        build_index_internal();
//        return true;
//    }
}