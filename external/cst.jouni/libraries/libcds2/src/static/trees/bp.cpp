#include <trees/bp.h>
#include <unistd.h>

//#define CHECK
#define RANDOM


#define max(a,b) \
  ({ __typeof__ (a) _a = (a); \
  __typeof__ (b) _b = (b); \
  _a > _b ? _a : _b; })


#define min(a,b) \
  ({ __typeof__ (a) _a = (a); \
  __typeof__ (b) _b = (b); \
   _a <= _b ? _a : _b; })
  
  

static int postorder_select_bsearch(bp *b,int s);

static int naive_depth(bp *b, int s)
{
  int i,d;
  if (s < 0) return 0;
  d = 0;
  for (i=0; i<=s; i++) {
    if (bp_getbit(b->B,i)==OP) {
      d++;
    } else {
      d--;
    }
  }
  return d;
}

void bp_print(bp *b, int s, int t)
{
  int i,c,d;
  d = 0;
  for (i=s; i<=t; i++) {
    if (bp_getbit(b->B,i)==OP) {
      c = '(';
      d++;
    } else {
      c = ')';
      d--;
    }
    putchar(c);
  }
}

int *matchtbl,*parenttbl;
static void make_naivetbl(bp_pb *B,int n)
{
  int i,v;
  int *stack;

  bp_xmalloc(matchtbl,n);
  bp_xmalloc(parenttbl,n);
  bp_xmalloc(stack,n);

  for (i=0; i<n; i++) matchtbl[i] = parenttbl[i] = -1;

  
  v = 0;
  stack[0] = -1;
  for (i=0; i<n; i++) {
    if (bp_getbit(B,i)==OP) {
      v++;
      if (v > 0) {
	parenttbl[i] = stack[v-1];
	stack[v] = i;
      }
    } else {
      if (v > 0) {
	matchtbl[stack[v]] = i;  // close
	matchtbl[i] = stack[v];   // open
      }
      v--;
    }
  }

  free(stack);
}


int fwdtbl[(2*bp_ETW+1)*(1<<bp_ETW)];
int bwdtbl[(2*bp_ETW+1)*(1<<bp_ETW)];
#if 0
int mintbl_li[1<<bp_ETW], mintbl_lv[1<<bp_ETW];
int mintbl_ri[1<<bp_ETW], mintbl_rv[1<<bp_ETW];
int maxtbl_li[1<<bp_ETW], maxtbl_lv[1<<bp_ETW];
int maxtbl_ri[1<<bp_ETW], maxtbl_rv[1<<bp_ETW];
#endif
int minmaxtbl_i[4][1<<bp_ETW], minmaxtbl_v[4][1<<bp_ETW];
int degtbl[1<<bp_ETW];
int degtbl2[(2*bp_ETW+1)*(1<<bp_ETW)];
int childtbl[(bp_ETW)*(1<<bp_ETW)];
int childtbl2[2*bp_ETW+1][bp_ETW][(1<<bp_ETW)];
int depthtbl[(2*bp_ETW+1)*(1<<bp_ETW)];
int inited = 0;

