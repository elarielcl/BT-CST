
#include <huTuckerNode.h>

namespace cds_static
{
	
huTuckerNode::huTuckerNode(uint *bitmap, uint n, BitSequenceBuilder *bsb){
	
	this->bitmap = bsb->build(bitmap, n); 
	this->leftChild = 0; 
	this->rightChild = 0; 
	this->parent = NULL;
}
huTuckerNode::huTuckerNode(uint *bitmap, uint n, BitSequenceBuilder *bsb, huTuckerNode *l, huTuckerNode *r){
	
	this->bitmap = bsb->build(bitmap, n); 
	
	this->leftChild = l; 
	this->rightChild = r; 
	this->leftChild->setParent(this); 
	this->rightChild->setParent(this); 
	this->parent = NULL;
}

huTuckerNode::huTuckerNode(uint *bitmap, uint n, BitSequenceBuilder *bsb, huTuckerNode *l, huTuckerNode *r, uint leaf){
	
	this->bitmap = bsb->build(bitmap, n); 
	this->leftChild = l; 
	this->rightChild = r; 
	this->leaf = leaf; 
	this->leftChild->setParent(this); 
	this->rightChild->setParent(this); 
	this->parent = NULL;
}

huTuckerNode::huTuckerNode(){
	this->bitmap =0;
	this->leftChild = 0; 
	this->rightChild = 0; 
	this->leaf = 0; 
	this->parent = NULL;
	//~ this->bitmap->destroy(); 
}

huTuckerNode::~huTuckerNode(){}

uint huTuckerNode::rank(uint symbol, uint pos, uint l){
	return -1; 
}
uint huTuckerNode::select(uint symbol, uint pos, uint l){
	return -1; 
}
uint huTuckerNode::access(uint pos){
	return -1; 
}
uint huTuckerNode::size(){
	if (isLeaf()){
		return sizeof(huTuckerNode); 
	}else{
		 //~ cout << "bmpSize: " << bitmap->getSize() << endl; 
		return sizeof(huTuckerNode) + bitmap->getSize()+leftChild->size()+rightChild->size(); 
	}
}

void huTuckerNode::save(ofstream & fp){
	uint w; 
	if (isLeaf()){
		w= HU_TUCKER_LEAF;
		saveValue(fp, w); 
		saveValue(fp, leaf); 
	}else{
		w= HU_TUCKER_INTERNAL;
		saveValue(fp,w); 
		this->bitmap->save(fp);
		this->leftChild->save(fp); 
		this->rightChild->save(fp); 
	}
}

huTuckerNode * huTuckerNode::load(ifstream  & fp){
	huTuckerNode *ret = NULL;
	uint w; 
	w=loadValue<uint>(fp); 
	if (w==HU_TUCKER_LEAF){
		ret = new huTuckerNode(); 
		ret->leaf = loadValue<uint>(fp); 
	}else{
		ret = new huTuckerNode(); 
		ret->bitmap = BitSequence::load(fp);
		ret->setLeftChild(huTuckerNode::load(fp)); 
		ret->setRightChild(huTuckerNode::load(fp)); 
	}
	return ret; 
	
}

uint huTuckerNode::bitmapLength(){
	if (bitmap==NULL) return 0; 
	return bitmap->getLength(); 
}

uint huTuckerNode::printNode(){
	if ((leftChild == NULL && rightChild!=NULL ) ||
	(leftChild == NULL && rightChild!=NULL ) ){
		cout <<  "Error!!!!!"  << endl; 
	} 
	cout << bitmap->getLength() << endl;
	if (leftChild!=NULL){ cout << "L" << endl;  
	leftChild->printNode();} 
	if (rightChild!=NULL){ cout << "R" << endl; 
	rightChild->printNode();} 
	return 0;
}
};
