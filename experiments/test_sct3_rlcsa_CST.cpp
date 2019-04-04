#include <iostream>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <fstream>
#include <sstream>
#include <unordered_set>

#include <compressed/PCBlockTree.h>
#include <compressed/CBlockTree.h>
#include <sdsl/cst_sada.hpp>
#include <sdsl/cst_sct3.hpp>
#include <compressed/CST_SCT3_RLCSA.h>

int main(int argc, char *argv[]) {

    char* filename = argv[1];
    const int Max_Sampling_Size = 100000;

    std::string input_string;
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    input_string = buffer.str();
    t.close();
    //input_string = "manuelarielcaceresreyes";


    sdsl::cst_sada<> sada;
    sdsl::construct_im(sada, input_string, 1);

    sdsl::cst_sct3<sdsl::csa_wt<sdsl::wt_huff<sdsl::rrr_vector<>>,32,64>,
            sdsl::lcp_support_sada<sdsl::rrr_vector<>>> sct3;
    sdsl::construct_im(sct3, input_string, 1);


    std::string base_name = filename;
    CST_SCT3_RLCSA * pbtrlcsacst = new CST_SCT3_RLCSA(base_name, input_string);
    pbtrlcsacst->print_statistics();

    std::vector<sdsl::bp_interval<uint64_t>> pos_sct3;
    std::vector<sdsl::bp_interval<uint64_t>> first_sampling_sct3; // Random leaves and their path to the root
    std::vector<sdsl::bp_interval<uint64_t>> second_sampling_sct3; // first_sampling but only considering nodes with >= 5 children
    std::vector<sdsl::bp_interval<uint64_t>> third_sampling_sct3; // Suffix_link walk
    std::vector<std::pair<sdsl::bp_interval<uint64_t>,sdsl::bp_interval<uint64_t>>> fourth_sampling_sct3;
    std::vector<sdsl::bp_interval<uint64_t>> fifth_sampling_sct3;
    std::vector<sdsl::bp_interval<uint64_t>> sixth_sampling_sct3;


    int nleaf = sada.size(sada.root());
    for (int i = 0; i < Max_Sampling_Size; ++i) {
        int r = (rand() % nleaf)+1;
        int p = sada.select_leaf(r);
        pos_sct3.push_back(sct3.select_leaf(r));
    }
    for (int i = 0; i < Max_Sampling_Size; ++i) {
        int r1 = (rand() % nleaf)+1;
        int r2 = (rand() % nleaf)+1;
        fourth_sampling_sct3.push_back({sct3.select_leaf(r1),sct3.select_leaf(r2)});
    }

    for (int i = 0; i < Max_Sampling_Size; ++i) {
        int r = (rand() % nleaf)+1;
        int p = sada.select_leaf(r);
        if (sada.node_depth(p) >= 10) {
            fifth_sampling_sct3.push_back(sct3.select_leaf(r));
        } if (sada.depth(p) >= 10) {
            sixth_sampling_sct3.push_back(sct3.select_leaf(r));
        }
    }


    for (sdsl::bp_interval<uint64_t> p : pos_sct3) {

        auto pp = p;
        while (p!=sct3.root() && second_sampling_sct3.size()< Max_Sampling_Size) {
            if (first_sampling_sct3.size() < Max_Sampling_Size) first_sampling_sct3.push_back(p);
            if (sct3.children(p).size() >= 3 && sct3.depth(p) >= 4)
                second_sampling_sct3.push_back(p);
            p = sct3.parent(p);
        }


        p = sct3.parent(pp);
        while (p != sct3.root() && third_sampling_sct3.size() < Max_Sampling_Size) {
            third_sampling_sct3.push_back(p);
            p = sct3.sl(p);
        }

    }


    /// fChild
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : first_sampling_sct3) {
        pbtrlcsacst->select_child(node, 1);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    long user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    long system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sada Total fChild Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling_sct3.size() << std::endl;
    /// fChild



    /// tDepth
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : first_sampling_sct3) {
        pbtrlcsacst->node_depth(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sct3 Total node_depth Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling_sct3.size() << std::endl;
    /// tDepth


    /// nSibling
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : first_sampling_sct3) {
        pbtrlcsacst->next_sibling(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sct3 Total next_sibling Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling_sct3.size() << std::endl;
    /// nSibling



    /// parent
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : first_sampling_sct3) {
        pbtrlcsacst->parent(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sct3 Total Parent Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling_sct3.size() << std::endl;
    /// parent


    std::vector<int> depths;
    for (auto node: fifth_sampling_sct3) {
        int depth = sct3.node_depth(node);
        depths.push_back((rand() % depth)+1);
    }


    /// level_ancestor
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int i = 0; i < fifth_sampling_sct3.size(); ++i) {
        // Implementations of level_ancestor
        int depth = depths[i];
        auto& node = fifth_sampling_sct3[i];
        pbtrlcsacst->level_ancestor(node, depth);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sct3 level_ancestor Time(us): " << user_time+system_time << std::endl;
    std::cout << fifth_sampling_sct3.size() << std::endl;
    /// level_ancestor


    /// lca
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto nodes : fourth_sampling_sct3) {
        pbtrlcsacst->lca(nodes.first, nodes.second);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sct3 Total lca Time(us): " << user_time+system_time << std::endl;
    std::cout << fourth_sampling_sct3.size() << std::endl;
    /// lca



    /// suffix link
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : third_sampling_sct3) {
        pbtrlcsacst->suffix_link(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sct3 Total suffix_link Time(us): " << user_time+system_time << std::endl;
    std::cout << third_sampling_sct3.size() << std::endl;
    /// suffix link



    /// sDepth
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : first_sampling_sct3) {
        pbtrlcsacst->string_depth(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sct3 Total string_depth Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling_sct3.size() << std::endl;
    /// sDepth

    std::vector<int> sdepths;
    for (auto node: sixth_sampling_sct3) {
        int depth = sct3.depth(node) ;
        sdepths.push_back(rand() % depth);
    }


    /// string_ancestor
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int i = 0; i < sixth_sampling_sct3.size(); ++i) {
        // Implementations of sancestor
        int sdepth = sdepths[i];
        auto& node = sixth_sampling_sct3[i];
        pbtrlcsacst->string_ancestor(node, sdepth);
        // Implementations of sancestor
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sct3 string_ancestor Time(us): " << user_time+system_time << std::endl;
    std::cout << sixth_sampling_sct3.size() << std::endl;
    /// string_ancestor


    /// letter
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : second_sampling_sct3) {
        pbtrlcsacst->string(node, 3);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sct3 Total letter Time(us): " << user_time+system_time << std::endl;
    std::cout << second_sampling_sct3.size() << std::endl;
    /// letter


    std::vector<int> letters;
    for (auto node: second_sampling_sct3) {
        int c = sct3.children(node).size();
        auto r = sct3.select_child(node, (rand() % c) + 1);
        letters.push_back(sct3.edge(r, 1+sct3.depth(node)));
    }

    /// child
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int i = 0; i < second_sampling_sct3.size(); ++i) {
        pbtrlcsacst->labeled_child(second_sampling_sct3[i], letters[i]);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sct3 Total child Time(us): " << user_time+system_time << std::endl;
    std::cout << second_sampling_sct3.size() << std::endl;
    /// child

    return 0;
}

