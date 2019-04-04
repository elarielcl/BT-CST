#include <stdio.h>
#include <stdlib.h>
#include <trees/bp-darray.h>

#define PBS (sizeof(bp_pb)*8)
#define bp_D (1<<bp_logD)
#define logM 5
#define M (1<<logM)
#define logP 8
#define P (1<<logP)
#define logLL 16    // size of bp_word
#define LL (1<<logLL)
#define logLLL 7
//#define logLLL 5
#define LLL (1<<logLLL)
//#define logL 10
//#define logL (logLL-3)
#define logL (logLL-1-5)
#define L (1<<logL)

#define max(a,b) \
  ({ __typeof__ (a) _a = (a); \
  __typeof__ (b) _b = (b); \
  _a > _b ? _a : _b; })


#define min(a,b) \
  ({ __typeof__ (a) _a = (a); \
  __typeof__ (b) _b = (b); \
   _a <= _b ? _a : _b; })




static bp_dword getbits(bp_pb *B, int i, int d)
{
  bp_dword j,x;

  x = 0;
  for (j=0; j<d; j++) {
    x <<= 1;
    x += bp_getbit(B,i+j);
  }
  return x;
}

static int getpattern(bp_pb *B, int i, int k, bp_pb pat)
{
  int j;
  int x;
  x = 1;
  for (j=0; j<k; j++) {
    x &= bp_getbit(B,i+j) ^ (~(pat>>(k-1-j)));
  }
  //printf("getpattern(%d) = %d\n",i,x);
  return x;
}

int selecttbl[8*256];
static int selecttbl_init = 0;
static void prbin(unsigned int x)
{
  int i;
  //  for(i = 31; i >= 0; i--){
  for (i = 0 ; i < 32; i ++) {
    fprintf(stderr,"%.1u", (x >> i)&1);
    if (i % 4 == 3)
      fprintf(stderr, " ");
  }

}
int clz(unsigned int x)
{
  if (x == 0)
    return -1;
  else
    __builtin_clz(x);
}

static void make_selecttbl(void)
{
  int i,x,r;
  bp_pb buf[1];

  if (selecttbl_init) return;

  selecttbl_init = 1;

  for (x = 0; x < 256; x++) {
    bp_setbits(buf,0,8,x);
    for (r=0; r<8; r++) selecttbl[(r<<8)+x] = -1;
    r = 0;
    for (i=0; i<8; i++) {
      if (bp_getbit(buf,i)) {
	//	fprintf(stderr, "Init: setting %i to %i (r= %i, x = %i)\n", (r<<8)+x, i, r, x);
	selecttbl[(r<<8)+x] = i;
	r++;
      }
    }
  }
  /*
  fprintf(stderr, "Select table:\n");
  for (i = 0; i < 8 * 256; i++){
    mask = i / 256 + 1;
    x = __builtin_clz((i + (i << 8)));
    prbin(i);
    fprintf(stderr, " (%.4i): %08i, %08i\n", i, selecttbl[i], x);
  };
  */
}


