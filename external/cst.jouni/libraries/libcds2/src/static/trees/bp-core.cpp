#include <trees/bp.h>
#include <trees/bp-utils.h>


#define NOTFOUND -2
#define CONTINUE -3
#define END -4
#define FOUND -5

#define SBid(i) ((i)>>bp_logSB)
#define SBfirst(i) ((i) & (~(bp_SB-1)))
#define SBlast(i) ((i) | (bp_SB-1))

#define MBid(i) ((i)>>bp_logMB)
#define MBfirst(i) ((i) & (~(bp_MB-1)))
#define MBlast(i) ((i) | (bp_MB-1))

static int min(int a, int b)
{
	return (a <= b) ? a : b;
}

static int max(int a, int b)
{
	return (a >= b) ? a : b;
}

bp_pb getpat_preorder(bp_pb *b)
{
  return *b;
}

bp_pb getpat_postorder(bp_pb *b)
{
  return ~(*b);
}

bp_pb getpat_leaf(bp_pb *b)
{
  bp_pb w1,w2,w;
  w1 = b[0];
  w2 = (w1 << 1) + (b[1] >> (bp_D-1));
  w = w1 & (~w2);
  return w;
}

bp_pb getpat_inorder(bp_pb *b)
{
  bp_pb w1,w2,w;
  w1 = b[0];
  w2 = (w1 << 1) + (b[1] >> (bp_D-1));
  w = (~w1) & w2;
  return w;
}

bp_pb getpat_dfuds_leaf(bp_pb *b)
{
  bp_pb w1,w2,w;
  w1 = b[0];
  w2 = (w1 << 1) + (b[1] >> (bp_D-1));
  w = (~w1) & (~w2);
  return w;
}



///////////////////////////////////////////
//  bp_depth(bp *b, int s)
//    returns the depth of s
//  The root node has depth 1
///////////////////////////////////////////
int bp_depth(bp *b, int s)
{
  int d;
  if (s < 0) return 0;
  d = 2 * bp_darray_rank(b->da,s) - (s+1);
  return d;
}

int fast_depth(bp *b, int s)
{
  int d;
  darray *da;
  int r,j;

  s++;
  if ((s & (bp_RRR-1)) != 0) {
    //printf("fast_depth:warning s=%d\n",s);
    return bp_depth(b,s);
  }
  da = b->da;
  //d = 2 * (da->rl[s>>bp_logR] + da->rm[s>>bp_logRR] + da->rs[s>>bp_logRRR]) - s;

  r = da->rl[s>>bp_logR] + da->rm[s>>bp_logRR];
  j = (s>>bp_logRRR) & (bp_RR/bp_RRR-1);
  while (j > 0) {
    r += da->rs[((s>>bp_logRR)<<(bp_logRR-bp_logRRR))+j-1];
    j--;
  }
  d = 2 * r - s;

  return d;
}

int search_SB_r(bp *b, int i, int rel)
{
  int j,r,n,il;
  bp_pb *p,x,w;

  n = b->n;
  il = min((SBid(i) + 1) << bp_logSB, n);
  p = &b->B[i>>bp_logD];
  while (i<il) {
    x = *p++;
    j = i & (bp_D-1);
    x <<= j;
    j = bp_D-j;
    while (j>0) {
      w = (x >> (bp_D-bp_ETW)) & ((1<<bp_ETW)-1);
      if (rel >= -bp_ETW && rel <= bp_ETW) {
        r = fwdtbl[((rel+bp_ETW)<<bp_ETW)+w];
        if (r<bp_ETW && r<j) {
          if (i+r >= n) return NOTFOUND;
          return i+r;
        }
      }
      r = min(j,bp_ETW);
      rel -= (popcount(w) << 1)-r;
      x <<= r;
      i += r;
      j -= r;
    }
  }
  return CONTINUE;
}

int search_MB_r(bp *b, int i, int td)
{
  int il,d;
  int m,M,n;
  

  
  n = b->n;

  il = min((MBid(i) + 1) << bp_logMB, n);
  for (  ;  i < il;  i+=bp_SB) {
#if (bp_SB % bp_RRR != 0)
    d = bp_depth(b,i-1);
#else
    d = fast_depth(b,i-1);
#endif
    m = d + b->sm[SBid(i)] - bp_SB;
    M = d + b->sM[SBid(i)] - 1;
    if (m <= td && td <= M) {
      return search_SB_r(b,i,td-d);
    }
  }
  if (i >= n) return NOTFOUND;
  return CONTINUE;
}

