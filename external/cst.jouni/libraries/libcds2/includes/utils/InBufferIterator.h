#ifndef  IN_BUFFER_ITERATOR_H_
#define IN_BUFFER_ITERATOR_H_

#include <fstream>
#include <cassert>
#include <exception>
#include <stdexcept>

using namespace std;
namespace cds_utils {


    template<typename T>
    class InBufferIterator : public iterator<random_access_iterator_tag, T> {

    public:

        InBufferIterator(long bufferSize, string &input, long blockLength);

        ~InBufferIterator();

    private:
//    dataContainer data;
        T *data;
        long numberOfTs;
        long inputSize;
        long bufferSize;
        long blockLength;

        string *inputFile;
        ifstream *file;

        long bufferIndex;
        long globalIndex;

        long readBlockForward();

        long restartIterator();

        long loadBlock(long blockIndex);

    public:

        T operator*() {
            if (!data) readBlockForward();
            return data[bufferIndex];
        }

        /**
    *it may cause a lot of disk-reads if you are not careful since it
    forces the loading of the block if is not already in memory
    */

    T operator[](long pos){
        if (pos<globalIndex || pos>= globalIndex+ bufferSize){
            //if the block currently in memory does not contain the searched position
            //--> force the loading of that block
            loadBlock(pos/bufferSize);
        }
            bufferIndex = pos -globalIndex;
        return data[bufferIndex];
    }

        InBufferIterator&operator++(){
            bufferIndex++;
        if (bufferIndex>= bufferSize){
            readBlockForward();
        }
        return *this;
        }

        InBufferIterator &operator--() {
            bufferIndex--;
            if (bufferIndex < 0) {
                if (globalIndex == 0) {
                    throw new runtime_error("Stop iterating back! There is nothing there!");
                } else {

                }
            }
            return *this;
        }

        InBufferIterator &operator++(int) {
            ++(*this);
        }

        InBufferIterator &operator!=(InBufferIterator &it) {
            return *this != it;
        }

        InBufferIterator &restart() {
            (*this)[0];//force to load the first block
            return *this;
        }

        bool end(){
            return (globalIndex+bufferIndex)>=numberOfTs;
        }

    };

    template<typename T>
    InBufferIterator<T>::InBufferIterator(long bs, string &in, long bl) {
        bufferSize = bs;
        inputFile = new string(in.c_str());
        blockLength = bl;
        assert(blockLength * 2 <= bufferSize);
        file = new ifstream(*inputFile, ios_base::binary);
        if (!(file->good())) {
            throw new runtime_error("Cannot open input file");
        }
        file->seekg(0, ios::end);
        inputSize = file->tellg();
        file->seekg(0, ios::beg);
        assert(inputSize > 0);
        if(inputSize%sizeof(T)){
            throw new runtime_error("The input is not of the required type.");
        }
        numberOfTs=inputSize /sizeof(T);
        bufferIndex = 0;
        globalIndex = 0;
        data = new T[bufferSize];
        if (data==nullptr){
            throw new runtime_error("Cannot allocate memory for input buffer");
        }
        loadBlock(0);
    }


    template<typename T>
    InBufferIterator<T>::~InBufferIterator() {
        file->close();
        delete inputFile;
        if (data) delete[] data;
    }

    template<typename T>
    long InBufferIterator<T>::readBlockForward() {
        if (!data) {
            data = new T[bufferSize];
            auto cur = file->tellg();
            auto toRead = min(bufferSize, (long) inputSize - cur);
            file->read((char *) data, sizeof(T) * toRead);
        } else {
            //copy the last blockLength part of the buffer into the very beginning of the buffer
            for (long i = bufferSize - blockLength, j = 0; i < bufferSize; i++, j++) {
                data[j] = data[i];
            }

            auto cur = file->tellg();
            auto toRead = min(bufferSize - blockLength, (long) inputSize - cur);
            if (toRead>0){
                data += blockLength;
                file->read((char *) data, toRead * sizeof(T));
                data -= blockLength;
                globalIndex += toRead;
                bufferIndex = blockLength;
            }else{
                assert(globalIndex+bufferIndex==numberOfTs);
            }
        }
        return bufferIndex;
    }

    template<typename T>
    long InBufferIterator<T>::restartIterator() {
        if (data == nullptr) return;
        file->seekg(0, ios_base::beg);
        globalIndex = 0;
        bufferIndex = 0;
    }

    template<typename T>
    long InBufferIterator<T>::loadBlock(long blockIndex) {
        if (blockIndex * bufferSize > inputSize) {
            throw new runtime_error("Trying to accessa an invalid position");
        }
        streampos pos = blockIndex * bufferSize * sizeof(T);
        file->seekg(pos);
        auto cur = file->tellg();
        auto toRead = min(bufferSize, (long) inputSize - cur);
        if (!data) data = new T[bufferSize];
        file->read((char *) data, toRead * sizeof(T));
        bufferIndex = 0;
        globalIndex = blockIndex*bufferSize;
        return bufferIndex;
    }
};
#endif