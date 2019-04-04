
#include <huTuckerPerm.h>

namespace cds_static
{
	
	huTuckerPerm::huTuckerPerm(int *weights, uint weightsSize, uint *seq, uint sizeSeq, uint sampling_rate, BitSequenceBuilder * bmb){
		
		htwtTree *perm;
		
		createHTWT(&perm, weights, weightsSize, seq, sizeSeq, sampling_rate); 

		n = perm->n;
		
		nleafs = perm->nleafs;
		minBits = perm->minBits;
		minBitsStarts = perm->minBitsStarts;
		vocSize = perm->vocSize;
		numberOfBitsTree = perm->numberOfBitsTree;
		 
		starts =  new uint[ uint_len(minBitsStarts , nleafs+1)]; 
		for (uint i = 0; i < uint_len(minBitsStarts , nleafs+1); i++){
			starts[i]= perm->starts[i];
		}
		children = new uint[uint_len_F(minBits, 2*nleafs+1) ];
		for (uint i=0;i<uint_len_F(minBits, 2*nleafs+1);i++){
			children[i] = perm->children[i];
		}
		
		bitmap = bmb->build(perm->bitmapPlano, numberOfBitsTree);

		free(perm);
	}
	
	huTuckerPerm::huTuckerPerm(){}
	
	huTuckerPerm::~huTuckerPerm() {

	}


	void huTuckerPerm::printTreeStructure(uint posInTree, uint prevPosInTree, uint level){
		if (posInTree >=nleafs){ 
			return; 
		}
		uint lc, rc; 
		lc = getLeftChild(posInTree,prevPosInTree); 
		rc = getRightChild(posInTree, prevPosInTree); 
		
		if (lc < nleafs && rc < nleafs) cout << "Level: " << level << ", L R" << endl;  
		if (lc < nleafs && rc >= nleafs) cout << "Level: " << level << ", L, (R: " << rc-nleafs << ")" << endl;  
		if (lc >= nleafs && rc < nleafs) cout << "Level: " << level << ",(L: " << lc-nleafs << ") R" << endl;  
		if (lc >= nleafs && rc >= nleafs) cout << "Level: " << level << ", (L: " << lc-nleafs << " R: " << rc-nleafs << " )"<< endl;  
		
		printTreeStructure(lc,posInTree,(level+1)); 
		printTreeStructure(rc,posInTree,(level+1)); 
	}
	
	void huTuckerPerm::destroy(){
		fprintf(stderr,"Destroying huTuckerPerm\n"); 
		
		delete bitmap;
		delete [] children;
		delete [] starts;

	}

	uint huTuckerPerm::directApplication( uint i){ 
		return recursiveDirectApplication(1, 0, n, i+1)-1; 
	}

	uint huTuckerPerm::inverseApplication(uint i, uint &leaf){
		return recursiveInverseApplication(1,0, i+1, 0, n, leaf); 
	}

	int huTuckerPerm::rank(uint pos, uint limInf, uint &leaf){
		return recursiveRankHTWT(1,  0, pos, 0, n, limInf,leaf); 	
	} 

	void huTuckerPerm::save( ofstream & f){
		uint wr = HUTUCKERPERM;
		//~ wr = f->savefwrite(&wr,sizeof(uint),1,f);
		saveValue(f,wr); 
		
		bitmap->save(f);//saveBitSequenceRRR(((bitSequenceRRR *)htwt->bitmap), f); 
		saveValue(f,n);
		//~ fwrite(&n, sizeof(uint),1, f); 
		saveValue(f,nleafs);
		//~ fwrite(&nleafs, sizeof(uint),1, f);
		saveValue(f,vocSize);
		//~ fwrite(&vocSize, sizeof(uint),1, f); 
		saveValue(f,numberOfBitsTree);
		//~ fwrite(&numberOfBitsTree, sizeof(uint),1, f); 
		saveValue(f,minBits);
		//~ fwrite(&minBits, sizeof(uint),1, f); //no es estrictamente necesario
		saveValue(f,minBitsStarts);
		//~ fwrite(&minBitsStarts, sizeof(uint),1, f); //no es estrictamente necesario
		saveValue(f,children, uint_len(minBits, 2*(nleafs+1)));
		//~ fwrite(children, sizeof(uint) * uint_len(minBits, 2*(nleafs+1)) , 1, f); 
		saveValue(f, starts, uint_len(minBitsStarts , nleafs+1)); 
		//~ fwrite(starts, sizeof(uint) * uint_len(minBitsStarts , nleafs+1) , 1, f); 
	} 

