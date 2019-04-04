#include <htwt.h>
#include <math.h>

void crearHT(htwtTree *tree, bNode *node, uint *bitmap, uint *pb, uint nleafs, 
					uint i, uint *libre, uint minBits);
void preMergePsiHTWT( bNode *f, bNode *l, bNode *r, uint *bitmap, uint *pb); 
void mergePsiHTWT(uint *psiL, uint sizePsiL, uint *psiR, uint sizePsiR, uint **psiN, uint *sizePsiN, uint *bitmap, uint pb);
void printBitmapRoot(htwtTree *ht); 
uint my_log2HTWT(uint value);

void calculateH0Runs(uint n, uint nruns, uint *runs); 
void generateFileHuffman(uint weightsSize, uint psiSize, uint *weights); 



int createHTWT(htwtTree **tree, int *weights, uint weightsSize, uint *psi, uint n, uint samplingRate){

	uint i; 
	

	//~ fprintf(stderr, "weights-size: %u\n", weightsSize); 
	
	double myH0 = 0; uint ble = 0; 
	for (i = 0; i < weightsSize; i++){
		ble+=weights[i]; 
		myH0+= (((double)weights[i]/n) * log2((double)n / weights[i])); 
	}					
	fprintf(stderr, "H0: %f, totalWords: %u\n", myH0, ble); 

	uint vocSize = weightsSize; 
	
	huTucker *ht = createHuTuckerPsi((int *) weights, weightsSize, psi,(uint *) weights); 
	//~ printHuTucker(*ht); 

	//mejor usar lo de numberOfBitsHuTucker...
	uint numberOfBitsHuTucker = numberOfBitsHutucker(*ht);
	fprintf(stderr, "Total number of bits of hu-tucker codification: %u\n",numberOfBitsHuTucker);   
	uint *bitmapHTWT = (uint *) calloc((((numberOfBitsHuTucker+1)+31)/32), sizeof(uint) );
	uint pbHTWT = 1; 
	
	if (bitmapHTWT == NULL) {
		fprintf(stderr,"Not enough memory to bitmapHTWT (malloc fail). Program will exit.\n"); 
		exit(0); 
	}
	
	(*tree) = (htwtTree *) malloc(sizeof(struct shtwtTree)); 
	if (tree == NULL){
		fprintf(stderr, "Not enough memory to create tree (htwt.c: crearHT). Program will exit. \n"); 
		exit(0); 
	}
	
		
	/*************************************************************/
	uint nleafs = weightsSize; 
	uint totalNodes = 2*nleafs-1; //binary tree
	uint minBits = my_log2HTWT(2*nleafs+1)+1;
	uint minBitsStarts;
	
	//no se usan las posiciones 0 de los vectores
	(*tree)->children = (uint *) malloc(sizeof(uint) * uint_len_F(minBits, 2*nleafs+1) );
	//~ (*tree)->starts = (uint *) malloc(sizeof(uint) * uint_len_F(minBitsStarts, nleafs)  );
	(*tree)->starts = (uint *) malloc(sizeof(uint) * nleafs  );

	/*************************************************************/
	uint firstGap = 2;//indicates the firs gap in the list of tree nodes 
	crearHT((*tree), ht->root, bitmapHTWT, &pbHTWT, nleafs, 1, &firstGap, minBits); 
	destroyHuTucker(ht);
	
	/************** TRANSFORME VECTOR OF UINT INTO VECTOR OF LOG2(BITMAP_LENGTH) **/
	
		minBitsStarts = my_log2HTWT(pbHTWT); 
		uint *starts =  (uint *) malloc(sizeof(uint) * uint_len_F(minBitsStarts , nleafs)  ); 
		for (i = 1; i < nleafs; i++){
			set_field_F(starts, minBitsStarts, i, (*tree)->starts[i]);
		}
		free((*tree)->starts); 
		(*tree)->starts = starts; 
		
	/*********************************************************************/
	(*tree)->n = n; 
	(*tree)->nleafs = nleafs; 
	(*tree)->numberOfBitsTree = pbHTWT; 
	(*tree)->vocSize = vocSize; 
	fprintf(stderr,"\n\n\tNumber of bits of the bitmap: %u\n\n", pbHTWT); 
	
	//~ (*tree)->bitmap = createBitSequenceRRR(bitmapHTWT ,(size_t)pbHTWT , samplingRate);
	
	(*tree)->bitmapPlano = bitmapHTWT;
	(*tree)->minBits = minBits; 
	(*tree)->minBitsStarts = minBitsStarts;

	
	return 0; 
	
	
}

