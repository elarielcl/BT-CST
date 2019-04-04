
#include <hutucker.h>



huTucker *createHuTucker(int* array, int szArray);

void freeHutucker(huTucker **ht);

bNode* merge(bNode* l, bNode* r);

int getPosMin(huTucker ht);

void removePos(huTucker *ht, int pos);

int findPosCompatible(huTucker ht, int pmin);

void combination(huTucker *ht);

//~ void combinationO3(huTucker *ht);

void levelAssignment(huTucker *ht);

void recLevelAssign( huTucker *ht, bNode* curr);

void recombination(huTucker *ht);



uint getCodes(huTucker ht, Tcode** codes, BitStringHT **tree);

void printLevels(huTucker ht);

huTucker *createHuTuckerPsi(int* array, int szArray, uint *psi, uint *psiIntervals){
	
	huTucker * ht = (huTucker *) malloc(sizeof(huTucker)); 
	int i, point, pPsi = 0; 
	
    ht->start=0;
    ht->root=0;
    ht->weight=0;
    ht->end=szArray-1;
    ht->len=szArray;
    ht->codes = NULL; 
    ht->seq = (bNode **) malloc(sizeof(bNode*) * szArray); 
    ht->levels = (int *) malloc(sizeof(int) * szArray); 
    
    for(i=ht->start, point=ht->start; i<=ht->end; i++){
		//array[i]: frequencia del símbolo i
		//i: posición del símbolo
		//0: tipo del nodo (external)
		//&(array[i]) pos de memoria del símbolo i (al estar ordenados
		//es como si fuese su valor alfanumérico. 
		//le pasamos el vector psi para que lo copie en el nodo hoja
		//va a copiar array[i] elementos.
        ht->seq[i] = createBnodePsi(array[i],i,&(array[i]), (uint *)(psi+pPsi), psiIntervals[i]);
        pPsi+=psiIntervals[i]; 
        //point: peso del símbolo anterior (de su adyacente). 
        //point+... peso del símbolo actual
        setEndpoints(ht->seq[i], point,point+(array[i])-1);
        point=point+(array[i]);
    }
	//El algoritmo tiene 3 fases: 
	//	1.- Combinación de pares T-C. 
	//	2.- Asignación de niveles.
	//	3.- Recombinación de elementos.
	//fprintf(stderr,"Hu-Tucker combination\n"); 
    combination(ht);
	//fprintf(stderr,"Hu-Tucker levelAssignment\n");
    levelAssignment(ht);
	//fprintf(stderr,"Hu-Tucker recombination\n"); 
    recombination(ht);
    //~ printHuTucker(*ht); 
    return ht; 
	
}

uint numberOfBitsHutucker(huTucker ht){
	   
	if(ht.root!=0){
		return numberOfBitsRec(*(ht.root), 0); 
    }
     
	return 0; 
}
huTucker *createHuTucker(int* array, int szArray){
	
	huTucker * ht = (huTucker *) malloc(sizeof(huTucker)); 
	int i, point; 
	
    ht->start=0;
    ht->root=0;
    ht->weight=0;
    ht->end=szArray-1;
    ht->len=szArray;
    
    ht->seq = (bNode **) malloc(sizeof(bNode*) * szArray); 
    ht->levels = (int *) malloc(sizeof(int) * szArray); 
    
    for(i=ht->start, point=ht->start; i<=ht->end; i++){
		//array[i]: frequencia del símbolo i
		//i: posición del símbolo
		//0: tipo del nodo (external)
		//&(array[i]) pos de memoria del símbolo i (al estar ordenados
		//es como si fuese su valor alfanumérico. 
        ht->seq[i] = createBnode(array[i],i,0,&(array[i]));
        //point: peso del símbolo anterior (de su adyacente). 
        //point+... peso del símbolo actual
        setEndpoints(ht->seq[i], point,point+(array[i])-1);
        point=point+(array[i]);
    }
	//El algoritmo tiene 3 fases: 
	//	1.- Combinación de pares T-C. 
	//	2.- Asignación de niveles.
	//	3.- Recombinación de elementos.
    combination(ht);
    levelAssignment(ht);
    recombination(ht);
    return ht; 
}

