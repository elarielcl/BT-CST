
#include <cstdlib>

#include <libcdsBasics.h>
#include <BitSequence.h>
#include <Mapper.h>
#include <DisjointSet.h>

using namespace std;
using namespace cds_utils;

bool testMake(uint i, uint n, uint *voc, DisjointSet<uint> *uf){
	assert(uf->sets[i]);
	assert(uf->sets[i]->getValue().id==voc[i]);
	assert(uf->sets[i]->getValue().rank==0);
	assert(uf->sets[i]->getValue().content==(voc[i]+n));
	return true;
}

//test the union. We should first test findSet but we do not do it (i'm a rash ;-)
bool testUnion(uint x0,uint x1, uint n, uint *voc, DisjointSet<uint> *uf){
	
	uint res[2];
	uint rnk[2];
	uint nChild[2];
	uf->findSet(x0,res[0]);
	uf->findSet(x1,res[1]);
	rnk[0] = uf->sets[res[0]]->getValue().rank;
	rnk[1] = uf->sets[res[1]]->getValue().rank;
	nChild[0] = uf->sets[res[0]]->getNChildren();
	nChild[1] = uf->sets[res[1]]->getNChildren();
	//union of sets 0 and 1. The result has to be set 0 or 1 since 
	//both have rank=0. The resulting parent will have rank=1 and 1 child
	uint resu;
	uf->unionSet(x0,x1,resu);
	assert(uf->sets[resu]->getValue().id==resu);
	//the result of the union has to be findSet(x0) or findSet(x1)
	uint p;
	(resu==res[0])?p=0:p=1;
	
	assert(uf->sets[resu]->getValue().rank>=rnk[p]);
	assert(uf->sets[res[p]]->getValue().rank>=rnk[p]);
	
	assert(uf->sets[resu]->getNChildren()>=1);
	assert(uf->sets[resu]->getNChildren()==(nChild[p]+1));
	uint ch;
	uf->findSet(uf->sets[resu]->getChild(0)->getParent()->getValue().id,ch);
	assert(ch==resu);
	//oppositeP=0 if p=1, 1 if p=0
	uint oppositeP=abs(1-p);
	
	uint res2;
	uf->findSet(x1,res2);
	assert(res2==res[p]);
	int i;
	for (i=0;i<uf->sets[res2]->getNChildren();i++)
		if (uf->sets[res2]->getChild(i)->getValue().id==res[oppositeP]) break;
	assert(i<uf->sets[res2]->getNChildren());
	assert(uf->sets[res2]->getChild(i)->getValue().id==res[oppositeP]);
	assert(uf->sets[res2]->getChild(i)->getValue().rank==rnk[oppositeP]);

	return true;
}

int main(int argc, char ** argv) {

    uint n = 20;
	uint *voc = new uint[n];
	for (uint i=0;i<n;i++)
		voc[i]=i;
	
	DisjointSet<uint> *uf = new DisjointSet<uint>(n);
	
	//TEST BUILD
	for (uint i=0;i<n;i++){
		uf->makeSet(voc[i],voc[i]+n);
		testMake(i,n,voc,uf);
	}
	assert(testUnion(0u,1u,n,voc,uf));
	assert(testUnion(0u,2u,n,voc,uf));
	
	assert(testUnion(3u,4u,n,voc,uf));
	
	assert(testUnion(0u,3u,n,voc,uf));
	cerr << "Test OK!" << endl;
  return 0;
}