uint isLeafBnode(bNode *node){
	
	if ((node==NULL) || ((node->children[0] == NULL) && (node->children[1]==NULL)))
		return 1; 
	return 0; 
}

void crearHT(htwtTree *tree, bNode *node, uint *bitmap, uint *pb, uint nleafs, uint i, 
				uint *libre, uint minBits){

	if (node == NULL) return ; 

	if (isLeafBnode(node->children[0])){
		set_field_F(tree->children, minBits, i*2, nleafs+node->children[0]->pos);//+node->children[0]->pos); //[i*2] = nleafs; // despois faráse con setValue...
	}else{
		//~ fprintf(stderr, "left\n"); 
		set_field_F(tree->children, minBits, i*2, (*libre));  
		(*libre)++;  
		crearHT(tree, node->children[0] , bitmap, pb , nleafs, (*libre)-1, libre, minBits);
	}
	if (isLeafBnode(node->children[1])){
		set_field_F(tree->children, minBits, i*2+1, nleafs+node->children[1]->pos);//+node->pos);
	}else{
//~ fprintf(stderr, "right\n");
		set_field_F(tree->children, minBits, i*2+1, (*libre));  
		(*libre)++;
		crearHT(tree, node->children[1] , bitmap, pb , nleafs, (*libre)-1, libre, minBits);
	}

	//i comienza en 0
	tree->starts[i] = (*pb)-1; 
	preMergePsiHTWT(node, node->children[0], node->children[1], bitmap, pb); 
	freebNode(node); 

}


void preMergePsiHTWT(bNode *f, bNode *l, bNode *r, uint *bitmap, uint *pb){
	uint *psiL, *psiR;
	uint sizePsiL, sizePsiR; 
	
	if (l !=NULL){
		sizePsiL = l->sizePsi;
		psiL = l->psi; 
	}else{
		sizePsiL = 0; 
	}
	
	if (r !=NULL){
		sizePsiR = r->sizePsi; 
		psiR = r->psi; 
	} else{
		sizePsiR = 0; 
	}
	uint *bmp = NULL; 
	uint w; 
	//~ fprintf(stderr, "SizePsi: %u\n", sizePsiR+sizePsiL); 
	
	mergePsiHTWT(psiL, sizePsiL, psiR, sizePsiR, &(f->psi), &w, bitmap, *pb); 
	
	if (w != 0){
		f->sizePsi = w; 
		(*pb)+= f->sizePsi; 
	}

}

void mergePsiHTWT(uint *psiL, uint sizePsiL, uint *psiR, uint sizePsiR, uint **psiN, uint *sizePsiN, uint *bitmap, uint pb){
	
	if (sizePsiL == 0 && sizePsiR == 0){
		psiN = NULL; 
		(*sizePsiN) = 0; 
		return; 
	}

	long int i; 
	uint pl=0, pr=0, pn; 
	
	
	
	(*psiN) = (uint *) malloc(sizeof(uint) * (sizePsiL + sizePsiR)); 
	if ((*psiN) == NULL){
		fprintf(stderr,"***Error in malloc (function htwt.c:mergePsiHTWT()). Program will exit.\n"); 
		exit(9); 
	}
	
	pn = 0; 
	while((pl < sizePsiL) && (pr < sizePsiR)){
		if (psiL[pl] < psiR[pr]){
			(*psiN)[pn++] = psiL[pl++]; 
		}else{
			bitset(bitmap, pb); 
			(*psiN)[pn++] = psiR[pr++]; 
		}
		pb++; 
	} 

	if (pl < sizePsiL){
		while (pl < sizePsiL){
			(*psiN)[pn++] = psiL[pl++]; 
		}
		
	}else{
		while (pr < sizePsiR){
			bitset(bitmap,pb); 
			pb++;
			(*psiN)[pn++] = psiR[pr++]; 
		}
	}
	(*sizePsiN) = (sizePsiL + sizePsiR); 

}

