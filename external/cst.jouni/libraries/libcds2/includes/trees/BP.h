    
#ifndef _HEADER_BP_H
#define _HEADER_BP_H
#include <stdlib.h>
#include <trees/Parentheses.h>
#include <trees/bp.h>
namespace cds_static{

	class BP: public Parentheses{
	  public:
	  	/**
			The bitmap input is in a weird format. The first bit has to be 
			at position 31 instead of at position 0. That is, each bitmap's word
			(each block of 32 bits) has to be reverse order!! That is because bp_bitget at 
			bp-utils.h works in that way (i do not know why and i do not want to 
			change it because i do not know what side efects could yield in the applications
			that uses this implementation (sxsi, for instance))
			TODO solve this (maybe changing the function bp_bitget and cross the fingers). At least 
			in rmq and in findclose it seems to work but i didn't carry out an exhaustive check. 
	  	*/
		BP(uint *input, size_t len, bool deleted=false);
		BP(uint *input, size_t len, uint opt, bool deleted=false);
		
		BP(uint *input, size_t len, bool reversed, bool deleted=false);
		virtual ~BP();
		virtual ulong fwdExcess(uint s, uint e);
		virtual ulong bwdExcess( uint s, int e);
		virtual ulong rmq( uint s, int t, int &m);
		virtual ulong depth( uint s);
		virtual ulong rankOpen( uint s);
		virtual ulong rankClose( uint s);
		virtual ulong selectOpen( uint s);
		virtual ulong selectClose( uint s);
		virtual ulong root_node();
		virtual ulong inspect(uint s);
		virtual ulong findClose(uint s);
		virtual ulong findOpen(uint s);
		virtual ulong parent(uint s);
		virtual ulong parentClose(uint s);
		virtual ulong enclose(uint s);
		virtual ulong levelAncestor(uint s, uint d);
		virtual ulong lca(uint a, uint b);
		virtual ulong preorderRank(uint s);
		virtual ulong preorderSelect(uint s);
		virtual ulong postorderRank(uint s);
		virtual ulong isLeaf(uint s);
		virtual ulong subtreeSize(uint s);
		virtual ulong firstChild(uint s);
	  	virtual ulong nextSibling(uint s);
	  	virtual ulong isAncestor(uint a, uint b);
		virtual ulong getSize() const;
		virtual ulong degree(uint x);
	  	virtual ulong childRank(uint x);
	  	virtual ulong child(uint x, uint i);
		virtual ulong prevSibling(uint x);
	  	
	  	virtual void save(ofstream &fp)const;
	  	static BP* load(ifstream &fp);

	  protected:
	  	BP();
	  	bp *internal_bp;

	  };
};
#endif
