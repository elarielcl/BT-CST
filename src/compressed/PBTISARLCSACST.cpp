//
// Created by elarielcl on 26/09/18.
//

#include <compressed/PBTISARLCSACST.h>
#include <sdsl/suffix_trees.hpp>
#include <compressed/PBTCLCPA.h>
#include <compressed/PBTCISA.h>

PBTISARLCSACST::PBTISARLCSACST(std::string& input_string,  int block_tree_version_top, int r_top, int mll_top, int c_top, int block_tree_version_isa, int r_isa, int mll_isa, int c_isa) {
    sdsl::cst_sada<> cst;
    construct_im(cst, input_string, 1);
    std::string topology;
    for (int i = 0; i < cst.bp.size(); ++i) {
        if (cst.bp[i]) topology += "(";
        else topology += ")";
    }


    pbtcTree_ = new PBTCTree(topology, block_tree_version_top, r_top, mll_top, c_top);
    PBTCLCPA* pbtcLCPA_ = new PBTCLCPA(input_string); // FIX THIS
    pbtcISA_ = new PBTCISA(input_string, block_tree_version_isa, r_isa, mll_isa, c_isa); // FIX THIS
    pa_ = pbtcISA_->access(0);
    pa2_ = pbtcISA_->access(1);


    TextIndexes::Parameters parameters;
    parameters.set(TextIndexes::RLCSA_BLOCK_SIZE);
    parameters.set(TextIndexes::SAMPLE_RATE);
    parameters.set(TextIndexes::SUPPORT_LOCATE);
    parameters.set(TextIndexes::SUPPORT_DISPLAY);
    parameters.set(TextIndexes::WEIGHTED_SAMPLES);

    unsigned char* data = new unsigned char[input_string.size()+1];
    for (int i = 0; i < input_string.size(); ++i) {
        data[i] = input_string[i];
    }
    data[input_string.size()] = 0;

    //std::cout << data << std::endl;


    rlcsa_ = new TextIndexes::RLCSA(data, input_string.size()+1, parameters.get(TextIndexes::RLCSA_BLOCK_SIZE), parameters.get(TextIndexes::SAMPLE_RATE), false, true);
    index_ = new TextIndexRLCSA();

    index_->rlcsa = rlcsa_;

    //delete[] data;
    data = new unsigned char[input_string.size()+1];
    for (int i = 0; i < input_string.size(); ++i) {
        data[i] = input_string[i];
    }
    data[input_string.size()] = 0;
    lcp_rlcsa_ = new LCP_FMN_RLCSA(rlcsa_, (char*)data, input_string.size()+1);

    input_string += "$";
    n_ = input_string.size();

    /*
    PBlockTree* bt = new PBlockTree(input_string, r, mll);
    switch (block_tree_version) {
        case PAPER:
            bt->process_back_pointers();
            bt->clean_unnecessary_expansions();
            break;
        case PAPER_NO_CLEAN:
            bt->process_back_pointers();
            break;
        case PRUNIG_C:
            bt->process_back_pointers();
            bt->clean_unnecessary_expansions(c);
            break;
        case HEURISTIC:
            bt->process_back_pointers_heuristic();
            break;
        case LIBERAL:
            bt->process_back_pointers_liberal_heuristic();
            break;
        case CONSERVATIVE:
            bt->process_back_pointers_conservative_heuristic();
            break;
    }

    //bt->add_rank_select_support('('); //REMOVETHIS!
    input_block_tree_ = new PCBlockTree(bt);

    delete bt;
*/




    /*
    for (int i = 0; i < input_string.size(); ++i) {
        std::cout << rlcsa_->psi(i) << ", ";
    }
    std::cout << std::endl;
*/


    std::set<char> characters;
    for (int i = 0; i < input_string.size(); ++i) {
        characters.insert(input_string[i]);
    }

    std::vector<int> alphabet_;
    for (char c : characters)
        alphabet_.push_back(c);

/*
    for (int i = 1; i < input_string.size(); ++i) {
        std::cout << pbtcLCPA_->access(i) << ", ";
    }
    std::cout << std::endl;
*/



    std::pair<int, int> positions;
    positions.first = 0;
    int counter = 0;
    for (int i = 1; i < input_string.size(); ++i) {
        if (pbtcLCPA_->access(i) == 0) {
            positions.second = i-1;
            C_[alphabet_[counter++]] = positions;
            positions.first = i;
        }
    }
    positions.second = input_string.size()-1;
    C_[alphabet_[counter]] = positions;
    delete pbtcLCPA_;
    //delete[] data;
/*
    for (auto it : C_) {
        std::cout << (char)it.first << ": " << it.second.first << ", " << it.second.second << std::endl;
    }
*/
}