uint isLeaf(htwtTree *tree, uint posInTree){
	return (posInTree == tree->nleafs);
}

uint getLeftChild(htwtTree *tree, uint posInTree){
	if (posInTree >= tree->nleafs) return 0; 
	return get_field_basic(tree->children, tree->minBits,posInTree*2);
}

uint getRightChild(htwtTree *tree, uint posInTree){
	if (posInTree >= tree->nleafs) return 0; 
	return get_field_basic(tree->children, tree->minBits,posInTree*2+1);
}


uint recursiveInverseApplication(htwtTree *tree, uint posInTree, uint i, uint p, uint s){

	if (posInTree == tree->nleafs){ 
		return p+i-1; 
	}
	uint start = get_field_basic(tree->starts, tree->minBitsStarts, posInTree); 
	
	uint r0, r1; 
	if (access1(((bitSequenceRRR *)tree->bitmap), start + i) == 0){
			r0 = rank0(((bitSequenceRRR *)tree->bitmap), start); 
		i = rank0(((bitSequenceRRR *)tree->bitmap), i + start ) - r0 ; 
		s = rank0(((bitSequenceRRR *)tree->bitmap), s + start ) - r0  ; 
		return recursiveInverseApplication(tree, getLeftChild(tree, posInTree), i, p, s); 
	}else{
			r0 = rank0(((bitSequenceRRR *)tree->bitmap), start); 
			r1 = rank1(((bitSequenceRRR *)tree->bitmap), start); 
		i =   rank1(((bitSequenceRRR *)tree->bitmap), i + start ) - r1 ; 
		p += (rank0(((bitSequenceRRR *)tree->bitmap), s + start ) - r0 ); 
		s =   rank1(((bitSequenceRRR *)tree->bitmap), s + start) - r1 ; 
		return recursiveInverseApplication(tree, getRightChild(tree, posInTree), i, p, s); 
	}
}

uint inverseApplicationHTWT(htwtTree *tree, uint i){
	
	return recursiveInverseApplication(tree, 1, i+1, 0, tree->n); 
	
	//that follow is an iterative version
	uint start, r0, r1, posInTree = 1, p = 0, s = tree->n;
	i++;
	while(posInTree != tree->nleafs){ 

		start = get_field_basic(tree->starts, tree->minBitsStarts, posInTree); 

		r0, r1; 
		if (access1(((bitSequenceRRR *)tree->bitmap), start + i) == 0){
				r0 = rank0(((bitSequenceRRR *)tree->bitmap), start); 
			i = rank0(((bitSequenceRRR *)tree->bitmap), i + start ) - r0 ; 
			s = rank0(((bitSequenceRRR *)tree->bitmap), s + start ) - r0  ; 
			posInTree = getLeftChild(tree, posInTree);
			//~ return recursiveInverseApplication(tree, getLeftChild(tree, posInTree), i, p, s); 
		}else{
				r0 = rank0(((bitSequenceRRR *)tree->bitmap), start); 
				r1 = rank1(((bitSequenceRRR *)tree->bitmap), start); 
			i =   rank1(((bitSequenceRRR *)tree->bitmap), i + start ) - r1 ; 
			p += (rank0(((bitSequenceRRR *)tree->bitmap), s + start ) - r0 ); 
			s =   rank1(((bitSequenceRRR *)tree->bitmap), s + start) - r1 ; 
			posInTree = getRightChild(tree, posInTree);
			//~ return recursiveInverseApplication(tree, getRightChild(tree, posInTree), i, p, s); 
		}
	}
		return p+i-1; 
	
}

