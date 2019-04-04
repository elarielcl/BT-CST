
/*

Repair -- an implementation of Larsson and Moffat's compression and
decompression algorithms.
Copyright (C) 2010-current_year Gonzalo Navarro

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Author's contact: Gonzalo Navarro, Dept. of Computer Science, University of
Chile. Blanco Encalada 2120, Santiago, Chile. gnavarro@dcc.uchile.cl

*/


#include <grammar/irepair.h>

namespace cds_static{
	

void prepare (int len)

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
  }

void prnSym(int c)
   { printf("%i",c); 
   }

void prnC (void)

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

void prnRec (void)

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
  
  int getSym(int c){
   //~if (c < alph) return c;//return (int)map(c); else return c;
   return c;
   }

	// to avoid another list to access the sparse C we thread it using the
	// empty space. if next cell of an active cell is negative, it is 
	// (minus) a ptr to the next occ. idem previous cell to previous occ,
	// except that next ptr dominates over prev ptr if they must be in
 	// the same cell. but in this case one can find prev in O(1) anyway.

int repair ()

  { 
	  
	RULES_LEFT = (int*)malloc(sizeof(int)*10000000);
	RULES_RIGHT = (int*)malloc(sizeof(int)*10000000);
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
 	rule_index++;
	//~ if (fwrite (&orec->pair,sizeof(Tpair_repair),1,R) != 1) return -1;
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
	      L = (Tlist_repair*)realloc (L, c * sizeof(Tlist_repair));
              assocRecords (&Rec,&Hash,&Heap,L);
	    }
       }
     return 0;
   }