///////////////////////////////////////////
//  bp_fwd_excess(bp *b,int s, int rel)
//    find the leftmost value depth(s)+rel to the right of s (exclusive)
///////////////////////////////////////////
int bp_fwd_excess(bp *b,int s, int rel)
{
  int i,n;
  int d,td;
  int m,M;
  int m_ofs;
  

  n = b->n;

  i = s+1;

  d = search_SB_r(b,i,rel);
  if (d >= NOTFOUND) return d;

  i = min((SBid(i) + 1) << bp_logSB, n);
  td = bp_depth(b,s) + rel;
  d = search_MB_r(b,i,td);
  if (d >= NOTFOUND) return d;

  m_ofs = b->m_ofs;

  i = MBid(s) + m_ofs;

  while (i > 0) {
    if ((i&1) == 0) {
      i++;
      m = b->mm[i];
      M = b->mM[i];

      if (m <= td && td <= M) {
	      while (i < m_ofs) {
		      i <<= 1;
		      m = b->mm[i];
		      M = b->mM[i];
		      if (!(m <= td && td <= M)) i++;
	      }
	      i -= m_ofs;
	      i <<= bp_logMB;

	      return search_MB_r(b,i,td);
      };

    }
    i >>= 1;
  }
  return NOTFOUND;
}


int degree_SB_slow(bp *b, int i, int t, int rel, int *ans, int ith)
{
  int n,il;
  
  int d, deg;

  n = t;
  il = min((SBid(i) + 1) << bp_logSB,n);
  d = deg = 0;

  while (i < il) {
    if (bp_getbit(b->B,i)==OP) {
      d++;
    } else {
      d--;
    }
    if (d < rel) {  // reached the end
      if (ith > 0) {
	return NOTFOUND;
      } else {
	*ans = deg;
	return END;
      }
    }
    if (d == rel) {  // found the same depth
      deg++;
      if (deg == ith) {
	*ans = i;
	return FOUND;
      }
    }
    i++;
  }
  *ans = deg;
  return CONTINUE;
}

int degree_SB(bp *b, int i, int t, int rel, int *ans, int ith)
{
  int j,r,n,il;
  bp_pb *p,x,w,w2;
  int d, deg, v;

  n = t;
  il = min((SBid(i) + 1) << bp_logSB,n);
  d = deg = 0;

  p = &b->B[i>>bp_logD];
  while (i < il) {
    x = *p++;
    j = i & (bp_D-1);
    x <<= j;
    j = min(bp_D-j,il-i);
    while (j>0) {
      w = (x >> (bp_D-bp_ETW)) & ((1<<bp_ETW)-1);
      w2 = 0;
      if (j < bp_ETW || il-i < bp_ETW) {
        r = max(bp_ETW-j,bp_ETW-(il-i));
        w2 = (1<<r)-1;
      }
      v = minmaxtbl_v[0][w | w2];
      if (d + v < rel) {
        if (ith > 0) {
#if 0
          for (r = 0; r < bp_ETW; r++) {
            if (w & 0x80) {
              d++;
            } else {
              d--;
              if (d < rel) break;
              if (d == rel) {
                ith--;
                if (ith == 0) {
                  *ans = i + r;
                  return FOUND;
                }
              }
            }
            w <<= 1;
          }
          return NOTFOUND;
#else
          r = childtbl2[rel-d+bp_ETW][ith-1][w];
          if (r >= 0) {
            *ans = i + r;
            return FOUND;
          }
          return NOTFOUND;
#endif
        }
        r = bp_ETW-1-minmaxtbl_i[0][w | w2];
        w2 = (1<<r)-1;
        deg += degtbl2[((rel-d+bp_ETW)<<bp_ETW) + (w & (~w2))];
        *ans = deg;
        return END;
      }
      if (d + v == rel) {
        r = degtbl[w | w2];
        deg += r;
        if (ith > 0) {
          if (ith <= r) {
            *ans = i + childtbl[((ith-1)<<bp_ETW) + (w | w2)];
            return FOUND;
          }
          ith -= r;
        }
      }

      r = min(j,bp_ETW);
      d += 2*popcount(w)-r;
      x <<= r;
      i += r;
      j -= r;
    }
  }

  *ans = deg;
  return CONTINUE;
}