darray * bp_darray_construct(int n, bp_pb *buf, int opt)
{
  int i,j,k,m;
  int nl;
  int p,pp;
  int il,is,ml,ms;
  int r,m2;
  int p1,p2,p3,p4,s1,s2,s3,s4;
  darray * da;

  bp_xmalloc(da, 1);

  da->idx_size = 0;

  make_selecttbl();


  if (L/LLL == 0) {
    printf("ERROR: L=%d LLL=%d\n",L,LLL);
    exit(1);
  }

  m = 0;
  for (i=0; i<n; i++) m += bp_getbit(buf,i);
  da->n = n;
  da->m = m;
  //printf("n=%d m=%d\n",n,m);

  da->buf = buf;

  if (opt & (~OPT_NO_RANK)) {  // construct select table

    nl = (m-1) / L + 1;
    bp_xmalloc(da->lp, nl+1);
    da->idx_size += (nl+1) * sizeof(*da->lp);
    bp_xmalloc(da->p, nl+1);
    da->idx_size += (nl+1) * sizeof(*da->p);

    for (r = 0; r < 2; r++) {
      s1 = s2 = s3 = s4 = 0;
      p1 = p2 = p3 = p4 = -1;

      ml = ms = 0;
      for (il = 0; il < nl; il++) {

	while (s1 <= il*L) {
	  if (bp_getbit(buf,p1+1)) s1++;
	  p1++;
	}
	pp = p1;
	da->lp[il] = pp;
	i = min((il+1)*L-1,m-1);

	while (s2 <= i) {
	  if (bp_getbit(buf,p2+1)) s2++;
	  p2++;
	}
	p = p2;

	if (p - pp >= LL) {
	  if (r == 1) {
	    for (is = 0; is < L; is++) {
	      if (il*L+is >= m) break;

	      while (s3 <= il*L+is) {
		if (bp_getbit(buf,p3+1)) s3++;
		p3++;
	      }
	      da->sl[ml*L+is] = p3;
	    }
	  }
	  da->p[il] = -(ml+1);
	  ml++;
	} else {
	  if (r == 1) {
	    for (is = 0; is < L/LLL; is++) {
	      if (il*L+is*LLL >= m) break;
	      while (s4 <= il*L+is*LLL) {
		if (bp_getbit(buf,p4+1)) s4++;
		p4++;
	      }

	      da->ss[ms*(L/LLL)+is] = p4 - pp;
	    }
	  }
	  da->p[il] = ms;
	  ms++;
	}
      }
      if (r == 0) {
	bp_xmalloc(da->sl,ml*L+1);
	da->idx_size += (ml*L+1) * sizeof(*da->sl);
	bp_xmalloc(da->ss, ms*(L/LLL)+1);
	da->idx_size += (ms*(L/LLL)+1) * sizeof(*da->ss);
      }
    }

  } else { // no select table
    da->lp = NULL;
    da->p = da->sl = NULL;
    da->ss = NULL;
  }

  // construct rank table

  if ((opt & OPT_NO_RANK) == 0) {
    bp_xmalloc(da->rl,n/bp_R1+2);
    da->idx_size += (n/bp_R1+2) * sizeof(*da->rl);
    bp_xmalloc(da->rm,n/bp_RR+2);

    da->idx_size += (n/bp_RR+2) * sizeof(*da->rm);

    bp_xmalloc(da->rs,n/bp_RRR+2);
    da->idx_size += (n/bp_RRR+2) * sizeof(*da->rs);

    r = 0;
    for (k=0; k<=n; k+=bp_R1) {
      da->rl[k/bp_R1] = r;
      m2 = 0;
      for (i=0; i<bp_R1; i+=bp_RR) {
	if (k+i <= n) da->rm[(k+i)/bp_RR] = m2;
	m = 0;
	for (j=0; j<bp_RR; j++) {
	  if (k+i+j < n && bp_getbit(buf,k+i+j)==1) m++;
	  if (j % bp_RRR == bp_RRR-1) {
	    if (k+i+j <= n) da->rs[(k+i+j)/bp_RRR] = m;
	    m2 += m;
	    m = 0;
	  }
	}
	if (m != 0) {
	  printf("???\n");
	}
      }
      r += m2;
    }
  }

  return da;
}


void bp_darray_free(darray *da) {

  if (!da) return;
  if (da->buf)  bp_free(da->buf);
  if (da->lp)   bp_free(da->lp);
  if (da->sl)   bp_free(da->sl);
  if (da->ss)   bp_free(da->ss);
  if (da->p)    bp_free(da->p);
  if (da->rl)   bp_free(da->rl);
  if (da->rm)   bp_free(da->rm);
  if (da->rs)   bp_free(da->rs);
  bp_free(da);

}