	uint huTuckerPerm::size(){
		
		if (bitmap == NULL){
			fprintf(stderr, "Bitmap is not already build\n"); 
			return 0;
		}
		uint treeStructure = sizeof(uint) * uint_len(minBits, 2*(nleafs)+1) ; 
		uint bitmapStarts = sizeof(uint) * uint_len(minBitsStarts , nleafs+1); 
		
		return 		
					 //~ bitmap->size()
					//~ + 
					treeStructure
					+ bitmapStarts; 
	} 

	huTuckerPerm* huTuckerPerm::load( ifstream  &f){
		uint rd;
		rd = loadValue<uint>(f); 
		//~ fread(&rd,sizeof(uint),1,f);
		if(rd!=HUTUCKERPERM) return NULL;
		
		huTuckerPerm *perm = new huTuckerPerm();
		perm->bitmap = BitSequence::load(f);
		if (perm->bitmap == NULL){
			cout << "Error loading bitmap" << endl;
		}
		 perm->n = loadValue<uint>(f); 
		//~ fread(&(perm->n), sizeof(uint), 1, f);
		 perm->nleafs= loadValue<uint>(f); 
		//~ fread(&(perm->nleafs), sizeof(uint), 1, f); 
		 perm->vocSize= loadValue<uint>(f); 
		//~ fread(&(perm->vocSize), sizeof(uint), 1, f); 
		perm->numberOfBitsTree = loadValue<uint>(f); 
		//~ fread(&(perm->numberOfBitsTree), sizeof(uint), 1, f); 
		
		 perm->minBits= loadValue<uint>(f); 
		//~ fread(&(perm->minBits), sizeof(uint), 1, f); 
		perm->minBitsStarts = loadValue<uint>(f); 
		//~ fread(&(perm->minBitsStarts), sizeof(uint), 1, f); 
		 perm->children = loadValue<uint>(f,uint_len(perm->minBits, 2*(perm->nleafs+1))); 
		//~ perm->children = new uint[uint_len(perm->minBits, 2*(perm->nleafs+1))]; 
		 perm->children= loadValue<uint>(f, uint_len(perm->minBits, 2*(perm->nleafs+1))); 
		//~ fread(perm->children, sizeof(uint) * uint_len(perm->minBits, 2*(perm->nleafs+1)), 1, f); 
		
		//~ perm->starts = new uint[uint_len(perm->minBitsStarts , perm->nleafs+1)]; 
		perm->starts = loadValue<uint>(f,uint_len(perm->minBitsStarts , perm->nleafs)); 
		//~ fread(perm->starts, sizeof(uint) * uint_len(perm->minBitsStarts , perm->nleafs) , 1, f); 
		
		return perm;
	} 

	void huTuckerPerm::printInfo(){
		cout << "Printing HTWT info....\n" << endl;

		uint treeStructure = sizeof(uint) * uint_len(minBits, 2*nleafs+1) ; 
		uint bitmapStarts = sizeof(uint) * uint_len(minBitsStarts , nleafs+1); 
		
		fprintf(stderr,"Number of bits of bitmap (plain): %u\n", numberOfBitsTree); 
		fprintf(stderr, "Number of elements of psi: %u\nMin number of bits per element of tree estructure: %u\n", n, minBits); 
		fprintf(stderr, "Min number of bpe to indicate the bitmap starts: %u\n",minBitsStarts); 
		fprintf(stderr,"Size of tree structure: %u\nSize of bitmapStarts %u\n", treeStructure, bitmapStarts); 
		fprintf(stderr, "Total size: %u\n", size()); 
	}
	
	
	uint huTuckerPerm::recursiveInverseApplication(uint posInTree, uint prevPosInTree, uint i, uint p, uint s, uint &leaf){
		
		//~ if (posInTree < prevPosInTree){ 
		if (posInTree >=nleafs){ 

			leaf=posInTree-nleafs;
			return p+i-1; 
		}
		
		uint start = get_field_basic(starts, minBitsStarts, posInTree); 
		
		uint r0, r1; 
		if (bitmap->access(start + i) == 0){
				r0 = bitmap->rank0(start); 
			i = bitmap->rank0(i + start ) - r0 ; 
			s = bitmap->rank0(s + start ) - r0  ; 
			return recursiveInverseApplication(getLeftChild(posInTree,prevPosInTree), posInTree,  i, p, s, leaf); 
		}else{
				r0 = bitmap->rank0( start); 
				r1 = bitmap->rank1( start); 
			i =   bitmap->rank1(i + start ) - r1 ; 
			p += (bitmap->rank0( s + start ) - r0 ); 
			s =   bitmap->rank1( s + start) - r1 ; 
			return recursiveInverseApplication(getRightChild( posInTree,prevPosInTree), posInTree,  i, p, s, leaf); 
		}
	}
	
