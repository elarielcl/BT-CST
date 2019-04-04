/*  
 * Copyright (C) 2013-current_year, Alberto Ordóñez, all rights reserved.
 *
 * Alberto Ordóñez <alberto.ordonez@udc.es>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <grammar/DictionaryRP.h>
#include <grammar/Repair.h>
#include <climits>


namespace cds_static{

	DictionaryRP::DictionaryRP(){}
	
	DictionaryRP::DictionaryRP(uint *LEFT, uint *RIGHT, uint alph, int R, uint *&PERM){
		permutation=NULL;
		this->R=R;
		buildTrees(LEFT,RIGHT,alph,R,PERM,new PermutationBuilderMRRR(5,new BitSequenceBuilderRG(10)));
		assert(testDictionaryRP(LEFT,RIGHT,alph,R,PERM));
	}
	DictionaryRP::DictionaryRP(uint *LEFT, uint *RIGHT, uint alph, int R, uint *&PERM,PermutationBuilder *pmb){
		permutation=NULL;
		this->R=R;
		buildTrees(LEFT,RIGHT,alph,R,PERM,pmb);
		assert(testDictionaryRP(LEFT,RIGHT,alph,R,PERM));
	}
	
	DictionaryRP::DictionaryRP(Repair *rp,uint *&PERM,PermutationBuilder *pmb){
		permutation=NULL;
		uint *LEFT,*RIGHT;
		this->R=rp->getNRules()+rp->getTermsSize();
		LEFT = new uint[this->R];
		RIGHT = new uint[this->R];
		for (int i=0;i<rp->getNRules();i++){
			LEFT[i]=rp->getLeft(i);
			RIGHT[i]=rp->getRight(i);
		}
		buildTrees(LEFT,RIGHT,rp->getTermsSize(),R,PERM,pmb);
		assert(testDictionaryRP(LEFT,RIGHT,rp->getTermsSize(),R,PERM));
		delete [] LEFT;
		delete [] RIGHT;
	}
	
	DictionaryRP::~DictionaryRP(){
		delete LT_LOUDS;
		delete RT_LOUDS;
		if (permutation)
			delete permutation;
	}
	//alph is the max value of a terminal symbol. We are supposing 
	//the terminal symbols take continuous values in the interval [0,alph]
	//thus, the number of terminals is alph+1 
	uint DictionaryRP::getAlph(){
		return alphaPlus1;
	}
	
	uint DictionaryRP::getNRules(){
		return R;
	}
	
	int cmpTreePointers(const void *p1, const void *p2){
		TreePointers<int> *t1,*t2;
		t1 = (TreePointers<int> *)p1;
		t2 = (TreePointers<int> *)p2;		
		return t1->getValue()-t2->getValue();
	}
	void DictionaryRP::buildTrees(uint *LEFT, uint *RIGHT, uint ALPH,  int R,  uint *&PERM, PermutationBuilder *pmb){
		int alph;
		alpha=ALPH-1;
		alph=ALPH;
		alphaPlus1=ALPH;
		TreePointers<int>* *ladded = new TreePointers<int>*[R+1];
		TreePointers<int>* *radded = new TreePointers<int>*[R+1];
		//PROCESS LEFT CHILDREN
		ladded = new TreePointers<int>*[R+1];
		//at ladded[0] is the root
		ladded++;
		for (int i=alph;i<R;i++) ladded[i]=NULL;
		//add terminal symbols
		for (int i=0;i<alph;i++){
			ladded[i] = new TreePointers<int>((int)i);
		}
		
		for (uint i=0;i<R-ALPH;i++){
			//~ int lc = rules->get_left(i);
			int lc = LEFT[i];
			if (ladded[lc]==NULL){
				ladded[lc]=new TreePointers<int>(lc);
			}
			ladded[i+alph] = new TreePointers<int>(i+alph); 
			ladded[lc]->addChild(ladded[i+alph]);	
		}
		
		
		ladded--;
		ladded[0]= new TreePointers<int>(-1);
		//add the terminals as childs of the root
		for (int i=1;i<=alph;i++){
			ladded[0]->addChild(ladded[i]);
		}
		//the terminals symbols (0,1,2,..alph-1) must be the child-0,child-1,...,child-alph-1 of the root)
		//they have to be sorted
		for (int i=0;i<alph-1;i++){
			if(ladded[0]->getChild(i)->getValue() > ladded[0]->getChild(i+1)->getValue()){
				TreePointers<int> **terms = new TreePointers<int>*[ALPH];
				for (int j=0;j<alph;j++)
					terms[j]=ladded[0]->popLastChild();
				qsort(terms,sizeof(TreePointers<int> *),alph,cmpTreePointers);
				for (int j=0;j<alph-1;j++)
					assert(terms[j]->getValue()<terms[j+1]->getValue());
				for (int j=0;j<alph;j++)
					ladded[0]->addChild(terms[j]);
			}
		}
		
		uint *LT = new uint[uint_len(2*(R+1),1)];
		uint *RT = new uint[uint_len(2*(R+1),1)];
		for (size_t i=0;i<uint_len(2*(R+1),1);i++)
			LT[i]=RT[i]=0;
	
		/** Obtain the left trie. This gives us the new id of each symbol (
		 * its position in LOUDS order)*/
		uint pl,pr,pq,head;
		TreePointers<int> **rLOUDS = new TreePointers<int>*[R+1];
		TreePointers<int> **lLOUDS = new TreePointers<int>*[R+1];
		RSIZE=R+1;
		//GET LOUDS TL
		for (int i=0;i<=R;i++)lLOUDS[i]=NULL;
		for (int i=0;i<alph;i++){
			lLOUDS[i]=ladded[i+1];
		}
		head=0;pq=alph-1;pl=0;
		getLOUDS(lLOUDS,head,pq,pl,LT);

		
		//PROCESS RIGHT CHILDREN
		
		radded = new TreePointers<int>*[R+1];
		radded++;
		for (int i=alph;i<R;i++) radded[i]=NULL;
		for (int i=0;i<alph;i++){
			radded[i] = new TreePointers<int>((int)i);
		}
		
		for (size_t i=0;i<R-ALPH;i++){			
			int rc = RIGHT[i];			
			if (radded[rc]==NULL){
				radded[rc]=new TreePointers<int>(rc);
			}
			radded[i+alph] = new TreePointers<int>(i+alph); 
			radded[rc]->addChild(radded[i+alph]);
		}
		
		radded--;
		radded[0]= new TreePointers<int>(-1);
		//radded[0]->addChild(radded[1]);radded[0]->addChild(radded[2]);
		for (int i=1;i<=alph;i++){
			radded[0]->addChild(radded[i]);
		}
		
		//BUILD LOUDS FOR RIGHT-TREE
			
		//GET LOUDS RT
		for (int i=0;i<=R;i++)rLOUDS[i]=NULL;
		for (int i=0;i<alph;i++){
			rLOUDS[i]=radded[i+1];
		}
		head=0;pq=alph-1;pr=0;
		getLOUDS(rLOUDS,head,pq,pr,RT);
		assert(pl==(uint)2*R-alph);
		assert(pl==pr);
		#if PRINT
			cout << "RT: " << endl;
			for (int i=0;i<pl;i++){
				if (bitget(RT,i))
					cout << "1";
				else
					cout << "0";
			}
			cout << endl;
			
			cout << "LT: " << endl;
			for (int i=0;i<pl;i++){
				if (bitget(LT,i))
					cout << "1";
				else
					cout << "0";
			}
			cout << endl;
		#endif
		//BUILD BITMAPS
		LT_LOUDS = new BitSequenceRG(LT,pl,10);
		RT_LOUDS = new BitSequenceRG(RT,pr,10);
		
		#if 0
		for (int i=2;i<R;i++){
			cout << "parent("<<i<<"): " << getParentLOUDS(i,LT_LOUDS) << endl;
		}
		#endif
		//~ deleteStructures(lLOUDS[0]);
		//~ deleteStructures(rLOUDS[0]);
		
		//PERMUTATIONS
		/**Obtain the new ids by transversing the l-trie in LOUDS order*/
		PERM = new uint[R];
		uint *RWRIGHT = new uint[R];
		for (int i=0;i<R;i++){
			PERM[lLOUDS[i]->getValue()]=i;
		}
		/** Rewrite the grammar*/
		for (uint i=0;i<R-ALPH;i++){
			LEFT[i]=PERM[LEFT[i]];
			RIGHT[i]=PERM[RIGHT[i]];
		}
		
		for (int i=0;i<R;i++){
			RWRIGHT[i] = PERM[rLOUDS[i]->getValue()];
		}
		PERM_SIZE=R;
		PERM_tmp = new uint[R];
		INVPERM_tmp = new uint[R];
		for (int i=0;i<R;i++){
			PERM_tmp[i] = RWRIGHT[i];
			INVPERM_tmp[RWRIGHT[i]]=i;
			//~ cout << " i: " << i << " pi: " << RWRIGHT[i] << ", invpi: " << INVPERM_tmp[RWRIGHT[i]] << endl;
		}
		
		uint nbits = bits(R-1);
		
		uint *inputP = new uint[uint_len(nbits,R)];
		for (int i=0;i<uint_len(nbits,R);i++)
			inputP[i]=0;
		for (int i=0;i<R;i++){
			set_field(inputP,nbits,i,PERM_tmp[i]);
		}
		permutation = pmb->build(inputP,R);
		delete [] inputP;
		
		
		for(int i=0;i<R;i++){
			assert(permutation->pi(i)==PERM_tmp[i]);
			assert(permutation->revpi(i)==INVPERM_tmp[i]);
		}
		delete [] PERM_tmp;
		delete [] INVPERM_tmp;
		
		
		//~ delete [] input;
		delete [] radded;
		delete [] ladded;
		delete [] lLOUDS;
		delete [] rLOUDS;
		delete [] RWRIGHT;
		
	}
	
	uint DictionaryRP::invertUnary(uint val, uint ini, uint len){
		uint res=0;
		for (size_t i=0;i<=len/2;i++){
			if (bitget(&val,i)) 
				bitset(&res,len-i-1);
			if (bitget(&val,len-i-1))
				bitset(&res,i);
		}

		return res;
	}

	void DictionaryRP::deleteStructures(TreePointers<int> *t){
		if (t==NULL) return;
		if (t->getNChildren()==0)
			delete t;
		for (int i=0;i<t->getNChildren();i++)
			deleteStructures(t->getChild(i));
		
	}
	
	void DictionaryRP::getLOUDS(TreePointers<int> **queue,uint &head,uint &pq, uint &p, uint *louds){
		
		while (head<=pq){
		//~ if (head>pq) return;
			if (head>(RSIZE-1)) 
				cerr << "error head: " << head << ", pq= " << pq << endl;
			
			TreePointers<int> *t = queue[head];
			head++;
			
			if (t==NULL) cerr << "T es nulo. Head = " << head << ", pq= " << pq << endl;
			else{			
				if (t->getNChildren()==0){
					p++;
				}else{
					size_t x = t->getNChildren();
					for (size_t i=0;i<x;i++)
						bitset(louds,i+p);
					p+=x+1;
					for (int i=0;i<t->getNChildren();i++){
						if (pq>(RSIZE-1)) cerr << "pq>rsize. head= "  << head << ", pq= " << pq << endl;
						queue[++pq]=t->getChild(i);
					}
					
				}
			}
			
		}
		//~ getLOUDS(queue,head,pq,p,louds);
		
	}
	
	void DictionaryRP::getPERM(TreePointers<int> *t, uint &p, uint *perm){
		if (t->getNChildren()==0){
			return;
		}
		for (int i=0;i<t->getNChildren();i++){
			perm[t->getChild(i)->getValue()]=p++;
			getPERM(t->getChild(i),p,perm);
			
		}
	}
	uint DictionaryRP::getRight(size_t rule){
		assert(rule>alpha);
		return permutation->pi(getParentLOUDS(permutation->revpi(rule),RT_LOUDS));
	}
	
	uint DictionaryRP::getLeft(size_t rule){
		assert(rule>alpha);
		return getParentLOUDS(rule,LT_LOUDS);
	}
	
	size_t DictionaryRP::getParentLOUDS(uint r, BitSequence *louds){
		size_t slt = louds->select0(r+1);
		size_t rnk = louds->rank0(slt-1);
		slt = louds->select1(rnk-alpha);
		return louds->rank0(slt);
	}
		
	size_t DictionaryRP::getSize(){
		size_t size = sizeof(DictionaryRP);
		size+=LT_LOUDS->getSize();
		size+=RT_LOUDS->getSize();
		if (permutation!=NULL)
			size+=permutation->getSize();
		return size;
	}
	
	void DictionaryRP::save(ofstream &of){
		saveValue(of, alpha);
		LT_LOUDS->save(of);
		RT_LOUDS->save(of);
		permutation->save(of);
	}

	DictionaryRP *DictionaryRP::load(ifstream &in){
		DictionaryRP *d = new DictionaryRP();
		d->alpha = loadValue<uint>(in);
		d->LT_LOUDS = BitSequence::load(in);
		d->RT_LOUDS = BitSequence::load(in);
		d->permutation = Permutation::load(in);
		return d;
	}
	
	size_t DictionaryRP::getSizeVerbose(){
		size_t size = sizeof(DictionaryRP);
		size+=LT_LOUDS->getSize();
		size+=RT_LOUDS->getSize();
		if (permutation!=NULL)
			size+=permutation->getSize();
		
			cerr << "LT_LOUDS: " << LT_LOUDS->getSize() << endl;
			cerr << "RT_LOUDS: " << RT_LOUDS->getSize() << endl;
			if (permutation!=NULL) cerr << "Permutation: " << permutation->getSize() << endl;
			cout << "Total dic: " << size << endl;
			cerr << "Total without compression: " << (2*R*(log(R)/log(2)))/8 << endl;
			cerr << "Ratio: " << (double)size/((2*R*(log(R)/log(2)))/8) << endl;
		return size;
	}
	
	void DictionaryRP::printTree(TreePointers<int> *t){
		cout << t->getValue() << "->"<< endl;
		for (int i=0;i<t->getNChildren();i++){
			cout << "child-" << i << ": " << endl;
			printTree(t->getChild(i));
		}
	}
	
	bool DictionaryRP::testDictionaryRP(uint *LEFT, uint *RIGHT, uint ALPH, int R, uint *PERM){
		for (int i=ALPH;i<R;i++){
			assert(LEFT[i-ALPH]==getLeft(PERM[i]));
			assert(RIGHT[i-ALPH]==getRight(PERM[i]));
		}
		//cerr << "Test DictionaryRP OK!" << endl;
		return true;
	}
}