uint recursiveDirectApplication(htwtTree *tree, uint posInTree, uint s, uint j){
	
	uint left, right; 

	if (posInTree == tree->nleafs){ 
		return j; 
	}
	left = getLeftChild(tree,posInTree); 
	right = getRightChild(tree,posInTree); 
	
	uint start = get_field_basic(tree->starts, tree->minBitsStarts, posInTree); 
	uint r0, r1; 
		r0 = rank0(((bitSequenceRRR *)tree->bitmap), start);
	if ((rank0(((bitSequenceRRR *)tree->bitmap), s + start) - r0) >= j){ 
		s = rank0(((bitSequenceRRR *)tree->bitmap), s + start ) - r0  ;
		j = recursiveDirectApplication(tree, getLeftChild(tree, posInTree), s, j);
		return select0(((bitSequenceRRR *)tree->bitmap), r0 +j) - start ; 
	}else{
		r1 = rank1(((bitSequenceRRR *)tree->bitmap), start); 
		j = j - (rank0(((bitSequenceRRR *)tree->bitmap), s + start) -r0) ; 
		s = rank1(((bitSequenceRRR *)tree->bitmap), start + s ) -r1 ; 
		j = recursiveDirectApplication(tree, getRightChild(tree, posInTree) , s, j);
		return select1(((bitSequenceRRR *)tree->bitmap), r1 + j) - start; 
	}
	
}

uint directApplicationHTWT(htwtTree *tree, uint i){

	return recursiveDirectApplication(tree, 1, tree->n, i+1)-1; 
}


uint recursiveRankHTWT(htwtTree *htwt, uint posInTree, uint pos,  uint s, uint end, uint intinf){

	uint start, r0, n0,n01, r1; 

	if (posInTree == htwt->nleafs){ 
		return  pos; 
	}
	start = get_field_basic(htwt->starts, htwt->minBitsStarts, posInTree); 
	r0 = rank0(((bitSequenceRRR *)htwt->bitmap), start); 
	r1 = rank1(((bitSequenceRRR *)htwt->bitmap), start);
	n0 = rank0(((bitSequenceRRR *)htwt->bitmap), start + (end - s)) - r0; 
	if (intinf >= (s + n0)){
		s += n0; 
		pos = rank1(((bitSequenceRRR *)htwt->bitmap), start + pos ) - r1;
		//~ if (pos == 0) { return 0;}
		return recursiveRankHTWT(htwt, getRightChild(htwt, posInTree), pos, s, end, intinf); 
		
	}else{
		end = s+n0;
		pos = rank0(((bitSequenceRRR *)htwt->bitmap), start + pos ) -r0;
		//~ if (pos == 0)  { return 0;}
		return recursiveRankHTWT(htwt, getLeftChild(htwt, posInTree),  pos, s, end, intinf); 
	}
	
}

int rankHTWT(htwtTree *htwt, uint pos, uint limInf){
	
	return recursiveRankHTWT(htwt, 1,  pos, 0, htwt->n, limInf); 	
}

void saveHTWT(htwtTree *htwt, FILE *f){
	
	//~ saveBitSequenceRRR(((bitSequenceRRR *)htwt->bitmap), f); 
	
	fwrite(&(htwt->n), sizeof(uint),1, f); 
	fwrite(&(htwt->nleafs), sizeof(uint),1, f);
	fwrite(&(htwt->vocSize), sizeof(uint),1, f); 
	fwrite(&(htwt->numberOfBitsTree), sizeof(uint),1, f); 
	fwrite(&(htwt->minBits), sizeof(uint),1, f); //no es estrictamente necesario
	fwrite(&(htwt->minBitsStarts), sizeof(uint),1, f); //no es estrictamente necesario
	fwrite(htwt->children, sizeof(uint) * uint_len_F(htwt->minBits, 2*(htwt->nleafs)+1) , 1, f); 
	fwrite(htwt->starts, sizeof(uint) * uint_len_F(htwt->minBitsStarts , htwt->nleafs) , 1, f); 

}

