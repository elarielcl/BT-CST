//
// Created by alberto on 3/19/15.
//
#ifndef _ALL_MAPPINGHELPER_H_
#define _ALL_MAPPINGHELPER_H_

#include <utils/libcdsBasicsGeneric.h>

using namespace cds_utils;
namespace cds_static {

    class MappingHelper {

        template<typename T>
        static uint getCodeLen(uint code, uint &ret_code, uint start_pos, uint maxLevel, T &nCodesAtLevel, T &minCodePerLevel) {
            uint len = -1;
            uint i = start_pos;
            ret_code = invertWord(code) >> (W - i);
            for (; i <= maxLevel; i++) {
                if (nCodesAtLevel[i] > 0) {
                    if (ret_code < (nCodesAtLevel[i] + minCodePerLevel[i])) {
                        len = i;
                        break;
                    }
                }
                ret_code = (ret_code << 1) | ((code >> i) & 1);
            }
            return len;
        }
    };
};
#endif //_ALL_MAPPINGHELPER_H_
