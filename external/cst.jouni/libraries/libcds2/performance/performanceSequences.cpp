#include <cstdio>
#include <cstdlib>
#include <utils/timeMeasure.h>
#include <sequence/BlockGraph.h>

using namespace cds_static;
using namespace cds_utils;

uint *loadSequence(const char *filename, long &len){
    ifstream in(filename,ios_base::binary|ios_base::in);
    in.seekg(0, ios_base::end);
    len = in.tellg();
    in.seekg(0, ios_base::beg);
    char *input = new char[len];
    in.read(input, len);
    in.close();
    uint *inp = new uint[len];
    for (auto i=0;i<len;i++){
        inp[i]=(uint)input[i];
    }
    delete [] input;
    MapperCont mp(inp,(size_t)len,BitSequenceBuilderRG(32));
    for (auto i=0;i<len;i++){
        inp[i] = mp.map(inp[i])-1;
    }

    return inp;
}

int testAccess(uint *input, Sequence *seq, size_t len){
    for (size_t i=0;i<len;i++){
        if (seq->access(i)!=input[i]){
            cerr << "Error in access" << endl;
            return 0;
        }
    }
    return 1;
}

int testRank(uint *input, Sequence *seq, size_t len, uint sigma){
    vector<size_t> occs(len,0);
    for (size_t i=0;i<len;i++) {
        occs[input[i]]++;
        seq->rank(input[i], i);
        if (seq->rank(input[i], i) != occs[input[i]]) {
            cerr << "Error in rank" << endl;
            return 0;
        }
    }
    return 1;
}

int main(int argc, char **argv){

    if (argc!=5){
        cerr << "Usage: " << argv[0] << " <input-file> <strategy:BU|TD> <nLevels> <BlockLength|arity>" << endl;
        exit(0);
    }
    long len;
    uint *input;
    uint sigma=-1;
    string inFile(argv[1]);
    input = loadSequence(argv[1], len);
    uint nLevels, arity;
    uint blockSizeAtLeaf;
    nLevels = atoi(argv[3]);
    blockSizeAtLeaf = arity = atoi(argv[4]);

    double t0,t1;
    t0 = getTime();
    StrategySplitBlock *st;
    if (!strcmp(argv[2],"TD")){
        st =new StrategySplitBlockTopDown(nLevels,arity);
    }else if (!strcmp(argv[2],"BU")){
        st = new StrategySplitBlockBottomUp(nLevels,blockSizeAtLeaf);
    }
    BlockGraph<uint> *bg = new SequenceBlockGraph<uint>(st);
    bg->build(inFile,false);
    t1 = getTime();
    cerr << "Construction time: " << t1-t0 << endl;
    cerr << "Size(bps): " << bg->getSize()*8.0/bg->getLenght() << endl;
    uint nqueries = min((uint)bg->getLenght(),1000000u);
    t0= getTime();
    if (!testAccess(input, (SequenceBlockGraph<uint>*)bg , nqueries)){
        delete bg;
        exit(0);
    }

    t1 = getTime();
    cerr << "Access performance: " << (t1-t0)*1.0/(1000000*nqueries) << " microsec/access" << endl;
    t0 = getTime();
    if (!testRank(input, (SequenceBlockGraph<uint>*)bg , nqueries,bg->getSigma())){
        delete bg;
        exit(0);
    }
    t1= getTime();
    cerr << "Rank performance: " << (t1-t0)*1.0/(1000000*nqueries) << " microsec/rank" << endl;
    delete bg;
    exit(0);
}