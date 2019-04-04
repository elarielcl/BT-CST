#include <compressed/CST_SADA_RLCSA.h>

CST_SADA_RLCSA::CST_SADA_RLCSA(std::string& input_string, int block_size, int sa_sampling_rate) {

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


    rlcsa_ = new TextIndexes::RLCSA(data, input_string.size()+1, block_size, sa_sampling_rate, false, true);
    index_ = new TextIndexRLCSA();

    index_->rlcsa = rlcsa_;

    //delete[] data;
    data = new unsigned char[input_string.size()+1];
    for (int i = 0; i < input_string.size(); ++i) {
        data[i] = input_string[i];
    }
    data[input_string.size()] = 0;
    lcp_rlcsa_ = new LCP_FMN_RLCSA(rlcsa_, (char*)data, input_string.size()+1);

    //input_string += "$";

    sdsl::cst_sada<> cst;
    construct_im(cst, input_string, 1);
    pm_bp = new sdsl::bit_vector(cst.m_bp.size());
    for (int i = 0; i < pm_bp->size(); ++i) {
        (*pm_bp)[i] = cst.m_bp[i];
    }
    pm_bp_support = new sdsl::bp_support_sada<>(pm_bp);

    pm_bp_rank10 = new sdsl::rank_support_v5<10,2>(pm_bp);
    pm_bp_select10 = new sdsl::select_support_mcl<10,2>(pm_bp);

    n_ = input_string.size()+1;


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

/*
    std::set<char> characters;
    for (int i = 0; i < input_string.size(); ++i) {
        characters.insert(input_string[i]);
    }

    std::vector<int> alphabet_;
    for (char c : characters)
        alphabet_.push_back(c);
        */

/*
    for (int i = 1; i < input_string.size(); ++i) {
        std::cout << pbtcLCPA_->access(i) << ", ";
    }
    std::cout << std::endl;
*/

/*
    std::pair<int, int> positions;
    positions.first = 0;
    int counter = 0;
    for (int i = 1; i < input_string.size(); ++i) {
        if (lcp_rlcsa_->get_LCP(i, index_) == 0) {
            positions.second = i-1;
            C_[alphabet_[counter++]] = positions;
            positions.first = i;
        }
    }
    positions.second = input_string.size()-1;
    C_[alphabet_[counter]] = positions;
    */
    //delete pbtcLCPA_;
    //delete[] data;
/*
    for (auto it : C_) {
        std::cout << (char)it.first << ": " << it.second.first << ", " << it.second.second << std::endl;
    }
*/
}


CST_SADA_RLCSA::CST_SADA_RLCSA(std::string& base_name, std::string& input_string, int block_size, int sa_sampling_rate) {
    //PBTCLCPA* pbtcLCPA_ = new PBTCLCPA(input_string); // FIX THIS
    //PBTCISA* pbtcisa = new PBTCISA(input_string); // FIX THIS
    //pa_ = pbtcisa->access(0);
    //pa2_ = pbtcisa->access(1);
    //delete pbtcisa;

/*
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
*/
    //std::cout << data << std::endl;


    std::string extension = ".csa_" + std::to_string(block_size) + "_" + std::to_string(sa_sampling_rate);
    index_ = (TextIndexRLCSA*)TextIndexRLCSA::load((base_name+extension).c_str());
    rlcsa_ = index_->rlcsa;
/*
    //delete[] data;
    data = new unsigned char[input_string.size()+1];
    for (int i = 0; i < input_string.size(); ++i) {
        data[i] = input_string[i];
    }
    data[input_string.size()] = 0;
    */

    ifstream lcp_in(base_name+".lcp_4");
    lcp_rlcsa_ = LCP_RLCSA::load(lcp_in);
    lcp_in.close();
    //input_string += "$";

    sdsl::cst_sada<> cst;
    construct_im(cst, input_string, 1);
    pm_bp = new sdsl::bit_vector(cst.m_bp.size());
    for (int i = 0; i < pm_bp->size(); ++i) {
        (*pm_bp)[i] = cst.m_bp[i];
    }
    pm_bp_support = new sdsl::bp_support_sada<>(pm_bp);

    pm_bp_rank10 = new sdsl::rank_support_v5<10,2>(pm_bp);
    pm_bp_select10 = new sdsl::select_support_mcl<10,2>(pm_bp);

    n_ = input_string.size()+1;

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

/*
    std::set<char> characters;
    for (int i = 0; i < input_string.size(); ++i) {
        characters.insert(input_string[i]);
    }

    std::vector<int> alphabet_;
    for (char c : characters)
        alphabet_.push_back(c);
        */

/*
    for (int i = 1; i < input_string.size(); ++i) {
        std::cout << pbtcLCPA_->access(i) << ", ";
    }
    std::cout << std::endl;
*/


/*
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
    */
    //delete[] data;
/*
    for (auto it : C_) {
        std::cout << (char)it.first << ": " << it.second.first << ", " << it.second.second << std::endl;
    }
*/
}


