

#ifndef _TYPE_TUPLE_H
#define _TYPE_TUPLE_H
#include <sys/types.h>

namespace cds_utils{
	
	typedef struct stuple{
		uint symbol;
		uint code;
		uint len;
		uint occs;
	}tuple;


	int cmpSymbols(const void *s1, const void *s2);

	int cmpTupleByCode(const void *t1, const void *t2);

	int cmpTupleBySymbol(const void *t1, const void *t2);

	//utility to sort in increasing order
	int cmpTupleByLength(const void *t1, const void *t2);
	
	int cmpTupleByLengthAndSymbol(const void *t1, const void *t2);
};

#endif
