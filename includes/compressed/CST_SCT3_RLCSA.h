//
// Created by elarielcl on 11/12/18.
//

#ifndef BLOCKTREE_CST_SCT3_RLCSA_H
#define BLOCKTREE_CST_SCT3_RLCSA_H


#include <compressed/BPSPCBlockTree.h>
#include <compressed/PBTCTree.h>
#include <LCP_RLCSA.h>
#include <rlcsa/rlcsa.h>
#include <sdsl/cst_sada.hpp>
#include <sdsl/suffix_tree_helper.hpp>

class CST_SCT3_RLCSA {
public:
    sdsl::bp_interval<uint64_t> node_;
    int64_t i_ = -2;
    int64_t psi_ = -2;

    TextIndexes::RLCSA* rlcsa_;
    TextIndexRLCSA* index_;
    //PCBlockTree* input_block_tree_;
    cds_static::LCP_RLCSA* lcp_rlcsa_;

    uint64_t sigma;
    sdsl::int_vector<64> C;
    sdsl::int_vector<8> char2comp;

    sdsl::bit_vector* pm_bp;
    sdsl::bit_vector* pm_first_child;
    sdsl::rank_support_v5<>* pm_first_child_rank;
    sdsl::bit_vector::select_1_type* pm_first_child_select;
    sdsl::bp_support_sada<>* pm_bp_support; // support for the balanced parentheses sequence

    //std::unordered_map<int, std::pair<int, int>> C_; //UNUSED?

    int n_;
    //int pa_;
    //int pa2_;

    CST_SCT3_RLCSA(std::string&, int = 32, int = 128);
    CST_SCT3_RLCSA(std::string&, std::string&, int = 32, int = 128);
    ~CST_SCT3_RLCSA();


    int access(int);

    sdsl::bp_interval<uint64_t> root();
    int is_leaf(sdsl::bp_interval<uint64_t>);
    sdsl::bp_interval<uint64_t> select_leaf(int);
    int string_depth(sdsl::bp_interval<uint64_t>);
    int node_depth(sdsl::bp_interval<uint64_t>);
    int size(sdsl::bp_interval<uint64_t>);
    sdsl::bp_interval<uint64_t> leftmost_leaf(sdsl::bp_interval<uint64_t>);
    sdsl::bp_interval<uint64_t> rightmost_leaf(sdsl::bp_interval<uint64_t>);
    int lb(sdsl::bp_interval<uint64_t>);
    int rb(sdsl::bp_interval<uint64_t>);
    sdsl::bp_interval<uint64_t> parent(sdsl::bp_interval<uint64_t>);
    sdsl::bp_interval<uint64_t> next_sibling(sdsl::bp_interval<uint64_t>);
    int string(sdsl::bp_interval<uint64_t>, int);
    std::string string(sdsl::bp_interval<uint64_t>, int, int);
    sdsl::bp_interval<uint64_t>  labeled_child(sdsl::bp_interval<uint64_t> , int);
    sdsl::bp_interval<uint64_t>  select_child(sdsl::bp_interval<uint64_t> , int);
    sdsl::bp_interval<uint64_t>  edge(sdsl::bp_interval<uint64_t> , int);
    sdsl::bp_interval<uint64_t>  lca(sdsl::bp_interval<uint64_t> , sdsl::bp_interval<uint64_t> );
    sdsl::bp_interval<uint64_t> suffix_link(sdsl::bp_interval<uint64_t>);
    //sdsl::bp_interval<uint64_t> suffix_link(sdsl::bp_interval<uint64_t>, int);
    //int psi(int);
    //int weiner_link(int, int);
    int sn(sdsl::bp_interval<uint64_t>);
    int id(sdsl::bp_interval<uint64_t>);
    sdsl::bp_interval<uint64_t> inv_id(int);
    int nodes();
    sdsl::bp_interval<uint64_t> node(int, int);
    int count(sdsl::bp_interval<uint64_t>);
    sdsl::bp_interval<uint64_t> locus(std::string);
    sdsl::bp_interval<uint64_t> string_ancestor(sdsl::bp_interval<uint64_t>, int);
    sdsl::bp_interval<uint64_t> level_ancestor(sdsl::bp_interval<uint64_t>, int);
    std::vector<int> locate(int);
    void print_statistics();

