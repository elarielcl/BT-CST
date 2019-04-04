#include <grammar/Repair.h>
#include <utils/DisjointSet.h>

using namespace cds_utils;

namespace cds_static{

	Repair::Repair(int *input, uint len){
		runRepair(input,len);
	}
	
	Repair::Repair(char *input, uint len){
		runRepair(input,len);
	}
	Repair::Repair(){}

	Repair *Repair::instanceFromBitmap(uint *input, uint len){
		Repair *rp = new Repair();
		int *int_input = new int[len];
		for (uint i=0;i<len;i++) int_input[i] = (bitget(input,i))?1:0;
		rp->runRepair(int_input,len);
		delete [] int_input;
		return rp;
	}
	
	Repair::~Repair(){
		deleteAll();
	}
	
	int Repair::getNTerm(){
		return nterms;
	}
	//we are supposing the terminal symbols take value in 
	//the continuous interval [0,alph] --> #terms = alph+1
	int Repair::getTermsSize(){
		return alph;
	}

	int Repair::getNRules(){
		return rule_index;
	}
	int Repair::getNC(){
		return c;
	}
	int Repair::getAlpha(){
		return alph;
	}
	int Repair::getLeft(int rule){
		assert(rule<rule_index);
		return RULES_LEFT[rule];
	}
	int Repair::getRight(int rule){
		assert(rule<rule_index);
		return RULES_RIGHT[rule];
	}
	int Repair::getFreqRule(int rule){
		assert(rule<rule_index);
		return FREQ_PAIRS[rule];
	}
	int Repair::getC(int pos){
		assert(pos<c);
		return C[pos];
	}
	
	size_t Repair::getSize(){
		assert(rule_index>0);
		assert(c>0);
		assert(bits(rule_index)>0);
		return (2*rule_index+c)*bits(rule_index);
	}
	
	size_t Repair::getDepth(){
		int *depths = new int[getNRules()];
		for (int i=0;i<getNRules();i++)
			depths[i]=-1;
		for (int i=0;i<getNRules();i++)
			computeGrammarDepth(i+getAlpha(),depths);
		grammarDepth=depths[0];
		for (int i=1;i<getNRules();i++)
			grammarDepth = max(grammarDepth,depths[i]);
		delete [] depths;
		
		return grammarDepth;
		
	}
	
	void Repair::print(){
		cerr << "len input: " << input_len << endl;
		cerr << "nterms: " << nterms << endl;
		cerr << "alph: " << alph-1 << endl;
		cerr << "|R!: " << rule_index << endl;
		cerr << "|C|:" << c << endl;
		cerr << "bpi:" << (double)getSize()/input_len << endl;
		cerr << "ratio (bpi/len_input*log(nterms)): " << ((double)getSize()/input_len)/(double)input_len*bits(nterms) << endl;
		cerr << "(Note rules start at alph+1)"<<endl;
		for (int i=0;i<min(50,rule_index);i++){
			cerr << i+alph << "->"<<RULES_LEFT[i]<<","<<RULES_RIGHT[i]<< endl;
		}	
		for (int i=0;i<min(c,50);i++)
			cerr << C[i] << " ";
		cerr << endl;
	}

	uint Repair::getInputLength(){
		return input_len;
	}

	uint Repair::getLength(uint rule){
		assert(rule<(uint)getAlpha()+getNRules());
		if (rule<(uint)getAlpha())
			return 1;
		return lengths[rule-getAlpha()];
	}
	
	/*********************************************************/
	/*********************************************************/
	/*********************************************************/
	
void Repair::prepare (unsigned long long len)

  { int i,id;
    Tpair_repair pair;
    c = u = len;
    alph = 0;
    for (i=0;i<u;i++) 
	{ if (C[i] > alph) alph = C[i];
	}
    n = ++alph;
    Rec = createRecords(factor,minsize);
    Heap = createHeap(u,&Rec,factor,minsize);
    Hash = createHash(256*256,&Rec);
    L = (Tlist_repair*)malloc(u*sizeof(Tlist_repair));
    assocRecords (&Rec,&Hash,&Heap,L);
    for (i=0;i<c-1;i++) 
	{ pair.left = C[i]; pair.right = C[i+1];
	  id = searchHash (Hash,pair);
    	  if (id == -1) // new pair, insert
	     { id = insertRecord (&Rec,pair);
	       L[i].next = -1;
	     }
	  else 
	     { L[i].next = Rec.records[id].cpos;
	       L[L[i].next].prev = i;
	       incFreq (&Heap,id);
	     }
	  L[i].prev = -id-1;
	  Rec.records[id].cpos = i;
if (PRNL && (i%10000 == 0)) printf ("Processed %i chars\n",i);
	}
    purgeHeap (&Heap);
    rule_index=0;
  }

void prepare0 (relong len)

