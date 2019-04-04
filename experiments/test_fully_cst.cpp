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
#include <sdsl/cst_fully.hpp>

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

    sdsl::cst_fully<sdsl::csa_wt<sdsl::wt_huff<sdsl::rrr_vector<>>,32,64>> fully;

    sdsl::construct_im(fully, input_string, 1);



    std::cout << "fully SIZE: " << sdsl::size_in_bytes(fully) << std::endl;
    std::cout << "fully CSA: " << sdsl::size_in_bytes(fully.csa) << std::endl;
    std::cout << "fully LCP: " << sdsl::size_in_bytes(fully.lcp) << std::endl;

    std::vector<std::pair<uint64_t, uint64_t>> pos_fully;
    std::vector<std::pair<uint64_t, uint64_t>> first_sampling_fully; // Random leaves and their path to the root
    std::vector<std::pair<uint64_t, uint64_t>> second_sampling_fully; // first_sampling but only considering nodes with >= 5 children
    std::vector<std::pair<uint64_t, uint64_t>> third_sampling_fully; // Suffix_link walk
    std::vector<std::pair<std::pair<uint64_t, uint64_t>,std::pair<uint64_t, uint64_t>>> fourth_sampling_fully;
    std::vector<std::pair<uint64_t, uint64_t>> fifth_sampling_fully;
    std::vector<std::pair<uint64_t, uint64_t>> sixth_sampling_fully;


    int nleaf = sada.size(sada.root());
    for (int i = 0; i < Max_Sampling_Size; ++i) {
        int r = (rand() % nleaf)+1;
        int p = sada.select_leaf(r);
        pos_fully.push_back(fully.select_leaf(r));
    }
    for (int i = 0; i < Max_Sampling_Size; ++i) {
        int r1 = (rand() % nleaf)+1;
        int r2 = (rand() % nleaf)+1;
        fourth_sampling_fully.push_back({fully.select_leaf(r1),fully.select_leaf(r2)});
    }

    for (int i = 0; i < Max_Sampling_Size; ++i) {
        int r = (rand() % nleaf)+1;
        int p = sada.select_leaf(r);
        if (sada.node_depth(p) >= 10) {
            fifth_sampling_fully.push_back(fully.select_leaf(r));
        } if (sada.depth(p) >= 10) {
            sixth_sampling_fully.push_back(fully.select_leaf(r));
        }
    }


    for (std::pair<uint64_t, uint64_t> p : pos_fully) {

        auto pp = p;
        while (p!=fully.root() && second_sampling_fully.size()< Max_Sampling_Size) {
            if (first_sampling_fully.size() < Max_Sampling_Size) first_sampling_fully.push_back(p);
            if (fully.children(p).size() >= 3 && fully.depth(p) >= 4)
                second_sampling_fully.push_back(p);
            p = fully.parent(p);
        }


        p = fully.parent(pp);
        while (p != fully.root() && third_sampling_fully.size() < Max_Sampling_Size) {
            third_sampling_fully.push_back(p);
            p = fully.sl(p);
        }

    }


    /// fChild
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : first_sampling_fully) {
        fully.select_child(node,1);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    long user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    long system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sada Total fChild Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling_fully.size() << std::endl;
    /// fChild



    /// tDepth
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : first_sampling_fully) {
        fully.node_depth(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "fully Total node_depth Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling_fully.size() << std::endl;
    /// tDepth


    /// nSibling
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : first_sampling_fully) {
        fully.sibling(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "fully Total next_sibling Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling_fully.size() << std::endl;
    /// nSibling



    /// parent
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : first_sampling_fully) {
        fully.parent(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "fully Total Parent Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling_fully.size() << std::endl;
    /// parent


    std::vector<int> depths;
    for (auto node: fifth_sampling_fully) {
        int depth = fully.node_depth(node);
        depths.push_back((rand() % depth)+1);
    }


    /// level_ancestor
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int i = 0; i < fifth_sampling_fully.size(); ++i) {
        // Implementations of level_ancestor
        int depth = depths[i];
        auto& node = fifth_sampling_fully[i];
        while (depth-- != 0) {
            node = fully.parent(node);
        }
        // Implementations of level_ancestor
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "fully level_ancestor Time(us): " << user_time+system_time << std::endl;
    std::cout << fifth_sampling_fully.size() << std::endl;
    /// level_ancestor


    /// lca
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto nodes : fourth_sampling_fully) {
        fully.lca(nodes.first, nodes.second);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "fully Total lca Time(us): " << user_time+system_time << std::endl;
    std::cout << fourth_sampling_fully.size() << std::endl;
    /// lca



    /// suffix link
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : third_sampling_fully) {
        fully.sl(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "fully Total suffix_link Time(us): " << user_time+system_time << std::endl;
    std::cout << third_sampling_fully.size() << std::endl;
    /// suffix link



    /// sDepth
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : first_sampling_fully) {
        fully.depth(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "fully Total string_depth Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling_fully.size() << std::endl;
    /// sDepth

    std::vector<int> sdepths;
    for (auto node: sixth_sampling_fully) {
        int depth = fully.depth(node) ;
        sdepths.push_back(rand() % depth);
    }


    /// string_ancestor
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int i = 0; i < sixth_sampling_fully.size(); ++i) {
        // Implementations of sancestor
        int sdepth = sdepths[i];
        auto& node = sixth_sampling_fully[i];
        while (node != fully.root()) {
            if (fully.depth(node) < sdepth) break;
            node = fully.parent(node);
        }
        // Implementations of sancestor
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "fully string_ancestor Time(us): " << user_time+system_time << std::endl;
    std::cout << sixth_sampling_fully.size() << std::endl;
    /// string_ancestor


    /// letter
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : second_sampling_fully) {
        fully.edge(node, 4);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "fully Total letter Time(us): " << user_time+system_time << std::endl;
    std::cout << second_sampling_fully.size() << std::endl;
    /// letter


    std::vector<int> letters;
    for (auto node: second_sampling_fully) {
        int c = fully.children(node).size();
        auto r = fully.select_child(node, (rand() % c) + 1);
        letters.push_back(fully.edge(r, 1+fully.depth(node)));
    }

    /// child
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int i = 0; i < second_sampling_fully.size(); ++i) {
        fully.child(second_sampling_fully[i], letters[i]);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "fully Total child Time(us): " << user_time+system_time << std::endl;
    std::cout << second_sampling_fully.size() << std::endl;
    /// child

    return 0;
}