	uint huTuckerPerm::recursiveDirectApplication(uint posInTree, uint prevPosInTree, uint s, uint j){
		
		//uint left, right; 
		//~ if (posInTree < prevPosInTree){ 
		if (posInTree >=nleafs){ 
			
			//~ cout << "j: " << j << endl; 
			return j; 
		}
		
		//left = getLeftChild(posInTree,prevPosInTree); 
		//right = getRightChild(posInTree,prevPosInTree); 
		uint start = get_field_basic(starts, minBitsStarts, posInTree); 
		uint r0, r1; 
			r0 = bitmap->rank0(start);
		if ((bitmap->rank0(s + start) - r0) >= j){ 
			s = bitmap->rank0(s + start ) - r0  ;
			j = recursiveDirectApplication(getLeftChild(posInTree,prevPosInTree), posInTree, s, j);
			return bitmap->select0(r0 +j) - start ; 
		}else{
			//Optimization
			//r1 = start - r0 +1; 
			//uint tmp= bitmap->rank0(s + start); j = j - tmp -r0) ; 
			// s = starts+s-tmp+1-r1; 
			r1 = bitmap->rank1(start); 
			j = j - (bitmap->rank0(s + start) -r0) ; 
			s = bitmap->rank1(start + s ) -r1 ; 
			j = recursiveDirectApplication(getRightChild(posInTree,prevPosInTree), posInTree, s, j);
			return bitmap->select1(r1 + j) - start; 
		}
		
	}

	uint huTuckerPerm::recursiveRankHTWT(uint posInTree, uint prevPosInTree, uint pos,  uint s, uint end, uint intinf, uint &leaf){

		uint start, r0, n0, r1; 

		if (posInTree >= nleafs){ 
			leaf=posInTree-nleafs;
			return  pos; 
		}
		start = get_field_basic(starts, minBitsStarts, posInTree); 
		r0 = bitmap->rank0(start); 
		r1 = bitmap->rank1(start);
		n0 = bitmap->rank0(start + (end - s)) - r0; 
		if (intinf >= (s + n0)){
			s += n0; 
			pos = bitmap->rank1(start + pos ) - r1;
			return recursiveRankHTWT(getRightChild(posInTree,prevPosInTree), posInTree, pos, s, end, intinf,leaf); 
			
		}else{
			end = s+n0;
			pos = bitmap->rank0(start + pos ) -r0;
			return recursiveRankHTWT(getLeftChild(posInTree,prevPosInTree), posInTree, pos, s, end, intinf,leaf); 
		}
		
	}

	uint huTuckerPerm::isLeaf(uint posInTree){
		return (posInTree >= nleafs);
	}

	uint huTuckerPerm::getLeftChild( uint posInTree, uint prevPosInTree){
		//~ if (posInTree < prevPosInTree) return 0;
		if (posInTree >=nleafs) return 0;
		return get_field_basic(children, minBits,posInTree*2);
	}

	uint huTuckerPerm::getRightChild( uint posInTree, uint prevPosInTree){
		//~ if (posInTree < prevPosInTree) return 0;
		if (posInTree >=nleafs) return 0;
		return get_field_basic(children, minBits,posInTree*2+1);
	}
	
