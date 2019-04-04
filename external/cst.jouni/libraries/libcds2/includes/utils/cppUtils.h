
#ifndef _CPPUTILS
#define _CPPUTILS

#include <utils/libcdsBasics.h>
#include <utils/cppUtils.h>
#include <cstdio>

#include <vector>
using namespace std;

namespace cds_utils
{

#ifndef FILE_DESCRIPTORS_LOCATION
#define FILE_DESCRIPTORS_LOCATION "/proc/self/fd/"
#endif

	uint transform(const string & s);

	template <typename T> void saveValue(ofstream &out, const T val) {
		assert(out.good());
		out.write((char*)&val,sizeof(T));
	}
	
	template <typename T> void saveValue(FILE &out, const T val) {
		size_t w = fwrite((char*)&val,sizeof(T),1,&out);
		assert(w==sizeof(T));
	}

	template <typename T> T loadValue(ifstream & in) {
		assert(in.good());
		T ret;
		in.read((char*)&ret,sizeof(T));
		return ret;
	}

	template <typename T> T loadValue(FILE &in) {
		T ret;
		uint r = fread((char*)&ret, sizeof(T), 1, &in);
		assert(r==sizeof(T));
		return ret;
	}

	template <typename T> void saveValue(ofstream & out, const T * val, const size_t len) {
		assert(out.good());
		out.write((char*)val,len*sizeof(T));
	}

	template <typename T> void saveValue(FILE &out, const T * val, const size_t len) {
		uint w = fwrite((char*)val,len*sizeof(T),1,&out);
		assert(w==len*sizeof(T));
	}

	template <typename T> T * loadValue(ifstream & in, const size_t len) {
		assert(in.good());
		T * ret = new T[len];
		in.read((char*)ret,len*sizeof(T));
		return ret;
	}

	template <typename T> T * loadValue(FILE &in, const size_t len) {
		T * ret = new T[len];
		int r = fread((char*)ret,len*sizeof(T),1,&in);
		assert(r==len*sizeof(T));
		return ret;
	}

	template <typename T> T * loadValue(const char * name, size_t & slen) {
		ifstream in(name);
		assert(in.good());
		in.seekg(0,ios_base::end);
		size_t len = in.tellg();
		slen = len/sizeof(T);
		if(len%sizeof(T)) slen--;
		in.seekg(0,ios_base::beg);
		T * ret = loadValue<T>(in,slen);
		in.close();
		return ret;
	}

	ofstream * ofstreamFromFd(uint fd);
	ifstream * ifstreamFromFd(uint fd);

	void tokenize(string str, vector<string> &tokens, char delim);


    template<typename T> vector<T> loadVector(ifstream &fp){
        ulong n = loadValue<ulong>(fp);
        T* arr = loadValue<T>(fp,(size_t)n);
        vector<T> res;
        for (ulong i=0;i<n;i++)
            res.push_back(arr[i]);
        delete [] arr;
        return res;
    }
};
#endif
