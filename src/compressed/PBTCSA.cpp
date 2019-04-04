//
// Created by elarielcl on 25/09/18.
//

#include <compressed/PBTCSA.h>
#include <sdsl/suffix_arrays.hpp>

PBTCSA::PBTCSA(std::string& input_string, int block_tree_version, int r, int mll, int c) {
    sdsl::csa_bitcompressed<> csa;
    sdsl::construct_im(csa, input_string, 1);
    std::basic_string<int64_t> dsa;
    dsa += csa[0];
    for (int i = 0; i < csa.size()-1; ++i) {
        dsa += csa[i+1]-csa[i];
    }

    PBlockTree* bt = new PBlockTree(dsa, r, mll);

    switch (block_tree_version) {
        case PAPER:
            bt->wprocess_back_pointers();
            bt->wclean_unnecessary_expansions();
            break;
        case PAPER_NO_CLEAN:
            bt->wprocess_back_pointers();
            break;
        case PRUNIG_C:
            bt->wprocess_back_pointers();
            bt->wclean_unnecessary_expansions(c);
            break;
        case HEURISTIC:
            bt->wprocess_back_pointers_heuristic();
            break;
        case LIBERAL:
            bt->wprocess_back_pointers_liberal_heuristic();
            break;
        case CONSERVATIVE:
            bt->wprocess_back_pointers_conservative_heuristic();
            break;
    }


    bt->add_differential_access_support();

    dsa_ = new DAPCBlockTree(bt);

    delete bt;

}


PBTCSA::~PBTCSA() {
    delete dsa_;
}

int PBTCSA::access(int i) {
    return dsa_->differential_access_alternative(i);
}

int PBTCSA::saccess(int i) {
    return dsa_->sdifferential_access_alternative(i);
}

int PBTCSA::caccess(int i) {
    return dsa_->cdifferential_access_alternative(i);
}

int PBTCSA::csaccess(int i) {
    return dsa_->csdifferential_access_alternative(i);
}



int PBTCSA::mapped_access(int i) {
    return dsa_->mapped_differential_access_alternative(i);
}

int PBTCSA::mapped_saccess(int i) {
    return dsa_->mapped_sdifferential_access_alternative(i);
}

int PBTCSA::mapped_caccess(int i) {
    return dsa_->mapped_cdifferential_access_alternative(i);
}

int PBTCSA::mapped_csaccess(int i) {
    return dsa_->mapped_csdifferential_access_alternative(i);
}

void PBTCSA::print_statistics() {
    std::cout << "STATISTICS!" << std::endl;
    dsa_->print_statistics();
}