static void make_matchtbl(void)
{
  int i,j,x,r;
  int m,M;
  bp_pb buf[1];
  int deg;
  if (inited)
    return;
  inited = 1;
  for (x = 0; x < (1<<bp_ETW); x++) {
    bp_setbits(buf,0,bp_ETW,x);
    for (r=-bp_ETW; r<=bp_ETW; r++) fwdtbl[((r+bp_ETW)<<bp_ETW)+x] = bp_ETW;
    for (r=-bp_ETW; r<=bp_ETW; r++) bwdtbl[((r+bp_ETW)<<bp_ETW)+x] = bp_ETW;
    for (r=-bp_ETW; r<=bp_ETW; r++) degtbl2[((r+bp_ETW)<<bp_ETW)+x] = 0;
    for (r=-bp_ETW; r<=bp_ETW; r++) depthtbl[((r+bp_ETW)<<bp_ETW)+x] = 0;

    r = 0;
    for (i=0; i<bp_ETW; i++) {
      if (bp_getbit(buf,i)==OP) {
	r++;
      } else {
	r--;
      }
      if (fwdtbl[((r+bp_ETW)<<bp_ETW)+x] == bp_ETW) fwdtbl[((r+bp_ETW)<<bp_ETW)+x] = i;
    }

    r = 0;
    for (i=bp_ETW-1; i>=0; i--) {
      if (bp_getbit(buf,i)==OP) {
	r--;
      } else {
	r++;
      }
      if (bwdtbl[((r+bp_ETW)<<bp_ETW)+x] == bp_ETW) bwdtbl[((r+bp_ETW)<<bp_ETW)+x] = bp_ETW-1-i;
    }

    r = 0;
    for (i=0; i<bp_ETW; i++) {
      if (bp_getbit(buf,i)==OP) {
	r++;
      } else {
	r--;
      }
      depthtbl[((r+bp_ETW)<<bp_ETW)+x] += (1<<(bp_ETW-1));
    }


    r = 0;  m = 0;  M = 0;
    m = bp_ETW+1;  M = -bp_ETW-1;
    //maxtbl_lv[x] = -bp_ETW-1;
    //mintbl_lv[x] = bp_ETW+1;
    minmaxtbl_v[OPT_MAX | OPT_LEFT][x] = -bp_ETW-1;
    minmaxtbl_v[OPT_MIN | OPT_LEFT][x] = bp_ETW+1;
    deg = 0;
    for (i=0; i<bp_ETW; i++) {
      if (bp_getbit(buf,i)==OP) {
	r++;
	if (r > M) {
	  M = r;
	  //maxtbl_li[x] = i;  maxtbl_lv[x] = r;
	  minmaxtbl_i[OPT_MAX | OPT_LEFT][x] = i;
	  minmaxtbl_v[OPT_MAX | OPT_LEFT][x] = r;
	}
      } else {
	r--;
	if (r == m) {
	  deg++;
	  childtbl[((deg-1)<<bp_ETW) + x] = i;
	}
	if (r < m) {
	  m = r;
	  //mintbl_li[x] = i;  mintbl_lv[x] = r;
	  minmaxtbl_i[OPT_MIN | OPT_LEFT][x] = i;
	  minmaxtbl_v[OPT_MIN | OPT_LEFT][x] = r;
	  deg = 1;
	  childtbl[((deg-1)<<bp_ETW) + x] = i;
	}
      }
      if (r <= m) degtbl2[((r+bp_ETW)<<bp_ETW)+x]++;
    }
    degtbl[x] = deg;

    r = 0;  m = 0;  M = 0;
    //maxtbl_rv[x] = -bp_ETW-1;
    //mintbl_rv[x] = bp_ETW+1;
    minmaxtbl_v[OPT_MAX | OPT_RIGHT][x] = -bp_ETW-1;
    minmaxtbl_v[OPT_MIN | OPT_RIGHT][x] = bp_ETW+1;
    for (i=0; i<bp_ETW; i++) {
      if (bp_getbit(buf,i)==OP) {
	r++;
	if (r >= M) {
	  M = r;
	  //maxtbl_ri[x] = i;  maxtbl_rv[x] = r;
	  minmaxtbl_i[OPT_MAX | OPT_RIGHT][x] = i;
	  minmaxtbl_v[OPT_MAX | OPT_RIGHT][x] = r;
	}
      } else {
	r--;
	if (r <= m) {
	  m = r;
	  //mintbl_ri[x] = i;  mintbl_rv[x] = r;
	  minmaxtbl_i[OPT_MIN | OPT_RIGHT][x] = i;
	  minmaxtbl_v[OPT_MIN | OPT_RIGHT][x] = r;
	}
      }
    }

    for (i = 0; i < bp_ETW; i++) {
      for (j = -bp_ETW; j <= bp_ETW; j++) {
	childtbl2[j+bp_ETW][i][x] = -1;
      }
    }

    for (j=-bp_ETW; j<=bp_ETW; j++) {
      int ith;
      ith = 0;
      r = 0;
      for (i = 0; i < bp_ETW; i++) {
	if (bp_getbit(buf,i)==OP) {
	  r++;
	} else {
	  r--;
	  if (r < j) break;
	  if (r == j) {
	    ith++;
	    childtbl2[j+bp_ETW][ith-1][x] = i;
	  }
	}
      }
    }
  }

};


