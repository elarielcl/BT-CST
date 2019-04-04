


#ifndef _STRING_UTILS
#define _STRING_UTILS

#include <utils/libcdsBasics.h>
#include <utils/StringUtils.h>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include <utils/IfstreamBuffer.h>

namespace cds_utils
{

    void mapToCont(char *input, size_t n, bool firstIs0);
    void mapToCont(uint *input, size_t n, bool firstIs0);
//    size_t loadFile(const char *filename, char *&input, size_t &n);
//    size_t loadFile(const char *filename, uint *&input, size_t &n);
	void reverseString(uchar *seq, size_t len);	
	
	void reverseStringAsUint(uint *seq, size_t len);

	template<typename  T> T getMaxV(T *seq, size_t len){
		assert(len>0);
		T max_v=seq[0];
		for (size_t i=1;i<len;i++){
			max_v = max(max_v,seq[i]);
		}
		return max_v;
	}

    uint getMaxV(char *seq, size_t len,uint &ndistinct);

	uint getGaps(uint *seq, size_t len, uint &ngaps, uint *&gaps);

	uint superSampleVector(uint **samples, uint nsamples, uint max_v, 
			uint **&ssamples, uint &nssamples, uint ss);

	uchar *fibonacciWord(uint length);

	/**
	input: input sequence
	len: block length
	base: base number of the Rabin-Karp
	modulo: return hash % modulo
	*/
	//the same than the previous but returns the hash without computing it modulo "modulo"
	template<typename T> size_t rabinKarpBlock(T *input, size_t ini, size_t end, size_t base){
		assert(base);
		assert(end>=ini);
		size_t hash_block=0;
		for (auto i=ini;i<=end;i++){
			hash_block = hash_block*base+(size_t)input[i];
		}
		return hash_block;
	}
    template<typename T> size_t rabinKarpBlockVerbatim(T &input, size_t ini, size_t end, size_t base, size_t modulo){
        assert(modulo);
        assert(base);
        assert(ini<=end);
        size_t hash_block=0;

        for (auto i=ini;i<=end;i++){
            // hash_block = (hash_block*base+input[i]);
            //hash_block*base is guaranteed <= 32/64 bits
            //since hash_block is < q and base*q < 32/64 bits
//			hash_block = ((hash_block*base)%modulo +(((size_t)input[i])%modulo))%modulo;
            hash_block = ((hash_block*base)%modulo +(size_t)input[i])%modulo;
            cerr << hash_block<<endl;
        }cerr << endl;
        return hash_block;
    }
	template<typename T> size_t rabinKarpBlock(T &input, size_t ini, size_t end, size_t base, size_t modulo){
		assert(modulo);
		assert(base);
		assert(ini<=end);
		size_t hash_block=0;

		for (auto i=ini;i<=end;i++){
			// hash_block = (hash_block*base+input[i]);
			//hash_block*base is guaranteed <= 32/64 bits
			//since hash_block is < q and base*q < 32/64 bits
			hash_block = ((hash_block*base)%modulo +(((size_t)input[i])%modulo))%modulo;
//            hash_block = ((hash_block*base)%modulo +(size_t)input[i])%modulo;
		}
		return hash_block;
	}

	//h = base^(window_length-1) % modulo
	template <typename T> size_t rabinKarpRoll( T leaving, T comming, size_t hash_block, size_t h, size_t base){
		return (base*(hash_block-leaving * h) + comming);
	}

	/*
		The Rabin-Karp hash rolling function acts like a sliding window. At each iteration, an element is
		leaving the window while another is comming in. hash_block is the Rabin-Karp hash computed for
		the previous window. h is base^(block_length-1)%modulo.
	*/
	template <typename T> size_t rabinKarpRoll( T leaving, T comming, size_t hash_block, size_t h, size_t base, size_t modulo){
		long long hbl,hl,bl, leavingl;
		hbl = (long long )hash_block;
		hl = (long long )h;
		bl = (long long )base;
//		ml = (long long )modulo;
		leavingl = (long long) leaving;
		//return (bl*(hbl-((((long long)leaving%ml) * hl)%ml)) + (long long)comming)%ml;
		long long tmp = hbl - ((leavingl*hl)%modulo);
		if (tmp<0) tmp = modulo + tmp;
		return (bl*(tmp) + comming) % modulo;
	}

	uint foldAndCopy(uint **input, uint len);
	size_t getPrime(uint nbits);

	unsigned long long computeH_RKRec(unsigned long long base, unsigned long long m, unsigned long long q);
	unsigned long long computeH_RK(unsigned long long base, unsigned long long m, unsigned long long q);
	unsigned long long computeQ_RK(unsigned long long base);
	
	//2^{8+i}-kprime[i] is the largest prime of length 8+i
	const size_t kprime[57] = {5,3,3,9,3,1,3,19,15,1,5,1,3,9,3,15,3,39,5,39,57,3,35,1,5,9,41,31,5,25,45,7,87,21,11,57,17,55,21,115,59,81,27,129,47,111,33,55,5,13,27,55,93,1,57,25,59};

	template<typename T> long loadFile(string &inFile, T *& input , long &n) throw(){
		ifstream fp(inFile);
		if (!fp.good()){
			throw new runtime_error("The given input file"+inFile+" is not good.");
		}
		fp.seekg(0,ios_base::end);
		auto len = fp.tellg();
		fp.seekg(0,ios_base::beg);
		if (len%sizeof(T)!=0){
			throw new runtime_error("The input file is not valid (It does not contain an integral number of elements)");
		}
		n = len/sizeof(T);
		input = new T[n];
		fp.read((char*)input, len);
		return n;
	}
};
#endif
