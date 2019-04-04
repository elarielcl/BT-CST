#ifndef BP_DARRAY_H_
#define BP_DARRAY_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "bp-utils.h"

typedef unsigned char bp_byte;
typedef unsigned short bp_word;
typedef unsigned int bp_dword;

#define OPT_NO_RANK (1<<30)


typedef bp_dword bp_pb;


typedef struct {
  int n,m;
  int size;
  bp_pb *buf;
  bp_dword *lp;
  bp_dword *sl;
  bp_word *ss;
  bp_dword *p;

  bp_dword *rl;
  bp_word *rm;
  bp_byte *rs;

  int idx_size;
} darray;


darray * bp_darray_construct(int n, bp_pb *buf,int opt);
void bp_darray_free(darray *da);

int bp_darray_select(darray *da, int i,int f);

static inline int bp_darray_rank(darray *da, int i)
{
  int r,j,i_rr, i_rrr;
  int offset;
  bp_pb *p;
  bp_byte *buff;
  i_rr = i >> bp_logRR;
  i_rrr = i >> bp_logRRR;
  r = da->rl[i>>bp_logR] + da->rm[i_rr];

  j = (i_rrr) & (bp_RR/bp_RRR-1);
  offset = i_rr << (bp_logRR-bp_logRRR);
  buff = &(da->rs[offset-1]);
  while (j > 0) {
    r += buff[j];
    j--;
  }

  p = da->buf + ((i_rrr)<<(bp_logRRR-bp_logD));
  j = i & (bp_RRR-1);
  while (j >= bp_D) {
    r += popcount(*p++);
    j -= bp_D;
  }
  r += popcount(*p >> (bp_D-1-j));

  return r;
}

darray * bp_darray_pat_construct(int n, bp_pb *buf, int k, bp_pb pat, int opt);
int bp_darray_pat_select(darray *da, int i, bp_pb (*getpat)(bp_pb *));
int bp_darray_pat_rank(darray *da, int i, bp_pb (*getpat)(bp_pb *));

int bp_darray_select_bsearch(darray *da, int i, bp_pb (*getpat)(bp_pb *));



#ifdef __cplusplus
}
#endif


#endif