bp * bp_construct(int n, bp_pb *B, int opt)
{
  bp *b;
  int i,j,d;
  int m,M,ds;
  int ns,nm;
  bp_byte *sm, *sM;
  bp_byte *sd;
  int *mm, *mM;
  int *md;
  int m_ofs;
  int r; // # of minimum values
  bp_xmalloc(b, 1);

  b->B = B;
  b->n = n;
  b->opt = opt;
  b->idx_size = 0;
  b->sm = NULL;
  b->sM = NULL;
  b->sd = NULL;
  b->mm = NULL;
  b->mM = NULL;
  b->md = NULL;
  b->da_leaf = NULL;
  b->da_inorder = NULL;
  b->da_postorder = NULL;
  b->da_dfuds_leaf = NULL;
  b->da = bp_darray_construct(n,B, opt & OPT_FAST_PREORDER_SELECT);
  b->idx_size += b->da->idx_size;

  //TODO check.
  make_matchtbl();

  ns = (n+bp_SB-1)/bp_SB;

  bp_xmalloc(sm, ns);
  b->idx_size += ns * sizeof(*sm);

  bp_xmalloc(sM, ns);
  b->idx_size += ns * sizeof(*sM);

  b->sm = sm;
  b->sM = sM;

  if (opt & OPT_DEGREE) {
    bp_xmalloc(sd, ns);
    b->idx_size += ns * sizeof(*sd);
    b->sd = sd;
  }

  for (i=0; i<n; i++) {
    if (i % bp_SB == 0) {
      ds = bp_depth(b,i);
      m = M = ds;
      r = 1;
    } else {
      d = bp_depth(b,i);
      if (d == m) r++;
      if (d < m) {
	m = d;
	r = 1;
      }
      if (d > M) M = d;
    }
    if (i % bp_SB == bp_SB-1 || i==n-1) {
      ds = bp_depth(b,(i/bp_SB)*bp_SB-1);
      if (m - ds + bp_SB < 0 || m - ds + bp_SB > 255) {
	printf("error m=%d ds=%d\n",m,ds);
      }
      if (M - ds + 1 < 0 || M - ds + 1 > 255) {
	printf("error M=%d ds=%d\n",M,ds);
      }
      sm[i/bp_SB] = m - ds + bp_SB;
      sM[i/bp_SB] = M - ds + 1;
      if (opt & OPT_DEGREE) sd[i/bp_SB] = r;
    }
  }


  nm = (n+bp_MB-1)/bp_MB;
  m_ofs = 1 << blog(nm-1);
  b->m_ofs = m_ofs;

  bp_xmalloc(mm, nm + m_ofs);
  b->idx_size += (nm+m_ofs) * sizeof(*mm);

  bp_xmalloc(mM, nm + m_ofs);
  b->idx_size += (nm+m_ofs) * sizeof(*mM);
  b->mm = mm;
  b->mM = mM;

  if (opt & OPT_DEGREE) {
    bp_xmalloc(md, nm + m_ofs);
    b->idx_size += (nm+m_ofs) * sizeof(*md);
    b->md = md;
  }

  for (i=0; i<n; i++) {
    d = bp_depth(b,i);
    if (i % bp_MB == 0) {
      m = M = d;
      r = 1;
    } else {
      if (d == m) r++;
      if (d < m) {
	m = d;
	r = 1;
      }
      if (d > M) M = d;
    }
    if (i % bp_MB == bp_MB-1 || i==n-1) {
      mm[m_ofs+ i/bp_MB] = m;
      mM[m_ofs+ i/bp_MB] = M;
      if (opt & OPT_DEGREE) md[m_ofs+ i/bp_MB] = r;
    }
  }

  for (j=m_ofs-1; j > 0; j--) {
    m = 0;
    if (j*2 < nm + m_ofs) m = mm[j*2];
    if (j*2+1 < nm + m_ofs) m = min(m,mm[j*2+1]);
    M = 0;
    if (j*2 < nm + m_ofs) M = mM[j*2];
    if (j*2+1 < nm + m_ofs) M = max(M,mM[j*2+1]);
    mm[j] = m;  mM[j] = M;
    if (opt & OPT_DEGREE) {
      d = 0;
      if (j*2 < nm + m_ofs) d = md[j*2];
      if (j*2+1 < nm + m_ofs) {
	if (mm[j*2] == mm[j*2+1]) d += md[j*2+1];
	if (mm[j*2] > mm[j*2+1]) d = md[j*2+1];
      }
      md[j] = d;
    }
  }
  mm[0] = -1;
  mM[0] = mM[1];
  if (opt & OPT_DEGREE) {
    md[0] = -1;
  }


  if (opt & OPT_LEAF) {
    b->da_leaf = bp_darray_pat_construct(n, B, 2, 0x2, opt & OPT_FAST_LEAF_SELECT);
    b->idx_size += b->da_leaf->idx_size;
  } else {
    b->da_leaf = NULL;
  }

  if (opt & OPT_INORDER) {

    b->da_inorder = bp_darray_pat_construct(n, B, 2, 0x1, opt & OPT_FAST_INORDER_SELECT);
    b->idx_size += b->da_inorder->idx_size;

  } else {
    b->da_inorder = NULL;
  }

  if (opt & OPT_FAST_POSTORDER_SELECT) {

    b->da_postorder = bp_darray_pat_construct(n, B, 1, 0x0, (opt & OPT_FAST_POSTORDER_SELECT) | OPT_NO_RANK);
    b->idx_size += b->da_postorder->idx_size;

  } else {
    b->da_postorder = NULL;
  }

  if (opt & OPT_DFUDS_LEAF) {
    b->da_dfuds_leaf = bp_darray_pat_construct( n, B, 2, 0x0, opt & OPT_FAST_DFUDS_LEAF_SELECT);
    b->idx_size += b->da_dfuds_leaf->idx_size;

  } else {
    b->da_dfuds_leaf = NULL;
  }

  return b;
}

