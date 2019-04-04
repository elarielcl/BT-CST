#ifndef BP_H_
#define BP_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <trees/bp-darray.h>
#include <trees/bp-utils.h>

#define OP 1
#define CP 0

#define OPT_MIN 0
#define OPT_MAX 1
#define OPT_LEFT 0
#define OPT_RIGHT 2

#define OPT_LEAF (1 << 0)
#define OPT_INORDER (1 << 1)
#define OPT_DEGREE (1 << 2)
#define OPT_FAST_PREORDER_SELECT (1 << 3)
#define OPT_FAST_LEAF_SELECT (1 << 4)
#define OPT_FAST_INORDER_SELECT (1 << 5)
#define OPT_FAST_POSTORDER_SELECT (1 << 6)
#define OPT_DFUDS_LEAF (1 << 7)
#define OPT_FAST_DFUDS_LEAF_SELECT (1 << 8)

//#define bp_logSB 9
#define bp_logSB 7
//#define bp_logSB 2
#define bp_SB (1<<bp_logSB)
//#define bp_logMB 16
//#define bp_logMB 12
#define bp_logMB 15
//#define bp_logMB 3
#define bp_MB (1<<bp_logMB)

#define bp_ETW 8   // width of excess lookup table
#define bp_W1 2    // branching factor



typedef struct {
  int n;
  bp_pb *B;
  darray *da;
  bp_byte *sm, *sM;
  bp_byte *sd;
  int *mm, *mM;
  int *md;
  int m_ofs;

  darray *da_leaf;
  darray *da_inorder;
  darray *da_postorder;
  darray *da_dfuds_leaf;
  int idx_size;
  int opt;
} bp;


bp_pb getpat_preorder(bp_pb *b);
bp_pb getpat_leaf(bp_pb *b);
bp_pb getpat_inorder(bp_pb *b);
bp_pb getpat_postorder(bp_pb *b);

bp * bp_construct(int n, bp_pb *B, int opt);
void bp_print(bp *b, int s, int t);

int bp_fwd_excess(bp *b,int s, int rel);
int bp_bwd_excess(bp *b,int s, int rel);
int bp_rmq(bp *b, int s, int t, int opt);
int bp_depth(bp *b, int s);

int bp_rank_open(bp *b, int s);
int bp_rank_close(bp *b, int s);
int bp_select_open(bp *b, int s);
int bp_select_close(bp *b, int s);

static inline uint bp_getD(){
  return bp_D;
}

static inline int bp_root_node(bp *b)
{
  return 0;
}

///////////////////////////////////////////
//  inspect(bp *b, int s)
//    returns OP (==1) or CP (==0) at s-th bit (0 <= s < n)
///////////////////////////////////////////
static inline int bp_inspect(bp *b, int s)
{
  return bp_getbit(b->B, s);
}

///////////////////////////////////////////
//  find_close(bp *b,int s)
//    returns the matching close parenthesis of s
///////////////////////////////////////////
static inline int bp_find_close(bp *b,int s)
{
  int s1 = s + 1;
  if (bp_inspect(b, s1) == CP)
    return s1;
  else
    return bp_fwd_excess(b, s, -1);
}

///////////////////////////////////////////
//  bp_find_open(bp *b,int s)
//    returns the matching open parenthesis of s
///////////////////////////////////////////
static inline int bp_find_open(bp *b,int s)
{
  int r = bp_bwd_excess(b, s, 0);
  return (r >= -1) ? r+1 : -1;
}


///////////////////////////////////////////
//  bp_parent(bp *b,int s)
//    returns the parent of s
//            -1 if s is the root
///////////////////////////////////////////
static inline int bp_parent(bp *b, int s)
{
  int r;
  if (bp_getbit(b->B, s - 1) == OP) return s - 1;
  r = bp_bwd_excess(b,s,-2);
  return (r >= -1) ? r+1 : -1;
}

///////////////////////////////////////////
//  bp_parent_close(bp *b,int s)
//    returns the closing parenthesis of the parent
//    of s
//            -1 if s is the root
///////////////////////////////////////////

static inline int bp_parent_close(bp *b, int s)
{
  return bp_fwd_excess(b,s,-2);
}


static inline int bp_enclose(bp *b, int s)
{
  return bp_parent(b, s);;
}


///////////////////////////////////////////
//  bp_level_ancestor(bp *b,int s,int d)
//    returns the ancestor of s with relative depth d (d < 0)
//            -1 if no such node
///////////////////////////////////////////
static inline int bp_level_ancestor(bp *b,int s,int d)
{
  int r = bp_bwd_excess(b,s,d-1);
  return (r >= -1) ? r+1 :-1;
}

///////////////////////////////////////////
//  bp_lca(bp *b, int s, int t)
//    returns the lowest common ancestor of s and t
///////////////////////////////////////////
static inline int bp_lca(bp *b, int s, int t)
{
  return bp_parent(b, bp_rmq(b,s,t,0)+1);
}

///////////////////////////////////////////
//  preorder_rank(bp *b,int s)
//    returns the preorder (>= 1) of node s (s >= 0)
///////////////////////////////////////////

