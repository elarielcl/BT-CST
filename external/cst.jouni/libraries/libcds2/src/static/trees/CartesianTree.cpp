#include <trees/CartesianTree.h>
#include <utils/BinaryTree.h>
#include <climits>

namespace cds_static{

typedef struct
{
	uint id;
	uint h;
	/* data */
}content;

void CartesianTree::obtainCartesianTreeRec(uint *input, size_t len, uint *treeBmp, size_t &treeLength){
	content *contenido = new content[len];
	for (size_t i=0;i<len;i++){
		contenido[i].id=input [i];
		contenido[i].h=0;
	}
	contenido[0].h=1;
	BinaryTree<content*> *tree = new BinaryTree<content*>(contenido);
	BinaryTree<content*> *root=tree;

	for (size_t i=1;i<len;i++){
		BinaryTree<content*> *t = new BinaryTree<content*>(contenido+i);
		
		while(tree->getValue()->id>t->getValue()->id && tree->getParent()){
			tree = tree->getParent();
		}
		if (tree->getValue()->id<t->getValue()->id){
			if (tree->getLeft() && tree->getRight()){
				t->setLeft(tree->getRight());
				t->getValue()->h=tree->getValue()->h++;
				tree->setRight(t);
				//parent pointer are automatically updated on setLeft/setRight
			}
			if (tree->getLeft()){
				tree->setRight(t);
				t->getValue()->h=tree->getValue()->h+1;
				tree=t;
			}else{
				tree->setLeft(t);
				t->getValue()->h=tree->getValue()->h+1;
				tree=t;
			}
		}else{
			//t is the new root (is the minimun found until now)
			t->setLeft(tree);
			t->getValue()->h = tree->getValue()->h+1;
			tree = t;
			root = t;
		}
	}

	cerr << "Tree depth: " << root->getValue()->h << endl;
	treeLength = root->obtainParenthesesEstructure(treeBmp,len);
	
}

void CartesianTree::obtainCartesianTreeRec(uint *input, size_t len, size_t i, size_t j, uint *tree, size_t &p){
	if (i>j)
		return;
	if (i==j){
		bitset(tree,p);
		p+=2;
		return;
	}
	uint min=INT_MAX;
	size_t posMin=-1;
	for (size_t k=i;k<=j;k++){
		if (input[k]<min){
			min=input[k];
			posMin = k;
		}
	}

	bitset(tree,p++);//opening parentheses
	if (posMin>0)
	obtainCartesianTreeRec(input,len, i,posMin-1,tree,p);
	if (posMin<len)
	obtainCartesianTreeRec(input,len, posMin+1,j,tree,p);
	p++;//closing parentheses
}

void CartesianTree::obtainCartesianTree(uint *input, size_t len, uint *&tree, size_t &treeLength){
	//testCartesianTree(input,len);
	createEmptyBitmap(&tree,len*2);

	treeLength=0;
	if (len<1000000){
		obtainCartesianTreeRec(input,len-1,0,len-1,tree,treeLength);
	}else{
		obtainCartesianTreeRec(input,len,tree,treeLength);
	}
	assert(treeLength<=len*2);
}


void CartesianTree::testCartesianTree(uint *input, size_t len){
	uint *tree, *tree2;
	createEmptyBitmap(&tree,len*2);
	createEmptyBitmap(&tree2,len*2);
	
		size_t pointer = 0;
		obtainCartesianTreeRec(input,len-1,0,len-1,tree,pointer);
		assert(pointer<=len*2);
		pointer=0;
		obtainCartesianTreeRec(input,len,tree2,pointer);
		assert(pointer<=2*len);
		len*=2;
	for (size_t i=0;i<len;i++){
		assert(bitget(tree,i)==bitget(tree2,i));
	}
	cerr << "Test 2 algorithms Cartesian OK" << endl;
	delete [] tree;
	delete [] tree2;
}

}
