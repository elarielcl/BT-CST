#if 0
#ifndef SA_SORT_H_
#define SA_SORT_H_


#include <climits>

//TODO fix this class. It is only needed to compute the Sufix Array. It should be static or something like that.
namespace cds_static{

#define KEY(p)          (V[*(p)+(h)])
#define SWAP(p, q)      (tmp=*(p), *(p)=*(q), *(q)=tmp)
#define MED3(a, b, c)   (KEY(a)<KEY(b) ?                        \
        (KEY(b)<KEY(c) ? (b) : KEY(a)<KEY(c) ? (c) : (a))       \
        : (KEY(b)>KEY(c) ? (b) : KEY(a)>KEY(c) ? (c) : (a)))


    class SuffixSort {
    protected:
        static int *I;                 /* group array, ultimately suffix array.*/
        static int *V;                          /* inverse array, ultimately inverse of I.*/
        static int r;                           /* number of symbols aggregated by transform.*/
        static int h;                           /* length of already-sorted prefixes.*/

        /* Subroutine for select_sort_split and sort_split. Sets group numbers for a
           group whose lowest position in I is pl and highest position is pm.*/
        static void update_group(int *pl, int *pm);

        /* Quadratic sorting method to use for small subarrays. To be able to update
           group numbers consistently, a variant of selection sorting is used.*/
        static void select_sort_split(int *p, int n);

        /* Subroutine for sort_split, algorithm by Bentley & McIlroy.*/
        static int choose_pivot(int *p, int n);

        /* Sorting routine called for each unsorted group. Sorts the array of integers
           (suffix numbers) of length n starting at p. The algorithm is a ternary-split
           quicksort taken from Bentley & McIlroy, "Engineering a Sort Function",
           Software -- Practice and Experience 23(11), 1249-1265 (November 1993). This
           function is based on Program 7.*/

        static void sort_split(int *p, int n);

        /* Bucketsort for first iteration.

           Input: x[0...n-1] holds integers in the range 1...k-1, all of which appear
           at least once. x[n] is 0. (This is the corresponding output of transform.) k
           must be at most n+1. p is array of size n+1 whose contents are disregarded.

           Output: x is V and p is SuffixSort::I after the initial sorting stage of the refined
           suffix sorting algorithm.*/

        static void bucketsort(int *x, int *p, int n, int k);
    /* Transforms the alphabet of x by attempting to aggregate several symbols into
       one, while preserving the suffix order of x. The alphabet may also be
       compacted, so that x on output comprises all integers of the new alphabet
       with no skipped numbers.

       Input: x is an array of size n+1 whose first n elements are positive
       integers in the range l...k-1. p is array of size n+1, used for temporary
       storage. q controls aggregation and compaction by defining the maximum value
       for any symbol during transformation: q must be at least k-l; if q<=n,
       compaction is guaranteed; if k-l>n, compaction is never done; if q is
       INT_MAX, the maximum number of symbols are aggregated into one.

       Output: Returns an integer j in the range 1...q representing the size of the
       new alphabet. If j<=n+1, the alphabet is compacted. The global variable r is
       set to the number of old symbols grouped into one. Only x[n] is 0.*/

        static int transform(int *x, int *p, int n, int k, int l, int q);

        static void reset();

    public:
    /* Makes suffix array p of x. x becomes inverse of p. p and x are both of size
       n+1. Contents of x[0...n-1] are integers in the range l...k-1. Original
       contents of x[n] is disregarded, the n-th symbol being regarded as
       end-of-string smaller than all other symbols.*/

        static void suffixsort(int *x, int *p, int n, int k, int l);
    private:
        SuffixSort(){}
        ~SuffixSort(){}
    };
}
#endif
#endif