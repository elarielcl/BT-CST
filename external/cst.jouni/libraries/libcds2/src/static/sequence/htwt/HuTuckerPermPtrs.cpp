
#include <sequence/htwt/HuTuckerPermPtrs.h>

namespace cds_static
{
	HuTuckerPermPtrs::HuTuckerPermPtrs(int *weights, uint weightsSize, uint *seq, uint sizeSeq,  BitSequenceBuilder * bmb){
		
		this->nleafs = weightsSize; 
		this->C = new uint[this->nleafs+1]; 
		//cout << "nleafs: " << nleafs << endl; 
		this->C[0]=0;
		this->C[1]=weights[0];
		for (uint i=2;i<=this->nleafs;i++) this->C[i] = weights[i-1]+this->C[i-1]; 
		
		huTucker *ht = createHuTuckerPsi((int *) weights, weightsSize, seq,(uint *) weights); 
		root = crearHT(ht->root, bmb); 
		uint leaf = 0; 
		leafs = (huTuckerNode **) malloc(sizeof(huTuckerNode*)*this->nleafs); 
		asignLeafNumber(this->root,leaf);
		//size();
	}
	
	HuTuckerPermPtrs::HuTuckerPermPtrs(){}
	
	HuTuckerPermPtrs::~HuTuckerPermPtrs() {

	}
	
	void HuTuckerPermPtrs::asignLeafNumber(huTuckerNode *node, uint &leafNo){
		if (node==NULL) return;
		if (node->isLeaf()){
			leafs[leafNo] = node; BitSequencePlain::BuilderSequencePlain builder = BitSequencePlain::BuilderSequencePlain::createBuilderSequencePlain(nullptr,0,33)->s1factor(32).s0factor(32);
      	
			node->setLeaf(leafNo); 
			leafNo++;
		}else{
			asignLeafNumber(node->getLeftChild(),leafNo); 
			asignLeafNumber(node->getRightChild(),leafNo); 
		}
	}
	
	void HuTuckerPermPtrs::loadLeafVector(huTuckerNode *node){
		if (node==NULL) return;
		if (node->isLeaf()){
			leafs[node->getLeaf()] = node; 
		}else{
			loadLeafVector(node->getLeftChild()); 
			loadLeafVector(node->getRightChild()); 
		}
	}
	
	void HuTuckerPermPtrs::printTreeStructure(huTuckerNode *node, uint level){
		if (node==NULL) return; 
		
		if (node->isLeaf()) cout << "Leaf: " << node->getLeaf() << endl; 		
		else{cout << "bitmapsize: " << node->getBitmap()->getLength() << endl; }
		if (node->getLeftChild()!=NULL){
			cout << "Level: " << level << ", L" << endl;  
		}
		if (node->getRightChild()!=NULL){
			cout << "Level: " << level << ", R" << endl;  
		}
		printTreeStructure(node->getLeftChild(),(level+1)); 
		printTreeStructure(node->getRightChild(),(level+1)); 
	}
	
	huTuckerNode * HuTuckerPermPtrs::crearHT(bNode *node, BitSequenceBuilder * bmb){

		huTuckerNode *left=NULL, *right=NULL; 
		uint leftSize=0, rightSize=0; 
		if (node == NULL) return NULL; 
		
		if (!isLeafBnode(node->children[0])){
			left = crearHT(node->children[0], bmb);
			leftSize = left->bitmapLength(); 
		}else{
			left=new huTuckerNode();  
		}
		
		if (!isLeafBnode(node->children[1])){
			right = crearHT(node->children[1], bmb);
			rightSize = right->bitmapLength(); 
		}else{
			right=new huTuckerNode();
		}
		uint *bitmap = new uint[uint_len_F(leftSize+rightSize,1)];
		for (uint i=0; i< uint_len_F(leftSize+rightSize,1); i++) bitmap[i]=0; 
		
		preMergePsiHTWT(node, node->children[0], node->children[1], &bitmap);
		
		huTuckerNode *result =  new huTuckerNode(bitmap, node->sizePsi , bmb, left, right); 
		//cout << node->sizePsi << endl;
		delete [] bitmap; 
		freebNode(node);
		 
		return result; 
	}


	void HuTuckerPermPtrs::preMergePsiHTWT(bNode *f, bNode *l, bNode *r, uint **bitmap){
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
		(*bitmap) = new uint[uint_len_F(sizePsiR+sizePsiL,1)];
		for (uint i=0; i< uint_len_F(sizePsiR+sizePsiL,1); i++) (*bitmap)[i]=0; 
		
		uint w; 
		mergePsiHTWT(psiL, sizePsiL, psiR, sizePsiR, &(f->psi), &w, *bitmap); 
		
		if (w != 0){
			f->sizePsi = w; 
			//~ (*pb)+= f->sizePsi; 
		}

	}