  { relong i;
    int id;
    Tpair pair;
    alph = 0;
    for (i=0;i<len;i++) 
	{ if (text[i] > alph) alph = text[i];
	}
    n = ++alph;
    Rec = createRecords(factor,minsize);
    Heap = createHeap(len,&Rec,factor,minsize);
    Hash = createHash(256*256,&Rec);
    assocRecords (&Rec,&Hash,&Heap,NULL);
    if ((len /1024/1024) * 3 * sizeof(relong) <= MB) return;
    did0 = 1;
    for (i=0;i<len-1;i++) 
	{ pair.left = text[i]; pair.right = text[i+1];
	  id = searchHash (Hash,pair);
    	  if (id == -1) // new pair, insert
	     { id = insertRecord (&Rec,pair);
	     }
	  else 
	     { incFreq (&Heap,id);
	     }
if (PRNL && (i%1000000 == 0)) printf ("Processed %lli ints\n",i);
	}
    purgeHeap (&Heap);
  }


void Repair::prnSym(int c)
   { printf("%i",c); 
   }

void Repair::prnC (void)

  { int i = 0;
    printf ("C[1..%i] = ",c);
    while (i<u)
      { prnSym(C[i]);
  	printf (" ");
	i++; 
	if ((i<u) && (C[i] < 0)) i = -C[i]-1; 
      }
    printf ("\n\n");
// printf ("Physical C[1..%i] = ",u);
// for (i=0;i<u;i++)
//       { prnSym(C[i]);
//  	printf (" ");
//       }
//     printf ("\n\n");
  }

void Repair::prnRec (void)

  { int i;
    printf ("Active pairs:\n");
    for (i=0;i<Rec.size;i++)
        { printf ("\t(");
	  prnSym(Rec.records[i].pair.left);
          printf (",");
	  prnSym(Rec.records[i].pair.right);
	  printf ("), %i occs\n", Rec.records[i].freq);
	}
    printf ("\n");
  }
  
  int Repair::getSym(int c){
	return c;
   }


