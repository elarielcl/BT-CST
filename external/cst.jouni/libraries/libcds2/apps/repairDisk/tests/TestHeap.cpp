/********************************************************************************
Copyright (c) 2014, Alberto Ordóñez
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

* Neither the name of libcds nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

********************************************************************************/

#include <gtest/gtest.h>
#include <algorithm>


using namespace std;


TEST(Empty, Empty) {
    vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    ASSERT_EQ(v.size(), 3ul);
    make_heap(v.begin(), v.end());
    //pop_heap(v.begin(), v.end());

    ASSERT_EQ(v[0] , (int)3);

}


#if 0
void testOneElem(uint bits) {
    uint v = rand_r(&seed_test_array) & ((1 << bits) - 1);
    Array *a = new Array(1, 1u<<bits);
    a->setField(0, v);
    uint r = a->getField(0);
    ASSERT_EQ(v, r);
}


TEST(Array, OneElem) {
    for (uint i = 0; i < (1u<<30); i+=113) {
        testOneElem(i);
    }
}
#endif