void bp_delete(bp *b) {
   if (!b) return; // nothing to free

   bp_darray_free(b->da); // destroys da data structure
   if (b->sm) free(b->sm);
   if (b->sM) free(b->sM);
   if (b->sd) free(b->sd);
   if (b->mm) free(b->mm);
   if (b->mM) free(b->mM);
   if (b->md) free(b->md);

   bp_darray_free(b->da_leaf);

   bp_darray_free(b->da_inorder);

   bp_darray_free(b->da_postorder);

   bp_darray_free(b->da_dfuds_leaf);

   bp_free(b);
}


// saveTree: saves parentheses data structure to file
// By Diego Arroyuelo
void saveTree(bp *b, FILE *fp) {

   if (fwrite(&(b->n), sizeof(int), 1, fp) != 1) {
      printf("Error: cannot save number of parentheses to file\n");
      exit(1);
   }
   //bp_getD() == bp_D. It is just to avoid the warning unused-function
   if (fwrite(b->B, sizeof(bp_pb), (b->n+bp_getD()-1)/bp_D, fp) != ((b->n+bp_D-1)/bp_D)) {
      printf("Error: cannot save parentheses sequence to file\n");
      exit(1);
   }

   if (fwrite(&(b->opt), sizeof(int), 1, fp) != 1) {
      printf("Error: cannot save opt in parentheses to file\n");
      exit(1);
   }
}

