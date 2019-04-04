/* wt_coder.cpp
 * Copyright (C) 2008, Francisco Claude, all rights reserved.
 *
 * wt_coder definition
 *
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
 */

#include <sequence/wt_coder.h>
#include <climits>
 
namespace cds_static
{

    wt_coder::wt_coder() {
        user_count=0;
    }

    void wt_coder::use() {
        user_count++;
    }

    void wt_coder::unuse() {
        user_count--;
        if(user_count==0) delete this;
    }

    float wt_coder::avgCodeLength() const{
        float avgCode=0.0;
        uint minCode=INT_MAX,maxCode=0;
        for (uint i=0;i<sigma;i++){
            uint code,len;
            len=getCode(i,code);
            avgCode+=len;
            assert(bits(code)<=len);
            minCode = min(minCode,len);
            maxCode = max(maxCode,len);
        }
        avgCode/=sigma;
        cout << ";sigma: " << sigma;
        cout << ";minle: " << minCode;
        cout << ";maxle: " << maxCode;
        cout << ";avgCo: " << avgCode;
        
        return avgCode;
    }

    wt_coder * wt_coder::load(ifstream & fp) {
        cerr << "warning: wt_coder::load() is not weel implemented" << endl;
        uint rd = loadValue<uint>(fp);
        size_t pos = fp.tellg();
        fp.seekg(pos-sizeof(uint));
        switch(rd) {
            case WT_CODER_HUFF_HDR: return wt_coder_huff::load(fp);
            case WT_CODER_BINARY_HDR: return wt_coder_binary::load(fp);
            case WT_CODER_HUFF_CAN_HDR: return wt_coder_huff_can::load(fp);
            case WT_CODER_SUBOPTIMAL_ADD: return wt_coder_SubOptimalAdd::load(fp);
            case WT_CODER_BALANCED: return wt_coder_Balanced::load(fp);
            case WT_CODER_PMIN: return wt_coder_Pmin::load(fp);
            case WT_CODER_SUBOPTIMAL: return wt_coder_Suboptimal::load(fp);
        }
        return NULL;
    }
};