void freeHutucker(huTucker **ht){

	int i; 
	for (i = 0 ; i < (*ht)->len; i++){
		free((*ht)->seq[i]); 
	}
	free((*ht)->seq); 
	free((*ht)->levels); 
	free(*ht); 
	
}
//~ template <class T>
//~ HuTucker<T>::~HuTucker(){
    //~ for(int i=start; i<=end; i++)
        //~ delete seq[i];
    //~ delete[]seq;
    //~ delete[]levels;
//~ }


bNode* merge(bNode* l, bNode* r){
	int value = 0; 
	//&value...posible diferencia
    bNode* n = createBnode(l->w+r->w,l->pos,1, &value);
    n->children[0] = l;
    n->children[1] = r;
    setEndpoints(n, l->endpoint[0],r->endpoint[1]);
    return n;
}

int getPosMin(huTucker ht){
    int min=ht.start;
    int i; 
    //sequential search of the minimum element
    for(i=min+1; i<=ht.end; i++)
        if(ht.seq[min]->w>ht.seq[i]->w)
            min=i ;
    return min;
}


void removePos(huTucker *ht, int pos){
	int i; 
    
    ht->seq[pos]=0; 
    //sequential update of pointers
    for(i=pos; i<ht->end; i++)
		ht->seq[i]=ht->seq[i+1];
    ht->seq[ht->end]=0;
    ht->end--;
}




int findPosCompatibleO2(huTucker ht, int *positionMin, int *positionCom){
    int pcomTmp, weightTmp ;

    int pmin, pcom, weight, i; 
    weight = 0x7fffffff; 

    for (i = ht.start; i < ht.end; i++){
		pcomTmp = findPosCompatible(ht, i); 
		weightTmp = ht.seq[i]->w + ht.seq[pcomTmp]->w; 
	
		if (weightTmp < weight){
			pmin = i; 
			pcom = pcomTmp; 
			weight = weightTmp; 
		}
	}
    (*positionMin) = pmin; 
    (*positionCom) = pcom; 
    return 0;
} 

//~ void combinationO3(huTucker *ht){
	//~ int pmin, pcom, lastEnd=ht->end;
   //~ 
    //~ while(ht->start!=ht->end){
		//~ //obtiene la posición del elmento con menor peso
        //~ pmin = getPosMin(*ht);
        //~ //busca hacia izq. y dcha. el menor peso compatible con la posición
        //~ //actual.
        //~ findPosCompatibleO2(*ht, &pmin, &pcom);
      
		//~ 
		//~ //crea un nuevo nodo interno a partir de los dos situados en pmin y pcom.
        //~ ht->seq[pmin]=merge(ht->seq[pmin],ht->seq[pcom]);
        //~ //elmina pcom de la lista de noso O(n)
        //~ removePos(ht, pcom);
    //~ }
    //~ ht->end=lastEnd;
    //~ ht->root=ht->seq[pmin];
		//~ 
//~ }

int findPosCompatible(huTucker ht, int pmin){
    int pcom, minleft, minright, left, right;
    minleft= pmin; 
    minright=pmin;
    //find towards left end of pmin's huffman seq
    if(pmin!= ht.start){
        minleft=pmin-1;
        for(left=pmin-2; left>=ht.start && ht.seq[left+1]->type!=0; left--){
            if(ht.seq[minleft]->w >= ht.seq[left]->w) minleft=left;
        }
    }
    //find towards right end of pmin's huffman seq
    if(pmin!=ht.end){
        minright=pmin+1;
        for(right=pmin+2; right<=ht.end && ht.seq[right-1]->type!=0; right++){
            if(ht.seq[minright]->w > ht.seq[right]->w) minright=right;
        }
    }
    //min{minleft,minright}
    if(minleft==pmin) pcom=minright;
    else{
        if(minright==pmin) pcom=minleft;
        else pcom=(ht.seq[minleft]->w<=ht.seq[minright]->w) ? minleft : minright;
    }
    return pcom;
}