int computeLengths(int rule, int *lengths){
	
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

void statistics(){
	//computes the voc size (number of terminal symbols)
	char *voc = (char *)malloc(sizeof(char)*alph);
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
	free(voc);
	//~ printf("nterms:%i\n",nterms);
	
}

void postRepair(){
	int i;
	i = 0;
     int csize=0;
     int *newC = (int*)malloc(sizeof(uint)*u);
     while (i<u)
        { 
			newC[csize]=getSym(C[i]);
			 csize++;
          i++;
          if ((i < u) && (C[i] < 0)) 
			i = -C[i]-1;
        }
     c=csize-1;
     #if 0
	 printf("\n");
	 for (i=0;i<rule_index;i++){
		 printf("%i->%i,%i\n",i,RULES_LEFT[i],RULES_RIGHT[i]);
	 }
	 #endif
     
     int *lengths = (int *)malloc(sizeof(int)*(rule_index));
     for (i=0;i<rule_index;i++)lengths[i]=-1;
     for (i=0;i<rule_index;i++){
		 computeLengths(i+alph,lengths);
	 }
}

void runRepair(char *input, uint len){
	input_len=len;
	C = (int *)malloc(sizeof(int)*len);
	uint i=0;
	for (i=0;i<len;i++)
		C[i]=(uint)input[i];
	prepare(len);
	repair();
	postRepair();
	statistics();
}

void runRepair(int *input, uint len){
	input_len=len;
	C = (int *)malloc(sizeof(int)*len);
	uint i;
	for (i=0;i<len;i++)
		C[i]=input[i];
	prepare(len);
	repair();
	postRepair();
	statistics();
}


void deleteAll(){
	free(RULES_LEFT);
	free(RULES_RIGHT);
	free(C);
	free(lengths);
}

void save(char *filename){
		//~ fprintf(stderr,"Writing the grammar structures to %s\n",filename);
	uint total=0;
	FILE *f = fopen(filename,"wb");
		 
		 fprintf(stderr,"n-alph: %i \n",rule_index);
		 total+=sizeof(int);
		 fwrite(&input_len,sizeof(int),1,f);
		 rule_index+=alph;
		 total+=sizeof(int);
		 fwrite(&rule_index,sizeof(int),1,f);
		 total+=sizeof(int);
		 fprintf(stderr,"Total=%u\n",total);
		 fwrite(&alph,sizeof(int),1,f);
		 total+=sizeof(int);
		 fprintf(stderr,"Total=%u\n",total);
		 fwrite(&c,sizeof(int),1,f);
		 rule_index-=alph;
		 
		 total+=sizeof(int)*rule_index;
		 fprintf(stderr,"Total=%u\n",total);
		 fwrite(RULES_LEFT,sizeof(int)*rule_index,1,f);
		 total+=sizeof(int)*rule_index;
		 fprintf(stderr,"Total=%u\n",total);
		 fwrite(RULES_RIGHT,sizeof(int)*rule_index,1,f);
		 total+=sizeof(int)*rule_index;
		 fprintf(stderr,"Total=%u\n",total);
		 fwrite(lengths,sizeof(int)*rule_index,1,f);
		 total+=sizeof(int)*c;
		 fprintf(stderr,"Total=%u\n",total);
		 fprintf(stderr,"Total=%u\n",total);
		 fprintf(stderr,"c: %i \n",c);
		 fwrite(C,sizeof(int)*c,1,f);
		 fprintf(stderr,"Total=%u\n",total);
	fclose(f);
}
#if 0
int main (int argc, char **argv)

   { char fname[1024];
     char *text;
     FILE *Tf,*Rf,*Cf;
     int i,len;
     struct stat s;

     if (argc <3)
	{ fprintf (stderr,"Usage: %s <filename> <c|i> [<file_output>]\n"
			  "Compresses <filename> with repair taking the input "
			  "as a sequence of integers <i> or chars <c> "
			  "and writes in <file_output> the following:\n"
			  "\tlen:\t\tlength of the input (1*sizeof(int))\n"
			  "\tn:\t\ttotal number of rules -terminals+nonterminals- (1*sizeof(int))\n"
			  "\talph:\t\tmax. value of a terminal symbol(1*sizeof(int))\n"
			  "\tc:\t\tsize of the sequence of rules that generates the input (1*sizeof(int))\n"
			  "\tRULES_LEFT:\tthe left part of each rule ((n-alph)*sizeof(int))\n"
			  "\tRULES_RIGHT:\tthe right part of each rule ((n-alph)*sizeof(int))\n"
			  "\tlengths:\t length of each rule\n"
			  "\tC:\t\tthe sequence of rules that generates the input (c*sizeof(int))\n\n",argv[0]);
	  exit(1);
	}
	
	if (stat (argv[1],&s) != 0)
	{ fprintf (stderr,"Error: cannot stat file %s\n",argv[1]);
	  exit(1);
	}
	Tf=fopen(argv[1],"r");
	if (argv[2][0]=='c'){
		len = s.st_size;
		char *tmp = (char*)malloc(len);
		if (fread(tmp,sizeof(char),len,Tf)!=len)
			{ fprintf (stderr,"Error: cannot read file %s\n",argv[1]);
				exit(1);
			}
			C = (int*)malloc(len*sizeof(int));
			int ww;
			for (ww=0;ww<len;ww++){
				C[ww]=(int)tmp[ww];
			}
		free(tmp);
	}else if (argv[2][0]=='i'){
		len = s.st_size/sizeof(uint);
		C = (int*)malloc(len*sizeof(int));
		if (fread(C,sizeof(int),len,Tf)!=len)
			{ fprintf (stderr,"Error: cannot read file %s\n",argv[1]);
				exit(1);
			}
	}else{
		fprintf(stderr,"The type of the input must be i or c (integer of chars). %s given\n",argv[2]);
		return -1;
	}
	fclose(Tf);
     
     prepare (len);
     repair();

	
     i = 0;
     int csize=0;
     int *newC = (int*)malloc(sizeof(uint)*u);
     while (i<u)
        { 
			newC[csize]=getSym(C[i]);
			 csize++;
          i++;
          if ((i < u) && (C[i] < 0)) 
			i = -C[i]-1;
        }
     c=csize-1;
     int *tmp = C;
     C = newC;
     free(tmp);
     //~ rewrite();
     #if 1
	 printf("\n");
	 for (i=0;i<rule_index;i++){
		 printf("%i->%i,%i\n",i+alph,RULES_LEFT[i],RULES_RIGHT[i]);
	 }
	 
	 for (i=0;i<csize;i++){
		 printf("%i ",newC[i]);
	 }
	 printf("\n");

	 #endif
     
     
     lengths = (int *)malloc(sizeof(int)*(rule_index));
     for (i=0;i<rule_index;i++)lengths[i]=-1;
     for (i=0;i<rule_index;i++){
		 computeLengths(i+alph,lengths);
	 }
     /**
      * Write C and R to files.
      * */
    //when processing bitmaps in the input (the aim of this program).

	if (argc==4){
		fprintf(stderr,"Writing the grammar structures to %s\n",argv[2]);
		uint total=0;
		FILE *f = fopen(argv[2],"wb");
		 
		 fprintf(stderr,"n-alph: %i \n",rule_index);
		 total+=sizeof(int);
		 fwrite(&len,sizeof(int),1,f);
		 rule_index+=alph;
		 total+=sizeof(int);
		 fwrite(&rule_index,sizeof(int),1,f);
		 total+=sizeof(int);
		 fprintf(stderr,"Total=%u\n",total);
		 fwrite(&alph,sizeof(int),1,f);
		 total+=sizeof(int);
		 fprintf(stderr,"Total=%u\n",total);
		 fwrite(&csize,sizeof(int),1,f);
		 rule_index-=alph;
		 
		 total+=sizeof(int)*rule_index;
		 fprintf(stderr,"Total=%u\n",total);
		 fwrite(RULES_LEFT,sizeof(int)*rule_index,1,f);
		 total+=sizeof(int)*rule_index;
		 fprintf(stderr,"Total=%u\n",total);
		 fwrite(RULES_RIGHT,sizeof(int)*rule_index,1,f);
		 total+=sizeof(int)*rule_index;
		 fprintf(stderr,"Total=%u\n",total);
		 fwrite(lengths,sizeof(int)*rule_index,1,f);
		 total+=sizeof(int)*csize;
		 fprintf(stderr,"Total=%u\n",total);
		 fprintf(stderr,"Total=%u\n",total);
		 fprintf(stderr,"c: %i \n",csize);
		 fwrite(C,sizeof(int)*csize,1,f);
		 fprintf(stderr,"Total=%u\n",total);
		 
     fclose(f);
	}	
	
     statistics();
    deleteAll();
     fprintf (stderr,"\nRePair succeeded\n");
     //~ fprintf (stderr,"   Original chars: %i\n",len);
     //~ fprintf (stderr,"   Number of rules: %i\n",n-alph);
     //~ fprintf (stderr,"   Final sequence length: %i\n",c);
     //~ fprintf (stderr,"   Compression ratio: %0.2f%%\n",
			//~ (2.0*(n-alph)+c)*(float)blog(n-1)/(len*8.0)*100.0);
     exit(0);
   }
   #endif

};
