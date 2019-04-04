#ifndef _BWT_BUILDER_H_
#define _BWT_BUILDER_H_
#include <utils/libcdsBasics.h>

using namespace cds_utils;

namespace  cds_static {

    class BWTBuilder {
    public:
        /**
        * Given an input (sequence of chars not $ terminated), maps the symbols to the continuous
        * interval and builds the BWT and the SA.
        */
        static uint *buildBWT(char *, size_t n, uint *&SA, bool del=false);

        /**
        * Given an input (sequence of integers not $ terminated), mapps the symbols to the continuous
        * interval and builds the BWT and the SA.
        */
        static uint *buildBWT(uint *, size_t n, uint *&SA, bool del=false);

        /**
        * Does not mapp the symbols
        */
        static uint *buildBWT(uint *, size_t n);
    protected:
        static uint *buildInternal(uint *, size_t n,uint *&SA);
    };
};
#endif