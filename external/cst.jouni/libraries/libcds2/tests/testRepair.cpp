#include <sequence/Sequence.h>
#include <utils/libcdsBasics.h>
#include <cassert>

using namespace cds_static;
using namespace cds_utils;


int main(int argc, char **argv) {
    if (argc<2){
        cerr << "usage: " << argv[0] << "<input_file>" << endl;
        exit(0);
    }
    char *input_text;
    size_t len;
    struct stat sdata;
    stat(argv[1],&sdata);
    len = sdata.st_size;
    input_text = new char[len];
    ifstream in(argv[1]);

        in.read(input_text,len);
    in.close();

    uint *input = new uint[len];
    for (size_t i=0;i<len;i++)
        input[i] = (uint)input_text[i];
    MapperCont *mc = new MapperCont(input,len,BitSequenceBuilderRG(32));
    for (size_t i=0;i<len;i++)
        input[i] = mc->map(input[i])-1;
    delete mc;
    delete []input_text;

    Repair *rp = new Repair((int*)input,(uint)len);
    assert(rp->testRepair((uint*)input));
    ofstream out("salida.bin");
    rp->save(out);
    out.close();

    ifstream ins("salida.bin");
    rp = Repair::load(ins);
    ins.close();
    assert(rp->testRepair((uint*)input));

    cerr << "Size(bps): " << rp->getSize()*8.0 / len << endl;
    //delete [] input_text;
    delete rp;
}