    // Position of the first l-index of a l-[i,j] interval in the BP.
    /* \par Time complexity
     *   \f$ \Order{1} \f$
     */
    inline uint64_t closing_pos_of_first_l_index(const sdsl::bp_interval<uint64_t>& node)const
    {
        if (node.cipos > node.jp1pos) { // corresponds to m_lcp[i] <= m_lcp[j+1]
            return node.jp1pos-1;
        } else { // corresponds to m_lcp[i] > m_lcp[j+1]
            return node.cipos-1;
        }
    }


    //! Get the number of children of a node v.
    /*!
     * \param v A valid node v.
     * \returns The number of children of node v.
     *  \par Time complexity
     *    \f$ \Order{\frac{\sigma}{w}} \f$, where w=64 is the word size,
     *    can be implemented in \f$\Order{1}\f$ with rank and select.
     */
    uint64_t degree(const sdsl::bp_interval<uint64_t>& v)
    {
        auto& m_bp_support = *pm_bp_support;
        auto& m_first_child = *pm_first_child;
        auto& m_first_child_rank = *pm_first_child_rank;
        auto& m_first_child_select = *pm_first_child_select;
        if (is_leaf(v))  // if v is a leave, v has no child
            return 0;
        // v is not a leave: v has at least two children
        uint64_t r = closing_pos_of_first_l_index(v);
        uint64_t r0 = r - m_bp_support.rank(r);
        const uint64_t* p = m_first_child.data() + (r0>>6);
        uint8_t offset = r0&0x3F;

        uint64_t w = (*p) & sdsl::bits::lo_set[offset];
        if (w) { // if there is a bit set in the current word
            return offset-sdsl::bits::hi(w)+1;
        } else if (m_first_child.data() == p) { // no bit set and we are in the first word
            return offset+2; // since would have to be bits::hi(w)=-1, child marked in previous word
        } else {
            uint64_t res = offset+2;
            int steps = 4;
            // search in previous four words for result
            while (p > m_first_child.data() and steps-- > 0) {
                w = *(--p);
                if (0 == w)
                    res += 64;
                else {
                    return res + (63-sdsl::bits::hi(w));
                }
            }
            // if not found: use rank + select to answer query
            auto goal_rank = m_first_child_rank(r0);
            if (goal_rank == 0) {
                return r0+2;
            } else {
                return r0-m_first_child_select(goal_rank)+1;
            }
        }
    }

    // Get the i-th l-index of a node
    // if there exists no ith l-index return node.j+1
    /* \param v Node
     * \param i l-index in [1..degree()]
     * \paran
     */
    uint64_t select_l_index(const sdsl::bp_interval<uint64_t>& v, uint64_t i, uint64_t& kpos, uint64_t& ckpos)
    {
        auto& m_bp_support = *pm_bp_support;
        auto& m_bp = *pm_bp;
        if (v.cipos > v.jp1pos) { // corresponds to m_lcp[i] <= m_lcp[j+1]
            ckpos    = v.jp1pos-1;
        } else { // corresponds to m_lcp[i] > m_lcp[j+1]
            ckpos    = v.cipos-1;
        }
        if (1 == i) {
            kpos    = m_bp_support.find_open(ckpos);
            return m_bp_support.rank(kpos)-1;
        } else { // i > 1
            // numbers of closing parentheses - 1 = index of first child in m_first_child
            uint64_t r = ckpos - m_bp_support.rank(ckpos);
            if (r+1 >= i) { // if there exist more than i l-indices
                // check if m_first_child[r-i+1..r-1] consists of zeros
                if (i < degree(v)) {  // there exists an i-th l-index
                    ckpos -= (i-1);
                    assert(m_bp[ckpos] == 0);
                    kpos   = m_bp_support.find_open(ckpos);
                    return m_bp_support.rank(kpos)-1;
                }
            }
            // if i >= degree(node)
            kpos = v.jp1pos;
            if (kpos < m_bp.size())
                ckpos = m_bp_support.find_close(kpos);
            else
                ckpos = m_bp.size();
            return v.j+1;
        }
    }