void loadHTWT(htwtTree **htwt, FILE *f){
	
	(*htwt) = (htwtTree *) malloc(sizeof(struct shtwtTree)); 
	
	//~ (*htwt)->bitmap = (bitSequenceRRR *) malloc(sizeof(struct sbitSequenceRRR));
	//~ loadBitSequenceRRR(((bitSequenceRRR *)(*htwt)->bitmap),  f); 
	
	fread(&((*htwt)->n), sizeof(uint), 1, f);
	fread(&((*htwt)->nleafs), sizeof(uint), 1, f); 
	fread(&((*htwt)->vocSize), sizeof(uint), 1, f); 
	fread(&((*htwt)->numberOfBitsTree), sizeof(uint), 1, f); 
	fread(&((*htwt)->minBits), sizeof(uint), 1, f); 
	fread(&((*htwt)->minBitsStarts), sizeof(uint), 1, f); 
	(*htwt)->children = (uint *) malloc(sizeof(uint) * uint_len_F((*htwt)->minBits, 2*((*htwt)->nleafs)+1)); 
	fread((*htwt)->children, sizeof(uint) * uint_len_F((*htwt)->minBits, 2*((*htwt)->nleafs)+1), 1, f); 
	
	(*htwt)->starts = (uint *) malloc(sizeof(uint) * uint_len_F((*htwt)->minBitsStarts , (*htwt)->nleafs)); 
	fread((*htwt)->starts, sizeof(uint) * uint_len_F((*htwt)->minBitsStarts , (*htwt)->nleafs) , 1, f); 
	
}


uint sizeHTWT(htwtTree *htwt){

	if (htwt == NULL) return 0; 
	if (((bitSequenceRRR *)htwt->bitmap) == NULL){
		fprintf(stderr, "Bitmap is not already build\n"); 
		return 0;
	}
	uint treeStructure = sizeof(uint) * uint_len_F(htwt->minBits, 2*(htwt->nleafs)+1) ; 
	uint bitmapStarts = sizeof(uint) * uint_len_F(htwt->minBitsStarts , htwt->nleafs); 
	
	return 		sizeof(struct shtwtTree) 
				+ getSize(((bitSequenceRRR *)htwt->bitmap))
				+ treeStructure
				+ bitmapStarts; 

}

void destroyHTWT(htwtTree **ht){
	
	fprintf(stderr,"Destroying HTWT\n"); 
	if ((*ht) == NULL) return; 
	
	delete_E((bitSequenceRRR *)(*ht)->bitmap);
	destroyBitSequenceRRR((bitSequenceRRR *)(*ht)->bitmap); 
	free((*ht)->bitmap); 
	free((*ht)->children); 
	free((*ht)->starts);
		
	free((*ht)); 
	*ht = NULL; 
}

void printHTWTInfo(htwtTree *htwt){
	fprintf(stderr,"Printing HTWT info....\n"); 
	if (htwt == NULL){
		fprintf(stderr,"Empty tree\n"); 
		return; 
	}
	uint treeStructure = sizeof(uint) * uint_len_F(htwt->minBits, 2*(htwt->nleafs)+1) ; 
	uint bitmapStarts = sizeof(uint) * uint_len_F(htwt->minBitsStarts , htwt->nleafs); 
	
	fprintf(stderr,"Number of bits of bitmap (plain): %u\n", htwt->numberOfBitsTree); 
	fprintf(stderr, "Number of elements of psi: %u\nMin number of bits per element of tree estructure: %u\n", htwt->n, htwt->minBits); 
	fprintf(stderr, "Min number of bpe to indicate the bitmap starts: %u\n", htwt->minBitsStarts); 
	fprintf(stderr,"Size of tree structure: %u\nSize of bitmapStarts %u\n", treeStructure, bitmapStarts); 
	if (((bitSequenceRRR *)htwt->bitmap)!=NULL)
	fprintf(stderr,"Size of bitmap: %u\n", getSize(((bitSequenceRRR *)htwt->bitmap))); 
	fprintf(stderr, "Total size: %u\n", sizeHTWT(htwt)); 
	
}


