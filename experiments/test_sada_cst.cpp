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
//#include <sdsl/cst.hpp>

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

    std::cout << "SADA SIZE: " << sdsl::size_in_bytes(sada) << std::endl;
    std::cout << "SADA CSA SIZE : " << sdsl::size_in_bytes(sada.csa) << std::endl;
    std::cout << "SADA LCP SIZE : " << sdsl::size_in_bytes(sada.lcp) << std::endl;

    std::vector<int> pos;
    std::vector<int > first_sampling; // Random leaves and their path to the root
    std::vector<int > second_sampling; // first_sampling but only considering nodes with >= 5 children
    std::vector<int > third_sampling; // Suffix_link walk
    std::vector<std::pair<int ,int >> fourth_sampling;
    std::vector<int > fifth_sampling;
    std::vector<int > sixth_sampling;


    int nleaf = sada.size(sada.root());
    for (int i = 0; i < Max_Sampling_Size; ++i) {
        int r = (rand() % nleaf)+1;
        int p = sada.select_leaf(r);
        pos.push_back(sada.select_leaf(r));
    }
    for (int i = 0; i < Max_Sampling_Size; ++i) {
        int r1 = (rand() % nleaf)+1;
        int r2 = (rand() % nleaf)+1;
        fourth_sampling.push_back({sada.select_leaf(r1),sada.select_leaf(r2)});
    }

    for (int i = 0; i < Max_Sampling_Size; ++i) {
        int r = (rand() % nleaf)+1;
        int p = sada.select_leaf(r);
        if (sada.node_depth(p) >= 10) {
            fifth_sampling.push_back(sada.select_leaf(r));
        } if (sada.depth(p) >= 10) {
            sixth_sampling.push_back(sada.select_leaf(r));
        }
    }


    for (int p : pos) {

        auto pp = p;
        while (p!=sada.root() && second_sampling.size()< Max_Sampling_Size) {
            if (first_sampling.size() < Max_Sampling_Size) first_sampling.push_back(p);
            if (sada.children(p).size() >= 3 && sada.depth(p) >= 4)
                second_sampling.push_back(p);
            p = sada.parent(p);
        }


        p = sada.parent(pp);
        while (p != sada.root() && third_sampling.size() < Max_Sampling_Size) {
            third_sampling.push_back(p);
            p = sada.sl(p);
        }

    }


    /// fChild
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : first_sampling) {
        sada.select_child(node,1);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    long user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    long system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sada Total fChild Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling.size() << std::endl;
    /// fChild



    /// tDepth
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : first_sampling) {
        sada.node_depth(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sada Total node_depth Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling.size() << std::endl;
    /// tDepth


    /// nSibling
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : first_sampling) {
        sada.sibling(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sada Total next_sibling Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling.size() << std::endl;
    /// nSibling



    /// parent
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : first_sampling) {
        sada.parent(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sada Total Parent Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling.size() << std::endl;
    /// parent


    std::vector<int> depths;
    for (auto node: fifth_sampling) {
        int depth = sada.node_depth(node);
        depths.push_back((rand() % depth)+1);
    }


    /// level_ancestor
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int i = 0; i < fifth_sampling.size(); ++i) {
        // Implementations of level_ancestor
        int depth = depths[i];
        auto& node = fifth_sampling[i];
        while (depth-- != 0) {
            node = sada.parent(node);
        }
        // Implementations of level_ancestor
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sada level_ancestor Time(us): " << user_time+system_time << std::endl;
    std::cout << fifth_sampling.size() << std::endl;
    /// level_ancestor


    /// lca
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto nodes : fourth_sampling) {
        sada.lca(nodes.first, nodes.second);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sada Total lca Time(us): " << user_time+system_time << std::endl;
    std::cout << fourth_sampling.size() << std::endl;
    /// lca



    /// suffix link
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : third_sampling) {
        sada.sl(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sada Total suffix_link Time(us): " << user_time+system_time << std::endl;
    std::cout << third_sampling.size() << std::endl;
    /// suffix link



    /// sDepth
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : first_sampling) {
        sada.depth(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sada Total string_depth Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling.size() << std::endl;
    /// sDepth

    std::vector<int> sdepths;
    for (auto node: sixth_sampling) {
        int depth = sada.depth(node) ;
        sdepths.push_back(rand() % depth);
    }


    /// string_ancestor
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int i = 0; i < sixth_sampling.size(); ++i) {
        // Implementations of sancestor
        int sdepth = sdepths[i];
        auto& node = sixth_sampling[i];
        while (node != sada.root()) {
            if (sada.depth(node) < sdepth) break;
            node = sada.parent(node);
        }
        // Implementations of sancestor
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sada string_ancestor Time(us): " << user_time+system_time << std::endl;
    std::cout << sixth_sampling.size() << std::endl;
    /// string_ancestor


    /// letter
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (auto node : second_sampling) {
        sada.edge(node, 4);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sada Total letter Time(us): " << user_time+system_time << std::endl;
    std::cout << second_sampling.size() << std::endl;
    /// letter


    std::vector<int> letters;
    for (auto node: second_sampling) {
        int c = sada.children(node).size();
        auto r = sada.select_child(node, (rand() % c) + 1);
        letters.push_back(sada.edge(r, 1+sada.depth(node)));
    }

    /// child
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int i = 0; i < second_sampling.size(); ++i) {
        sada.child(second_sampling[i], letters[i]);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "sada Total child Time(us): " << user_time+system_time << std::endl;
    std::cout << second_sampling.size() << std::endl;
    /// child

    return 0;
}