    // Get the previous smaller value.
    /*
     * \param i      Position in the original vector.
     * \param ipos   Corresponding opening parenthesis in m_bp
     * \param cipos  Corresponding closing parenthesis to ipos
     * \par Time complexity
     *    \f$ \Order{\frac{\sigma}{w}} \f$, where w=64 is the word size,
     *    can be implemented in \f$\Order{1}\f$ with rank and select.
     */
    inline uint64_t psv(SDSL_UNUSED uint64_t i, uint64_t ipos,
                        uint64_t cipos, uint64_t& psvpos,
                        uint64_t& psvcpos)const
    {
        auto& m_bp = *pm_bp;
        auto& m_bp_support = *pm_bp_support;
        auto& m_first_child = *pm_first_child;
        auto& m_first_child_rank = *pm_first_child_rank;
        auto& m_first_child_select = *pm_first_child_select;

        // if lcp[i]==0 => psv is the 0-th index by definition
        if ((cipos + (uint64_t)sigma) >= m_bp.size()) {
            psvpos = 0;
            psvcpos = m_bp.size()-1;
            return 0;
        }
        if (m_bp[cipos+1]) {
            psvpos = m_bp_support.enclose(ipos);
            psvcpos = m_bp_support.find_close(psvpos);
            return m_bp_support.rank(psvpos)-1;
        }
        // r0 = index of clothing parenthesis in m_first_child
        uint64_t r0 = cipos - m_bp_support.rank(cipos);
        uint64_t next_first_child = 0;
        const uint64_t* p = m_first_child.data() + (r0>>6);
        uint64_t w = (*p) >> (r0&0x3F);
        if (w) { // if w!=0
            next_first_child = cipos + sdsl::bits::lo(w);
            if (cipos == next_first_child and m_bp[next_first_child+1]) {
                psvpos = m_bp_support.enclose(ipos);
                psvcpos = m_bp_support.find_close(psvpos);
                return m_bp_support.rank(psvpos)-1;
            }
        } else {
            uint64_t delta = 63-(r0&0x3F);
            ++p;
            int steps = 4;
            while (!(w=*p) and steps-- > 0) { // while w==0
                ++p;
                delta += 64;
            }
            if (w != 0) {
                delta += sdsl::bits::lo(w) + 1;
            } else {
                auto pos = m_first_child_select(m_first_child_rank(r0+1)+1);
                delta    = pos - r0;
            }
            next_first_child = cipos + delta;
        }
        if (!m_bp[next_first_child+1]) { // if next parenthesis is a closing one
            psvcpos = next_first_child+1;
            psvpos = m_bp_support.find_open(psvcpos);
            return m_bp_support.rank(psvpos)-1;
        } else {
            psvpos = m_bp_support.enclose(m_bp_support.find_open(next_first_child));
            psvcpos = m_bp_support.find_close(psvpos);
            return m_bp_support.rank(psvpos)-1;
        }
    }

    // Get the next smaller value.
    /*
     * \param i    Position in the original vector.
     * \param ipos Position of the corresponding opening parenthesis in BP.
     * \return Position of the next smaller value in [i+1..n-1], and n when
     *         no such value exists.
     * \par Time complexity
     *      \f$ \Order{1} \f$
     */
    // possible optimization: calculate also position of nsv,
    // i.e. next ( following position cipos
    inline uint64_t nsv(SDSL_UNUSED uint64_t i, uint64_t ipos)const
    {
        auto& m_bp_support = *pm_bp_support;
        uint64_t cipos = m_bp_support.find_close(ipos);
        uint64_t result = m_bp_support.rank(cipos);
        return result;
    }


