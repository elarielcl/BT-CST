#ifndef _HTWT_INCLUDED
#define _HTWT_INCLUDED
#include <sequence/htwt/hu-tucker/hutucker.h>
#include <sequence/htwt/hu-tucker/binarynode.h>
#include <bitsequence/BitSequenceRRR.h>


#define ESTADISTICAS_RUNS


typedef struct shtwtTree{
	void *bitmap; 
	uint *bitmapPlano;
	uint *children; 
	uint *starts; 
	uint n;//size of psi
	uint nleafs; 
	uint minBits; //se podría almacenar en la primera posición de childre que no se usa
	uint minBitsStarts; 
	uint vocSize; 
	uint numberOfBitsTree; 
}htwtTree; 

//~ 
//~ int createHTWT(htwtTree **tree, int *weights, uint weightsSize, uint *psi, uint n, uint samplingRate);  
//~ void destroyHTWT(htwtTree **ht); 
//~ 
//~ uint directApplicationHTWT(htwtTree *ht, uint i);
//~ uint inverseApplicationHTWT(htwtTree *tree, uint i);
//~ 
//~ int rankHTWT(htwtTree *htwt, uint pos, uint limInf); 
//~ 
//~ void saveHTWT(htwtTree *htwt, FILE *f); 
//~ 
//~ uint sizeHTWT(htwtTree *htwt); 
//~ 
//~ void loadHTWT(htwtTree **htwt, FILE *f); 
//~ 
//~ void printHTWTInfo(htwtTree *htwt); 

#endif
