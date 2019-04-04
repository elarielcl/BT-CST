#include <compressed/CST_SCT3_RLCSA.h>
#include <sdsl/cst_sct3.hpp>

CST_SCT3_RLCSA::CST_SCT3_RLCSA(std::string& input_string, int block_size, int sa_sampling_rate) {



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

    sdsl::cst_sct3<sdsl::csa_wt<sdsl::wt_huff<sdsl::rrr_vector<>>,32,64>,
            sdsl::lcp_support_sada<sdsl::rrr_vector<>>> cst;
    sdsl::construct_im(cst, input_string, 1);
    sigma = cst.csa.sigma;
    C = cst.csa.C;
    char2comp = cst.csa.char2comp;
    pm_bp = new sdsl::bit_vector(cst.m_bp.size());
    for (int i = 0; i < pm_bp->size(); ++i) {
        (*pm_bp)[i] = cst.m_bp[i];
    }

    pm_first_child = new sdsl::bit_vector(cst.m_first_child.size());
    for (int i = 0; i < pm_first_child->size(); ++i) {
        (*pm_first_child)[i] = cst.m_first_child[i];
    }

    pm_bp_support = new sdsl::bp_support_sada<>();
    auto& m_bp_support = *pm_bp_support;
    sdsl::util::init_support(m_bp_support, pm_bp);

    pm_first_child_rank = new sdsl::rank_support_v5<>();
    auto& m_first_child_rank = *pm_first_child_rank;
    sdsl::util::init_support(m_first_child_rank, pm_first_child);

    pm_first_child_select = new sdsl::bit_vector::select_1_type();
    auto& m_first_child_select = *pm_first_child_select;
    sdsl::util::init_support(m_first_child_select, pm_first_child);

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


CST_SCT3_RLCSA::CST_SCT3_RLCSA(std::string& base_name, std::string& input_string, int block_size, int sa_sampling_rate) {
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

    sdsl::cst_sct3<sdsl::csa_wt<sdsl::wt_huff<sdsl::rrr_vector<>>,32,64>,
            sdsl::lcp_support_sada<sdsl::rrr_vector<>>> cst;
    sdsl::construct_im(cst, input_string, 1);

    sigma = cst.csa.sigma;
    C = cst.csa.C;
    char2comp = cst.csa.char2comp;
    pm_bp = new sdsl::bit_vector(cst.m_bp.size());
    for (int i = 0; i < pm_bp->size(); ++i) {
        (*pm_bp)[i] = cst.m_bp[i];
    }

    pm_first_child = new sdsl::bit_vector(cst.m_first_child.size());
    for (int i = 0; i < pm_first_child->size(); ++i) {
        (*pm_first_child)[i] = cst.m_first_child[i];
    }

    pm_bp_support = new sdsl::bp_support_sada<>();
    auto& m_bp_support = *pm_bp_support;
    sdsl::util::init_support(m_bp_support, pm_bp);

    pm_first_child_rank = new sdsl::rank_support_v5<>();
    auto& m_first_child_rank = *pm_first_child_rank;
    sdsl::util::init_support(m_first_child_rank, pm_first_child);

    pm_first_child_select = new sdsl::bit_vector::select_1_type();
    auto& m_first_child_select = *pm_first_child_select;

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


CST_SCT3_RLCSA::~CST_SCT3_RLCSA() {
    //delete input_block_tree_;
    delete lcp_rlcsa_;
    delete rlcsa_;
    delete pm_bp;
    delete pm_bp_support;
    delete pm_first_child;
    delete pm_first_child_rank;
    delete pm_first_child_select;
}

int CST_SCT3_RLCSA::access(int i) {
    //return input_block_tree_->access(i);
    int r = rlcsa_->getT(rlcsa_->getISA(i));
    if (r == 0) return '$';
    return r;
}

sdsl::bp_interval<uint64_t> CST_SCT3_RLCSA::root() {
    auto& m_bp = *pm_bp;
    return sdsl::bp_interval<uint64_t>(0, n_-1, 0, m_bp.size()-1, m_bp.size());
}

int CST_SCT3_RLCSA::is_leaf(sdsl::bp_interval<uint64_t> node) {
    return node.i==node.j;
}

sdsl::bp_interval<uint64_t> CST_SCT3_RLCSA::select_leaf(int i) {
    auto& m_bp_support = *pm_bp_support;
    auto& m_bp = *pm_bp;
    uint64_t ipos = m_bp_support.select(i);
    uint64_t jp1pos;
    if (i == n_)
        jp1pos = m_bp.size();
    else if (m_bp[ipos+1])
        jp1pos = ipos+1;
    else
        jp1pos = m_bp_support.select(i+1);
    return sdsl::bp_interval<uint64_t>(i-1, i-1, ipos, m_bp_support.find_close(ipos), jp1pos);
}

int CST_SCT3_RLCSA::string_depth(sdsl::bp_interval<uint64_t> node) {
    if (node.i == node.j) {
        return n_ - rlcsa_->locate(node.i);
    } else if (node == root()) {
        return 0;
    } else {
        uint64_t kpos, ckpos;
        uint64_t l = select_l_index(node, 1, kpos, ckpos);
        return lcp_rlcsa_->get_LCP(l, index_);
    }
}

int CST_SCT3_RLCSA::node_depth(sdsl::bp_interval<uint64_t> node) {
    uint64_t d = 0;
    while (node != root()) {
        ++d;
        node = parent(node);
    }
    return d;
}

int CST_SCT3_RLCSA::size(sdsl::bp_interval<uint64_t> node) {
    return node.j-node.i+1;
}

sdsl::bp_interval<uint64_t> CST_SCT3_RLCSA::leftmost_leaf(sdsl::bp_interval<uint64_t> node) {
    return select_leaf(node.i+1);
}

sdsl::bp_interval<uint64_t> CST_SCT3_RLCSA::rightmost_leaf(sdsl::bp_interval<uint64_t> node) {
    return select_leaf(node.j+1);
}

int CST_SCT3_RLCSA::lb(sdsl::bp_interval<uint64_t> node) {
    return node.i;
}

int CST_SCT3_RLCSA::rb(sdsl::bp_interval<uint64_t> node) {
    return node.j;
}

sdsl::bp_interval<uint64_t> CST_SCT3_RLCSA::parent(sdsl::bp_interval<uint64_t> node) {
    auto& m_bp_support = *pm_bp_support;
    auto& m_bp = *pm_bp;
    if (node.cipos > node.jp1pos) { // LCP[i] <= LCP[j+1]
        uint64_t psv_pos, psv_cpos, psv_v, nsv_v, nsv_p1pos;
        psv_v = psv(node.j+1, node.jp1pos, m_bp_support.find_close(node.jp1pos), psv_pos, psv_cpos);
        nsv_v = nsv(node.j+1, node.jp1pos)-1;
        if (nsv_v == n_-1) {
            nsv_p1pos = m_bp.size();
        } else { // nsv_v < size()-1
            nsv_p1pos = m_bp_support.select(nsv_v+2);
        }
        return sdsl::bp_interval<uint64_t>(psv_v, nsv_v, psv_pos, psv_cpos, nsv_p1pos);
    } else { // LCP[i] > LCP[j+1]
        uint64_t psv_pos, psv_cpos, psv_v;
        psv_v = psv(node.i, node.ipos, node.cipos, psv_pos, psv_cpos);
        return sdsl::bp_interval<uint64_t>(psv_v, node.j, psv_pos, psv_cpos, node.jp1pos);
    }
}

sdsl::bp_interval<uint64_t> CST_SCT3_RLCSA::next_sibling(sdsl::bp_interval<uint64_t> node) {
    auto& m_bp = *pm_bp;
    auto& m_bp_support = *pm_bp_support;
    auto& m_first_child = *pm_first_child;

    //Procedure:(1) Determine, if v has a right sibling.
    if (node.cipos < node.jp1pos) { // LCP[i] > LCP[j+1] => v has the same right border as parent(v) => no right sibling
        return root();
    }
//          (2)    There exists a right sibling, LCP[j+1] >= LCP[i] and j>i
    // Now it holds:  v.cipos > v.jp1pos
    uint64_t cjp1posm1 = m_bp_support.find_close(node.jp1pos)-1; // v.cipos-2 ???
    // m_bp[cjp1posm1] equals 1 =>  v is the last child
    bool last_child = m_bp[cjp1posm1];
    // otherwise if m_bp[cjp1posm1] equals 0 => we don't know if it is the last child
    if (!last_child) {
        uint64_t first_child_idx = cjp1posm1 - m_bp_support.rank(cjp1posm1);
        last_child = m_first_child[first_child_idx]; // if first_child indicator is true => the new sibling is the rightmost sibling
    }
    if (last_child) {
        uint64_t nsv_v = nsv(node.j+1, node.jp1pos)-1, nsv_p1pos;
        if (nsv_v == n_-1) {
            nsv_p1pos = m_bp.size();
        } else {
            nsv_p1pos = m_bp_support.select(nsv_v+2);
        }
        return sdsl::bp_interval<uint64_t>(node.j+1, nsv_v, node.jp1pos, m_bp_support.find_close(node.jp1pos), nsv_p1pos);
    } else {
        uint64_t new_j = m_bp_support.rank(m_bp_support.find_open(cjp1posm1))-2;
        return sdsl::bp_interval<uint64_t>(node.j+1, new_j, node.jp1pos, m_bp_support.find_close(node.jp1pos), m_bp_support.select(new_j+2));
    }

}

int CST_SCT3_RLCSA::string(sdsl::bp_interval<uint64_t> node, int i) {
    int lr = node.i;
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
        //psi_ = ISA[SA[lr]+i];
        int r = rlcsa_->getT(psi_);
        if (r == 0) return '$';
        return r;
    }

}

std::string CST_SCT3_RLCSA::string(sdsl::bp_interval<uint64_t> node, int i, int j) { //Maybe path extraction would be util here
    int lr = node.i;
    int sa = rlcsa_->locate(lr);
    std::string output = "";
    while (i <= j) {
        output += access(sa+i);
        i++;
    }
    return output;
}

sdsl::bp_interval<uint64_t> CST_SCT3_RLCSA::labeled_child(sdsl::bp_interval<uint64_t> node, int c) {

    uint64_t char_pos;
    if (c == '$') c = 0;
    return child(node, c, char_pos);
}

sdsl::bp_interval<uint64_t>  CST_SCT3_RLCSA::select_child(sdsl::bp_interval<uint64_t>  node, int i) {
    if (is_leaf(node))  // if v is a leave, v has no child
        return root();
    if (1 == i) {
        uint64_t k = 0, kpos = 0, k_find_close = 0;
        // v is not a leave: v has at least two children
        k = select_l_index(node, 1, kpos, k_find_close);// get first l-index k and the position of k
        return sdsl::bp_interval<uint64_t>(node.i, k-1, node.ipos, node.cipos, kpos);
    } else { // i > 1
        uint64_t k1, kpos1, k_find_close1;
        k1 = select_l_index(node, i-1, kpos1, k_find_close1);
        if (k1 == node.j+1)
            return root();
        uint64_t k2, kpos2, k_find_close2;
        k2 = select_l_index(node, i, kpos2, k_find_close2);
        return sdsl::bp_interval<uint64_t>(k1, k2-1, kpos1, k_find_close1, kpos2);
    }
}

sdsl::bp_interval<uint64_t>  CST_SCT3_RLCSA::edge(sdsl::bp_interval<uint64_t>  node, int i) {
    int parent_depth = string_depth(parent(node));
    return string(node, parent_depth+i);
}

sdsl::bp_interval<uint64_t>  CST_SCT3_RLCSA::lca(sdsl::bp_interval<uint64_t>  node1, sdsl::bp_interval<uint64_t>  node2) {
    auto& m_bp = *pm_bp;
    auto& m_bp_support = *pm_bp_support;
    if (node1.i > node2.i or(node1.i == node2.i and node1.j < node2.j)) {
        std::swap(node1, node2);
    }
    if (node1.j >= node2.j) { // v encloses w or v==w
        return node1;
    } else { // v.i < v.j < w.i < w.j
        uint64_t min_index = rmq(node1.i+1, node2.j);
        uint64_t min_index_pos     = m_bp_support.select(min_index+1);
        uint64_t min_index_cpos     = m_bp_support.find_close(min_index_pos);

        if (min_index_cpos >= (m_bp.size() - sigma)) {   // if lcp[min_index]==0 => return root
            return root();
        }
        uint64_t new_j = nsv(min_index, min_index_pos)-1;
        uint64_t new_ipos, new_icpos;
        uint64_t new_i = psv(min_index, min_index_pos, min_index_cpos, new_ipos, new_icpos);
        uint64_t jp1pos = m_bp.size();
        if (new_j < n_-1) {
            jp1pos = m_bp_support.select(new_j+2);
        }
        return sdsl::bp_interval<uint64_t>(new_i, new_j, new_ipos, new_icpos, jp1pos);
    }
}

sdsl::bp_interval<uint64_t> CST_SCT3_RLCSA::suffix_link(sdsl::bp_interval<uint64_t> node) {
    auto& m_bp_support = *pm_bp_support;
    auto& m_bp = *pm_bp;
    if (node == root())
        return root();
    // get interval with first char deleted
    uint64_t i     =rlcsa_->getPsi(node.i,1);
    if (is_leaf(node)) {
        if (node.i==0 and node.j==0) // if( v.l==1 )
            return root();
        else
            return select_leaf(i+1);
    }
    uint64_t j     = rlcsa_->getPsi(node.j,1);
    uint64_t min_index = rmq(i+1, j); // rmq
    uint64_t min_index_pos     = m_bp_support.select(min_index+1);
    uint64_t min_index_cpos     = m_bp_support.find_close(min_index_pos);
    if (min_index_cpos >= (m_bp.size() - sigma)) {  // if lcp[min_index]==0 => return root
        return root();
    }
    uint64_t new_j = nsv(min_index, min_index_pos)-1;
    uint64_t new_ipos, new_icpos;
    uint64_t new_i = psv(min_index, min_index_pos, min_index_cpos, new_ipos, new_icpos);
    uint64_t jp1pos = m_bp.size();
    if (new_j < n_-1) {
        jp1pos = m_bp_support.select(new_j+2);
    }
    return sdsl::bp_interval<uint64_t>(new_i, new_j, new_ipos, new_icpos, jp1pos);
}
/*
int CST_SCT3_RLCSA::suffix_link(int node, int i) {
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
*/
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
//}

/*
int CST_SCT3_RLCSA::psi(int i) {
    int r = rlcsa_->psi(i) -1;
    return r == -1 ? pa_ : r;
}
 */
/*
int CST_SCT3_RLCSA::weiner_link(int node, int c) { //REIMPLEMENT WITH rlcsa

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

int CST_SCT3_RLCSA::sn(sdsl::bp_interval<uint64_t> node) {
    return rlcsa_->locate(node.i);
}

int CST_SCT3_RLCSA::id(sdsl::bp_interval<uint64_t> node) {
    auto& m_bp_support = *pm_bp_support;
    auto& m_first_child_rank = *pm_first_child_rank;
    if (is_leaf(node)) { // return id in the range from 0..csa.size()-1
        return node.i;
    }
    uint64_t ckpos; // closing parentheses of the l-index
    if (node.cipos > node.jp1pos) { // corresponds to m_lcp[i] <= m_lcp[j+1]
        ckpos     = node.jp1pos-1;
    } else { // corresponds to m_lcp[i] > m_lcp[j+1]
        ckpos    = node.cipos-1;
    }
    uint64_t  r0ckpos = ckpos-m_bp_support.rank(ckpos); // determine the rank of the closing parenthesis
    return n_+m_first_child_rank(r0ckpos);
}

sdsl::bp_interval<uint64_t> CST_SCT3_RLCSA::inv_id(int id) {
    auto& m_bp_support = *pm_bp_support;
    auto& m_first_child_rank = *pm_first_child_rank;
    auto& m_bp = *pm_bp;
    if (id < n_) {  // the corresponding node is a leaf
        return select_leaf(id+1);
    } else { // the corresponding node is a inner node
        // (1) get index of the closing parenthesis in m_first_child
        uint64_t r0ckpos = 0;
        {
            //binary search for the position of the (id-size()+1)-th set bit in
            id = id-n_+1;
            uint64_t lb = 0, rb = m_bp.size(); // lb inclusive, rb exclusive
            // invariant: arg(lb) < id, arg(rb) >= id
            while (rb-lb > 1) {
                uint64_t mid = lb + (rb-lb)/2;
                uint64_t arg = m_first_child_rank(mid); // ones in the prefix [0..mid-1]
                if (arg < id) {
                    lb = mid;
                } else { // arg >= id
                    rb = mid;
                }
            }
            r0ckpos = lb;
        }
        // (2) determine position clpos of the r0clpos-th closing parentheses in the parentheses sequence
        uint64_t ckpos = 0;
        {
            // binary search for the position of the (r0ckpos+1)-th closing parenthesis
            uint64_t lb = 0, rb = m_bp.size(); // lb inclusive, rb exclusive
            // invariant: arg(lb) < r0ckpos+1,  arg(rb) >= r0ckpos+1
            while (rb-lb > 1) {
                uint64_t mid = lb + (rb-lb)/2;
                uint64_t arg = mid - m_bp_support.rank(mid-1);  // zeros in the prefix [0..mid-1]
                if (arg < r0ckpos+1) {
                    lb = mid;
                } else { // arg >= x
                    rb = mid;
                }
            }
            ckpos = lb;
        }
        if (ckpos == m_bp.size()-1) {
            return root();
        }
        if (m_bp[ckpos+1]) {  // jp1pos < cipos
            uint64_t jp1pos= ckpos+1;
            uint64_t j     = m_bp_support.rank(jp1pos-1)-1;
            uint64_t kpos  = m_bp_support.find_open(ckpos);
            uint64_t ipos    = m_bp_support.enclose(kpos);
            uint64_t cipos = m_bp_support.find_close(ipos);
            uint64_t i        = m_bp_support.rank(ipos-1);
            return sdsl::bp_interval<uint64_t>(i, j, ipos, cipos, jp1pos);
        } else { //
            uint64_t cipos = ckpos+1;
            uint64_t ipos  = m_bp_support.find_open(cipos);
            uint64_t i     = m_bp_support.rank(ipos-1);
            uint64_t j     = nsv(i, ipos)-1;
            uint64_t jp1pos= m_bp.size();
            if (j != n_-1) {
                jp1pos = m_bp_support.select(j+2);
            }
            return sdsl::bp_interval<uint64_t>(i, j, ipos, cipos, jp1pos);
        }
    }
}

int CST_SCT3_RLCSA::nodes() {
    return n_/2;
}

sdsl::bp_interval<uint64_t> CST_SCT3_RLCSA::node(int lb, int rb) {
    return lca(select_leaf(lb+1), select_leaf(rb+1));
}

int CST_SCT3_RLCSA::count(sdsl::bp_interval<uint64_t> node) {
    return size(node);
}


sdsl::bp_interval<uint64_t> CST_SCT3_RLCSA::locus(std::string pattern) {
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

    if (hi < lo && !flag) return sdsl::bp_interval<uint64_t>(-1,-1,0,0);


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


std::vector<int> CST_SCT3_RLCSA::locate(int node) {
    std::vector<int> positions;
    int init = lb(node);
    int end = rb(node);
    for (int i = init; i< end; ++i) {
        positions.push_back(rlcsa_->locate(i));
    }
    return positions;
}

sdsl::bp_interval<uint64_t> CST_SCT3_RLCSA::level_ancestor(sdsl::bp_interval<uint64_t> node, int d) {
    int depth = d;
    while (depth-- != 0) {
        node = parent(node);
    }
    return node;
}

sdsl::bp_interval<uint64_t> CST_SCT3_RLCSA::string_ancestor(sdsl::bp_interval<uint64_t> node, int d) {
    int sdepth = d;
    sdsl::bp_interval<uint64_t> candidate = sdsl::bp_interval<uint64_t>(-1,-1,0,0,0);
    while (node != root()) {
        if (string_depth(node) < sdepth) return candidate;
        candidate = node;
        node = parent(node);
    }

}

void CST_SCT3_RLCSA::print_statistics() {
    uint64_t top_lcp_bytes = sdsl::size_in_bytes(*pm_bp);
    top_lcp_bytes += sdsl::size_in_bytes(*pm_bp_support);
    top_lcp_bytes += sdsl::size_in_bytes(*pm_first_child);
    top_lcp_bytes += sdsl::size_in_bytes(*pm_first_child_rank);
    top_lcp_bytes += sdsl::size_in_bytes(*pm_first_child_select);
    std::cout << "Top_LCP : " << top_lcp_bytes << std::endl;
    std::cout << "RLCSA: " << rlcsa_->getSize() << std::endl;
    std::cout << "Size of LCP: " <<  lcp_rlcsa_->getSize() << std::endl << std::endl;
}