CST_SADA_RLCSA::~CST_SADA_RLCSA() {
    //delete input_block_tree_;
    delete lcp_rlcsa_;
    delete rlcsa_;
    delete pm_bp;
    delete pm_bp_support;
    delete pm_bp_rank10;
    delete pm_bp_select10;
}

int CST_SADA_RLCSA::access(int i) {
    //return input_block_tree_->access(i);
    int r = rlcsa_->getT(rlcsa_->getISA(i));
    if (r == 0) return '$';
    return r;
}

int CST_SADA_RLCSA::root() {
    return 0;
}

int CST_SADA_RLCSA::is_leaf(int node) {
    auto& m_bp = *pm_bp;
    return !m_bp[node+1];
}

int CST_SADA_RLCSA::select_leaf(int i) {
    auto& m_bp_select10 = *pm_bp_select10;
    return m_bp_select10.select(i)-1;
}

int CST_SADA_RLCSA::string_depth(int node) {
    if (node == 0) return 0;
    auto& m_bp_rank10 = *pm_bp_rank10;
    if (is_leaf(node)) { // if v is a leave
        uint64_t i = m_bp_rank10(node); // get the index in the suffix array
        return n_ - rlcsa_->locate(i);
    }
    return lcp_rlcsa_->get_LCP(inorder(node), index_);
}

int CST_SADA_RLCSA::node_depth(int node) {
    auto& m_bp_support = *pm_bp_support;
    return (m_bp_support.rank(node)<<1)-node-2;
}

int CST_SADA_RLCSA::size(int node) {
    auto& m_bp_support = *pm_bp_support;
    auto& m_bp_rank10 = *pm_bp_rank10;
    uint64_t r = m_bp_support.find_close(node);
    return m_bp_rank10(r+1) - m_bp_rank10(node);
}

int CST_SADA_RLCSA::leftmost_leaf(int node) {
    auto& m_bp_rank10 = *pm_bp_rank10;
    auto& m_bp_select10 = *pm_bp_select10;
    return m_bp_select10(m_bp_rank10(node)+1)-1;
}

int CST_SADA_RLCSA::rightmost_leaf(int node) {
    auto& m_bp_support = *pm_bp_support;
    auto& m_bp_rank10 = *pm_bp_rank10;
    auto& m_bp_select10 = *pm_bp_select10;
    uint64_t r = m_bp_support.find_close(node);
    return m_bp_select10(m_bp_rank10(r+1))-1;;
}

int CST_SADA_RLCSA::lb(int node) {
    auto& m_bp_rank10 = *pm_bp_rank10;
    return m_bp_rank10(node);
}

int CST_SADA_RLCSA::rb(int node) {
    auto& m_bp_support = *pm_bp_support;
    auto& m_bp_rank10 = *pm_bp_rank10;
    uint64_t r = m_bp_support.find_close(node);
    return m_bp_rank10(r+1)-1;
}

int CST_SADA_RLCSA::parent(int node) {
    auto& m_bp_support = *pm_bp_support;
    if (node == root())
        return root();
    else {
        return m_bp_support.enclose(node);
    }
}

int CST_SADA_RLCSA::next_sibling(int node) {
    auto& m_bp_support = *pm_bp_support;
    auto& m_bp = *pm_bp;
    uint64_t sib = m_bp_support.find_close(node)+1;
    if (m_bp[sib])
        return sib;
    else
        return -1;
}

int CST_SADA_RLCSA::string(int node, int i) {
    auto& m_bp_rank10 = *pm_bp_rank10;
    int lr = m_bp_rank10(node);

    if (node == node_ && i == i_+1) {
        ++i_;
        psi_ = rlcsa_->getPsi(psi_,1);
        int r = rlcsa_->getT(psi_);
        if (r == 0) return '$';
        return r;
    } else {
        node_ = node;
        i_ = i;
        
        psi_ = rlcsa_->getPsi(lr,i);
        int r = rlcsa_->getT(psi_);
        if (r == 0) return '$';
        return r;
    }



    int sa = rlcsa_->locate(lr-1);
    if (sa+i >= n_) return -1;
    return access(sa + i);

}