PBTISARLCSACST::~PBTISARLCSACST() {
    //delete input_block_tree_;
    delete lcp_rlcsa_;
    delete rlcsa_;
    delete pbtcTree_;
    delete pbtcISA_;
}

int PBTISARLCSACST::access(int i) {
    //return input_block_tree_->access(i);
    int r = rlcsa_->getT(pbtcISA_->access(i));
    if (r == 0) return '$';
    return r;
}

int PBTISARLCSACST::root() {
    return 0;
}

int PBTISARLCSACST::is_leaf(int node) {
    return pbtcTree_->is_leaf(node);
}

int PBTISARLCSACST::select_leaf(int i) {
    return pbtcTree_->leaf_select(i);
}

int PBTISARLCSACST::string_depth(int node) {
    if (node == 0) return 0;
    /*
    if (pbtcTree_->is_leaf(node)) {
        int i = pbtcTree_->leaf_rank(node);
        return n_ - rlcsa_->locate(i-1);
    }*/
    int lr = 0;
    if (pbtcTree_->is_leaf_rank(node, lr)) {
        return n_ - rlcsa_->locate(lr-1);
    }

    int sc = pbtcTree_->bps_->my_fwdsearch(node+1, -1) + 1;
    return lcp_rlcsa_->get_LCP(pbtcTree_->leaf_rank(sc)-1, index_);
}

int PBTISARLCSACST::node_depth(int node) {
    return pbtcTree_->depth(node);
}

int PBTISARLCSACST::size(int node) {
    return pbtcTree_->number_of_leaves(node);
}

int PBTISARLCSACST::leftmost_leaf(int node) {
    if (pbtcTree_->is_leaf(node)) return node;

    int lr = pbtcTree_->leaf_rank(node);
    return pbtcTree_->leaf_select(lr);
}

int PBTISARLCSACST::rightmost_leaf(int node) {
    if (pbtcTree_->is_leaf(node)) return node;

    int c = pbtcTree_->bps_->my_fwdsearch(node, -1);
    int lr = pbtcTree_->leaf_rank(c);
    return pbtcTree_->leaf_select(lr-1);
}

int PBTISARLCSACST::lb(int node) {
    return pbtcTree_->leaf_rank(node)-1;
}

int PBTISARLCSACST::rb(int node) {
    int c = pbtcTree_->bps_->my_fwdsearch(node, -1);
    return pbtcTree_->leaf_rank(c) - 1;
}

int PBTISARLCSACST::parent(int node) {
    return pbtcTree_->parent(node);
}

int PBTISARLCSACST::next_sibling(int node) {
    return pbtcTree_->next_sibling(node);
}

int PBTISARLCSACST::string(int node, int i) {
    int lr = pbtcTree_->leaf_rank(node);
    //int r = rlcsa_->getPsi(lr,i);
    //if (r == 23)
    //    ++i;
    //if (r == 23 || r == 0) return '$';
    //return rlcsa_->getT(r);

    int sa = rlcsa_->locate(lr-1);
    if (sa+i >= n_) return -1;
    return access(sa + i);

}

std::string PBTISARLCSACST::string(int node, int i, int j) { //Maybe path extraction would be util here
    int lr = pbtcTree_->leaf_rank(node);
    int sa = rlcsa_->locate(lr-1);
    std::string output = "";
    while (i <= j) {
        output += access(sa+i);
        i++;
    }
    return output;
}

int PBTISARLCSACST::labeled_child(int node, int c) {
    int sdepth = string_depth(node);
    int child = node+1;
    while (true) {
        int ch = string(child, sdepth);
        if (ch > c) return -1;
        if (ch == c) return child;
        child = pbtcTree_->next_sibling(child);
    }
}

int PBTISARLCSACST::select_child(int node, int t) {
    int child = node + 1;
    while (--t) {
        child = pbtcTree_->next_sibling(child);
    }
    return child;
}