static int darray_rank0(darray *da, int i)
{
  int r,j;
  bp_pb *p;

#if (bp_RRR == bp_D*2)
  r = da->rl[i>>bp_logR] + da->rm[i>>bp_logRR] + da->rs[i>>bp_logRRR];
  p = da->buf + ((i>>bp_logRRR)<<(bp_logRRR-bp_logD));
  j = i & (bp_RRR-1);
  if (j < bp_D) r += popcount(*p >> (bp_D-1-j));
  else r += popcount(*p) + popcount(p[1] >> (bp_D-1-(j-bp_D)));
#else

  j = i & (bp_RRR-1);
  if (j < bp_RRR/2) {
    r = da->rl[i>>bp_logR] + da->rm[i>>bp_logRR] + da->rs[i>>bp_logRRR];
    p = da->buf + ((i>>bp_logRRR)<<(bp_logRRR-bp_logD));
    while (j >= bp_D) {
      r += popcount(*p++);
      j -= bp_D;
    }
    r += popcount(*p >> (bp_D-1-j));
  } else {
    j = bp_RRR-1 - (i & (bp_RRR-1));
    i += j+1;
    r = da->rl[i>>bp_logR] + da->rm[i>>bp_logRR] + da->rs[i>>bp_logRRR];
    p = da->buf + ((i>>bp_logRRR)<<(bp_logRRR-bp_logD));
    while (j >= bp_D) {
      r -= popcount(*--p);
      j -= bp_D;
    }
    if (j > 0) r -= popcount(*--p << (bp_D-j));
  }

#endif

  return r;
}

int bp_darray_select_bsearch(darray *da, int i, bp_pb (*getpat)(bp_pb *))
{
  
  int l,r,m,n;
  
  int t,x,rr;
  bp_pb *p,w;


  if (i == 0) return -1;

  if (i > da->m) {
    return -1;
  }

  i--;



  n = da->m;

  t = i;

  l = 0;  r = (n-1)>>bp_logR;
  // find the smallest index x s.t. rl[x] >= t
  while (l < r) {
    m = (l+r)/2;
    //printf("m=%d rl[m+1]=%d t=%d\n",m,da->rl[m+1],t);
    if (da->rl[m+1] > t) { // m+1 is out of range
      r = m;  // new r = m >= l
    } else {
      l = m+1; // new l = m+1 <= r
    }
  }
  x = l;
  t -= da->rl[x];

  x <<= bp_logR;

  l = x >> bp_logRR;  r = (min(x+bp_R1-1,n))>>bp_logRR;
  while (l < r) {
    m = (l+r)/2;
    //printf("m=%d rm[m+1]=%d t=%d\n",m,da->rm[m+1],t);
    if (da->rm[m+1] > t) { // m+1 is out of range
      r = m;
    } else {
      l = m+1; // new l = m+1 <= r
    }
  }
  x = l;
  t -= da->rm[x];

  x <<= bp_logRR;

#if 0
  l = x >> bp_logRRR;  r = (min(x+bp_RR-1,n))>>bp_logRRR;
  while (l < r) {
    m = (l+r)/2;
    //printf("m=%d rs[m+1]=%d t=%d\n",m,da->rs[m+1],t);
    if (da->rs[m+1] > t) { // m+1 is out of range
      r = m;
    } else {
      l = m+1; // new l = m+1 <= r
    }
  }
  x = l;
  t -= da->rs[x];
#else
  l = x >> bp_logRRR;
  while (t > da->rs[l]) {
    t -= da->rs[l];
    l++;
  }
  x = l;
#endif

  x <<= bp_logRRR;

  p = &da->buf[x >> bp_logD];
  while (1) {
    m = popcount(getpat(p));
    if (m > t) break;
    t -= m;
    x += bp_D;
    p++;
  }

  w = getpat(p);
  while (1) {
    rr = popcount8(w >> (bp_D-8));
    if (rr > t) break;
    t -= rr;
    x += 8;
    w <<= 8;
  }
  x += selecttbl[((t-0)<<8)+(w>>(bp_D-8))];

#if 0
  if (x != s) {
    printf("error x=%d s=%d\n",x,s);
  }
#endif
  return x;
}

int bp_darray_pat_rank(darray *da, int i, bp_pb (*getpat)(bp_pb *))
{
  int r,j;
  bp_pb *p;

  r = da->rl[i>>bp_logR] + da->rm[i>>bp_logRR];
  j = (i>>bp_logRRR) & (bp_RR/bp_RRR-1);
  while (j > 0) {
    r += da->rs[((i>>bp_logRR)<<(bp_logRR-bp_logRRR))+j-1];
    j--;
  }

  p = da->buf + ((i>>bp_logRRR)<<(bp_logRRR-bp_logD));
  j = i & (bp_RRR-1);
  while (j >= bp_D) {
    r += popcount(getpat(p));
    p++;
    j -= bp_D;
  }
  r += popcount(getpat(p) >> (bp_D-1-j));

  return r;
}


