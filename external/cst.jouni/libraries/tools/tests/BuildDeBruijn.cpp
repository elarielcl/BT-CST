#include <deBruijn/DeBruijnBuilder.h>
#include <iostream>
#include <deBruijn/ConstructCDBG.h>

using namespace gctbio;
using namespace std;

uint doJob(int argc, char **argv){
    if (argc<4){
        cerr << "Usage: " << argv[0] << " <input.txt> <k> <basename>" << endl;
        return -1;
    }
    char *input = argv[1];
    ulong k = atol(argv[2]);
    char *basename = argv[3];

    ConstructCDBG cdbg(input,basename);
    auto cdbe = cdbg.buildCDBG(k);
    return 0;
}

int main(int argc, char **argv){

    uint ret = doJob(argc,argv);
    exit(ret);
}