	uint huTuckerPerm::isLeafBnode(bNode *node){
		
		if ((node==NULL) || ((node->children[0] == NULL) && (node->children[1]==NULL)))
			return 1; 
		return 0; 
	}
	int huTuckerPerm::createHTWT(htwtTree **tree, int *weights, uint weightsSize, uint *psi, uint n, uint samplingRate){
		uint i; 

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
	//uint totalNodes = 2*nleafs-1; //binary tree
	uint minBits = my_log2HTWT(2*nleafs+1)+1;
	uint minBitsStarts;
	
	//no se usan las posiciones 0 de los vectores
	(*tree)->children = (uint *) malloc(sizeof(uint) * uint_len(minBits, 2*nleafs+1) );
	//~ (*tree)->starts = (uint *) malloc(sizeof(uint) * uint_len_F(minBitsStarts, nleafs)  );
	(*tree)->starts = (uint *) malloc(sizeof(uint) * nleafs  );

	/*************************************************************/
	uint firstGap = 2;//indicates the firs gap in the list of tree nodes 
	crearHT((*tree), ht->root, bitmapHTWT, &pbHTWT, nleafs, 1, &firstGap, minBits); 
	destroyHuTucker(ht);
	
	/************** TRANSFORME VECTOR OF UINT INTO VECTOR OF LOG2(BITMAP_LENGTH) **/
	
		minBitsStarts = my_log2HTWT(pbHTWT); 
		uint *starts =  (uint *) malloc(sizeof(uint) * uint_len(minBitsStarts , nleafs)  ); 
		for (i = 1; i < nleafs; i++){
			set_field(starts, minBitsStarts, i, (*tree)->starts[i]);
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

	void huTuckerPerm::crearHT(htwtTree *tree, bNode *node, uint *bitmap, uint *pb, uint nleafs, uint i, 
					uint *libre, uint minBits){

		if (node == NULL) return ; 

		if (isLeafBnode(node->children[0])){
			set_field(tree->children, minBits, i*2, nleafs+node->children[0]->pos);//+node->children[0]->pos); //[i*2] = nleafs; // despois faráse con setValue...
		}else{
			//~ fprintf(stderr, "left\n"); 
			set_field(tree->children, minBits, i*2, (*libre));  
			(*libre)++;  
			crearHT(tree, node->children[0] , bitmap, pb , nleafs, (*libre)-1, libre, minBits);
		}
		if (isLeafBnode(node->children[1])){
			set_field(tree->children, minBits, i*2+1, nleafs+node->children[1]->pos);//+node->pos);
		}else{
	//~ fprintf(stderr, "right\n");
			set_field(tree->children, minBits, i*2+1, (*libre));  
			(*libre)++;
			crearHT(tree, node->children[1] , bitmap, pb , nleafs, (*libre)-1, libre, minBits);
		}

		//i comienza en 0
		tree->starts[i] = (*pb)-1; 
		preMergePsiHTWT(node, node->children[0], node->children[1], bitmap, pb); 
		freebNode(node); 

	}


	void huTuckerPerm::preMergePsiHTWT(bNode *f, bNode *l, bNode *r, uint *bitmap, uint *pb){
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
		//uint *bmp = NULL; 
		uint w; 
		//~ fprintf(stderr, "SizePsi: %u\n", sizePsiR+sizePsiL); 
		
		mergePsiHTWT(psiL, sizePsiL, psiR, sizePsiR, &(f->psi), &w, bitmap, *pb); 
		
		if (w != 0){
			f->sizePsi = w; 
			(*pb)+= f->sizePsi; 
		}

	}

	void huTuckerPerm::mergePsiHTWT(uint *psiL, uint sizePsiL, uint *psiR, uint sizePsiR, uint **psiN, uint *sizePsiN, uint *bitmap, uint pb){
		
		if (sizePsiL == 0 && sizePsiR == 0){
			psiN = NULL; 
			(*sizePsiN) = 0; 
			return; 
		}

		//long int i; 
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

	uint huTuckerPerm::isLeaf(htwtTree *tree, uint posInTree){
		return (posInTree == tree->nleafs);
	}

	uint huTuckerPerm::getLeftChild(htwtTree *tree, uint posInTree){
		if (posInTree >= tree->nleafs) return 0; 
		return get_field_basic(tree->children, tree->minBits,posInTree*2);
	}

	uint huTuckerPerm::getRightChild(htwtTree *tree, uint posInTree){
		if (posInTree >= tree->nleafs) return 0; 
		return get_field_basic(tree->children, tree->minBits,posInTree*2+1);
	}

	uint huTuckerPerm::my_log2HTWT(uint value){
		
		int i = 0; 
		for (i = 31; i >=0 ; i--){
			if ((value >> i) != 0) 
				return i+1; 
		}
		return 0; 
	}
};
