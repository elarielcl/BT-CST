#if 0
#ifndef IREPAIR_H
#define IREPAIR_H
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <grammar/basicsRepair.h>
#include <grammar/records.h>
#include <grammar/hash.h>
#include <grammar/heap.h>


namespace cds_static{
	


int PRNC = 0;
int PRNR = 0;
int PRNP = 0;
int PRNL = 0;

int *lengths;//length of each rule

int nterms;//number of terminal symbols

int rule_index;//number of rules (nterms not included)

int *RULES_LEFT;

int *RULES_RIGHT;

float factor = 0.75; // 1/extra space overhead; set closer to 1 for smaller and
		     // slower execution
int minsize = 256; // to avoid many reallocs at small sizes, should be ok as is

int input_len;

int u; // |text| and later current |C| with gaps

int *C; // compressed text

int c;  // real |C|

int alph; // max used terminal symbol

int n; // |R|

Tlist_repair *L; // |L| = c;

Thash_repair Hash; // hash table of pairs

Theap_repair Heap; // special heap of pairs

Trarray_repair Rec; // records

/**
 * Runs the repair (balanced) algorithm on the sequence of chars of length len
 * 
 * */
void runRepair(int *input, uint len);

/**
 * Runs the repair (balanced) algorithm on the sequence of ints of length len
 * 
 * */
void runRepair(char *input, uint len);

/**
 * Deletes all the data structures allocated for irepair
 * 
 * */
void deleteAll();
};
#endif
#endif