static ssize_t uwrite(int fd, const void* buff, size_t count)
{
  ssize_t written;
  char *p = (char *) buff;
  do {
    written = write(fd, p, count);
    p += written;
    count -= written;
  } while (written > 0);

  return (written == -1);
}

static ssize_t uread(int fd, const void* buff, size_t count)
{
  ssize_t loaded;
  char *p = (char *) buff;
  do {
    loaded = read(fd, p, count);
    p += loaded;
    count -= loaded;
  } while (loaded > 0);

  return (loaded == -1);
}

int bp_save(bp *b, int fd)
{
  if (uwrite(fd, &b->n, sizeof(int))) return 1;
  if (uwrite(fd, b->B, sizeof(bp_pb) * (b->n+bp_D-1)/bp_D)) return 1;
  if (uwrite(fd, &b->opt, sizeof(int))) return 1;
  return 0;
}

bp* bp_load(int fd)
{
  bp_pb *B;
  int n, opt;
  if (uread(fd, &n, sizeof(int))) return NULL;
  bp_xmalloc(B, (n+bp_D-1)/bp_D);

  if (B == NULL) return NULL;

  if (uread(fd, B, sizeof(bp_pb) * (n+bp_D-1)/bp_D)) {
    free(B);
    return NULL;
  };

  if (uread(fd, &opt, sizeof(int))){
    free(B);
    return NULL;
  };

  return bp_construct(n, B, opt);
}


// loadTree: load parentheses data structure from file
// By Diego Arroyuelo
bp * loadTree(FILE *fp) {

   bp_pb *B;
   int n, opt;

   if (fread(&n, sizeof(int), 1, fp) != 1) {
      printf("Error: cannot read number of parentheses from file\n");
      exit(1);
   }

   bp_xmalloc(B, (n+bp_D-1)/bp_D);

   if (fread(B, sizeof(bp_pb), (n+bp_D-1)/bp_D, fp) != ((n+bp_D-1)/bp_D)) {
      printf("Error: cannot read parentheses sequence from file\n");
      exit(1);
   }

   if (fread(&opt, sizeof(int), 1, fp) != 1) {
      printf("Error: cannot read opt in parentheses from file\n");
      exit(1);
   }

   return bp_construct(n, B, opt);

}


static int naive_fwd_excess(bp *b,int s, int rel)
{
  int i,v,n;
  bp_pb *B;
  n = b->n;  B = b->B;
  v = 0;
  for (i=s+1; i<n; i++) {
    if (bp_getbit(B,i)==OP) {
      v++;
    } else {
      v--;
    }
    if (v == rel) return i;
  }
  return -1;
}

static int naive_bwd_excess(bp *b,int s, int rel)
{
  int i,v;
  bp_pb *B;
  B = b->B;
  v = 0;
  for (i=s; i>=0; i--) {
    if (bp_getbit(B,i)==OP) {
      v--;
    } else {
      v++;
    }
    if (v == rel) return i-1;
  }
  return -2;
}

static int naive_search_SB_l(bp *b, int i, int rel)
{
  int il;

  il = (i / bp_SB) * bp_SB;
  for (; i>=il; i--) {
    if (bp_getbit(b->B,i)==OP) {
      rel++;
    } else {
      rel--;
    }
    if (rel == 0) return i-1;
  }
  if (i < 0) return -2;
  return -3;
}