int PBTISARLCSACST::edge(int node, int i) {
    int parent_depth = string_depth(parent(node));
    return string(node, parent_depth+i);
}

int PBTISARLCSACST::lca(int node1, int node2) {
    return pbtcTree_->lca(node1, node2);
}

int PBTISARLCSACST::suffix_link(int node) {
    if (node == root()) return root();

    if (is_leaf(node)) {
        int lml = lb(node);
        int psi_lml = psi(lml);
        if (psi_lml == pa_) return  root();
        return select_leaf(psi_lml+1);
    }

    int lml = lb(node);
    int psi_lml = psi(lml);

    int rml = rb(node);
    int psi_rml = psi(rml-1);

    if (lml == 0 && rml == 1) return root();
    return lca(select_leaf(psi_lml+1), select_leaf(psi_rml+1));
}

int PBTISARLCSACST::suffix_link(int node, int i) {

    if (node == root()) return root();

    int lml = lb(node);
    int next_suf_lml = rlcsa_->locate(lml) + i;
    if (next_suf_lml >= n_) return root();
    int psi_lml = pbtcISA_->access(next_suf_lml);

    int rml = rb(node);
    int next_suf_rml = rlcsa_->locate(rml-1) + i;
    if (next_suf_rml >= n_) return root();
    int psi_rml = pbtcISA_->access(next_suf_rml);

    return lca(select_leaf(psi_lml+1), select_leaf(psi_rml+1));

    /*
    i--;
    int lml = lb(node);
    int psi_lml = psi(lml);

    int rml = rb(node);
    int psi_rml = psi(rml-1);

    while (i--) {
        psi_lml = psi(psi_lml);
        psi_rml = psi(psi_rml);
        if (psi_lml == pa_ && psi_rml == pa_)  return root();
    }

    return lca(select_leaf(psi_lml+1), select_leaf(psi_rml+1));
     */
}

int PBTISARLCSACST::psi(int i) {
    int r = rlcsa_->psi(i) -1;
    return r == -1 ? pa_ : r;
}

int PBTISARLCSACST::weiner_link(int node, int c) { //REIMPLEMENT WITH rlcsa

    int s = lb(node);
    int e = rb(node)-1;
/*
    int new_s = rlcsa_->LF(s, c)-1;
    int new_e = rlcsa_->LF(e, c)-1;
*/

    int inf = C_[c].first;
    int sup = C_[c].second;

    int new_s = inf; // Need to find the succesor of s
    int new_e = sup; // Need to find the predecesor of e

    int lo = inf;
    int hi = sup;
    while (true) {
        if (lo > hi) return root();

        int mid = lo + (hi - lo) / 2;
        int item = psi(mid);

        if (s < item) {
            hi = mid;
            if (lo + 1 >= hi) {
                int p = psi(lo);
                if (s <= p) {
                    new_s = lo;
                    break;
                } else {
                    lo = lo+1;
                }
            }
        }
        else if (s > item) lo = mid + 1;
        else {
            new_s = mid;
            break;
        }
    }



    lo = inf;
    hi = sup;
    while (true) {
        if (lo > hi) return root();

        int mid = lo + (hi - lo) / 2;
        int item = psi(mid);
        if      (e < item) hi = mid - 1;
        else if (e > item) {
            if (lo == hi) {
                new_e = lo;
                break;
            }
            if (lo + 1 == hi) {
                int p = psi(hi);
                if (e >= p) {
                    new_e = hi;
                    break;
                } else {
                    new_e = lo;
                    break;
                }
            }
            lo = mid;
        }
        else {
            new_e = mid;
            break;
        }
    }


    if (new_s > new_e) {
        return root();
    }

    if (new_s == new_e) {
        return select_leaf(new_s+1);
    }
    int left_leaf = select_leaf(new_s+1);
    int right_leaf= select_leaf(new_e+1);
    return lca(left_leaf, right_leaf);
}


int PBTISARLCSACST::sn(int node) {
    int lr = pbtcTree_->leaf_rank(node);
    return rlcsa_->locate(lr);
}

int PBTISARLCSACST::id(int node) {
    return pbtcTree_->node_map(node)-1;
}

int PBTISARLCSACST::inv_id(int id) {
    return pbtcTree_->node_select(id+1);
}

