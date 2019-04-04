#include <gtest/gtest.h>
#include <deBruijn/DeBruijnBuilder.h>
#include <deBruijn/NodeMarker.h>
#include <deBruijn/ConstructCDBG.h>

using namespace gctbio;

uint binSearchGEQ(uint *input, uint n, uint v){
    uint l,p,r;
    uint pos,m, value;
    l=0;r=n-1;
    while(l<=r){
        p = (l+r)/2;

        if (v>input[p]){
            l=p+1;
        }else if (v<input[p]){
            r=p-1;
        }else{
            break;
        }
    }
    if (input[p]<v)
        return p+1;
    else
        return p;
}

TEST(DE_BRUIJN, DISABLED_BinSearch){
    const uint n = 7;
    uint input[n] = {0,2,3,5,6,9,12};
    ASSERT_EQ(binSearchGEQ(input, 7, 8), 5);
    ASSERT_EQ(binSearchGEQ(input, 7, 9), 5);
    ASSERT_EQ(binSearchGEQ(input, 6, 8), 5);
    ASSERT_EQ(binSearchGEQ(input, 6, 9), 5);
    ASSERT_EQ(binSearchGEQ(input, 7, 4), 3);
    ASSERT_EQ(binSearchGEQ(input, 2, 1), 1);
    ASSERT_EQ(binSearchGEQ(input, 1, 0), 0);
    ASSERT_EQ(binSearchGEQ(input, 7, 20), 7);
    ASSERT_EQ(binSearchGEQ(input, 1, 20), 1);
}


TEST(DE_BRUIJN, Build1){

//    ConstructCDBG *cdbg = new ConstructCDBG((char*)"/home/alberto/research/data/alabar.txt",(char*)"/home/alberto/research/data/alabar");
//    ConstructCDBG cdbg((char*)"/home/alberto/research/gct+bio/uwe/debruijn/test/testdata/yeast16.txt",(char*)"/home/alberto/research/gct+bio/uwe/debruijn/test/testdata/yeast16.fa.cst");
//    auto cdbe = cdbg.buildCDBG(2);

//    cout << "digraph G {" << endl;
//    for (ulong n = 0; n < cdbe->size(); n++) { //for each node
//        cout << "  " << n << " [label=\"" << (cdbe->suffix(n, 0)+1);
//        //print suffixes
//        for (ulong s = 1; s < cdbe->suffixes(n); s++) {
//            cout << "," << (cdbe->suffix(n, s)+1);
//        }
//        //print length of node
//        cout << ":" << cdbe->length(n) << "\"]" << endl;
//        //second, print edges
//        for (ulong e = 0; e < cdbe->edges(n); e++) {
//            cout << "  " << n << " -> " << cdbe->edge(n,e) << endl;
//        }
//    }
//    cout << "}";

}