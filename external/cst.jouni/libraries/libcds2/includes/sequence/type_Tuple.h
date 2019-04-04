

#ifndef _TYPE_TUPLE_H
#define _TYPE_TUPLE_H

namespace cds_static{
	
	typedef struct stuple{
		uint symbol;
		uint code;
		uint len;
		uint occs;
	}tuple;


	int cmpSymbols(const void *s1, const void *s2){

		return *((uint *)(s1)) > *((uint *)(s2));
	}

	int cmpTupleByCode(const void *t1, const void *t2){
		
		return ((tuple *)(t1))->code > ((tuple *)(t2))->code;
	}

	int cmpTupleBySymbol(const void *t1, const void *t2){
		
		return ((tuple *)(t1))->symbol > ((tuple *)(t2))->symbol;
	}

	//utility to sort in increasing order
	int cmpTupleByLength(const void *t1, const void *t2){
		
		return ((tuple *)(t1))->len > ((tuple *)(t2))->len;
	}
};

#endif
