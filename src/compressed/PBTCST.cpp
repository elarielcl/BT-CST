//
// Created by elarielcl on 26/09/18.
//

#include <compressed/PBTCST.h>
#include <sdsl/suffix_trees.hpp>

PBTCST::PBTCST(std::string& input_string, int block_tree_version, int r, int mll, int c, int block_tree_version_sa, int r_sa, int mll_sa, int c_sa, int block_tree_version_isa, int r_isa, int mll_isa, int c_isa, int block_tree_version_lcp, int r_lcp, int mll_lcp, int c_lcp, int block_tree_version_top, int r_top, int mll_top, int c_top) {

    pbtcSA_ = new PBTCSA(input_string, block_tree_version_sa, r_sa, mll_sa, c_sa);
    pbtcISA_ = new PBTCISA(input_string, block_tree_version_isa, r_isa, mll_isa, c_isa);
    pbtcLCPA_ = new PBTCLCPA(input_string, block_tree_version_lcp, r_lcp, mll_lcp, c_lcp);





    sdsl::cst_sada<> cst;
    construct_im(cst, input_string, 1);
    std::string topology;
    for (int i = 0; i < cst.bp.size(); ++i) {
        if (cst.bp[i]) topology += "(";
        else topology += ")";
    }
    pbtcTree_ = new PBTCTree(topology, block_tree_version_top, r_top, mll_top, c_top);

    input_string += "$";
    n_ = input_string.size();


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



    std::set<char> characters;
    for (int i = 0; i < input_string.size(); ++i) {
        characters.insert(input_string[i]);
    }

    std::vector<int> alphabet_;
    for (char c : characters)
        alphabet_.push_back(c);

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
/*
    for (auto it : C_) {
        std::cout << (char)it.first << ": " << it.second.first << ", " << it.second.second << std::endl;
    }
*/
}


PBTCST::~PBTCST() {
    delete input_block_tree_;
    delete pbtcLCPA_;
    delete pbtcSA_;
    delete pbtcISA_;
    delete pbtcTree_;
}

int PBTCST::access(int i) {
    return input_block_tree_->access(i);
}

int PBTCST::root() {
    return 0;
}

int PBTCST::is_leaf(int node) {
    return pbtcTree_->is_leaf(node);
}

int PBTCST::select_leaf(int i) {
    return pbtcTree_->leaf_select(i);
}

int PBTCST::string_depth(int node) {
    if (node == 0) return 0;
    int lr = 0;
    if (pbtcTree_->is_leaf_rank(node, lr)) {
        return n_ - pbtcSA_->access(lr-1);
    }

    int sc = pbtcTree_->next_sibling(node+1); // Better than child 2
    int i = pbtcTree_->leaf_rank(sc);
    return pbtcLCPA_->access(i-1);
}

int PBTCST::node_depth(int node) {
    return pbtcTree_->depth(node);
}

int PBTCST::size(int node) {
    pbtcTree_->number_of_leaves(node);
}

int PBTCST::leftmost_leaf(int node) {
    if (pbtcTree_->is_leaf(node)) return node;

    int lr = pbtcTree_->leaf_rank(node);
    return pbtcTree_->leaf_select(lr);
}

int PBTCST::rightmost_leaf(int node) {
    if (pbtcTree_->is_leaf(node)) return node;

    int c = pbtcTree_->bps_->my_fwdsearch(node, -1);
    int lr = pbtcTree_->leaf_rank(c);
    return pbtcTree_->leaf_select(lr-1);
}

int PBTCST::lb(int node) {
    return pbtcTree_->leaf_rank(node)-1;
}

int PBTCST::rb(int node) {
    int c = pbtcTree_->bps_->my_fwdsearch(node, -1);
    return pbtcTree_->leaf_rank(c) - 1;
}

int PBTCST::parent(int node) {
    return pbtcTree_->parent(node);
}

int PBTCST::next_sibling(int node) {
    return pbtcTree_->next_sibling(node);
}

int PBTCST::string(int node, int i) {
    int lr = pbtcTree_->leaf_rank(node);
    int sa = pbtcSA_->access(lr-1);
    return input_block_tree_->access(sa + i);
}

std::string PBTCST::string(int node, int i, int j) { //Maybe path extraction would be util here
    int lr = pbtcTree_->leaf_rank(node);
    int sa = pbtcSA_->access(lr-1);
    std::string output = "";
    while (i <= j) {
        output += input_block_tree_->access(sa+i);
        i++;
    }
    return output;
}