std::string CST_SADA_RLCSA::string(int node, int i, int j) { //Maybe path extraction would be util here
    auto& m_bp_rank10 = *pm_bp_rank10;
    int lr = m_bp_rank10(node);
    int sa = rlcsa_->locate(lr);
    std::string output = "";
    while (i <= j) {
        output += access(sa+i);
        i++;
    }
    return output;
}

int CST_SADA_RLCSA::labeled_child(int node, int c) {

    if (c == 0) c = '$';
    int sdepth = string_depth(node);
    int child = node+1;
    while (true) {
        int ch = string(child, sdepth);
        if (ch > c) return root();
        if (ch == c) return child;
        child = next_sibling(child);
        if (child == -1) return root();
    }
}

int CST_SADA_RLCSA::select_child(int node, int t) {
    int child = node + 1;
    while (--t) {
        child = next_sibling(child);
    }
    return child;
}

int CST_SADA_RLCSA::edge(int node, int i) {
    int parent_depth = string_depth(parent(node));
    return string(node, parent_depth+i);
}

int CST_SADA_RLCSA::lca(int node1, int node2) {
    auto& m_bp_support = *pm_bp_support;
    if (node1 > node2) {
        std::swap(node1,node2);
    } else if (node1==node2) {
        return node1;
    }
    if (node1 == root())
        return root();
    return m_bp_support.double_enclose(node1, node2);
}

int CST_SADA_RLCSA::suffix_link(int node) {
    if (node == root() || node == 1) return root();

    if (is_leaf(node)) {
        int lml = lb(node);
        int psi_lml = rlcsa_->getPsi(lml,1);
        return select_leaf(psi_lml+1);
    }

    int lml = lb(node);
    int rml = rb(node);
    if (lml == 0 && rml == 0) return root();
    int psi_lml = rlcsa_->getPsi(lml,1);
    int psi_rml = rlcsa_->getPsi(rml,1);


    return lca(select_leaf(psi_lml+1), select_leaf(psi_rml+1));
}