int bp_naive_rmq(bp *b, int s, int t,int opt)
{
  int d,i,dm,im;

  if (opt & OPT_RIGHT) {
    d = dm = bp_depth(b,t);  im = t;
    i = t-1;
    while (i >= s) {
      if (bp_getbit(b->B,i+1)==CP) {
	d++;
	if (opt & OPT_MAX) {
	  if (d > dm) {
	    dm = d;  im = i;
	  }
	}
      } else {
	d--;
	if (!(opt & OPT_MAX)) {
	  if (d < dm) {
	    dm = d;  im = i;
	  }
	}
      }
      i--;
    }
  } else {
    d = dm = bp_depth(b,s);  im = s;
    i = s+1;
    while (i <= t) {
      if (bp_getbit(b->B,i)==OP) {
	d++;
	if (opt & OPT_MAX) {
	  if (d > dm) {
	    dm = d;  im = i;
	  }
	}
      } else {
	d--;
	if (!(opt & OPT_MAX)) {
	  if (d < dm) {
	    dm = d;  im = i;
	  }
	}
      }
      i++;
    }
  }
  return im;
}



int bp_rank_open(bp *b, int s)
{
  return bp_darray_rank(b->da, s);
}

int bp_rank_close(bp *b, int s)
{
  return s+1 - bp_darray_rank(b->da, s);
}

int bp_select_open(bp *b, int s)
{
  if (b->opt & OPT_FAST_PREORDER_SELECT) {
    return bp_darray_select(b->da,s,1);
  } else {
    return bp_darray_select_bsearch(b->da, s, getpat_preorder);
  }
}

int bp_select_close(bp *b, int s)
{
  if (b->opt & OPT_FAST_POSTORDER_SELECT) {
    return bp_darray_pat_select(b->da_postorder, s, getpat_postorder);
  } else {
    return postorder_select_bsearch(b,s);
  }
}


///////////////////////////////////////////
//  bp_postorder_rank(bp *b,int s)
//    returns the postorder (>= 1) of node s (s >= 0)
//            -1 if s-th bit is not OP
///////////////////////////////////////////
int bp_postorder_rank(bp *b,int s)
{
  int t;
  if (bp_inspect(b,s) == CP) return -1;
  t = bp_find_close(b,s);
  //  return t+1 - darray_rank(b->da,t);
  return bp_rank_close(b,t);
}

static int postorder_select_bsearch(bp *b,int s)
{
  int l,r,m;

  if (s == 0) return -1;

  if (s > b->da->n - b->da->m) {
    return -1;
  }
  l = 0;  r = b->da->n - 1;

  while (l < r) {
    m = (l+r)/2;
    //printf("m=%d rank=%d s=%d\n",m,m+1 - darray_rank(b->da,m),s);
    if (m+1 - bp_darray_rank(b->da,m) >= s) {
      r = m;
    } else {
      l = m+1;
    }
  }
  return l;
}

///////////////////////////////////////////
//  bp_postorder_select(bp *b,int s)
//    returns the position of CP of the node with postorder s (>= 1)
///////////////////////////////////////////
int bp_postorder_select(bp *b,int s)
{
#if 0
  if (b->opt & OPT_FAST_POSTORDER_SELECT) {
    return bp_darray_pat_select(b->da_postorder,s,getpat_postorder);
  } else {
    return postorder_select_bsearch(b->da,s);
  }
#else
  return bp_select_close(b,s);
#endif
}

///////////////////////////////////////////
//  bp_leaf_rank(bp *b,int s)
//    returns the number of leaves to the left of s
///////////////////////////////////////////
int bp_leaf_rank(bp *b,int s)
{
  if ((b->opt & OPT_LEAF) == 0) {
    printf("leaf_rank: error!!!   not supported\n");
    return -1;
  }
  if (s >= b->n-1) {
    s = b->n-2;
  }
  return bp_darray_pat_rank(b->da_leaf,s,getpat_leaf);
}

