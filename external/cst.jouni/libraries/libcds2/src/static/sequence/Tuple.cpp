

#include <sequence/Tuple.h>

namespace cds_static{
	
	Tuple::Tuple(){}
	
	Tuple::Tuple(uint symbol, uint code, uint len, uint occs){
		this->symbol = symbol; 
		this->code = code; 
		this->len = len; 
		this->occs = occs; 
	}
	
	//~ int Tuple::cmpSymbols(const void *s1, const void *s2){
			//~ return *((uint *)(s1)) > *((uint *)(s2));
	//~ }
	//~ 
	//~ bool Tuple::cmpTupleByCode(const void *t1, const void *t2){
		//~ 
		//~ return ((Tuple *)(t1))->getCode() > ((Tuple *)(t2))->getCode();
	//~ }
//~ 
	//~ int Tuple::cmpTupleBySymbol(const void *t1, const void *t2){
		//~ 
		//~ return ((Tuple *)(t1))->getSymbol() > ((Tuple *)(t2))->getSymbol();
	//~ }
//~ 
	//~ int Tuple::cmpTupleByLength(const void *t1, const void *t2){
		//~ 
		//~ return ((Tuple *)(t1))->getLen() > ((Tuple *)(t2))->getLen();
	//~ }
	
}