int degree_MB(bp *b, int i, int t, int td, int *ans,int ith)
{
  int il,d;
  int m,n,r;
  
  int deg,degtmp;

  d = 0;
  
  n = t;

  il = min((MBid(i) + 1) << bp_logMB,n);
  deg = 0;
  for (  ;  i+bp_SB-1 < il;  i+=bp_SB) {
#if (bp_SB % bp_RRR != 0)
    d = bp_depth(b,i-1);
#else
    d = fast_depth(b,i-1);
#endif
    m = d + b->sm[SBid(i)] - bp_SB;
    if (m < td) {
      r = degree_SB(b,i,n,td-d,&degtmp,ith);
      if (ith > 0) {
        if (r == NOTFOUND) return NOTFOUND;
        *ans = degtmp;
        return FOUND;
      } else {
        *ans = deg + degtmp;
        return END;
      }
    }
    if (m == td) {
      if (ith > 0) {
        if (ith <= b->sd[SBid(i)]) break;
        ith -= b->sd[SBid(i)];
      }
      deg += b->sd[SBid(i)];
    }
  }
  if (i < il) {
#if (bp_SB % bp_RRR != 0)
    d = bp_depth(b,i-1);
#else
    d = fast_depth(b,i-1);
#endif
    r = degree_SB(b,i,n,td-d,&degtmp,ith);
    if (ith > 0) {
      if (r == NOTFOUND) return NOTFOUND;
      if (r == FOUND) {
        *ans = degtmp;
        return FOUND;
      }
    } else {
      deg += degtmp;
    }
  }
  *ans = deg;
  if (i >= n) return END;
  return CONTINUE;
}

///////////////////////////////////////////
//  bp_fast_degree(bp *b,int s, int t, int ith)
//    returns the number of children of s, to the left of t
//    returns the position of (ith)-th child of s if (ith > 0)
///////////////////////////////////////////
int bp_fast_degree(bp *b,int s, int t, int ith)
{
  int i,j,n;
  int d,td;
  
  int m_ofs;
  
  int deg,degtmp;
  int sm,tm,ss,h;

  n = t;
  

  deg = 0;

  i = s+1;
  if (i != SBfirst(i)) {
    d = degree_SB(b,i,n,0,&degtmp,ith);
    if (ith > 0) {
      if (d == NOTFOUND) return -1;
      if (d == FOUND) return degtmp;
      ith -= degtmp;
    }
    if (d == END) return degtmp;
    deg += degtmp;
  }

  td = bp_depth(b,s);

  i = SBid(i+bp_SB-1) << bp_logSB;

  if (i != MBfirst(i)) {
    d = degree_MB(b,i,n,td,&degtmp,ith);
    if (ith > 0) {
      if (d == NOTFOUND) return -1;
      if (d == FOUND) return degtmp;
      ith -= degtmp;
      deg += degtmp;
    } else {
      deg += degtmp;
      if (d == END) {
        return deg;
      }
    }
  }

#if 0
  // sequential search

  sm = MBid(i+bp_MB-1);
  tm = MBid((n-1)+1)-1; // the rightmost bp_MB fully contained in [0,n-1]

  m_ofs = b->m_ofs;
  sm += m_ofs;  tm += m_ofs;
  for (i=sm; i<=tm; i++) {
    if (b->mm[i] < td) {
      break;
    }
    if (b->mm[i] == td) {
      if (ith > 0) {
        if (ith <= b->md[i]) break;
        ith -= b->md[i];
      }
      deg += b->md[i];
    }
  }
  ss = i - m_ofs;
#else
  sm = MBid(i+bp_MB-1);
  tm = MBid((n-1)+1)-1; // the rightmost bp_MB fully contained in [0,n-1]

  m_ofs = b->m_ofs;
  sm += m_ofs;  tm += m_ofs;
  ss = sm;

  //partition_range(sm,tm);

  //printf("partition [%d,%d] => ",sm,tm);
  h = 1;
  while (sm <= tm) {
    if (sm & h) {
      if (sm+h-1 <= tm) {
        //printf("[%d,%d] ",sm,sm+h-1);
        j = sm / h;
        if (b->mm[j] < td) {
          h >>= 1;
          break;
        }
        if (b->mm[j] == td) {
          if (ith > 0) {
            if (ith <= b->md[j]) {
              h >>= 1;
              break;
            }
            ith -= b->md[j];
          }
          deg += b->md[j];
        }
        sm += h;
      }
    } else {
      if (sm+h > tm) break;
    }
    h <<= 1;
  }
  while (h > 0) {
    if (sm+h-1 <= tm) {
      //printf("[%d,%d] ",sm,sm+h-1);
      j = sm / h;
      if (ith > 0) {
        if (b->mm[j] >= td) {
          if (b->mm[j] == td) {
            if (ith > b->md[j]) {
              ith -= b->md[j];
              sm += h;
            } else {
              deg += b->md[j];
            }
          } else {
            sm += h;
          }
        }
      } else {
        if (b->mm[j] >= td) {
          if (b->mm[j] == td) {
            deg += b->md[j];
          }
          sm += h;
        }
      }
    }
    h >>= 1;
  }
  //printf("\n");
  ss = sm;

  ss -= m_ofs;

#endif

  ss <<= bp_logMB;

  d = degree_MB(b,ss,n,td,&degtmp,ith);
  if (ith > 0) {
    if (d == NOTFOUND) return -1;
    if (d == FOUND) return degtmp;
  }
  deg += degtmp;
  if (d == END) return deg;
  return deg;

  // unexpected (bug)
  printf("degree: ???\n");
  return -99;

}