int CST_SADA_RLCSA::suffix_link(int node, int i) {
    if (node == root()) return root();

    int lml = lb(node);
    int next_suf_lml = rlcsa_->locate(lml) + i;
    if (next_suf_lml >= n_) return root();
    int psi_lml = rlcsa_->getISA(next_suf_lml);

    int rml = rb(node);
    int next_suf_rml = rlcsa_->locate(rml) + i;
    if (next_suf_rml >= n_) return root();
    int psi_rml = rlcsa_->getISA(next_suf_rml);

    return lca(select_leaf(psi_lml+1), select_leaf(psi_rml+1));

    /*
    if (node == root()) return root();
    int lml = lb(node);
    int next_suf_lml = rlcsa_->locate(lml) + i;
    if (next_suf_lml >= n_) return root();
    int psi_lml;
    if (next_suf_lml == 0) psi_lml = rlcsa_->getISA(next_suf_lml);
    else if ( next_suf_lml == 1 ) psi_lml = pa2_;
    else psi_lml = rlcsa_->getISA(next_suf_lml-1);

    int rml = rb(node);
    int next_suf_rml = rlcsa_->locate(rml-1) + i;
    if (next_suf_rml >= n_) return root();
    int psi_rml;
    if (next_suf_rml == 0) psi_rml = rlcsa_->getISA(next_suf_rml);
    else if ( next_suf_rml == 1 ) psi_rml = pa2_;
    else psi_rml = rlcsa_->getISA(next_suf_rml-1);


    return lca(select_leaf(psi_lml+1), select_leaf(psi_rml+1));
*/
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

/*
int CST_SADA_RLCSA::psi(int i) {
    int r = rlcsa_->psi(i) -1;
    return r == -1 ? pa_ : r;
}
 */
/*
int CST_SADA_RLCSA::weiner_link(int node, int c) { //REIMPLEMENT WITH rlcsa

    int s = lb(node);
    int e = rb(node)-1;

    //int new_s = rlcsa_->LF(s, c)-1;
    //int new_e = rlcsa_->LF(e, c)-1;


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
*/

int CST_SADA_RLCSA::sn(int node) {
    auto& m_bp_rank10 = *pm_bp_rank10;
    return rlcsa_->locate(m_bp_rank10(node));
}

int CST_SADA_RLCSA::id(int node) {
    auto& m_bp = *pm_bp;
    auto& m_bp_rank10 = *pm_bp_rank10;
    auto& m_bp_support = *pm_bp_support;
    if (m_bp[node+1]) {    // case (a) inner node
        return n_ + (m_bp_support.rank(node) - 1) - m_bp_rank10(node);
    } else {            // case (b) leaf
        return m_bp_rank10(node);
    }
}

int CST_SADA_RLCSA::inv_id(int id) {
    auto& m_bp = *pm_bp;
    auto& m_bp_rank10 = *pm_bp_rank10;
    auto& m_bp_support = *pm_bp_support;
    if (id < n_) {  // the corresponding node is a leaf
        return select_leaf(id+1);
    } else { // the corresponding node is a inner node
        id = id + 1 - n_;
        // solved by binary search; TODO: can be done in constant time by using a select structure on the bitpattern 11
        uint64_t lb = 0, rb = m_bp.size(); // lb inclusive, rb exclusive
        // invariant: arg(lb) < id, arg(rb)>= id
        while (rb-lb > 1) {
            uint64_t mid = lb + (rb-lb)/2; // mid \in [0..m_bp.size()-1]
            if (m_bp[mid] == 0 and m_bp[mid-1] == 1) {  // if we are ``half on a leaf''
                ++mid; //we step one to the right to include it
            }
            // get the number of open inner nodes before position mid, i.e. arg(mid)
            uint64_t mid_id = m_bp_support.rank(mid-1) - m_bp_rank10(mid);  // Note: mid-1 is valid of mid is of type ``size_type'' as us the parameter of rank
            if (mid_id < id) {
                lb = mid;
            } else { // mid_id >= x
                rb = mid;
            }
        }
        return lb;
    }
}

int CST_SADA_RLCSA::nodes() {
    return n_/2;
}

int CST_SADA_RLCSA::node(int lb, int rb) {
    return lca(select_leaf(lb+1), select_leaf(rb+1));
}

int CST_SADA_RLCSA::degree(int node) {
    auto& m_bp = *pm_bp;
    auto& m_bp_support = *pm_bp_support;
    uint64_t res = 0;
    node = node+1;
    while (m_bp[node]) { // found open parentheses
        ++res;
        node = m_bp_support.find_close(node)+1;
    }
    return res;
}

int CST_SADA_RLCSA::count(int node) {
    auto& m_bp_support = *pm_bp_support;
    auto& m_bp_rank10 = *pm_bp_rank10;
    uint64_t r = m_bp_support.find_close(node);
    return m_bp_rank10(r+1) - m_bp_rank10(node);
}


int CST_SADA_RLCSA::locus(std::string pattern) {
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


std::vector<int> CST_SADA_RLCSA::locate(int node) {
    std::vector<int> positions;
    int init = lb(node);
    int end = rb(node);
    for (int i = init; i< end; ++i) {
        positions.push_back(rlcsa_->locate(i));
    }
    return positions;
}

int CST_SADA_RLCSA::level_ancestor(int node, int d) {
    auto& m_bp_support = *pm_bp_support;
    return m_bp_support.level_anc(node, d);
}

int CST_SADA_RLCSA::string_ancestor(int node, int d) {
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

    int initial_bottom_depth = node_depth(node)+1;
    int bottom_depth = initial_bottom_depth;
    int top_depth = 0;
    int top = 0;
    int bottom = node;

    while (true) {
        if (bottom == top) return bottom;
        int guess_depth = top_depth + (bottom_depth-top_depth)/2;
        int middle_node = level_ancestor(node, initial_bottom_depth - guess_depth);
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

void CST_SADA_RLCSA::print_statistics() {
    uint64_t top_bytes = sdsl::size_in_bytes(*pm_bp);
    top_bytes += sdsl::size_in_bytes(*pm_bp_support);
    top_bytes += sdsl::size_in_bytes(*pm_bp_rank10);
    top_bytes += sdsl::size_in_bytes(*pm_bp_select10);
    std::cout << "Topology: " << top_bytes << std::endl;
    std::cout << "RLCSA: " << rlcsa_->getSize() << std::endl;
    std::cout << "Size of LCP: " <<  lcp_rlcsa_->getSize() << std::endl << std::endl;
}