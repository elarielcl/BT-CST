#include <TextIndexRLCSA.h>

namespace TextIndexes{

    TextIndexRLCSA::TextIndexRLCSA(unsigned char *data, ulong bytes, ulong block_size, ulong sa_sample_rate, bool multiple_sequences, bool delete_data) : TextIndex(bytes){
        this->data = data;
        this->bytes = bytes;
        this->block_size = block_size;
        this->sa_sample_rate = sa_sample_rate;
        this->multiple_sequences = multiple_sequences;
        this->delete_data = delete_data;
        build_index_internal();
    }

    TextIndexRLCSA::TextIndexRLCSA():TextIndex(0) {}

    TextIndexRLCSA::~TextIndexRLCSA() {
        delete rlcsa;
    }

    bool TextIndexRLCSA::build_index_internal() {
        rlcsa = new RLCSA(data,bytes,block_size,sa_sample_rate,multiple_sequences,delete_data);
        return (rlcsa!= nullptr);
    }

    ulong TextIndexRLCSA::getSize() {
        return rlcsa->getSize();
    }

    ulong TextIndexRLCSA::getSizeCount() {
        return rlcsa->reportSizeCount(false);
    }

    uint TextIndexRLCSA::locate(uchar *pattern, uint m, uint **occ) {
        throw new runtime_error("locate for TextIndexRLCSA is not implmented yet");
    }

    ulong TextIndexRLCSA::count(uchar *pattern, ulong m) {
        ulong noccs;
        rlcsa->count(pattern,(ulong)m,&noccs);
        return noccs;
    }

    uchar *TextIndexRLCSA::extract(ulong i, ulong j) {
        throw new runtime_error("extract for TextIndexRLCSA is not implmented yet");
    }

    uchar TextIndexRLCSA::getT(ulong i) {
        return rlcsa->getT(i);
    }

    ulong TextIndexRLCSA::getISA (ulong pos){
        return rlcsa->getISA(pos);
    }

    RLCSA *TextIndexRLCSA::getRLCSA(){
        return rlcsa;
    }

    void TextIndexRLCSA::save(char *filename) const {
        rlcsa->writeTo(filename);
        struct stat sdata;
        if (stat(filename,&sdata) < 0) {
            system(("touch "+std::string(filename)).c_str());
            stat(filename,&sdata);
        }

        char *data = new char[sdata.st_size];
        ifstream fp(filename);
        fp.read(data, sdata.st_size);
        fp.close();

        uint w = RLCSA_HDR;
        ofstream out(filename);
        out.write((char*)&w,sizeof(uint));
        out.write(data,sdata.st_size);
        out.close();

        delete [] data;
    }


    TextIndex *TextIndexRLCSA::load(const char *filename) {
        cerr << "loading :" << filename << endl;
        /*
        struct stat sdata;
        stat(filename,&sdata);
        uint w;
        ifstream in(filename);
        in.read((char*)&w,sizeof(uint));
        if (w!= RLCSA_HDR){
            in.close();
            return nullptr;
        }
         */
        /*
        char *data = new char[sdata.st_size-sizeof(uint)];
        in.read(data,sdata.st_size-sizeof(uint));
        in.close();

        ofstream out(filename);
        out.write(data,sdata.st_size-sizeof(uint));
        out.close();
        delete [] data;
        */
        TextIndexRLCSA *ret = new TextIndexRLCSA();
        ret->rlcsa = RLCSA::load(filename);
        return ret;
    }


    ulong TextIndexRLCSA::getPsi(ulong pos) {
        return rlcsa->getPsi(pos);
    }

    ulong TextIndexRLCSA::getPsi (ulong pos, ulong k){
        return rlcsa->getPsi(pos,k);
    }

    uchar TextIndexRLCSA::getBWT(ulong pos) {
        return 0;
    }

    ulong TextIndexRLCSA::getSA(ulong pos){
        return rlcsa->getSA(pos);
    }
}