int search_SB_l(bp *b, int i, int rel)
{
  int j,r,il;
  bp_pb *p,x,w;

  il = SBfirst(i);

  p = &b->B[i>>bp_logD];
  while (i>=il) {
    x = *p--;
    j = (i & (bp_D-1))+1;
    x >>= bp_D-j;
    while (j>0) {
      w = x & ((1<<bp_ETW)-1);
      if (rel >= -bp_ETW && rel <= bp_ETW) {
        r = bwdtbl[((rel+bp_ETW)<<bp_ETW)+w];
        if (r<bp_ETW && r<j) {
          if (i-r < 0) return NOTFOUND;
          return i-r-1;
        }
      }
      r = min(j,bp_ETW);
      rel += 2*popcount(w)-r;
      x >>= r;
      i -= r;
      j -= r;
    }

  }
  if (i < 0) return NOTFOUND;
  return CONTINUE;
}

int search_MB_l(bp *b, int i, int td)
{
  int il,d;
  int m,M;
  

#if 0
  if (i % bp_SB != bp_SB-1) {
    printf("search_MB_l:error!!! i=%d bp_SB=%d\n",i,i%bp_SB);
  }
#endif
  

  il = MBfirst(i);
  for (  ;  i >= il;  i-=bp_SB) {
#if (bp_SB % bp_RRR != 0)
    d = bp_depth(b,i-bp_SB);
#else
    d = fast_depth(b,i-bp_SB);
#endif
    m = d + b->sm[SBid(i)] - bp_SB;
    M = d + b->sM[SBid(i)] - 1;
    if (m <= td && td <= M) {
#if (bp_SB % bp_RRR != 0)
      d = bp_depth(b,i);
#else
      d = fast_depth(b,i);
#endif
      if (d == td) return i;
      return search_SB_l(b,i,td-d);
    }
  }
  return CONTINUE;
}

///////////////////////////////////////////
//  bwd_excess(bp *b,int s, int rel)
//    find the rightmost value depth(s)+rel to the left of s (exclusive)
///////////////////////////////////////////
int bp_bwd_excess(bp *b,int s, int rel)
{
  int i;
  int d,td;
  int m,M;
  int m_ofs;
  
  

  i = s;
  d = search_SB_l(b,i,rel);
  if (d >= NOTFOUND) return d;

  i = SBfirst(i) -1;

  td = bp_depth(b,s) + rel;

  d = search_MB_l(b,i,td);
  if (d >= NOTFOUND) return d;

  m_ofs = b->m_ofs;
  i = (s>>bp_logMB) + m_ofs;
  while (i > 0) {
    if ((i&1) == 1) {
      i--;
      m = b->mm[i];
      M = b->mM[i];
      if (m <= td && td <= M) break;
    }
    i >>= 1;
  }
  if (i == 0) {
    if (td == 0) return -1;
    else return NOTFOUND;
  }

  while (i < m_ofs) {
    i = i*2 + 1;
    m = b->mm[i];
    M = b->mM[i];
    if (!(m <= td && td <= M)) i--;
  }
  i -= m_ofs;
  i = ((i+1)<<bp_logMB)-1;

  d = search_MB_l(b,i,td);
  if (d >= NOTFOUND) return d;

  // unexpected (bug)
  printf("bwd_excess: ???\n");
  return -99;

}

