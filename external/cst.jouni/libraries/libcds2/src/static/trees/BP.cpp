#include <trees/BP.h>
#include <stdlib.h>

namespace cds_static{

		/**
			Why to copy the bitmap before constructing the structure? The bp_construct 
			algorithm does not copy the bitmap, thus, if you will need it for a further use
			or whatever (if you will modify it), you need to copy it. del does not mean you 
			are going to modify it but you think you could modify it and you want his BP structure
			still works. (it has the same meaning in all the constructors in this class) 
		*/
			
		BP::BP(uint *input, size_t len, bool del):Parentheses((ulong)len){
			uint *newInput = input;
			if (!del){
				newInput = new uint[uint_len(len,1)];
				for (uint i=0;i<uint_len(len,1);i++)
					newInput[i] = input[i];
			}
			internal_bp =  bp_construct((int) len, (bp_dword*)newInput, 0);
		}

		BP::BP(uint *input, size_t len, uint opt, bool del):Parentheses((ulong)len){
			
			uint *newInput = input;
			if (!del){
				newInput = new uint[uint_len(len,1)];
				for (uint i=0;i<uint_len(len,1);i++)
					newInput[i] = input[i];
			}
			internal_bp =  bp_construct((int) len, (bp_dword*)input, opt);
			
		}

		BP::BP(uint *input, size_t len, bool reversed, bool deleted){

			size_t nb = uint_len(len,1);
			uint *new_input;
			createEmptyBitmap(&new_input,len);

			for (size_t i=0;i<nb;i++){

				uint lim = sizeof(uint)*8/2;
				
				for (uint j=0;j<lim;j++){
					if (bitget(input+i,j)){
						bitset(new_input+i,j);
					}
					if (bitget(input+i,31-j)){
						bitset(new_input+i,31-j);
					}
				}
			}
			if (reversed){
				internal_bp =  bp_construct((int) len, (bp_dword*)input, 0);
			}else{
				internal_bp =  bp_construct((int) len, (bp_dword*)new_input, 0);
			}
		}

		BP::BP():Parentheses(0){
			internal_bp=nullptr;
		}

		BP::~BP(){
			bp_delete(internal_bp);
		}

		void BP::save(ofstream &fp)const{
			uint wr = BP_HDR;
			saveValue(fp, wr);
			saveValue(fp, internal_bp->n);
			saveValue(fp,internal_bp->B,(internal_bp->n + bp_getD()-1)/bp_getD());
			saveValue(fp,internal_bp->opt);
		}
		ulong BP::getSize() const{
			return internal_bp->idx_size+n/8.0;
		}
	  	BP* BP::load(ifstream &fp){
	  		BP *ret = NULL;
	  		try{
				uint rd = loadValue<uint>(fp);
				if (rd!=BP_HDR){
					abort();
				}
				uint n;
				uint opt;
				uint *b;
				n = loadValue<uint>(fp);
				b = loadValue<uint>(fp,(n + bp_getD()-1)/bp_getD());
				opt = loadValue<uint>(fp);
				ret = new BP(b,n,opt);
	  		}catch (...){
	  			delete ret;
	  			ret = NULL;
	  		}
	  		return ret;
	  	}
	  	
		ulong BP::fwdExcess(uint s, uint e){
			return bp_fwd_excess(internal_bp,s,e);
		}
		ulong BP::bwdExcess( uint s, int e){
			return bp_bwd_excess(internal_bp,s,e);
		}
		ulong BP::rmq( uint s, int t, int &m){
			m = -1;
			return bp_rmq(internal_bp, s, t, 0);
		}
		ulong BP::depth( uint s){
			return bp_depth(internal_bp, s);
		}
		ulong BP::rankOpen( uint s){
			return bp_rank_open(internal_bp,s);
		}
		ulong BP::rankClose( uint s){
			return bp_rank_close(internal_bp, s);
		}
		ulong BP::selectOpen( uint s){
			return bp_select_open(internal_bp, s);
		}
		ulong BP::selectClose( uint s){
			return bp_select_close(internal_bp, s);
		}
		ulong BP::root_node(){
			return 0;
		}
		ulong BP::inspect(uint s){
			return bp_inspect(internal_bp, s);
		}
		ulong BP::findClose(uint s){
			return bp_find_close(internal_bp, s);
		}
		ulong BP::findOpen(uint s){
			return bp_find_open(internal_bp, s);
		}
		ulong BP::parent(uint s){
			return bp_parent(internal_bp, s);
		}
		ulong BP::parentClose(uint s){
			return bp_parent_close(internal_bp, s);
		}
		ulong BP::enclose(uint s){
			return bp_enclose(internal_bp, s);
		}
		ulong BP::levelAncestor(uint s, uint d){
			return bp_level_ancestor(internal_bp, s, d);
		}
		ulong BP::lca(uint a, uint b){
			return bp_lca(internal_bp, a, b);
		}
		ulong BP::preorderRank(uint s){
			return bp_preorder_rank(internal_bp, s);
		}
		ulong BP::preorderSelect(uint s){
			return bp_preorder_select(internal_bp,s);
		}
		ulong BP::postorderRank(uint s){
			return bp_postorder_rank(internal_bp, s);
		}
		ulong BP::isLeaf(uint s){
			return bp_isleaf(internal_bp, s);
		}
		ulong BP::subtreeSize(uint s){
			return bp_subtree_size(internal_bp, s);
		}
		ulong BP::firstChild(uint s){
			return bp_first_child(internal_bp, s);
		}
	  	ulong BP::nextSibling(uint s){
	  		return bp_next_sibling(internal_bp, s);
	  	}
	  	ulong BP::isAncestor(uint a, uint b){
	  		return bp_is_ancestor(internal_bp, a,b);
	  	}
	  	ulong BP::degree(uint x){
	  		return bp_degree(internal_bp, x);
	  	}
	  	ulong BP::childRank(uint x){
	  		return bp_child_rank(internal_bp, x);
	  	}
	  	ulong BP::child(uint x, uint i){
	  		return bp_child(internal_bp, x, i);
	  	}	
		ulong BP::prevSibling(uint x){
			return bp_prev_sibling(internal_bp, x);
		}
}
