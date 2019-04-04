//
// Created by alberto on 3/17/15.
//

#ifndef _ALL_BITSTREAMITERATOR_H_
#define _ALL_BITSTREAMITERATOR_H_
#include <utils/libcdsBasics.h>

namespace cds_utils {

    class BitStreamIterator {
    public:
        BitStreamIterator(const string &input){
            ifstream fp(input);
            if (fp.good()){
                fp.read((char*)&length,sizeof(length));
                if (static_cast<ulong>(length)!=0UL) {
                    ulong bits_T;
                    ulong n_Ts;
                    bits_T = sizeof(uint) * 8UL;
                    n_Ts = ((size_t) length + bits_T - 1) / bits_T;
                    data = new uint[n_Ts];
                    fp.read((char *) data, n_Ts);
                }
            }
            length = 0UL;
            index = 0UL;
            fp.close();
        }
        BitStreamIterator(uint *&_data, ulong _length){
            this->length = _length;
            this->data = _data;
            _data = nullptr;
            index = 0UL;
        }

        virtual ~BitStreamIterator(){
            delete [] data;
        }

        //shifts out nbits to the right and push the next nbits to the right of out
        virtual void readAndShift(ulong nbits, uint &out){
            uint r = get_var_field(data,index,index+nbits-1);
            out = (r<<(W-nbits)) | (out>>nbits);
            index = index+nbits;
        }

        virtual void reset(){
            index=0UL;
        }
    protected:
        BitStreamIterator(){}
        uint *data;
        ulong length;
        ulong index;
    };

    class BitStreamIteratorDisk: public BitStreamIterator{
    public:
        BitStreamIteratorDisk(const string &input):BitStreamIterator(){
            in_file = new ifstream(input);
            if (in_file->good()){
                init_offset = in_file->tellg();
                data    = new uint[1];
                data[0] = 0U;
                index=-1UL;

            }
            global_index = 0;
        }

        virtual ~BitStreamIteratorDisk(){
            in_file->close();
            delete in_file;
        }

        void readAndShift(ulong nbits, uint &out){
            ulong current,next;
            current = index/W;
            next = (index+nbits) / W;
            if (next!=current){
                uint nb = static_cast<uint>(((index+1+nbits)%W));
                out = data[0] >> nb;
                in_file->read((char*)data,sizeof(uint));
                out = (data[0] << (W-nb)) | out;
            }else {
                uint shift_left = static_cast<uint>(W-((index+1+nbits)%W));
                out = (data[0] << shift_left) | (out>>nbits);
            }
            index = (index+nbits)%W;
            global_index += nbits;
        }

        void reset(){
            //TODO: restart ifstream
            data[0] = 0;
            in_file->clear();
            in_file->seekg(init_offset);
            assert(static_cast<ulong>(in_file->tellg())==static_cast<ulong>(init_offset));
            index = static_cast<ulong>(-1UL);
            global_index = 0UL;
        }

    protected:
        ulong init_offset;
        ifstream *in_file;
        ulong n_uints;
        ulong global_index;
    };

    class BitStreamOutputWriter{
    public:
        BitStreamOutputWriter(const string &input){
            out_file = new ofstream(input,ios_base::binary);
            index = 0u;
            buffer = 0u;
        }

        virtual ~BitStreamOutputWriter(){
            flush();
            out_file->close();
            delete out_file;
        }

        void write(uint code, ulong nbits){
            ulong head = index+nbits;
            if (head>=W){
                buffer = buffer | (code << index);
                out_file->write((char*)&buffer,sizeof(uint));
                buffer = static_cast<uint>(static_cast<ulong>(code) >>(W-index));
                index = head%W;
            }else{
                buffer = buffer | (code << index);
                index=head;
            }
        }

        void flush(){
            if (index>0)
            out_file->write((char*)&buffer,sizeof(uint));
            index=0;
        }

    protected:

        ofstream *out_file;
        ulong index;
        uint buffer;
    };

};
#endif //_ALL_BITSTREAMITERATOR_H_