int PBTISARLCSACST::nodes() {
    return n_/2;
}

int PBTISARLCSACST::node(int lb, int rb) {
    return lca(select_leaf(lb+1), select_leaf(rb+1));
}

int PBTISARLCSACST::degree(int node) {
    return pbtcTree_->number_of_children(node);
}

int PBTISARLCSACST::count(int node) {
    pbtcTree_->number_of_leaves(node);
}


int PBTISARLCSACST::locus(std::string pattern) {
    int init = 0;
    int end = n_-1;
    bool flag = false;

    int lo = 0;
    int hi = n_-1;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        int suffix = rlcsa_->locate(mid);

        bool less = false;
        bool equal = true;
        for (int i = 0; i < pattern.length(); ++i) {
            int c = access(i+suffix);
            if (pattern[i] < c) {
                equal = false;
                break;
            }
            else if (pattern[i] > c) {
                less = true;
                equal = false;
                break;
            }
        }

        if (equal) {
            if (hi <= lo+1) {
                init = lo;
                flag = true;
                break;
            }
            hi = mid;
        } else if (less) {
            lo = mid+1;
        } else {
            hi = mid-1;
        }
    }

    if (hi < lo && !flag) return -1;


    lo = 0;
    hi = n_-1;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        int suffix = rlcsa_->locate(mid);

        bool greater = false;
        bool equal = true;
        for (int i = 0; i < pattern.length(); ++i) {
            int c = access(i+suffix);
            if (pattern[i] < c) {
                greater = true;
                equal = false;
                break;
            }
            else if (pattern[i] > c) {
                equal = false;
                break;
            }
        }

        if (equal) {
            if (hi <= lo + 1) {
                end = lo;
                int suffix = rlcsa_->locate(hi);

                bool equal = true;
                for (int i = 0; i < pattern.length(); ++i) {
                    int c = access(i+suffix);
                    if (pattern[i] < c) {
                        equal = false;
                        break;
                    }
                    else if (pattern[i] > c) {
                        equal = false;
                        break;
                    }
                }
                if (equal) end = hi;
                break;
            }
            lo = mid;
        } else if (greater) {
            hi = mid-1;
        } else {
            lo = mid+1;
        }
    }

    return lca(select_leaf(init+1), select_leaf(end+1));



}


std::vector<int> PBTISARLCSACST::locate(int node) {
    std::vector<int> positions;
    int init = lb(node);
    int end = rb(node);
    for (int i = init; i< end; ++i) {
        positions.push_back(rlcsa_->locate(i));
    }
    return positions;
}

int PBTISARLCSACST::string_ancestor(int node, int d) {
    /*
    if (d==0) return root();
    if (v==root()) return 0;
    size_t td = pbtcTree_->depth(v)-1;
    size_t vv,value, newv;
    uint l,p,r;
    l=1;r=td;value=d;vv=v;newv=v;
    while(l<=r){
        p = (l+r)/2;
        newv = pbtcTree_->level_ancestor(vv,p);
        value = string_depth(newv);
        if (value<d){
            l=p+1;
        }else if (value>d){
            r=p-1;
        }else{
            break;
        }
    }
    if (value<d){
        if (p+1==td) return v;
        return pbtcTree_->level_ancestor(vv,p+1);
    }
    return newv;
     */

    int bottom_depth = pbtcTree_->depth(node);
    int top_depth = 0;
    int top = 0;
    int bottom = node;

    while (true) {
        if (bottom == top) return bottom;
        int guess_depth = top_depth + (bottom_depth-top_depth)/2;
        int middle_node = pbtcTree_->level_ancestor(node, bottom_depth - guess_depth);
        int s_depth = string_depth(middle_node);
        if (s_depth == d) return middle_node;
        if (s_depth < d) {
            if (top_depth == bottom_depth-1) return bottom;
            top = middle_node;
            top_depth = guess_depth;
        } else {
            if (top_depth == bottom_depth-1) return top;
            bottom = middle_node;
            bottom_depth = guess_depth;
        }
    }


}

void PBTISARLCSACST::print_statistics() {
    pbtcTree_->print_statistics();
    pbtcISA_->print_statistics();
    rlcsa_->reportSize(true);
    std::cout << "Size of LCP: " <<  lcp_rlcsa_->getSize() << std::endl << std::endl;
}