int bp_darray_select(darray *da, int i,int f)
{
  int p,r;
  int il;
  int rr;
  bp_pb x;
  bp_pb *q;

  if (i <= 0 || i > da->m) return -1;

  i--;

  il = da->p[ i >> logL ];
  if (il < 0) {
    il = -il-1;
    p = da->sl[(il<<logL)+(i & (L-1))];
  } else {
    p = da->lp[i>>logL];
    p += da->ss[(il<<(logL-logLLL))+(i & (L-1))/LLL];
    r = i - (i & (LLL-1));

    q = &(da->buf[p>>bp_logD]);

    if (f == 1) {
      rr = p & (bp_D-1);
      r -= popcount(*q >> (bp_D-1-rr));
      p = p - rr;

      while (1) {
	rr = popcount(*q);
	if (r + rr >= i) break;
	r += rr;
	p += bp_D;
	q++;
      }

      x = *q;
      while (1) {
	//rr = popcount(x >> (bp_D-8));
	//rr = popcount(x >> (bp_D-8));
	rr = popcount8(x >> (bp_D-8));
	if (r + rr >= i) break;
	r += rr;
	p += 8;
	x <<= 8;
      }
      p += selecttbl[((i-r-1)<<8)+(x>>(bp_D-8))];
    } else {
      rr = p & (bp_D-1);
      r -= popcount((~(*q))  >> (bp_D-1-rr));
      p = p - rr;

      while (1) {
	rr = popcount(~(*q));
	if (r + rr >= i) break;
	r += rr;
	p += bp_D;
	q++;
      }

      x = ~(*q);

      while (1) {
	rr = popcount8(x >> (bp_D-8));
	if (r + rr >= i) break;
	r += rr;
	p += 8;
	x <<= 8;
      }
      p += selecttbl[((i-r-1)<<8)+(x>>(bp_D-8))];
    }
  }
  return p;
}

int bp_darray_pat_select(darray *da, int i, bp_pb (*getpat)(bp_pb *))
{
  int p,r;
  int il;
  int rr;
  bp_pb x;
  bp_pb *q;

  if (i == 0) return -1;

  if (i > da->m) {
    return -1;
    //printf("ERROR: m=%d i=%d\n",da->m,i);
    //exit(1);
  }

  i--;

  il = da->p[i>>logL];
  if (il < 0) {
    il = -il-1;
    p = da->sl[(il<<logL)+(i & (L-1))];
  } else {
    p = da->lp[i>>logL];
    p += da->ss[(il<<(logL-logLLL))+(i & (L-1))/LLL];
    r = i - (i & (LLL-1));

    q = &(da->buf[p>>bp_logD]);

    rr = p & (bp_D-1);
    r -= popcount(getpat(q) >> (bp_D-1-rr));
    p = p - rr;

    while (1) {
      rr = popcount(getpat(q));
      if (r + rr >= i) break;
      r += rr;
      p += bp_D;
      q++;
    }

    x = getpat(q);
    while (1) {
      rr = popcount8(x >> (bp_D-8));
      if (r + rr >= i) break;
      r += rr;
      p += 8;
      x <<= 8;
    }
    p += selecttbl[((i-r-1)<<8)+(x>>(bp_D-8))];
  }
  return p;
}


darray * bp_darray_pat_construct(int n, bp_pb *buf, int k, bp_pb pat, int opt)
{
  int i;
  bp_pb *b;
  darray *da;

  bp_xmalloc(b, (n+bp_D-1)/bp_D);

  for (i=0; i<n-k+1; i++) {
    bp_setbit(b, i, getpattern(buf,i,k,pat));
  }
  for (i=n-k+1; i<n; i++) {
    bp_setbit(b, i, 0);
  }

  da = bp_darray_construct(n, b, opt);
  da->buf = buf;

  bp_free(b);

  return da;
}
