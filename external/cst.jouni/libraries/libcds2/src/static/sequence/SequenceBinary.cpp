#include <sequence/SequenceBinary.h>

namespace cds_static {

    SequenceBinary::SequenceBinary(uint *input, size_t len, BitSequenceBuilder *bsb) : Sequence(len) {
        for (size_t i = 0; i < len; i++) {
            if (input[i] > 1) {
                throw runtime_error("The input must be binary");
            }
        }
        bs = bsb->build(input, len);
    }

    SequenceBinary::SequenceBinary(const Array &values, BitSequenceBuilder *bsb) : Sequence(values.getLength()) {
        throw runtime_error("Not implemented yet");
        bs = nullptr;
    }

    SequenceBinary::~SequenceBinary(){
        delete bs;
    }

    SequenceBinary::SequenceBinary():Sequence(0){}


    size_t SequenceBinary::rank(uint c, size_t i) const {
        if (c == 0) {
            return bs->rank0(i);
        } else if (c == 1) {
            return bs->rank1(i);
        }
        return -1;
    }

    size_t SequenceBinary::select(uint c, size_t j) const {
        if (c==0){
            return bs->select0(j);
        }else if (c==1){
            return bs->select1(j);
        }
        return -1;
    }



    uint SequenceBinary::access(size_t i) const {
        return bs->access(i);
    }

    size_t SequenceBinary::getSize() const{
        return sizeof(this) + bs->getSize();
    }

    void SequenceBinary::save(ofstream &fp) const{
        uint wr = SEQUENCE_BINARY_HDR;
        saveValue(fp, wr);
        saveValue(fp,length);
        bs->save(fp);
    }
    SequenceBinary *SequenceBinary::load(ifstream &fp) {
        uint type = loadValue<uint>(fp);
        if (type!= SEQUENCE_BINARY_HDR){
            return NULL;
        }
        SequenceBinary *ret = new SequenceBinary();
        ret->length = loadValue<size_t>(fp);
        ret->bs = BitSequence::load(fp);
        return ret;
    }

}