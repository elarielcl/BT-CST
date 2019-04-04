//
// Created by elarielcl on 25/09/18.
//

#include <compressed/PBTCLCPA.h>
#include <sdsl/lcp.hpp>

PBTCLCPA::PBTCLCPA(std::string& input_string, int block_tree_version, int r, int mll, int c) {

    sdsl::lcp_bitcompressed<> clcpa;
    sdsl::construct_im(clcpa, input_string, 1);
    std::basic_string<int64_t> dlcpa;
    dlcpa += clcpa[0];
    for (int i = 0; i < clcpa.size()-1; ++i) {
        dlcpa += clcpa[i+1]-clcpa[i];
    }


    PBlockTree* bt = new PBlockTree(dlcpa, r, mll);

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

    dlcpa_ = new DAPCBlockTree(bt);

    delete bt;

}


PBTCLCPA::~PBTCLCPA() {
    delete dlcpa_;
}

int PBTCLCPA::access(int i) {
    return dlcpa_->differential_access_alternative(i);
}

int PBTCLCPA::saccess(int i) {
    return dlcpa_->sdifferential_access_alternative(i);
}

int PBTCLCPA::caccess(int i) {
    return dlcpa_->cdifferential_access_alternative(i);
}

int PBTCLCPA::csaccess(int i) {
    return dlcpa_->csdifferential_access_alternative(i);
}


int PBTCLCPA::mapped_access(int i) {
    return dlcpa_->mapped_differential_access_alternative(i);
}

int PBTCLCPA::mapped_saccess(int i) {
    return dlcpa_->mapped_sdifferential_access_alternative(i);
}

int PBTCLCPA::mapped_caccess(int i) {
    return dlcpa_->mapped_cdifferential_access_alternative(i);
}

int PBTCLCPA::mapped_csaccess(int i) {
    return dlcpa_->mapped_csdifferential_access_alternative(i);
}

void PBTCLCPA::print_statistics() {
    std::cout << "STATISTICS!" << std::endl;
    dlcpa_->print_statistics();
}