int rmq_SB(bp *b, int s, int t, int opt, int *dm)
{
  int i,d;
  int is,ds;
  bp_pb *p,x,w,w2;
  int j,v,r;
  int lr;
  int op;

  lr = 0;  if (opt & OPT_RIGHT) lr = 1;
  op = opt & (OPT_RIGHT | OPT_MAX);

  is = s;  ds = d = 0;
  i = s+1;

#if bp_SB >= bp_ETW
  p = &b->B[i>>bp_logD];
  while (i <= t) {
    x = *p++;
    j = i & (bp_D-1);
    x <<= j;
    j = min(bp_D-j,t-i+1);
    while (j>0) {
      w = (x >> (bp_D-bp_ETW)) & ((1<<bp_ETW)-1);
      w2 = 0;
      if (j < bp_ETW || t-i < bp_ETW-1) {
        r = max(bp_ETW-j,bp_ETW-1-(t-i));
        w2 = (1<<r)-1;
      }

      if (op & OPT_MAX) {
        v = minmaxtbl_v[op][w & (~w2)];
        if (d + v + lr > ds) {
          ds = d + v;
          is = i + minmaxtbl_i[op][w & (~w2)];
        }
      } else {
        v = minmaxtbl_v[op][w | w2];
        if (d + v < ds +lr) {
          ds = d + v;
          is = i + minmaxtbl_i[op][w | w2];
        }
      }

      r = min(j,bp_ETW);
      d += 2*popcount(w)-r;
      x <<= r;
      i += r;
      j -= r;
    }
  }
#else
  while (i <= t) {
    if (bp_getbit(b->B,i)==OP) {
      d++;
      if (op & OPT_MAX) {
        if (d + lr > ds) {
          ds = d;  is = i;
        }
      }
    } else {
      d--;
      if (!(op & OPT_MAX)) {
        if (d < ds + lr) {
          ds = d;  is = i;
        }
      }
    }
    i++;
  }
#endif
  *dm = ds;
  return is;
}

int rmq_MB(bp *b, int s, int t, int opt, int *dm)
{
  int i,d,m;
  int mi,md;
  int lr;

  lr = 0;  if (opt & OPT_RIGHT) lr = 1;

  md = *dm;  mi = -1;
  for (i = s;  i <= t;  i++) {
#if (bp_SB % bp_RRR != 0)
    d = bp_depth(b,(i<<bp_logSB)-1);
#else
    d = fast_depth(b,(i<<bp_logSB)-1);
#endif
    if (opt & OPT_MAX) {
      m = d + b->sM[i] - 1;
      if (m + lr > md) {
        md = m;  mi = i;
      }
    } else {
      m = d + b->sm[i] - bp_SB;
      if (m < md + lr) {
        md = m;  mi = i;
      }
    }
  }
  *dm = md;
  return mi;
}