	void HuTuckerPermPtrs::mergePsiHTWT(uint *psiL, uint sizePsiL, uint *psiR, uint sizePsiR, uint **psiN, uint *sizePsiN, uint *bitmap){
		
		uint pb = 0; 
		if (sizePsiL == 0 && sizePsiR == 0){
			psiN = NULL; 
			(*sizePsiN) = 0; 
			return; 
		}

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

	void HuTuckerPermPtrs::destroy(){
		//~ fprintf(stderr,"Destroying HuTuckerPermPtrs\n"); 
		//~ 
		//~ delete bitmap;
		//~ delete [] children;
		//~ delete [] starts;

	}



	uint HuTuckerPermPtrs::inverseApplication(uint i, uint &leaf){
		return recursiveInverseApplication(root, i, 0, root->getBitmap()->getLength()-1, leaf); 
	}

	//~ uint HuTuckerPermPtrs::inverseApplication2(uint i, uint &leaf){
		//~ return recursiveInverseApplication(leafs[leaf], i); 
	//~ }
	int HuTuckerPermPtrs::rank(uint pos, uint limInf, uint &leaf){
		return recursiveRankHTWT(root, pos-1, 0, root->getBitmap()->getLength()-1, limInf,leaf); 	
	} 

	void HuTuckerPermPtrs::save( ofstream & f){
		uint wr = HUTUCKERPERMPTRS;
		saveValue(f,wr); 
		saveValue(f,this->nleafs); 
		saveValue(f, this->C, this->nleafs+1);  
		root->save(f); 
	} 

	uint HuTuckerPermPtrs::size(){
		uint total = sizeof(HuTuckerPermPtrs) + root->size()+sizeof(huTuckerNode*)*this->nleafs+sizeof(uint)*(this->nleafs+1); 
		//cout << "Hu-Tucker total size: " << total << endl;
		return total; 
		
	} 

	HuTuckerPermPtrs* HuTuckerPermPtrs::load( ifstream & f){
		cout << "Loading HuTucker Perm Ptrs" << endl; 
		uint rd;
		rd = loadValue<uint>(f); 
		if(rd!=HUTUCKERPERMPTRS) return NULL;
		HuTuckerPermPtrs * ret = new HuTuckerPermPtrs();
		ret->nleafs = loadValue<uint>(f); 
		ret->C = loadValue<uint>(f, ret->nleafs+1); 
 
		ret->root = huTuckerNode::load(f);
		ret->leafs = (huTuckerNode **) malloc(sizeof(huTuckerNode*)*ret->nleafs);
		ret->loadLeafVector(ret->root); 
		return ret; 
	} 

	void HuTuckerPermPtrs::printInfo(){

	}
	
	
	uint HuTuckerPermPtrs::recursiveInverseApplication(huTuckerNode *node,  uint i, uint p, uint s, uint &leaf){
			
		if (node->isLeaf()){ 
			leaf = node->getLeaf(); 
			return p+i; 
		}
		leaf = node->getLeaf(); 
		if (node->getBitmap()->access(i) == 0){
			i = node->getBitmap()->rank0(i)-1; 
			s = node->getBitmap()->rank0(s)-1; 
			return recursiveInverseApplication(node->getLeftChild(), i, p, s, leaf); 
		}else{

			i =   node->getBitmap()->rank1(i)-1; 
			//Optimization
			uint r0=node->getBitmap()->rank0(s);
			p+=r0;
			s=s-r0;//+1-1;
			//~ p +=  node->getBitmap()->rank0(s); 
			//~ s =   node->getBitmap()->rank1(s) -1; 
			return recursiveInverseApplication(node->getRightChild(), i, p, s, leaf); 
		}
	}
	
	uint HuTuckerPermPtrs::directApplication(uint i){
		huTuckerNode *node; 
		for (uint j=1;j<=this->nleafs;j++){
			if (this->C[j]>i){
				node = leafs[j-1];
				i = i - this->C[j-1]+1;  
				//positions start in 0 but select0/1 takes parameter in [1..]
				break;
			}
		}
		
		while(node->getParent()!=NULL){
			if (node->getParent()->getLeftChild()==node){
				i = node->getParent()->getBitmap()->select0(i)+1; 
			}else{ 
				i = node->getParent()->getBitmap()->select1(i)+1; 
			}
			node = node->getParent(); 
		}
		return i-1; 
	}
	
	uint HuTuckerPermPtrs::recursiveRankHTWT(huTuckerNode *node,  uint pos,  uint s, uint end, uint intinf, uint &leaf){

		if (node->isLeaf()){
			leaf = node->getLeaf(); 
			return  pos+1; 
		}
		leaf = node->getLeaf(); 
		uint n0 = node->getBitmap()->rank0(end - s); 
		if (intinf >= (s + n0)){
			s += n0; 
			pos = node->getBitmap()->rank1(pos)-1;
			return recursiveRankHTWT(node->getRightChild(), pos, s, end, intinf,leaf); 
			
		}else{
			end = s+n0-1;
			pos = node->getBitmap()->rank0(pos)-1;
			return recursiveRankHTWT(node->getLeftChild(), pos, s, end, intinf,leaf); 
		}

		
	}

	uint HuTuckerPermPtrs::isLeaf(uint posInTree){
		//~ return (posInTree >= nleafs);
		return 0; 
	}

	uint HuTuckerPermPtrs::getLeftChild( uint posInTree, uint prevPosInTree){
		
		//~ if (posInTree >=nleafs) return 0;
		//~ return get_field_basic(children, minBits,posInTree*2);
		return 0; 
	}

	uint HuTuckerPermPtrs::getRightChild( uint posInTree, uint prevPosInTree){
		//~ if (posInTree < prevPosInTree) return 0;
		//~ if (posInTree >=nleafs) return 0;
		//~ return get_field_basic(children, minBits,posInTree*2+1);
		return 0; 
	}

	uint HuTuckerPermPtrs::isLeafBnode(bNode *node){
		
		if ((node==NULL) || ((node->children[0] == NULL) && (node->children[1]==NULL)))
			return 1; 
		return 0; 
	}

};