int PBTCST::labeled_child(int node, int c) {
    int sdepth = string_depth(node);
    int child = node+1;
    while (true) {
        int ch = string(child, sdepth);
        if (ch > c) return -1;
        if (ch == c) return child;
        child = pbtcTree_->next_sibling(child);
    }
}

int PBTCST::select_child(int node, int t) {
    int child = node + 1;
    while (--t) {
        child = pbtcTree_->next_sibling(child);
    }
    return child;
}

int PBTCST::edge(int node, int i) {
    int parent_depth = string_depth(parent(node));
    return string(node, parent_depth+i);
}

int PBTCST::lca(int node1, int node2) {
    return pbtcTree_->lca(node1, node2);
}

int PBTCST::suffix_link(int node) {
    if (node == root()) return root();

    int lml = lb(node);
    int next_suf_lml = pbtcSA_->access(lml) + 1;
    if (next_suf_lml == n_) next_suf_lml = 0;
    int psi_lml = pbtcISA_->access(next_suf_lml);

    int rml = rb(node);
    int next_suf_rml = pbtcSA_->access(rml-1) + 1;
    if (next_suf_rml == n_) next_suf_rml = 0;
    int psi_rml = pbtcISA_->access(next_suf_rml);

    if (next_suf_lml == 0 && next_suf_rml == 0) return 0;

    return lca(select_leaf(psi_lml+1), select_leaf(psi_rml+1));
}

int PBTCST::suffix_link(int node, int i) {
    if (node == root()) return root();

    int lml = lb(node);
    int next_suf_lml = pbtcSA_->access(lml) + i;
    if (next_suf_lml >= n_) return root();
    int psi_lml = pbtcISA_->access(next_suf_lml);

    int rml = rb(node);
    int next_suf_rml = pbtcSA_->access(rml-1) + i;
    if (next_suf_rml >= n_) return root();
    int psi_rml = pbtcISA_->access(next_suf_rml);

    return lca(select_leaf(psi_lml+1), select_leaf(psi_rml+1));
}

int PBTCST::psi(int i) {
    int sa = pbtcSA_->access(i) + 1;
    if (sa == n_) sa = 0;
    return pbtcISA_->access(sa);
}

int PBTCST::weiner_link(int node, int c) {
    int s = lb(node);
    int e = rb(node)-1;

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


int PBTCST::sn(int node) {
    int lr = pbtcTree_->leaf_rank(node);
    return pbtcSA_->access(lr);
}

int PBTCST::id(int node) {
    return pbtcTree_->node_map(node)-1;
}

int PBTCST::inv_id(int id) {
    return pbtcTree_->node_select(id+1);
}

int PBTCST::nodes() {
    return n_/2;
}

int PBTCST::node(int lb, int rb) {
    return lca(select_leaf(lb+1), select_leaf(rb+1));
}

int PBTCST::degree(int node) {
    return pbtcTree_->number_of_children(node);
}

int PBTCST::count(int node) {
    pbtcTree_->number_of_leaves(node);
}


int PBTCST::locus(std::string pattern) {
    int init = 0;
    int end = n_-1;
    bool flag = false;

    int lo = 0;
    int hi = n_-1;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        int suffix = pbtcSA_->access(mid);

        bool less = false;
        bool equal = true;
        for (int i = 0; i < pattern.length(); ++i) {
            int c = input_block_tree_->access(i+suffix);
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
        int suffix = pbtcSA_->access(mid);

        bool greater = false;
        bool equal = true;
        for (int i = 0; i < pattern.length(); ++i) {
            int c = input_block_tree_->access(i+suffix);
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
                int suffix = pbtcSA_->access(hi);

                bool equal = true;
                for (int i = 0; i < pattern.length(); ++i) {
                    int c = input_block_tree_->access(i+suffix);
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


std::vector<int> PBTCST::locate(int node) {
    std::vector<int> positions;
    int init = lb(node);
    int end = rb(node);
    for (int i = init; i< end; ++i) {
        positions.push_back(pbtcSA_->access(i));
    }
    return positions;
}

int PBTCST::string_ancestor(int node, int d) {
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

void PBTCST::print_statistics() {
    std::cout << "STATISTICS!" << std::endl;
}