///////////////////////////////////////////
//  bp_rmq(bp *b, int s, int t, int opt)
//    returns the index of leftmost/rightmost minimum/maximum value
//                 in the range [s,t] (inclusive)
//    returns the leftmost minimum if opt == 0
//            the rightmost one if (opt & OPT_RIGHT)
//            the maximum if (opt & OPT_MAX)
///////////////////////////////////////////
int bp_rmq(bp *b, int s, int t, int opt)
{
  int ss, ts;  // bp_SB index of s and t
  int sm, tm;  // bp_MB index of s and t
  int ds;   // current min value
  int is;   // current min index
  int ys;   // type of current min index
               // 0: is is the index of min
               // 1: is is the bp_SB index
               // 2: is is the bp_MB index
  int m_ofs;
  int i,j,il,d,n;
  int dm;
  int lr;

  lr = 0;  if (opt & OPT_RIGHT) lr = 1;

  n = b->n;
  if (s < 0 || t >= n || s > t) {
    printf("rmq: error s=%d t=%d n=%d\n",s,t,n);
    return -1;
  }
  if (s == t) return s;


  ////////////////////////////////////////////////////////////
  // search the bp_SB of s
  ////////////////////////////////////////////////////////////

  il = min(SBlast(s),t);
  is = rmq_SB(b,s,il,opt,&dm);
  if (il == t) {  // scan reached the end of the range
    return is;
  }
  ds = bp_depth(b,s) + dm;  ys = 0;

  ////////////////////////////////////////////////////////////
  // search the bp_MB of s
  ////////////////////////////////////////////////////////////

  ss = SBid(s) + 1;
  il = min(SBid(MBlast(s)),SBid(t)-1);
  dm = ds;
  j = rmq_MB(b,ss,il,opt,&dm);
  //if (dm < ds + lr) {
  if (j >= 0) {
    ds = dm;  is = j;  ys = 1;
  }

  ////////////////////////////////////////////////////////////
  // search the tree
  ////////////////////////////////////////////////////////////

  sm = MBid(s) + 1;
  tm = MBid(t) - 1;

#if 0
  // sequential search
  m_ofs = b->m_ofs;
  sm += m_ofs;  tm += m_ofs;
  for (i=sm; i<=tm; i++) {
    if (opt & OPT_MAX) {
      if (b->mM[i] + lr > ds) {
        ds = b->mM[i];  is = i - m_ofs;  ys = 2;
      }
    } else {
      if (b->mm[i] < ds + lr) {
        ds = b->mm[i];  is = i - m_ofs;  ys = 2;
      }
    }
  }

#else
  if (sm <= tm) {
    int h;
    h = blog(sm ^ tm);

    m_ofs = b->m_ofs;
    sm += m_ofs;  tm += m_ofs;

    if (opt & OPT_MAX) {
      if (b->mM[sm] + lr > ds) {
        ds = b->mM[sm];  is = sm;  ys = 2;
      }
      for (i=0; i<=h-2; i++) {
        j = sm>>i;
        if ((j&1) == 0) {
          if (b->mM[j+1] + lr > ds) {
            ds = b->mM[j+1];  is = j+1;  ys = 2;
          }
        }
      }
      for (i=h-2; i>=0; i--) {
        j = tm>>i;
        if ((j&1)==1) {
          if (b->mM[j-1] + lr > ds) {
            ds = b->mM[j-1];  is = j-1;  ys = 2;
          }
        }
      }
      if (b->mM[tm] + lr > ds) {
        ds = b->mM[tm];  is = tm;  ys = 2;
      }
      if (ys == 2) {
        while (is < m_ofs) {
          is <<= 1;
          if (b->mM[is+1] + lr > b->mM[is]) is++;
        }
        is -= m_ofs;
      }
    } else { // MIN
      if (b->mm[sm] < ds + lr) {
        ds = b->mm[sm];  is = sm;  ys = 2;
      }
      for (i=0; i<=h-2; i++) {
        j = sm>>i;
        if ((j&1) == 0) {
          if (b->mm[j+1] < ds + lr) {
            ds = b->mm[j+1];  is = j+1;  ys = 2;
          }
        }
      }
      for (i=h-2; i>=0; i--) {
        j = tm>>i;
        if ((j&1)==1) {
          if (b->mm[j-1] < ds + lr) {
            ds = b->mm[j-1];  is = j-1;  ys = 2;
          }
        }
      }
      if (b->mm[tm] < ds + lr) {
        ds = b->mm[tm];  is = tm;  ys = 2;
      }
      if (ys == 2) {
        while (is < m_ofs) {
          is <<= 1;
          if (b->mm[is+1] < b->mm[is] + lr) is++;
        }
        is -= m_ofs;
      }
    }
  }

#endif

  ////////////////////////////////////////////////////////////
  // search the bp_MB of t
  ////////////////////////////////////////////////////////////


  ts = max(SBid(MBfirst(t)),SBid(s)+1);
  il = SBid(SBfirst(t)-1);
  dm = ds;
  j = rmq_MB(b,ts,il,opt,&dm);
  //if (dm < ds + lr) {
  if (j >= 0) {
    ds = dm;  is = j;  ys = 1;
  }

  ////////////////////////////////////////////////////////////
  // search the bp_SB of t
  ////////////////////////////////////////////////////////////

  i = SBfirst(t);
  j = rmq_SB(b,i,t,opt,&dm);
  d = bp_depth(b,i) + dm;
  if (opt & OPT_MAX) {
    if (d + lr > ds) {
      ds = d;  is = j;  ys = 0;
    }
  } else {
    if (d < ds + lr) {
      ds = d;  is = j;  ys = 0;
    }
  }

  ////////////////////////////////////////////////////////////
  // search the rest
  ////////////////////////////////////////////////////////////

  if (ys == 2) {
    ss = SBid(is << bp_logMB);
    il = SBid(MBlast(is << bp_logMB));
    if (opt & OPT_MAX) {
      dm = -n-1;
    } else {
      dm = n+1;
    }
    j = rmq_MB(b,ss,il,opt,&dm);
    ds = dm;  is = j;  ys = 1;
  }

  if (ys == 1) {
    ss = is << bp_logSB;
    il = SBlast(is << bp_logSB);
    is = rmq_SB(b,ss,il,opt,&dm);
    //ds = bp_depth(b,ss) + dm;  ys = 0;
  }

  return is;
}