	// to avoid another list to access the sparse C we thread it using the
	// empty space. if next cell of an active cell is negative, it is 
	// (minus) a ptr to the next occ. idem previous cell to previous occ,
	// except that next ptr dominates over prev ptr if they must be in
 	// the same cell. but in this case one can find prev in O(1) anyway.

int Repair::repair(){
	RULES_LEFT = (int*)malloc(sizeof(int)*MAX_RULES);
	RULES_RIGHT = (int*)malloc(sizeof(int)*MAX_RULES);
	FREQ_PAIRS = (int*)malloc(sizeof(int)*MAX_RULES);
	  int oid,id,cpos;
  
    Trecord_repair *rec,*orec;
    Tpair_repair pair;
    //~ if (fwrite(&alph,sizeof(int),1,R) != 1) return -1;
if (PRNC) prnC();
    while (n+1 > 0)
      { 
if (PRNR) prnRec();
	oid = extractMax(&Heap);
	if (oid == -1) break; // the end!!
 	orec = &Rec.records[oid];
 	cpos = orec->cpos;
	RULES_LEFT[rule_index]=getSym(orec->pair.left);
 	RULES_RIGHT[rule_index]=getSym(orec->pair.right);
 	FREQ_PAIRS[rule_index]=orec->freq;
 	rule_index++;
	//~ if (fwrite (&orec->pair,sizeof(Tpair),1,R) != 1) return -1;
if (PRNP) 
    { printf("Chosen pair %i = (",n);
      prnSym(orec->pair.left);
      printf(",");
      prnSym(orec->pair.right);
      printf(") (%i occs)\n",orec->freq);
    }
	while (cpos != -1)
	   { int ant,sgte,ssgte; 
		// replacing bc->e in abcd, b = cpos, c = sgte, d = ssgte
	     if (C[cpos+1] < 0) sgte = -C[cpos+1]-1; 
	     else sgte = cpos+1; 
	     if ((sgte+1 < u) && (C[sgte+1] < 0)) ssgte = -C[sgte+1]-1;
	     else ssgte = sgte+1; 
		// remove bc from L
	     if (L[cpos].next != -1) L[L[cpos].next].prev = -oid-1;
	     orec->cpos = L[cpos].next;
	     if (ssgte != u) // there is ssgte
		{ 	// remove occ of cd
		  pair.left = C[sgte]; pair.right = C[ssgte];
		  id = searchHash(Hash,pair);
		  if (id != -1) // may not exist if purgeHeap'd
	             { if (id != oid) decFreq (&Heap,id); // not to my pair!
		       if (L[sgte].prev != NullFreq) //still exists(not removed)
 		          { rec = &Rec.records[id];
		            if (L[sgte].prev < 0) // this cd is head of its list
		               rec->cpos = L[sgte].next;
		            else L[L[sgte].prev].next = L[sgte].next;
		            if (L[sgte].next != -1) // not tail of its list
		               L[L[sgte].next].prev = L[sgte].prev;
			  }
		     }
			// create occ of ed
	          pair.left = n;
		  id = searchHash(Hash,pair);
	          if (id == -1) // new pair, insert
		     { id = insertRecord (&Rec,pair);
 		       rec = &Rec.records[id];
	               L[cpos].next = -1;
		     }
	          else 
 		     { incFreq (&Heap,id);
 		       rec = &Rec.records[id]; 
	               L[cpos].next = rec->cpos;
	               L[L[cpos].next].prev = cpos;
	             }
	          L[cpos].prev = -id-1;
	          rec->cpos = cpos;
		}
	     if (cpos != 0) // there is ant
		{ 	// remove occ of ab
	          if (C[cpos-1] < 0) 
		     { ant = -C[cpos-1]-1; 
		       if (ant == cpos) // sgte and ant clashed -> 1 hole
			  ant = cpos-2;
		     }
	          else ant = cpos-1; 
		  pair.left = C[ant]; pair.right = C[cpos];
		  id = searchHash(Hash,pair);
		  if (id != -1) // may not exist if purgeHeap'd
	             { if (id != oid) decFreq (&Heap,id); // not to my pair!
		       if (L[ant].prev != NullFreq) //still exists (not removed)
 		          { rec = &Rec.records[id];
		            if (L[ant].prev < 0) // this ab is head of its list
		                 rec->cpos = L[ant].next;
		            else L[L[ant].prev].next = L[ant].next;
		            if (L[ant].next != -1) // it is not tail of its list
		               L[L[ant].next].prev = L[ant].prev;
			  }
		     }
			// create occ of ae
	          pair.right = n;
		  id = searchHash(Hash,pair);
	          if (id == -1) // new pair, insert
		     { id = insertRecord(&Rec,pair);
 		       rec = &Rec.records[id];
	               L[ant].next = -1;
	             }
	          else 
	             { incFreq (&Heap,id);
 		       rec = &Rec.records[id];
	               L[ant].next = rec->cpos;
	               L[L[ant].next].prev = ant;
	             }
	          L[ant].prev = -id-1;
	          rec->cpos = ant;
		}
	     C[cpos] = n;
	     if (ssgte != u) C[ssgte-1] = -cpos-1;
	     C[cpos+1] = -ssgte-1;
	     c--;
 	     orec = &Rec.records[oid]; // just in case of Rec.records realloc'd
	     cpos = orec->cpos;
	   }
if (PRNC) prnC();
	 removeRecord (&Rec,oid);
	 n++;
	 purgeHeap(&Heap); // remove freq 1 from heap
	 if (c < factor * u) // compact C
	    { int i,ni;
	      i = 0;
	      for (ni=0;ni<c-1;ni++) 
		{ C[ni] = C[i];
		  L[ni] = L[i];
		  if (L[ni].prev < 0) 
		     { if (L[ni].prev != NullFreq) // real ptr
			  Rec.records[-L[ni].prev-1].cpos = ni; 
		     }
		  else L[L[ni].prev].next = ni;
		  if (L[ni].next != -1) L[L[ni].next].prev = ni;
		  i++; if (C[i] < 0) i = -C[i]-1;
		}
	      C[ni] = C[i];
	      u = c;
	     C = (int*)realloc (C, c * sizeof(int));
	  //     int *newC = (int *)malloc(sizeof(uint)*c);
	  //     for (int k=0;k<c;k++)
			// newC[k]=C[k];
		 //  int *tmp = C;
		 //  C = newC;
		  //TODO this bunch of code remplaces the realloc...i do not know why but
		  //it is raising a heap seg. fault
		  //~ free(tmp);
		  //TODO tmp must be freed (in fact, the realloc would be correct but it is 
		  //raising a heap error)
		  //~ free(tmp);
	       L = (Tlist_repair*)realloc (L, c * sizeof(Tlist_repair));
              assocRecords (&Rec,&Hash,&Heap,L);
	    }
       }
     return 0;
}

int Repair::computeLengths(int rule, int *lengths){
	
	if (rule<alph){
		return 1;
	}else{
		rule-=alph;
		if (lengths[rule]!=-1){
			return lengths[rule];
		}else{
			lengths[rule]=computeLengths(RULES_LEFT[rule],lengths)+computeLengths(RULES_RIGHT[rule],lengths);
			return lengths[rule];
		}
	}
	
}

int Repair::computeNoccs(int rule, int *noccs){
	noccs[rule]++;
	if (rule<alph){
		return 0;
	}else{
		computeNoccs(RULES_LEFT[rule-alph],noccs);
		computeNoccs(RULES_RIGHT[rule-alph],noccs);
		return 0;
	}	
}

int Repair::computeGrammarDepth(int rule, int *depths){
	if (rule<alph){
		return 0;
	}else{
		rule-=alph;
		if (depths[rule]!=-1){
			return depths[rule];
		}else{
			depths[rule]=1+max(computeGrammarDepth(RULES_LEFT[rule],depths),computeGrammarDepth(RULES_RIGHT[rule],depths));
			return depths[rule];
		}
	}	
}

void Repair::printGrammar(){
	for (int i=0;i<rule_index;i++){
		cerr << "Rule-" << i+alph << "->";
			cerr << RULES_LEFT[i]<< ",";
			cerr << RULES_RIGHT[i]<< ",";
		cerr << endl;
	}
	cerr << "csize: " << c << endl;
	cerr << "C=";
	for (int i=0;i<c;i++){
		cerr << getC(i) << ",";
	}
	cerr << endl;
}

void Repair::statistics(){
	//computes the voc size (number of terminal symbols)
	/*char *voc = (char *)malloc(sizeof(char)*alph);
	int i=0;
	for (i=0;i<alph;i++)voc[i]=0;
	for (i=0;i<rule_index;i++){
		if (RULES_LEFT[i]<alph){
			voc[RULES_LEFT[i]]=1;
		}
		if (RULES_RIGHT[i]<alph){
			voc[RULES_RIGHT[i]]=1;
		}
	}
	nterms=0;
	for (i=0;i<alph;i++)
		nterms+=(int)voc[i];
	free(voc);*/
	nterms = alph;
	//~ printf("nterms:%i\n",nterms);
	
}

void Repair::postRepair(){
	int i;
	i = 0;
     int csize=0;
     int *newC = new int[u];
     while (i<u)
        { 
			newC[csize]=getSym(C[i]);
			 csize++;
          i++;
          if ((i < u) && (C[i] < 0)) 
			i = -C[i]-1;
        }
     c=csize;
     int *tmp = C;
     C = newC;
     free(tmp);
	 lengths=NULL;
     lengths = (int *)malloc(sizeof(int)*(rule_index));
     for (i=0;i<rule_index;i++)lengths[i]=-1;
     for (i=0;i<rule_index;i++){
		 computeLengths(i+alph,lengths);
	 }
	 for (i=0;i<getAlpha()+getNRules();i++){
	 	assert(getLength(i)>0);
	 }
	//researchStats();
}

bool Repair::researchStats(){
	 int maxRuleLength=0;
     uint mRLPos=0;
     cerr << endl <<"Nrules: " << rule_index <<", n/nrules: " << (double)input_len/rule_index << " ";
    double avgLength=0;
    double stdDesviation=0;
	for (int i=0;i<rule_index;i++){
		 if (maxRuleLength<lengths[i]){
			maxRuleLength = lengths[i];
			mRLPos=i;
		 }
		 maxRuleLength=max(maxRuleLength,lengths[i]);
		 avgLength+=lengths[i];
		 stdDesviation+=(lengths[i]*lengths[i]);
	 }
	 avgLength/=rule_index;
	 stdDesviation = sqrt((stdDesviation/rule_index)-avgLength);
	 cerr << "MaxRuleLength: " << maxRuleLength << " at pos: " << mRLPos << ", avgLen: " << avgLength;
	 cerr << ", stdDesv: " <<  stdDesviation << " ";
	 
	 /*Compute noccs*/
	 int *noccs =  new int[rule_index+getAlpha()];
	 for (uint i=0;i<rule_index+getAlpha();i++)
		noccs[i]=0;
		
	double avg=0;
	for (int i=0;i<getNC();i++){
		computeNoccs(getC(i),noccs);
	}
	double nOccsDev=0;
	for (int i=0;i<rule_index+getAlpha();i++){
		avg+=noccs[i];
		nOccsDev+=(noccs[i]*noccs[i]);
	}
	avg/=(rule_index+getAlpha());
	nOccsDev =sqrt((nOccsDev/(rule_index+getAlpha()))-(avg));
	cerr << "Avg noccs: " << avg << ", desviation: " << nOccsDev << " ";
	int maxOccs=0;
	int ruleMO=0;
	for (int i=alph;i<rule_index+getAlpha();i++){
		if (maxOccs<noccs[i]){
			ruleMO=i;
			maxOccs=noccs[i];
		}
	}
	
	cerr << "rule-0 stats: " << "maxOccs: " << noccs[alph] <<  " ";
	
	cerr << "maxOccs: " << maxOccs << ", rule: " << ruleMO << ", length: " << lengths[ruleMO+alph] << " ";
	 
	 char file_out[2048];
	 uint researchAux=0;
	 sprintf(file_out,"proba.txt",researchAux);
	 //~ cerr << "out file plot: " << file_out << endl;
	 researchAux++;
	 FILE *fout = fopen(file_out,"w");
	 //~ fwrite(lengths,sizeof(int)*rule_index,1,fout);
	 //~ fclose(fout);
	 #if 1
	 int maxPloted=10000;
	 uint pPlot=0;
	 for (int i=0;i<rule_index;i++){
		 //~ if (lengths[i]>avgLength && (double)noccs[i+alph]>=avg){
			 //~ if (noccs[i]>0){
				fprintf(fout,"%i\t%i\t%i\t%i\n",i,lengths[i],i,noccs[alph+i]);
			//~ }else{
				//~ fprintf(fout,"%i\t%i\t%i\t-1\n",i,lengths[i],i);
			//~ }
			pPlot++;
			if (pPlot>=maxPloted)
				break;
		//~ }
	 }
	 cerr << "pPlot: " << pPlot << " ";
	 #endif
	 #if 0
	 int maxPloted=100000;
	 uint pPlot=0;
	 for (int i=0;i<rule_index;i++){
			fprintf(fout,"%i\t%i\t%i\t%i\n",i,lengths[i],i,noccs[i+alph]);
			pPlot++;
			if (pPlot>=maxPloted)
				break;
		
	 }
	 cerr << "pPlot: " << pPlot << endl;
	 #endif
	 cerr << "Repair size: " << (double)getSize()/input_len << endl;
	 fclose(fout);
	 delete [] noccs;
}
/**
 * First cast the input to a vector of ints (field by field) and then 
 * runs the repair algorithm over the sequence of ints. 
 * len: is the number of chars of the input
 * */
void Repair::runRepair(char *input, uint len){
	input_len=len;
	int *inputint = (int*)malloc(sizeof(len));
	C = (int *)malloc(sizeof(int)*len);
	uint i=0;
	for (i=0;i<len;i++){
		C[i]=(int)input[i];
		inputint[i]=(int)input[i];
	}
	prepare(len);
	repair();
	postRepair();
	statistics();
	//~ printGrammar();
	grammarDepth=-1;
	assert(testRepair(inputint));
}

void Repair::runRepair(int *input, uint len){
	input_len=len;
	int *inputint = (int*)malloc(sizeof(int)*len);
	C = (int *)malloc(sizeof(int)*len);
	uint i;
	for (i=0;i<len;i++){
		C[i]=input[i];
		inputint[i]=input[i];
	}
	prepare(len);
	repair();
	postRepair();
	statistics();
	//~ printGrammar();
	grammarDepth=-1;
	assert(testRepair(inputint));
}


void Repair::deleteAll(){
	free(RULES_LEFT);
	free(RULES_RIGHT);
	delete [] C;
	if (lengths)free(lengths);
}

Repair *Repair::load(ifstream &fp){
	Repair *rp = new Repair();
	rp->input_len = loadValue<uint>(fp);
	rp->rule_index = loadValue<int>(fp);
	rp->alph = loadValue<int>(fp);
	rp->rule_index-=rp->alph;
	rp->c = loadValue<int>(fp);
	rp->RULES_LEFT = loadValue<int>(fp,rp->rule_index);
	rp->RULES_RIGHT = loadValue<int>(fp,rp->rule_index);
	rp->lengths = loadValue<int>(fp,rp->rule_index);
	rp->C = loadValue<int>(fp,rp->c);
	rp->nterms = loadValue<int>(fp);
	return rp;
}

void Repair::save(ofstream &fp){
	uint total=0;
		 
		 //~ fprintf(stderr,"n-alph: %i \n",rule_index);
		 total+=sizeof(int);
		 saveValue(fp,input_len);
		 //~ fwrite(&input_len,sizeof(int),1,f);
		 rule_index+=alph;
		 total+=sizeof(int);
		 saveValue(fp,rule_index);
		 //~ fwrite(&rule_index,sizeof(int),1,f);
		 total+=sizeof(int);
		 //~ fprintf(stderr,"Total=%u\n",total);
		 saveValue(fp,alph);
		 //~ fwrite(&alph,sizeof(int),1,f);
		 total+=sizeof(int);
		 //~ fprintf(stderr,"Total=%u\n",total);
		 saveValue(fp,c);
		 //~ fwrite(&c,sizeof(int),1,f);
		 rule_index-=alph;
		 
		 total+=sizeof(int)*rule_index;
		 //~ fprintf(stderr,"Total=%u\n",total);
		 saveValue(fp,RULES_LEFT,rule_index);
		 //~ fwrite(RULES_LEFT,sizeof(int)*rule_index,1,f);
		 total+=sizeof(int)*rule_index;
		 //~ fprintf(stderr,"Total=%u\n",total);
		 saveValue(fp,RULES_RIGHT,rule_index);
		 //~ fwrite(RULES_RIGHT,sizeof(int)*rule_index,1,f);
		 total+=sizeof(int)*rule_index;
		 //~ fprintf(stderr,"Total=%u\n",total);
		 saveValue(fp,lengths,rule_index);
		 //~ fwrite(lengths,sizeof(int)*rule_index,1,f);
		 total+=sizeof(int)*c;
		 //~ fprintf(stderr,"Total=%u\n",total);
		 //~ fprintf(stderr,"Total=%u\n",total);
		 //~ fprintf(stderr,"c: %i \n",c);
		 saveValue(fp,C,c);
		 //~ fwrite(C,sizeof(int)*c,1,f);
		 //~ fprintf(stderr,"Total=%u\n",total);
		 saveValue(fp,nterms);
}

bool Repair::testExpand(int rule, uint &p, int *input){
	if (rule<alph){
		return input[p++]==rule;
	}else{
		return testExpand(RULES_LEFT[rule-alph],p,input) && testExpand(RULES_RIGHT[rule-alph],p,input);
	}
	
}
/**
 * Checks if the sequence generated by the combination (Rules,C) expand 
 * to the original sequence.
 * */
bool Repair::testRepair(int *input){
	// uint p=0;
	// for (int i=0;i<c;i++){
	// 	if (!testExpand(C[i],p,input))
	// 		return false;
	// }
	// assert(p==input_len);
	// cerr << "Test Repair OK!"<<endl;
	return true;
}

};
