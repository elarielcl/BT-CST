#include <utils/StringUtils.h>
#include <mapper/Mapper.h>
#include <utils/IfstreamBuffer.h>

namespace cds_utils
{

    void mapToCont(char *input, size_t n, bool firstIs0){
        uint mp[256];
        for (uint i=0;i<256;i++){
            mp[i]=0;
        }
        for (size_t i=0;i<n;i++){
            mp[(int)input[i]]++;
        }
        uint m = (firstIs0)?0:1;
        for (uint i=0;i<256;i++){
            if (mp[i]!=0){
                mp[i]=m;
                m++;
            }
        }

        for (size_t i=0;i<n;i++){
            input[i] = (char)mp[(int)input[i]];
        }
    }


    void mapToCont(uint *input, size_t n, bool firstIs0){
        cds_static::MapperCont *mc = new cds_static::MapperCont(input,n,cds_static::BitSequenceBuilderRG(32));
        uint *new_input;
        uint sub = (firstIs0)?(uint)-1:0;
        for (size_t i=0;i<n;i++){
            new_input[i] = mc->map(input[i])+sub;
        }

    }

	void reverseString(uchar *seq, size_t len){
		size_t lim = len/2;
		for (size_t i=0;i<lim;i++){
			uchar tmp = seq[i];
			seq[i] = seq[len-i-1];
			seq[len-i-1] = tmp;
		}
	}


	void reverseStringAsUint(uint *seq, size_t len){
		size_t lim = len/2;
		for (size_t i=0;i<lim;i++){
			uchar tmp = seq[i];
			seq[i] = seq[len-i-1];
			seq[len-i-1] = tmp;
		}
	}

    uint getMaxV(char *seq, size_t len,uint &ndistinct){
        uint *count = new uint[256];
        for (uint i=0;i<256;i++)
            count[i]=0;
        for (size_t i=0;i<len;i++){
            count[(int)seq[i]]++;
        }
        uint max_v=0;
        ndistinct=0;
        for (size_t i=0;i<256;i++){
            if (count[i]) {
                max_v = i;
                ndistinct++;
            }
        }
        return max_v;
    }


    uint getGaps(uint *seq, size_t len, uint &ngaps, uint *&gaps){
		uint max_v = getMaxV(seq, len);
		uchar *g = new uchar[max_v+1];
		for (uint i=0;i<=max_v;i++)
			g[i]=0;
		
		for (size_t i=0;i<len;i++){
			g[seq[i]]=1;
		}
		ngaps=0;
		for (uint i=0;i<=max_v;i++){
			if (g[i]==0) ngaps++;
		}
		if (ngaps==0){
			gaps=nullptr;
			return 0;
		}
		gaps = new uint[ngaps];
		ngaps=0;
		for (uint i=0;i<=max_v;i++){
			if (g[i]==0){
				gaps[ngaps++]=i;
			}
		}
		return ngaps;
	}

	uint superSampleVector(uint **samples, uint nsamples, uint max_v, uint **&ssamples, uint &nssamples, uint ss){
		
		
		nssamples = (nsamples) / ss +((nsamples%ss)?1:0);

		ssamples = new uint*[nssamples];
		for (uint i=0;i<nssamples;i++){
			ssamples[i]=new uint[max_v+1];
		}
		uint pointerSSamples=0;
		for (uint i=0;i<nsamples;i++){
			if (i%ss==0){
				//if the sample is at a multiple of ss, then we store the absolute value in 
				//ssamples and we set the samples at that position to 0 (as the difference)
				//w.r.t. ssamples
				for (uint j=0;j<=max_v;j++){
					ssamples[i/ss][j]=samples[i][j];
					samples[i][j]=0;
				}
				pointerSSamples++;
			}else{
				// assert(i>0);
				for (uint j=0;j<=max_v;j++){
					samples[i][j]=samples[i][j]-ssamples[i/ss][j];
				}
			}
		}
		assert(pointerSSamples==nssamples);
		
		return nssamples;
	}

	uchar *fibonacciWord(uint length){
   		unsigned char *seq = new unsigned char [length];
	   	seq[0]='0';
	   seq[1]='1';
	   seq[2]='0';
	   uint fin1,fin2,ini1,ini2;
	   fin1=ini1=0;
	   ini2=1;fin2=2;
	   uint pos = 3;
	   while(pos<length){
	      uint ini3,fin3;
	      ini3=pos;
	      for (uint i=ini1;i<=fin1 && pos<length;i++){
	         seq[pos++]=seq[i];
	      }
	      if (pos==length) break;
	      for (uint i=ini2;i<=fin2 && pos<length;i++){
	         seq[pos++]=seq[i];
	      }
	      if (pos==length) break;
	      fin3=pos;
	      ini1=ini2;
	      fin1=fin2;
	      ini2=ini3;
	      fin2=fin3;
	   }
	   return seq;
	}

	//requires initially m>2
	//returns b^{m-1} mod q
	unsigned long long computeH_RKRec(unsigned long long base, unsigned long long m, unsigned long long q){
		if (m<=2)return base;
		return (base%q * (computeH_RK(base, m-1, q)%q) ) %q;
		// return (base * (computeH_RK(base, m-1, q)) ) %q;
	}
		//requires initially m>2
	//returns b^{m-1} mod q
	unsigned long long computeH_RK(unsigned long long base, unsigned long long m, unsigned long long q){

		unsigned long long mprime = 3;
		unsigned long long res=base;
		for (;mprime<=m;mprime++){
			res = ((base%q) * (res%q)) % q;
		}
		return res;
	}

	//returns the largest q s.t. base*q = arch_word_length [32 or 64 bits]
	unsigned long long computeQ_RK(unsigned long long base){
		unsigned long long b = bits64(base);
		uint large_n = sizeof(unsigned long long)*8 - b;
		unsigned long long q = getPrime(large_n);
		unsigned long long t = b * q;
		assert(t>b);
		assert(t>q);
		//assert(bits(t)==64);
		return t;
	}

	uint foldAndCopy(uint **input, size_t len){
		uint *newinput = new uint[len*2];
		for (uint i=0;i<len;i++)
			newinput[i] = (*input)[i];

		delete [] (*input);
		(*input) = newinput;

		return len*2;
	}

	//2^{8+i}-kprime[i] is the largest prime of length 8+i
	size_t getPrime(uint nbits){
		size_t one = 1;
		return (one<<nbits)-kprime[nbits-8];
	}

};
