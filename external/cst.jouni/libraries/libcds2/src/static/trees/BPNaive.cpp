#include <trees/BPNaive.h>

namespace cds_static{

		BPNaive::~BPNaive(){
			delete bs;
		}
		BPNaive::BPNaive():Parentheses(0){
			bs = NULL;
		}

		BPNaive::BPNaive(uint *input, ulong len):Parentheses(len){
			bs = new BitSequenceRG(input,len,16);
		}

		ulong BPNaive::getSize() const{
			return 0;
		}
		
		ulong BPNaive::getExcess(uint pos){
			return pos - 2*bs->rank0(pos)+1;
		}

		ulong BPNaive::fwdExcess(uint s, uint e){
			uint pos = s;
			uint excess = getExcess(pos++);
			while(excess!=e){
				excess+=(bs->access(pos++))?1:-1;
			}
			return pos;
		}

		ulong BPNaive::bwdExcess( uint s, int e){
			uint pos = s;
			uint excess = getExcess(pos--);
			while(excess!=e){
				excess+=(bs->access(pos--))?1:-1;
			}
			return pos;
		}

		ulong BPNaive::rmq( uint s, int t, int &m){
			uint excess = getExcess(s-1);
		    m = excess;
		    uint minPos = s;

		    for (uint i=s;i<=t;i++){
		        excess+=(bs->access(i))?1:-1;
		        if (excess<m){
		            m = excess;
		            minPos = i;
		        }
		    }	
			return minPos;
		}

		ulong BPNaive::depth( uint s){
			return getExcess(s);
		}
		ulong BPNaive::rankOpen( uint s){
			return bs->rank1(s);
		}
		ulong BPNaive::rankClose( uint s){
			return bs->rank0(s);
		}
		ulong BPNaive::selectOpen( uint s){
			return bs->select1(s);
		}
		ulong BPNaive::selectClose( uint s){
			return bs->select0(s);
		}
		ulong BPNaive::root_node(){
			return 0;
		}
		ulong BPNaive::inspect(uint s){
			return bs->access(s);
		}
		ulong BPNaive::findClose(uint s){
			uint excess = getExcess(s);
			if (excess==0) return -1;
			uint target = excess-1;
			
			while(excess!=target && ++s<n){
				excess+=(bs->access(s))?1:-1;
			}
			if (excess==target) return s;
			return -1;
		}
		ulong BPNaive::findOpen(uint s){
			// throw runtime_error ("findOpen not implemented");
			uint excess = getExcess(s);
			if (excess==0) return 0;
			uint target = excess;
			int add = 1;
			do{
				excess+=(bs->access(s))?-1:1;
				s--;
			}while(target!=excess && s>0);

			if (excess==target) return s+1;
			return -1;
		}

		ulong BPNaive::parent(uint s){
			return 0;
		}
		ulong BPNaive::parentClose(uint s){
			return 0;
		}
		ulong BPNaive::enclose(uint s){
			return 0;
		}
		ulong BPNaive::levelAncestor(uint s, uint d){
			return 0;
		}
		ulong BPNaive::lca(uint a, uint b){
			return 0;
		}
		ulong BPNaive::preorderRank(uint s){
			return 0;
		}
		ulong BPNaive::preorderSelect(uint s){
			return 0;
		}
		ulong BPNaive::postorderRank(uint s){
			return 0;
		}
		ulong BPNaive::isLeaf(uint s){
			assert(s<n-2);
			return bs->access(s+1);
		}
		ulong BPNaive::subtreeSize(uint s){
			return 0;
		}
		ulong BPNaive::firstChild(uint s){
			return 0;
		}
	  	ulong BPNaive::nextSibling(uint s){
	  		return 0;
	  	}
	  	ulong BPNaive::isAncestor(uint a, uint b){
	  		return 0;
	  	}

	  	ulong BPNaive::prevSibling(uint x){
	  		return 0;
	  	}

	  	void BPNaive::save(ofstream &fp)const{
	  		uint wr = BPNAIVE_HDR;
	  		saveValue(fp, wr);
	  		saveValue(fp, n);
	  		bs->save(fp);
	  	}

	  	BPNaive *BPNaive::load(ifstream &fp){
	  		BPNaive *ret = NULL;
	  		try{
	  			uint rd = loadValue<uint>(fp);
	  			if (rd!=BPNAIVE_HDR){
	  				abort();
	  			}
	  			ret = new BPNaive();
	  			ret->n = loadValue<ulong>(fp);
	  			ret->bs = BitSequence::load(fp);
	  		}catch(...){
	  			delete ret;
	  			ret = NULL;
	  		}
	  		return ret;
	  	}
	  	

}
