/* IfstreamBuffer.h
   Copyright (C) 2015, Alberto Ordóñez, all rights reserved.

   Implements an input buffer to read from disk. It is said it implements
   a random_access_iterator but that was true until I lost the version
   that does that in a disk crash. Now, it is just a forward iterator
   where you can use the operator [] if you read a position that is
   actually buffered or in a forward position. It is implemented as an
   EAGER buffer: when created loads the first block into main memory.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef  IN_BUFFER_ITERATOR_H_
#define IN_BUFFER_ITERATOR_H_

#include <fstream>
#include <cassert>
#include <exception>
#include <stdexcept>

using namespace std;
namespace cds_utils {


    template<typename T>
    class IfstreamBuffer : public iterator<random_access_iterator_tag, T> {
    public:
        IfstreamBuffer(long bs, string &in, long bl=0l){

            inputFile = new string(in.c_str());
            file = new ifstream(*inputFile, ios_base::binary);
            if (!(file->good())) {

                throw new runtime_error("Cannot open input file"+in);
            }
            file->seekg(0, ios::end);
            inputSize = file->tellg();
            file->seekg(0, ios::beg);
            assert(inputSize > 0);
            if(inputSize%sizeof(T)){
                throw new runtime_error("The input is not of the required type.");
            }
            numberOfTs=inputSize /sizeof(T);

            this->bufferSize = bs;
            this->blockLength = bl;
            this->bufferIndex = 0;
            this->globalIndex = 0;
            this->data = new T[this->bufferSize];
            if (this->data==nullptr){
                throw new runtime_error("Cannot allocate memory for input buffer");
            }
            loadBlock(0);
        }

        ulong size(){
            return numberOfTs;
        }
        /**
        * Returns the length of the input stream in bytes.
        */
        ulong getLength(){
            return inputSize;
        }

        void close(){
            if (file){
                file->close();
                delete file;
                file = nullptr;
            }
            if (data){
                delete [] data;
                data = nullptr;
            }
            if (inputFile){
                delete inputFile;
                inputFile = nullptr;
            }
        }



        virtual ~IfstreamBuffer(){
            close();
        }

    protected:
        IfstreamBuffer(string &in);

        T *data;
        long numberOfTs;
        long inputSize;
        long bufferSize;
        string *inputFile;
        ifstream *file;
        long bufferIndex;
        long blockLength;
        long bufferUpperLimit;
        long globalIndex;

        long readBlockForward();

        long restartIterator();

        long loadBlock(long blockIndex);

    public:

        virtual void printSettings(){
            if (inputFile) cerr << "InputBufferName: " <<   *inputFile << endl;
            if (file){
                if (file->good()){
                    cerr << "The input file is good." << endl;
                }else{
                    cerr << "The input file is not good" << endl;
                }
            }
            cerr << "inputSize: " << inputSize << endl;
            cerr << "bufferSize: " << bufferSize << endl;
        }

        virtual T operator*() {
            return this->data[this->bufferIndex];
        }
        /**
        *it may cause a lot of disk-reads if you are not careful since it
        forces the loading of the block if is not already in memory
        */
        virtual T operator[](long pos){
//            if (pos<this->globalIndex || pos>= this->bufferUpperLimit){
            if (pos<this->globalIndex ||pos>= this->bufferUpperLimit){
                //if the block currently in memory does not contain the searched position
                //--> force the loading of that block
//                loadBlock(pos/this->bufferSize);
//                if (pos==this->bufferUpperLimit) {
//                    this->bufferIndex++;
//                    readBlockForward();
//                }else
                    loadBlock(pos);
            }
            this->bufferIndex = pos -this->globalIndex;
            return this->data[this->bufferIndex];
        }

        virtual IfstreamBuffer&operator++(){
            this->bufferIndex++;
            if (this->bufferIndex>= this->bufferSize){
                readBlockForward();
            }
            return *this;
        }

        virtual    IfstreamBuffer &operator--() {
            this->bufferIndex--;
            if (this->bufferIndex < 0) {
                if (this->globalIndex == 0) {
                    throw new runtime_error("Stop iterating back! There is nothing there!");
                } else {

                }
            }
            return *this;
        }

        virtual  IfstreamBuffer &operator++(int) {
            return ++(*this);
        }

        virtual  IfstreamBuffer &operator!=(IfstreamBuffer &it) {
            return *this != it;
        }

        virtual IfstreamBuffer &restart() {
            (*this)[0];//force to load the first block
            return *this;
        }

        virtual bool end(){
            return (this->globalIndex+this->bufferIndex)>=this->numberOfTs;
        }
    };


    /****************************************************************************************/
    template <typename T> IfstreamBuffer<T>::IfstreamBuffer(string &in){
        inputFile = new string(in.c_str());
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

        this->bufferSize  = 0;
        this->blockLength = 0;
        this->bufferIndex = 0;
        this->globalIndex = 0;
    }

    template<typename T>
    long IfstreamBuffer<T>::readBlockForward() {
//        if (!this->data) {
//            If we wan to implement the buffer as LAZY, we may uncomment this lines (among
//            other others).
//            this->data = new T[this->bufferSize];
//            auto cur = this->file->tellg();
//            auto toRead = min(this->bufferSize, (long) this->inputSize - cur);
//            this->file->read((char *) this->data, sizeof(T) * toRead);
//        } else {
        //copy the last blockLength part of the buffer into the very beginning of the buffer
        for (long i = this->bufferSize - this->blockLength, j = 0; i < this->bufferSize; i++, j++) {
            this->data[j] = this->data[i];
        }

        auto cur = this->file->tellg();
        auto toRead = min(this->bufferSize - this->blockLength, (long) this->inputSize - cur);
        if (toRead>0){
            this->data += this->blockLength;
            this->file->read((char *) this->data, toRead * sizeof(T));
            this->data -= this->blockLength;
            this->globalIndex += this->bufferIndex-this->blockLength;
            this->bufferIndex = this->blockLength;
            this->bufferUpperLimit += toRead;
        }
//        else{
//            assert(this->globalIndex+this->bufferIndex==this->numberOfTs);
//        }
//        }
        return this->bufferIndex;
    }

    template<typename T>
    long IfstreamBuffer<T>::restartIterator() {
        if (this->data == nullptr) return 0;
        this->file->seekg(0, ios_base::beg);
        this->globalIndex = 0;
        this-> bufferIndex = 0;
    }

    template<typename T>
    long IfstreamBuffer<T>::loadBlock(long position) {

        if (position>=numberOfTs) {
            throw new runtime_error("Trying to accessa an invalid position");
        }
        position = max(0L,position-this->blockLength);
        streampos pos = position * sizeof(T);
        //if we have reached the end of the file, eofbit is set and seekg
        //becomes irresponsive
        this->file->clear();
        this->file->seekg(pos);
        auto cur = this->file->tellg();
        auto toRead = min(this->bufferSize, (long) this->inputSize - cur);
        if (!this->data) this->data = new T[this->bufferSize];
        this->file->read((char *) this->data, toRead * sizeof(T));
        this->bufferIndex = 0;
        this->globalIndex = position;
        this->bufferUpperLimit = min(this->globalIndex+this->bufferSize,this->numberOfTs);
        return this->bufferIndex;
    }


    /**
    * Given a file, it implements an input buffer. It loads the whole content of
    * the input file in memory and then offer the typical operations of a random iterator
    */
    template<typename T> class AllInMemoryBuffer : public IfstreamBuffer<T> {

    public:
        AllInMemoryBuffer(string &f) : IfstreamBuffer<T>(f){
            this->bufferSize = this->inputSize;
            this->data = new T[this->bufferSize];
            this->file->read((char*) this->data, this->inputSize);
        }

        virtual T operator*() {
            return this->data[this->bufferIndex];
        }

        virtual T operator[](long pos){
            this->bufferIndex = pos;
            return this->data[pos];
        }

        virtual  AllInMemoryBuffer&operator++(){
            this->bufferIndex++;
            return *this;
        }

        virtual AllInMemoryBuffer &operator--() {
            this->bufferIndex--;
            return *this;
        }

        virtual AllInMemoryBuffer &operator++(int) {
            ++(*this);
            return *this;
        }

        virtual AllInMemoryBuffer &operator!=(AllInMemoryBuffer &it) {
            return *this != it;
        }

        virtual  AllInMemoryBuffer &restart() {
            this->bufferIndex=0;
            return *this;
        }

        virtual bool end(){
            return this->bufferIndex >= this->numberOfTs;
        }
    };

};
#endif