    //! Get the child w of node v which edge label (v,w) starts with character c.
    /*!
     * \param v        A valid tree node of the cst.
     * \param c        First character on the edge label.
     * \param char_pos Reference which will hold the position (0-based) of
     *                 the matching char c in the sorted text/suffix array.
     * \return The child node w which edge label (v,w) starts with c or
     *         root() if it does not exist.
     * \par Time complexity
     *   \f$ \Order{(\saaccess+\isaaccess) \cdot \log\sigma + \lcpaccess} \f$
     */
    sdsl::bp_interval<uint64_t> child(const sdsl::bp_interval<uint64_t>& v, const int c, uint64_t& char_pos)
    {
        auto& m_bp_support = *pm_bp_support;
        auto& m_bp = *pm_bp;
        if (is_leaf(v))  // if v is a leaf = (), v has no child
            return root();
        // else v = ( (     ))
        uint8_t cc = char2comp[c];
        if (cc==0 and c!=0) // TODO: change char2comp so that we don't need this special case
            return root();
        uint64_t char_ex_max_pos = C[((uint64_t)1)+cc], char_inc_min_pos = C[cc];

        uint64_t d            = string_depth(v);

//            (1) check the first child
        char_pos = rlcsa_->getPsi(v.i,d);
        if (char_pos >= char_ex_max_pos) {// the first character of the first child interval is lex. greater than c
            // => all other first characters of the child intervals are also greater than c => no solution
            return root();
        } else if (char_pos >= char_inc_min_pos) { // i.e. char_pos < char_ex_max_pos and char_pos >= char_inc_min_pos
            return select_child(v, 1);
        }

        uint64_t child_cnt     = degree(v);

//            (2) check the last child
        char_pos = rlcsa_->getPsi(v.j,d);
        if (char_pos < char_inc_min_pos) {// the first character of the last child interval is lex. smaller than c
            // =>    all other first characters of the child intervals are also smaller than c => no solution
            return root();
        } else if (char_pos < char_ex_max_pos) { // i.e. char_pos < char_ex_max_pos and char_pos >= char_inc_min_pos
            return select_child(v, child_cnt);
        }

//             (3) binary search for c in the children [2..children)
        uint64_t l_bound = 2, r_bound = child_cnt, mid, kpos, ckpos, l_index;
        while (l_bound < r_bound) {
            mid = (l_bound + r_bound) >> 1;

            l_index = select_l_index(v, mid-1, kpos, ckpos);
            char_pos = rlcsa_->getPsi(l_index,d);

            if (char_inc_min_pos > char_pos) {
                l_bound = mid+1;
            } else if (char_ex_max_pos <= char_pos) {
                r_bound = mid;
            } else { // char_inc_min_pos <= char_pos < char_ex_max_pos => found child
                // we know that the child is not the last child, see (2)
                // find next l_index: we know that a new l_index exists: i.e. assert( 0 == m_bp[ckpos-1]);
                uint64_t lp1_index = m_bp_support.rank(m_bp_support.find_open(ckpos-1))-1;
                uint64_t jp1pos = m_bp.size();
                if (lp1_index-1 < n_-1) {
                    jp1pos = m_bp_support.select(lp1_index+1);
                }
                return sdsl::bp_interval<uint64_t>(l_index, lp1_index-1, kpos, ckpos, jp1pos);
            }
        }
        return root();
    }

    // Range minimum query based on the rr_enclose method.
    /* \par Time complexity
     *   \f$ \Order{\rrenclose} \f$
     */
    inline uint64_t rmq(uint64_t l, uint64_t r)const
    {
        auto& m_bp_support = *pm_bp_support;
        uint64_t i     = m_bp_support.select(l+1);
        uint64_t j     = m_bp_support.select(r+1);
        uint64_t fc_i     = m_bp_support.find_close(i);
        if (j < fc_i) { // i < j < find_close(j) < find_close(i)
            return l;
        } else { // i < find_close(i) < j < find_close(j)
            uint64_t ec = m_bp_support.rr_enclose(i,j);
            if (ec == m_bp_support.size()) {// no restricted enclosing pair found
                return r;
            } else { // found range restricted enclosing pair
                return m_bp_support.rank(ec)-1; // subtract 1, as the index is 0 based
            }
        }
    }

};

#endif //BLOCKTREE_CST_SCT3_RLCSA_H
