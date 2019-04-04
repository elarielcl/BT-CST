#include <stdio.h>
#include <trees/bp-utils.h>

unsigned char popCount[] = {
  0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
};


static size_t allocated = 0;

void * bp_malloc(size_t n)
{
  void * res = malloc(n);
  if (!res) {
    fprintf(stderr, __FILE__  ": failure to allocate %lu bytes\n", n);
    exit(1);
  };
  allocated += n;
  return res;
}

void bp_free(void * p)
{
  if (p) free(p);
}
size_t bp_get_alloc_stats(void)
{
  return allocated;
}
void bp_reset_alloc_states(void)
{
  allocated = 0;
}

int bp_setbit(unsigned int *B, int i,int x)
{
  int j,l;

  j = i / bp_D;
  l = i % bp_D;
  if (x==0) B[j] &= (~(1<<(bp_D-1-l)));
  else if (x==1) B[j] |= (1<<(bp_D-1-l));
  else {
    printf("error setbit x=%d\n",x);
    exit(1);
  }
  return x;
}

int bp_setbits(unsigned int *B, int i, int d, int x)
{
  int j;

  for (j=0; j<d; j++) {
    bp_setbit(B,i+j,(x>>(d-j-1))&1);
  }
  return x;
}
