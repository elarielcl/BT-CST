//
// Created by elarielcl on 26/09/18.
//

#include <compressed/PBTCISA.h>
#include <sdsl/suffix_arrays.hpp>

PBTCISA::PBTCISA(std::string& input_string, int block_tree_version, int r, int mll, int c) {
    sdsl::csa_bitcompressed<> csa;
    sdsl::construct_im(csa, input_string, 1);

    sdsl::int_vector<> isa(csa.size(),0);
    for (int i = 0; i < isa.size(); ++i) {
        isa[csa[i]] = i;
    }

    std::basic_string<int64_t> disa;
    disa += isa[0];
    for (int i = 0; i < isa.size()-1; ++i) {
        disa += isa[i+1]-isa[i];
    }


    PBlockTree* bt = new PBlockTree(disa, r, mll);

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

    disa_ = new DAPCBlockTree(bt);

    delete bt;

}


PBTCISA::~PBTCISA() {
    delete disa_;
}

int PBTCISA::access(int i) {
    return disa_->differential_access_alternative(i);
}

int PBTCISA::saccess(int i) {
    return disa_->sdifferential_access_alternative(i);
}

int PBTCISA::caccess(int i) {
    return disa_->cdifferential_access_alternative(i);
}

int PBTCISA::csaccess(int i) {
    return disa_->csdifferential_access_alternative(i);
}

int PBTCISA::mapped_access(int i) {
    return disa_->mapped_differential_access_alternative(i);
}

int PBTCISA::mapped_saccess(int i) {
    return disa_->mapped_sdifferential_access_alternative(i);
}

int PBTCISA::mapped_caccess(int i) {
    return disa_->mapped_cdifferential_access_alternative(i);
}

int PBTCISA::mapped_csaccess(int i) {
    return disa_->mapped_csdifferential_access_alternative(i);
}

void PBTCISA::print_statistics() {
    std::cout << "STATISTICS!" << std::endl;
    disa_->print_statistics();
}