///////////////////////////////////////////
//  leaf_select(bp *b,int s)
//    returns the position of s-th leaf
///////////////////////////////////////////
int bp_leaf_select(bp *b,int s)
{
  if ((b->opt & OPT_LEAF) == 0) {
    printf("leaf_select: error!!!   not supported\n");
    return -1;
  }
  if (s > b->da_leaf->m) return -1;
  if (b->opt & OPT_FAST_LEAF_SELECT) {
    return bp_darray_pat_select(b->da_leaf,s,getpat_leaf);
  } else {
    return bp_darray_select_bsearch(b->da_leaf,s,getpat_leaf);
  }
}


///////////////////////////////////////////
//  bp_inorder_rank(bp *b,int s)
//    returns the number of ")("  (s >= 0)
///////////////////////////////////////////
int bp_inorder_rank(bp *b,int s)
{
  if ((b->opt & OPT_INORDER) == 0) {
    printf("inorder_rank: error!!!   not supported\n");
    return -1;
  }
  if (s >= b->n-1) {
    s = b->n-2;
  }
  return bp_darray_pat_rank(b->da_inorder,s,getpat_inorder);
}

///////////////////////////////////////////
//  bp_inorder_select(bp *b,int s)
//    returns the s-th position of ")("  (s >= 1)
///////////////////////////////////////////
int bp_inorder_select(bp *b,int s)
{
  if ((b->opt & OPT_INORDER) == 0) {
    printf("inorder_select: error!!!   not supported\n");
    return -1;
  }
  if (b->opt & OPT_FAST_INORDER_SELECT) {
    return bp_darray_pat_select(b->da_inorder,s,getpat_inorder);
  } else {
    return bp_darray_select_bsearch(b->da_inorder,s,getpat_inorder);
  }
}

///////////////////////////////////////////
//  bp_leftmost_leaf(bp *b, int s)
///////////////////////////////////////////
int bp_leftmost_leaf(bp *b, int s)
{
  if ((b->opt & OPT_LEAF) == 0) {
    printf("leftmost_leaf: error!!!   not supported\n");
    return -1;
  }
  return bp_leaf_select(b, bp_leaf_rank(b,s)+1);
}

///////////////////////////////////////////
//  rightmost_leaf(bp *b, int s)
///////////////////////////////////////////
int bp_rightmost_leaf(bp *b, int s)
{
  int t;
  if ((b->opt & OPT_LEAF) == 0) {
    printf("leftmost_leaf: error!!!   not supported\n");
    return -1;
  }
  t = bp_find_close(b,s);
  return bp_leaf_select(b, bp_leaf_rank(b,t));
}


///////////////////////////////////////////
//  bp_prev_sibling(bp *b,int s)
//    returns the previous sibling of parent(s)
//            -1 if s is the first child
//////////////////////////////////////////
int bp_prev_sibling(bp *b, int s)
{
  int t;
  if (s == 0) return -1;
  if (bp_inspect(b,s-1) == OP) return -1;
  t = bp_find_open(b,s-1);
  return t;
}

///////////////////////////////////////////
//  bp_deepest_node(bp *b,int s)
//    returns the first node with the largest depth in the subtree of s
///////////////////////////////////////////
int bp_deepest_node(bp *b,int s)
{
  int t,m;
  t = bp_find_close(b,s);
  m = bp_rmq(b,s,t, OPT_MAX);
  return m;
}

///////////////////////////////////////////
//  bp_subtree_height(bp *b,int s)
//    returns the height of the subtree of s
//            0 if s is a leaf
///////////////////////////////////////////
int bp_subtree_height(bp *b,int s)
{
  int t;
  t = bp_deepest_node(b,s);
  return bp_depth(b,t) - bp_depth(b,s);
}

int bp_naive_degree(bp *b, int s)
{
  int t,d;
  d = 0;
  t = bp_first_child(b,s);
  while (t >= 0) {
    d++;
    t = bp_next_sibling(b,t);
  }
  return d;
}

