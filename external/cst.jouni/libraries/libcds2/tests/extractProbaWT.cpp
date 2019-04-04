
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

int main(int argc, char ** argv) {

  if(argc!=3){
    cerr << "Usaga: "<< argv[0] << " input output" << endl;
    exit(0);
  }
  struct stat sdata;

  FILE *fin = fopen(argv[1],"rb");
  stat(argv[1],&sdata);
  char *input = new char[sdata.st_size];
  fread(input,sdata.st_size,1,fin);
  fclose(fin);
  uint *intMap = new uint[sdata.st_size];
  for (uint i=0;i<sdata.st_size;i++){
    intMap[i] = ((int)input[i])-97;
    if (!(intMap[i]>=0 && intMap[i]<=15)){
      cerr << endl;
    }
    assert(intMap[i]>=0 && intMap[i]<=15);
  }
  fin = fopen(argv[2],"wb");
  uint out=fwrite(intMap,sizeof(int),sdata.st_size,fin);
  assert(out==sdata.st_size);
  fclose(fin);
  delete [] intMap;
  delete [] input;
  return 0;
}

