#include <binarynode.h>


bNode * createEmptyBnode(){
	bNode * bn = (bNode *) malloc(sizeof(bNode)); 
    bn->w=-1;
    bn->obj=0;
    bn->pos=-1;
    bn->type=0;
    bn->endpoint[0]=-1; 
    bn->endpoint[1]=-1;
    bn->children[0]=0;
    bn->children[1]=0;	
    bn->psi = NULL; 
    return bn; 
}

bNode * createBnode(int v, int p, int t, int* o){
	bNode * bn = (bNode *) malloc(sizeof(bNode)); 
    bn->w=v;
    bn->obj=(*o);
    bn->pos=p;
    bn->type=t;
    bn->endpoint[0]=-1; 
    bn->endpoint[1]=-1;
    bn->children[0]=0;
    bn->children[1]=0;
    bn->psiSize = 0; 
    bn->psi  = NULL; 
    return bn; 
}
bNode * createBnodePsi(int v, int p, int* o, uint *psi, uint sizePsi){
	bNode * bn = (bNode *) malloc(sizeof(bNode)); 
    bn->w=v;
    bn->obj=(*o);
    bn->pos=p;
    bn->type= 0 ;
    bn->endpoint[0]=-1; 
    bn->endpoint[1]=-1;
    bn->children[0]=0;
    bn->children[1]=0;
    bn->sizePsi = sizePsi; 
    bn->psi = (uint *) malloc(sizeof(uint) * v); 
    uint i; 
    for (i = 0; i < bn->sizePsi; i++){
		bn->psi[i] = psi[i]; 
	}
	return bn; 
}

void freeAllbNode(bNode *node){
	
	if (node == NULL) return;
	freeAllbNode(node->children[0]); 
	freeAllbNode(node->children[1]); 
	freebNode(node); 
}

void freebNode(bNode *node){
	if (node == NULL) return ; 
	
	if (node->children[0] !=NULL){
		if (node->children[0][0].psi != NULL) {
			free(node->children[0][0].psi); 
			node->children[0][0].psi = NULL;
		}
		free(node->children[0]); 
		node->children[0] = NULL;
	}
	if (node->children[1] !=NULL){
		if (node->children[1][0].psi != NULL) {
			free(node->children[1][0].psi); 
			node->children[1][0].psi = NULL; 
		}
		free(node->children[1]); 
		node->children[1] = NULL; 
	}
}

void setEndpoints(bNode *b, int start, int end){
    b->endpoint[0]=start;
    b->endpoint[1]=end;
}


void print(bNode b){
	fprintf(stderr, "@ %p: w=%u pos=%u type=%u",&b, b.w, b.pos, b.type); 
    if(b.obj) fprintf(stderr," obj: %u", b.obj); 
    fprintf(stderr,"[%u, %u]\n", b.endpoint[0], b.endpoint[1]); 
    
}


void code(bNode b, int level, Tcode **codes, BitStringHT **tree, uint *bits, int *tpos)
{
    (*tpos)++;

    if(b.children[0])
    {
	bits[level] = 0;
       code(*(b.children[0]), level+1, codes, tree, bits, tpos);

	bits[level] = 1;
       code(*(b.children[1]), level+1, codes, tree, bits, tpos);
    }
    else
    {
	uint code = 0;
	int i; 
	for (i=1; i<=level; i++) if (bits[i-1]) bitset_F(&code, level-i);

	(*codes)[b.pos].code = code;
	(*codes)[b.pos].cbits = level;
    }

	setBit_BitStringHT((*tree), (*tpos), 1);
    (*tpos)++;
}

void recPrint(bNode b, int level){
	fprintf(stderr, "@L:%u w=%u pos=%u type=%u",level, b.w, b.pos, b.type);
    if (b.type == 0) fprintf(stderr," ************************* sizePsi: %u", b.sizePsi); 
    fprintf(stderr," [%u, %u]", b.endpoint[0], b.endpoint[1]); 
    if (b.type ==0){
			
			fprintf(stderr,"("); 
			//~ for (i = 0; i < b.sizePsi; i++){
				//~ fprintf(stderr,"%u, ", b.psi[i]); 
			//~ }
			fprintf(stderr,")\n"); 
	}else{
		fprintf(stderr,"\n"); 
	}

    if(b.children[0]){
        recPrint(*(b.children[0]), level+1);
        recPrint(*(b.children[1]), level+1);
    }
}

uint numberOfBitsRec(bNode b, int level){
	uint total  = 0; 
    if (b.type ==0){
			total += level * b.sizePsi; 
	}
	
    if(b.children[0]){
     return total 
				+ numberOfBitsRec(*(b.children[0]), level+1) 
				+ numberOfBitsRec(*(b.children[1]), level+1);
    }else{
		return total;
	}
}

bool isLeafBinaryNode(bNode *node){
    return ((node==NULL) || ((node->children[0] == NULL) && (node->children[1]==NULL)));
}