///////////////////////////////////////////
//  bp_degree(bp *b, int s)
//    returns the number of children of s
//            0 if s is a leaf
///////////////////////////////////////////
int bp_degree(bp *b, int s)
{
  if (b->opt & OPT_DEGREE) {
    return bp_fast_degree(b,s,b->n,0);
  } else {
    return bp_naive_degree(b,s);
  }
}

int bp_naive_child(bp *b, int s, int d)
{
  int t,i;
  t = bp_first_child(b,s);
  for (i = 1; i < d; i++) {
    if (t == -1) break;
    t = bp_next_sibling(b,t);
  }
  return t;
}

///////////////////////////////////////////
//  bp_child(bp *b, int s, int d)
//    returns the d-th child of s  (1 <= d <= degree(s))
//            -1 if no such node
///////////////////////////////////////////
int bp_child(bp *b, int s, int d)
{
  int r;
  if (b->opt & OPT_DEGREE) {
    //return find_open(b,fast_degree(b,s,b->n,d));
    if (d==1) return bp_first_child(b,s);
    r = bp_fast_degree(b,s,b->n,d-1)+1;
    if (bp_inspect(b,r) == CP) return -1;
    return r;
  } else {
    return bp_naive_child(b,s,d);
  }

}


int bp_naive_child_rank(bp *b, int t)
{
  int d;
  d = 0;
  while (t != -1) {
    d++;
    t = bp_prev_sibling(b,t);
  }
  return d;
}

///////////////////////////////////////////
//  bp_child_rank(bp *b, int t)
//    returns d if t is the d-th child of the parent of t (d >= 1)
//            1 if t is the root
///////////////////////////////////////////
int bp_child_rank(bp *b, int t)
{
  int r;
  if (t == bp_root_node(b)) return 1;
  if (b->opt & OPT_DEGREE) {
    r = bp_parent(b,t);
    return bp_fast_degree(b,r,t,0)+1;
  } else {
    return bp_naive_child_rank(b,t);
  }
}


///////////////////////////////////////////
//  distance(bp *b, int s, int t)
//    returns the length of the shortest path from s to t in the tree
///////////////////////////////////////////
int bp_distance(bp *b, int s, int t)
{
  int v,d;
  v = bp_lca(b,s,t);
  d = bp_depth(b,v);
  return (bp_depth(b,s) - d) + (bp_depth(b,t) - d);
}

///////////////////////////////////////////
//  level_next(bp *b, int d)
///////////////////////////////////////////
int bp_level_next(bp *b,int s)
{
  int t;
  t = bp_fwd_excess(b,s,0);
  return t;
}

///////////////////////////////////////////
//  level_prev(bp *b, int d)
///////////////////////////////////////////
int bp_level_prev(bp *b,int s)
{
  int t;
  t = bp_bwd_excess(b,s,0);
  return t;
}

///////////////////////////////////////////
//  bp_level_leftmost(bp *b, int d)
///////////////////////////////////////////
int bp_level_leftmost(bp *b, int d)
{
  int t;
  if (d < 1) return -1;
  if (d == 1) return 0;
  t = bp_fwd_excess(b,0,d);
  return t;
}

///////////////////////////////////////////
//  bp_level_rigthmost(bp *b, int d)
///////////////////////////////////////////
int level_rigthmost(bp *b, int d)
{
  int t;
  if (d < 1) return -1;
  if (d == 1) return 0;
  t = bp_bwd_excess(b,0,d-1);
  return bp_find_open(b,t);
}

///////////////////////////////////////////
//  leaf_size(bp *b, int s)
///////////////////////////////////////////
int bp_leaf_size(bp *b, int s)
{
  int t;
  if ((b->opt & OPT_LEAF) == 0) {
    printf("leaf_size: error!!!   not supported\n");
    return -1;
  }
  t = bp_find_close(b,s);
  return bp_leaf_rank(b,t) - bp_leaf_rank(b,s);
}
