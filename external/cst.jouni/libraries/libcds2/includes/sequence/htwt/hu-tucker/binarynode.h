
#ifndef _BINARYNODE_INCLUDED
#define _BINARYNODE_INCLUDED


#include <sequence/htwt/hu-tucker/basicsHT.h>
#include <sequence/htwt/hu-tucker/bitStringHT.h>



/** Auxiliar class to handle binary nodes.
 *
 *  @author Tradución de Alberto Ordóñez del código de Carlos Bedregal. 
 * 
 */

typedef struct
{ uint cbits;
  uint code;
} Tcode;


typedef struct bNode{
    int w; //weight
    int pos; //position
    int type; //0=external, 1=internal
    int endpoint[2];
    int psiSize; //only for leafs
    struct bNode* children[2];
    int obj;
    uint sizePsi;
    uint *psi; 

}bNode;

bNode * createEmptyBnode();

bNode * createBnode(int v, int p, int t, int* o);
bNode * createBnodePsi(int v, int p, int* o, uint *psi, uint sizePsi);

void print(bNode b);

void code(bNode b, int level, Tcode **codes, BitStringHT **tree, uint *bits, int *tpos);

void recPrint(bNode b, int level);

void freebNode(bNode *node); 
void freeAllbNode(bNode *node); 

void setEndpoints(bNode *b, int start, int end);
uint numberOfBitsRec(bNode b, int level);
bool isLeafBinaryNode(bNode *node);

#endif