static inline int bp_preorder_rank(bp *b,int s)
{
  return bp_darray_rank(b->da,s);
}

///////////////////////////////////////////
//  preorder_select(bp *b,int s)
//    returns the node with preorder s (s >= 1)
//            -1 if no such node
///////////////////////////////////////////
static inline int bp_preorder_select(bp *b,int s)
{
  //  no error handling
  if (b->opt & OPT_FAST_PREORDER_SELECT) {
    return bp_darray_select(b->da,s,1);
  } else {
    return bp_darray_select_bsearch(b->da,s, getpat_preorder);
  }
}


int bp_postorder_rank(bp *b,int s);

static inline int bp_isleaf(bp *b, int s)
{
  return (bp_inspect(b, s+1) == CP);
}

///////////////////////////////////////////
//  bp_subtree_size(bp *b, int s)
//    returns the number of nodes in the subtree of s
///////////////////////////////////////////
static inline int bp_subtree_size(bp *b, int s)
{
  return (bp_find_close(b, s) - s + 1) / 2;
}

///////////////////////////////////////////
//  first_child(bp *b, int s)
//    returns the first child
//            -1 if s is a leaf
///////////////////////////////////////////

static inline int bp_first_child(bp *b, int s)
{
  int s1 = s + 1;
  return bp_inspect(b, s1) ? s1 : -1;
}


///////////////////////////////////////////
//  next_sibling(bp *b,int s)
//    returns the next sibling of parent(s)
//            -1 if s is the last child
//////////////////////////////////////////
static inline int bp_next_sibling(bp *b, int s)
{
  int t;
  t = bp_find_close(b, s) + 1;
  return bp_inspect(b, t) ? t : -1;

}

int bp_prev_sibling(bp *b, int s);
int bp_deepest_node(bp *b,int s);
int bp_subtree_height(bp *b,int s);
///////////////////////////////////////////
//  bp_is_ancestor(bp *b, int s, int t)
//     returns 1 if s is an ancestor of t
//             0 otherwise
///////////////////////////////////////////
static inline int bp_is_ancestor(bp *b, int s, int t)
{
  int v;
  if (s == 0) return 1;
  v = bp_find_close(b, s);
  return (s <= t && t <= v);
}

int bp_distance(bp *b, int s, int t);
int bp_level_lefthmost(bp *b, int d);
int bp_level_rigthmost(bp *b, int d);
int bp_degree(bp *b,int s);

// not efficient
int bp_naive_degree(bp *b, int s);
int bp_naive_child(bp *b, int s, int d);
int bp_naive_child_rank(bp *b, int t);
int bp_naive_rmq(bp *b, int s, int t,int opt);
int bp_postorder_select(bp *b,int s);

// using preorder select index
int preorder_select(bp *b,int s);

// using leaf index
int bp_leaf_rank(bp *b,int s);
int bp_leaf_size(bp *b, int s);
int bp_leftmost_leaf(bp *b, int s);
int bp_rightmost_leaf(bp *b, int s);

// using leaf select index
int bp_leaf_select(bp *b,int s);

// using inorder index
int bp_inorder_rank(bp *b,int s);

// using inorder select index
int bp_inorder_select(bp *b,int s);

// using degree index
int bp_fast_degree(bp *b,int s, int t, int ith);
int bp_child_rank(bp *b, int t);
int bp_child(bp *b, int s, int d);


// new functions for persistence purposes, added by Diego Arroyuelo
void saveTree(bp *b, FILE *fp);
bp * loadTree(FILE *fp);

//0: success 1: failure (errno)
int bp_save(bp *b, int fd);

//non-null: sucess, null: failure (errno)

bp * bp_load(int fd);
void bp_delete(bp *b);


static inline int blog(int x)
{
  int l;
  l = 0;
  while (x>0) {
    x>>=1;
    l++;
  }
  return l;
}




extern int *matchtbl,*parenttbl;

extern int fwdtbl[(2*bp_ETW+1)*(1<<bp_ETW)];
extern int bwdtbl[(2*bp_ETW+1)*(1<<bp_ETW)];
extern int mintbl_li[1<<bp_ETW], mintbl_lv[1<<bp_ETW];
extern int mintbl_ri[1<<bp_ETW], mintbl_rv[1<<bp_ETW];
extern int maxtbl_li[1<<bp_ETW], maxtbl_lv[1<<bp_ETW];
extern int maxtbl_ri[1<<bp_ETW], maxtbl_rv[1<<bp_ETW];

extern int minmaxtbl_i[4][1<<bp_ETW], minmaxtbl_v[4][1<<bp_ETW];
extern int degtbl[1<<bp_ETW];
extern int degtbl2[(2*bp_ETW+1)*(1<<bp_ETW)];
extern int childtbl[(bp_ETW)*(1<<bp_ETW)];
extern int depthtbl[(2*bp_ETW+1)*(1<<bp_ETW)];
extern int childtbl2[2*bp_ETW+1][bp_ETW][(1<<bp_ETW)];

#ifdef __cplusplus
}
#endif


#endif