void combination(huTucker *ht){
    int pmin=0, pcom, temp, lastEnd=ht->end;
   
    while(ht->start!=ht->end){
		//obtiene la posición del elmento con menor peso
        pmin = getPosMin(*ht);
        //busca hacia izq. y dcha. el menor peso compatible con la posición
        //actual.
        pcom = findPosCompatible(*ht, pmin);
        if(pmin > pcom){ temp=pmin; pmin=pcom; pcom=temp; }
		
		//crea un nuevo nodo interno a partir de los dos situados en pmin y pcom.
        ht->seq[pmin]=merge(ht->seq[pmin],ht->seq[pcom]);
        //elmina pcom de la lista de noso O(n)
        removePos(ht, pcom);
    }
    ht->end=lastEnd;
    ht->root=ht->seq[pmin];
}


void levelAssignment(huTucker *ht){
    recLevelAssign(ht, ht->root);
    
}


void recLevelAssign( huTucker *ht, bNode* curr){
    static int lvl=0;
    if(curr->type==1){//internal node
		//~ fprintf(stderr,"Internal node: "); print(*curr); 
        ht->weight++; //counter of internal nodes
        lvl++;
        recLevelAssign(ht, curr->children[0]);
        recLevelAssign(ht, curr->children[1]);
        //~ delete curr;
        lvl--;
    }
    else{//leaf node
		//~ fprintf(stderr,"leaf node: "); print(*curr); 
        ht->levels[curr->pos]=lvl;
        ht->seq[curr->pos]=curr;
    }
}


void recombination(huTucker *ht){
    //~ int* stack = new int[len];
    int *stack = (int *) malloc(sizeof(int)*ht->len); 
    int pos=0;
    int cont=0;
    while (cont < ht->len){
		cont++;
	}
	cont = 0;
    //cont nos indica la siguiente posición a comprobar en el vector levels (cont siempre aumenta). 
    //pos indica la posición de la cima de la pila. 
    //cuando asignamos cont++ a stack[pos] es que estamos introduciendo en la cima de la pila 
    //el siguiente valor a analizar del vector levels. 
    stack[pos]=cont++;
    while(cont<ht->len){
        pos++;
        stack[pos]=cont++;
        while(pos>0 && ht->levels[stack[pos]]==ht->levels[stack[pos-1]]){//merge
            pos--;
            
            ht->seq[stack[pos]]=merge(ht->seq[stack[pos]],ht->seq[stack[pos+1]]);
            ht->seq[stack[pos+1]]=0;
            ht->levels[stack[pos]]--;
        }
    }
    ht->root=ht->seq[stack[pos]];
    free(stack); 
}


void printHuTucker(huTucker ht){
    int i;
    fprintf(stderr,"internal nodes: %u, objects: %u, starting at pos: %u, ending at pos: %u\n", ht.weight, ht.len, ht.start, ht.end); 

	for(i=0; i<ht.len; i++)
        if(ht.seq[i]){
			fprintf(stderr,"(%u): ", i); 
			print(*(ht.seq[i])); 
		}
    fprintf(stderr,"\n"); 
    if(ht.root!=0){
		recPrint(*(ht.root), 0); 
    }
    fprintf(stderr,"\n"); 
}


uint getCodes(huTucker ht, Tcode** codes, BitStringHT **tree)
{
	uint *bits = (uint *) malloc(sizeof(int)*32); 
	*codes = (Tcode *) malloc(sizeof(Tcode)*ht.len); 
	 *tree = (BitStringHT *) create_BitStringHT( 2*(ht.weight+ht.len)); 

	if (ht.root != 0)
	{
		int pos = 0;
		int i; 
		for (i=0; i<ht.len; i++) (*codes)[i].cbits = 0;
		code(*(ht.root), 0, codes, tree, bits, &pos);
	}
	
	free(bits); 
	return ht.len;
}


void printLevels(huTucker ht){
    int i;
    fprintf(stderr,"Levels: "); 
    for(i=0; i< ht.len; i++)
		fprintf(stderr,"%u ", ht.levels[i]); 

	fprintf(stderr,"\n"); 
}

void destroyHuTucker(huTucker *ht){
	
	if (ht == NULL) return; 
	freeAllbNode(ht->root);
	if (ht->codes != NULL) free(ht->codes); 
	free(ht); 
	
}





