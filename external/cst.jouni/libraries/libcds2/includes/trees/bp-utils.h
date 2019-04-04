#ifndef BP_UTILS_H_
#define BP_UTILS_H_

#ifdef __cplusplus

extern "C" {
#endif

#ifndef W_BP
#define W_BP 32
#endif
#define bp_logD 5
#define bp_D (1<<bp_logD)

#define bp_logR 16
#define bp_R1 (1<<bp_logR)
#define bp_logRR 10
//#define bp_logRR 8
#define bp_RR (1<<bp_logRR)
#define bp_logRRR 7
#define bp_RRR (1<<bp_logRRR)


#include <stdlib.h>

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

#ifdef HAS_NATIVE_POPCOUNT
static inline UNUSED unsigned int popcount(unsigned int n){
  return __builtin_popcount(n);
}

static inline UNUSED unsigned int popcount8(unsigned int n) {
  return popcount(n & 0xff);
}

#else
#ifdef HAS_POPCOUNT_TABLE

extern unsigned char popCount[256];

static UNUSED unsigned int popcount8(unsigned int x)
{
  return (unsigned int) popCount[x & 0xff];
}

static UNUSED unsigned int popcount(unsigned int x)
{
  return popcount8(x) +
    popcount8((x >> 8)) +
    popcount8((x >> 16)) +
    popcount8((x >> 24));
}


#else
static UNUSED unsigned int popcount8(unsigned int x)
{
  unsigned int r;
  r = x;
  r = ((r & 0xaa)>>1) + (r & 0x55);
  r = ((r & 0xcc)>>2) + (r & 0x33);
  r = ((r>>4) + r) & 0x0f;
  return r;
}

static inline UNUSED unsigned int popcount(unsigned int x)
{
  unsigned int m1 = 0x55555555;
  unsigned int m2 = 0xc30c30c3;
  x -= (x >> 1) & m1;
  x = (x & m2) + ((x >> 2) & m2) + ((x >> 4) & m2);
  x += x >> 6;
  return  (x + (x >> 12) + (x >> 24)) & 0x3f;
}

#endif
#endif


void * bp_malloc(size_t);

#define bp_xmalloc(p, n) p = (__typeof__(p)) bp_malloc((n)*sizeof(*p))



void bp_free(void *);

size_t bp_get_alloc_stats(void);

void bp_reset_alloc_states(void);

int bp_setbit(unsigned int *B, int i,int x);

int bp_setbits(unsigned int *B, int i, int d, int x);

static inline int bp_getbit(unsigned int *B, int i)
{
  int j = i >> bp_logD;
  int l = i & (bp_D-1);
  return (B[j] >> (bp_D-1-l)) & 1;
  //bitget: //return (B[i/W_BP]>>((i%W_BP)&1));
  
}

#ifdef __cplusplus
 }
#endif

#endif
