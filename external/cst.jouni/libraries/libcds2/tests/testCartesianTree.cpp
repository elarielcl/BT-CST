
#include <cstdlib>
#include <libcdsBasics.h>
#include <CartesianTree.h>
#include <BitSequence.h>

using namespace std;
using namespace cds_static;

int main(int argc, char ** argv) {

  #if 1
  size_t len = 12;
  uint data[]={17,4,9,2,11,25,9,5,3,1,8,20};
  #endif
  /*
  if (argc<3){
    cerr << "Usage: " << argv[0] << " <max_v> <len>" << endl;
    exit(0);
  }

  size_t len = (size_t)atoi(argv[1]);
  uint max_v = atoi(argv[2]);
  uint *data = new uint[len];
  for (size_t i=0;i<len;i++){
    data[i]=rand()%max_v;
  }*/

  uint *bmp;
  size_t length;
  CartesianTree::obtainCartesianTree(data,len,bmp,length);
  // for (size_t i=0;i<2*len;i++){
  //   if (bitget(bmp,i))
  //     cerr << "(";
  //   else
  //     cerr << ")";
  // }
  // cout << endl;
  cerr << "Done" << endl;
  return 0;
}

