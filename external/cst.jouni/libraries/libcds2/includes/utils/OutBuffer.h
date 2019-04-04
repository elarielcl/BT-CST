#ifndef  OUT_BUFFER_H_
#define OUT_BUFFER_H_

#include <fstream>
#include <cassert>
#include <exception>
#include <stdexcept>

using namespace std;
namespace cds_utils {


    template<typename T>
    class OutBuffer{

    public:
        /**
        * It uses an internal buffer which is save to disk whenever it is full. It shows an EAGER
        * performance since the memory for the buffer is allocated in the constructor, not the first
        * time it is used. This will avoid checking whether we have a buffer available when we have to
        * write something down on it.
        *
        * @param bufferSize: writing buffer size (the memory allocated is bufferSize*sizeof(T)
        * @param output: filename where we want to write the output
        */
        OutBuffer(long bufferSize, string &output);
        ~OutBuffer();

        /**
        * Adds an element t to the back of the buffer
        */
        void push_back(const T &t);

        /**
        * Forces the buffer to be written on disk (if it has something in it)
        */
        void flush();
    private:
        T *data;
        long bufferSize;
        string *outFile;
        ofstream *file;
        long bufferIndex;
        long globalIndex;
        long writeBlock();
        void close();
    };

    template<typename T>
    OutBuffer<T>::OutBuffer(long bs, string &out) {
        bufferSize = bs;
        outFile = new string(out);
        file = new ofstream(*outFile, ios_base::binary);
        if (!(file->good())) {
            throw new runtime_error("Cannot open output file "+ (*outFile));
        }
        data = new T[bufferSize];
        if (!data){
            throw new runtime_error("Cannot allocate memory for OutBuffer");
        }
        bufferIndex = 0;
        globalIndex = 0;
    }


    template<typename T>
    OutBuffer<T>::~OutBuffer() {
        close();
    }

    template<typename T>
    long OutBuffer<T>::writeBlock() {
        if (bufferIndex>0) {
            file->write((char *) data, bufferIndex * sizeof(T));
            globalIndex += bufferIndex;
            bufferIndex = 0;
        }
        return bufferSize;
    }

    template<typename T>
    void OutBuffer<T>::push_back(const T &t) {
        if (bufferIndex>=bufferSize){
            writeBlock();
        }
        data[bufferIndex++]=t;
    }

    template <typename T> void OutBuffer<T>::close(){
        cerr << "Out buffer closing and flushing: " << bufferIndex*sizeof(T) << " bytes" << endl;
        if (bufferIndex>0) flush();
        file->close();
        if (data) delete[] data;

        ifstream tmp(*outFile);
        if (!tmp.good()){
            cerr << "I cannot read the file that i've just wrote (" << outFile<< ")" << endl;
        }
        tmp.close();
        delete outFile;
    }

    template <typename T> void OutBuffer<T>::flush(){
        writeBlock();
    }

};
#endif