uint my_log2HTWT(uint value){
	
	int i = 0; 
	for (i = 31; i >=0 ; i--){
		if ((value >> i) != 0) 
			return i+1; 
	}
	return 0; 
}

//Calculates the zero-order entropy of runs
void calculateH0Runs(uint textSize, uint nruns, uint *runs){

	uint i, max = 0; 
	uint *bmp; 
	uint nnumbers, *numbers; 

	nnumbers = nruns; 
	numbers = runs;
	
	//calculates the maximum value of the vector (the length of the largest run)
	for (i = 0; i< nnumbers; i++){
		if (numbers[i]>max){
			max = numbers[i]; 
		}
	}
	
	//builds a bitmap os size max
	//sets to one the positions that containsa existing run length
	bmp = (uint *) calloc(max, sizeof(uint)); 
	for (i = 0; i < nnumbers; i++){
		bitset(bmp, numbers[i]); 
	}
	
	bitSequenceRRR *bs = (bitSequenceRRR *) malloc(sizeof(struct sbitSequenceRRR));
	bs = createBitSequenceRRR(bmp ,(size_t)max+1 , 16);
	//nacum is the number of distinct run lengths
	uint nacum = (double) rank1(bs, max); 
	fprintf(stderr,"Distinct run lengths: %u\n", nacum); 
	//~ for (i = 0; i <= max; i++){
		//~ fprintf(stderr,"bitmap[%u]=%u\n", i, access1(bs, i)); 
		//~ fprintf(stderr,"rank1[%u]=%u\n", i, rank1(bs, i)); 
		//~ 
	//~ }
	
	//~ fprintf(stderr,"nacum: %u\n", nacum); 
	uint *acumuladores = (uint *) calloc(nacum, sizeof(uint)); 

	//counts the number of runs of each length
	for (i = 0; i < nnumbers ; i++){
		acumuladores[rank1(bs, numbers[i])-1]+= numbers[i];
		//~ fprintf(stderr,"numbers: %u, acumuladores[%u]=%u\n", numbers[i], rank1(bs, numbers[i])-1, acumuladores[rank1(bs, numbers[i])-1]); 
	}
	 
	//H0 = sum pi*log(1/pi)
	double H0 = 0, tmp, total = (double) textSize;
	//~ fprintf(stderr, "total: %u\n", textSize);
	uint aux = 0; 

	for (i = 0; i < nacum; i++){
		//~ aux+=acumuladores[i]; 
		tmp = (double) acumuladores[i]; 
		H0+= (tmp / total) * log2(total / tmp); 
	} 
	//~ fprintf(stderr,"aux: %u\n", aux); 
	fprintf(stderr,"H0 = %f\n", H0); 
	free(acumuladores); 
	free(bmp);
	destroyBitSequenceRRR(bs); 	
	free(bs);
}

void generateFileHuffman(uint weightsSize, uint psiSize, uint *weights){
	
	//~ uint *psi = (uint *) malloc(sizeof(uint)*psiSize); 
//~ 
	//~ uint ppsi = 0, j, i; 
	//~ for (i = 0; i< weightsSize; i++){
		//~ for (j = 0; j< weights[i]; j++){
			//~ psi[ppsi++] = weights[i]; 
		//~ }
	//~ }
//~ 
	//~ FILE *f; 
	//~ f = fopen("psi2huffman","wb"); 
	//~ fwrite(psi, sizeof(uint)*psiSize, 1, f); 
	//~ fclose(f); 
	//~ 
	//~ free(psi); 
	
	
}
