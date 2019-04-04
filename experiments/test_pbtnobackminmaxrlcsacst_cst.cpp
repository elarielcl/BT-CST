#include <iostream>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <fstream>
#include <sstream>
#include <unordered_set>

#include <compressed/PCBlockTree.h>
#include <compressed/CBlockTree.h>
#include <compressed/PBTCTree.h>
#include <compressed/PBTCSA.h>
#include <compressed/PBTCST.h>
#include <rlcsa/rlcsa.h>
#include <compressed/PBTnobackminmaxRLCSACST.h>
#include <sdsl/cst_sada.hpp>
#include <sdsl/lcp.hpp>

int main(int argc, char *argv[]) {

    char* filename = argv[1];
    const int Max_Sampling_Size = 100000;

    std::istringstream iss( argv[2] );
    int version;
    iss >> version;

    std::istringstream iss2( argv[3] );
    int r;
    iss2 >> r;

    std::istringstream iss3( argv[4] );
    int mll;
    iss3 >> mll;

    std::istringstream iss4( argv[5] );
    int c;
    iss4 >> c;

    std::string input_string;
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    input_string = buffer.str();
    t.close();
    //input_string = "manuelarielcaceresreyes";


    sdsl::cst_sada<> sada;
    sdsl::construct_im(sada, input_string, 1);

    std::string base_name = filename;
    PBTnobackminmaxRLCSACST * pbtrlcsacst = new PBTnobackminmaxRLCSACST(base_name, input_string, version, r, mll, c);
    pbtrlcsacst->print_statistics();

    std::vector<int> pos;
    std::vector<int> first_sampling; // Random leaves and their path to the root
    std::vector<int> second_sampling; // first_sampling but only considering nodes with >= 5 children
    std::vector<int> third_sampling; // Suffix_link walk
    std::vector<std::pair<int,int>> fourth_sampling;
    std::vector<int> fifth_sampling;
    std::vector<int> sixth_sampling;



    int nleaf = sada.size(sada.root());
    for (int i = 0; i < Max_Sampling_Size; ++i) {
        int r = (rand() % nleaf)+1;
        int p = sada.select_leaf(r);
        pos.push_back(p);
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
            fifth_sampling.push_back(p);
        } if (sada.depth(p) >= 10) {
            sixth_sampling.push_back(p);
        }
    }


    for (int p : pos) {
        int pp = p;
        while (p!=0 && second_sampling.size()< Max_Sampling_Size) {
            if (first_sampling.size() < Max_Sampling_Size) first_sampling.push_back(p);
            if (sada.children(p).size() >= 3 && sada.depth(p) >= 4)
                second_sampling.push_back(p);
            p = sada.parent(p);
        }

        p = sada.parent(pp);
        while (p != 0 && third_sampling.size() < Max_Sampling_Size) {
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
    for (int node : first_sampling) {
        pbtrlcsacst->pbtcTree_->first_child(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    long user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    long system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total fChild Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling.size() << std::endl;
    /// fChild



    /// tDepth
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int node : first_sampling) {
        pbtrlcsacst->node_depth(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total node_depth Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling.size() << std::endl;
    /// tDepth


    /// nSibling
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int node : first_sampling) {
        pbtrlcsacst->next_sibling(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total next_sibling Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling.size() << std::endl;
    /// nSibling



    /// parent
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int node : first_sampling) {
        pbtrlcsacst->parent(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total Parent Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling.size() << std::endl;
    /// parent


    std::vector<int> depths;
    for (int node: fifth_sampling) {
        int depth = pbtrlcsacst->node_depth(node) - 1;
        depths.push_back((rand() % depth)+1);
    }


    /// level_ancestor
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int i = 0; i < fifth_sampling.size(); ++i) {
        pbtrlcsacst->pbtcTree_->level_ancestor(fifth_sampling[i], depths[i]);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "level_ancestor Time(us): " << user_time+system_time << std::endl;
    std::cout << fifth_sampling.size() << std::endl;
    /// level_ancestor


    /// lca
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (std::pair<int,int> nodes : fourth_sampling) {
        pbtrlcsacst->lca(nodes.first, nodes.second);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total lca Time(us): " << user_time+system_time << std::endl;
    std::cout << fourth_sampling.size() << std::endl;
    /// lca



    /// suffix link
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int node : third_sampling) {
        pbtrlcsacst->suffix_link(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total suffix_link Time(us): " << user_time+system_time << std::endl;
    std::cout << third_sampling.size() << std::endl;
    /// suffix link



    /// sDepth
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int node : first_sampling) {
        pbtrlcsacst->string_depth(node);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total string_depth Time(us): " << user_time+system_time << std::endl;
    std::cout << first_sampling.size() << std::endl;
    /// sDepth

    std::vector<int> sdepths;
    for (int node: sixth_sampling) {
        int depth = pbtrlcsacst->string_depth(node) + 1 ; ///CHECK THIS
        sdepths.push_back(rand() % depth);
    }


    /// string_ancestor
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int i = 0; i < sixth_sampling.size(); ++i) {
        pbtrlcsacst->string_ancestor(sixth_sampling[i], sdepths[i]);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "string_ancestor Time(us): " << user_time+system_time << std::endl;
    std::cout << sixth_sampling.size() << std::endl;
    /// string_ancestor


    /// letter
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int node : second_sampling) {
        pbtrlcsacst->string(node, 3);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total letter Time(us): " << user_time+system_time << std::endl;
    std::cout << second_sampling.size() << std::endl;
    /// letter

/*
    std::vector<int> fifth_sampling;
    for (int node : first_sampling) {
        if (!pbtrlcsacst->is_leaf(node)) fifth_sampling.push_back(node);
    }
    std::cout << "MARCA" << fifth_sampling.size()<< std::endl;
*/
    std::vector<int> letters;
    for (int node: second_sampling) {
        int c = sada.children(node).size();
        int r = sada.select_child(node, (rand() % c) + 1);
        letters.push_back(sada.edge(r, 1+sada.depth(node)));
    }

    /// child
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int i = 0; i < second_sampling.size(); ++i) {
        pbtrlcsacst->labeled_child(second_sampling[i], letters[i]);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total child Time(us): " << user_time+system_time << std::endl;
    std::cout << second_sampling.size() << std::endl;
    /// child

    return 0;
}

