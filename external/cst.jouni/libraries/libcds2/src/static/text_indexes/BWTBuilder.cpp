
#include <text_indexes/BWTBuilder.h>
#include <mapper/MapperCont.h>
#include <text_indexes/IntSuffixSort.h>
#include <utils/StringUtils.h>


namespace cds_static {

    uint *BWTBuilder::buildBWT(char *input, size_t n, uint *&SA, bool del) {

        uint mp[256];
        for (uint i=0;i<256;i++){
            mp[i]=0;
        }
        for (size_t i=0;i<n;i++){
            mp[(int)input[i]]++;
        }
        uint m = 1;
        for (uint i=0;i<256;i++){
            if (mp[i]!=0){
                mp[i]=m;
                m++;
            }
        }
        uint *new_input = new uint[n];
        for (size_t i=0;i<n;i++){
            new_input[i] = mp[(int)input[i]];
        }
        if (del) delete [] input;

        uint *BWT = buildInternal(new_input, n,SA);
        return BWT;
    }

    uint *BWTBuilder::buildBWT(uint *input, size_t n, uint *&SA, bool del) {

        MapperCont *mc = new MapperCont(input,n,BitSequenceBuilderRG(32));
        uint *new_input;
        if (del){
            new_input = input;
        }else{
            new_input = new uint[n];
        }
        for (size_t i=0;i<n;i++){
            new_input[i] = mc->map(input[i]);
        }

        uint *BWT = buildInternal(new_input, n,SA);

        if (del){
            delete [] input;
        }else{
            delete [] new_input;
        }

        return BWT;
    }

    uint *BWTBuilder::buildBWT(uint *input, size_t n) {

        uint *SA;
        uint *BWT = buildInternal(input, n, SA);
        delete SA;
        return BWT;
    }


    uint *BWTBuilder::buildInternal(uint *input, size_t n, uint *&SA){
        int *psi  = new int[n+1];
        int *invA = new int[n+1];
        int max_v=0;
        for (uint i=0;i<n;i++){
            invA[i]=input[i];
            max_v = max(max_v,invA[i]);
        }

        integer_suffix_sort::IntSuffixSort<int>::suffixsort(invA, psi, n, max_v+1,1);

        SA = new uint[n+1];
        for (uint i=0;i<n;i++){
            assert(invA[i]!=0);
            SA[invA[i]]=i;
        }
        SA[0]=n;

        delete [] psi;
        delete [] invA;
        uint *BWT = new uint[n+1];
        for (size_t i=0;i<=n;i++){
            if (SA[i]==0) BWT[i] = 0;
            else BWT[i] = input[SA[i]-1];
        }

        return BWT;
    }
}