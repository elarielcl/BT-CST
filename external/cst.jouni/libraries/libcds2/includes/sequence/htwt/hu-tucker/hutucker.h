#ifndef _HUTUCKER_INCLUDED
#define _HUTUCKER_INCLUDED

#include <sequence/htwt/hu-tucker/binarynode.h>

typedef struct HuTucker{

    bNode** seq; //auxiliar array for nodes
    bNode* root; //root of the tree
    int* levels; //depth of each leaf
    int start; //start position in the array
    int end; //end position in the array
    int len; //number of leafs/objects
    int weight; //number of internal nodes
    Tcode* codes;

}huTucker;


huTucker *createHuTucker(int* array, int szArray);
huTucker *createHuTuckerPsi(int* array, int szArray, uint *psi, uint *psiIntervals); 
uint numberOfBitsHutucker(huTucker ht); 
void destroyHuTucker(huTucker *ht);
void printHuTucker(huTucker ht);
#endif
