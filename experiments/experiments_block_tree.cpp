#include <iostream>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <stdlib.h>

#include <compressed/CBlockTree.h>
#include <compressed/PCBlockTree.h>


void c_access_experiment(PCBlockTree* cbt, std::vector<int>& permutation) {
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int i = 0; i < permutation.size(); ++i) {
        cbt->c_access(permutation[i]);
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    long user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    long system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total Access Time(us): " << user_time+system_time << std::endl;
}

void access_experiment(PCBlockTree* cbt, std::vector<int>& permutation) {
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int i = 0; i < permutation.size(); ++i) {
        cbt->access(permutation[i]);
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    int user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    int system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total Access Time(us): " << user_time+system_time << std::endl;
}

void access_experiment(CBlockTree* cbt, std::vector<int>& permutation) {
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int i = 0; i < permutation.size(); ++i) {
        cbt->access(permutation[i]);
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    int user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    int system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total Access Time(us): " << user_time+system_time << std::endl;
}

void ranks_experiment(CBlockTree* cbt, std::vector<int>& permutation, std::vector<int>& input_permutation) {
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int i = 0; i < permutation.size(); ++i) {
        cbt->rank(input_permutation[i] , permutation[i]);
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    int user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    int system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total rank Time(us): " << user_time+system_time << std::endl;





    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int i = 0; i < permutation.size(); ++i) {
        cbt->rank_alternative(input_permutation[i] , permutation[i]);
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total rank_alternative Time(us): " << user_time+system_time << std::endl;



    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int i = 0; i < permutation.size(); ++i) {
        cbt->better_rank(input_permutation[i] , permutation[i]);
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total better_rank Time(us): " << user_time+system_time << std::endl;
}



void ranks_experiment(PCBlockTree* cbt, std::vector<int>& permutation, std::vector<int>& input_permutation) {
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int i = 0; i < permutation.size(); ++i) {
        cbt->rank(input_permutation[i] , permutation[i]);
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    int user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    int system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total rank Time(us): " << user_time+system_time << std::endl;





    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int i = 0; i < permutation.size(); ++i) {
        cbt->rank_alternative(input_permutation[i] , permutation[i]);
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total rank_alternative Time(us): " << user_time+system_time << std::endl;



    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int i = 0; i < permutation.size(); ++i) {
        cbt->better_rank(input_permutation[i] , permutation[i]);
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total better_rank Time(us): " << user_time+system_time << std::endl;

    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int i = 0; i < permutation.size(); ++i) {
        cbt->better_paper_rank(input_permutation[i] , permutation[i]);
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total better_paper_rank Time(us): " << user_time+system_time << std::endl;
}



void c_ranks_experiment(PCBlockTree* cbt, std::vector<int>& permutation, std::vector<int>& input_permutation) {
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int i = 0; i < permutation.size(); ++i) {
        cbt->noback_rank_alternative(input_permutation[i] , permutation[i]);
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    long user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    long system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total noback_rank_alternative Time(us): " << user_time+system_time << std::endl;

    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int i = 0; i < permutation.size(); ++i) {
        cbt->c_rank_alternative(input_permutation[i] , permutation[i]);
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total c_rank_alternative Time(us): " << user_time+system_time << std::endl;
}


void selects_experiments(CBlockTree* cbt, std::vector<int>& characters, std::unordered_map<int, std::vector<int>>& select_permutations) {
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int c : characters) {
        std::vector<int>& s_p = select_permutations[c];
        for (int i = 0; i < s_p.size(); ++i) {
            cbt->select(c, s_p[i]);
        }
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    int user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    int system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total select Time(us): " << user_time+system_time << std::endl;



    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int c : characters) {
        for (int j : select_permutations[c]) {
            cbt->select_alternative(c, j);
        }
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total select_alternative Time(us): " << user_time+system_time << std::endl;



    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int c : characters) {
        for (int j : select_permutations[c]) {
            cbt->better_select(c, j);
        }
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total better_select Time(us): " << user_time+system_time << std::endl;
}



void selects_experiments(PCBlockTree* cbt, std::vector<int>& characters, std::unordered_map<int, std::vector<int>>& select_permutations) {
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int c : characters) {
        std::vector<int>& s_p = select_permutations[c];
        for (int i = 0; i < s_p.size(); ++i) {
            cbt->select(c, s_p[i]);
        }
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    int user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    int system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total select Time(us): " << user_time+system_time << std::endl;



    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int c : characters) {
        for (int j : select_permutations[c]) {
            cbt->select_alternative(c, j);
        }
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total select_alternative Time(us): " << user_time+system_time << std::endl;



    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int c : characters) {
        for (int j : select_permutations[c]) {
            cbt->better_select(c, j);
        }
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total better_select Time(us): " << user_time+system_time << std::endl;

    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int c : characters) {
        for (int j : select_permutations[c]) {
            cbt->better_paper_select(c, j);
        }
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total better_paper_select Time(us): " << user_time+system_time << std::endl;

}

void c_selects_experiments(PCBlockTree* cbt, std::vector<int>& characters, std::unordered_map<int, std::vector<int>>& select_permutations) {
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int c : characters) {
        for (int j : select_permutations[c]) {
            cbt->noback_select_alternative(c, j);
        }
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    long user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    long system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total noback_select_alternative Time(us): " << user_time+system_time << std::endl;


    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;

    for (int c : characters) {
        for (int j : select_permutations[c]) {
            cbt->c_select_alternative(c, j);
        }
    }

    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);

    std::cout << "Total c_select_alternative Time(us): " << user_time+system_time << std::endl;

}


void experiments(CBlockTree* cbt, std::vector<int>& characters, std::vector<int>& permutation, std::unordered_map<int, std::vector<int>>& select_permutations, std::vector<int>& input_permutation) {
    access_experiment(cbt, permutation);
    //ranks_experiment(cbt, permutation, input_permutation);
    //selects_experiments(cbt, characters, select_permutations);
}

void experiments(PCBlockTree* cbt, std::vector<int>& characters, std::vector<int>& permutation, std::unordered_map<int, std::vector<int>>& select_permutations, std::vector<int>& input_permutation) {
    access_experiment(cbt, permutation);
    //ranks_experiment(cbt, permutation, input_permutation);
    //selects_experiments(cbt, characters, select_permutations);
}


void c_experiments(PCBlockTree* cbt, std::vector<int>& characters, std::vector<int>& permutation, std::unordered_map<int, std::vector<int>>& select_permutations, std::vector<int>& input_permutation) {
    c_access_experiment(cbt, permutation);
    c_ranks_experiment(cbt, permutation, input_permutation);
    c_selects_experiments(cbt, characters, select_permutations);
}


int main() {
    std::string input;
    std::ifstream t("/home/mcaceres/BlockTree/build/in");
    std::stringstream buffer;
    buffer << t.rdbuf();
    input = buffer.str();


    std::vector<int> permutation;
    for (int i = 0; i<input.size(); ++i)
        permutation.push_back(i);
    int seed = 0;
    std::shuffle(permutation.begin(), permutation.end(), std::default_random_engine(seed));

    std::vector<int> input_permutation;
    for (int i = 0; i<input.size(); ++i)
        input_permutation.push_back(input[i]);
    seed = 0;
    std::shuffle(input_permutation.begin(), input_permutation.end(), std::default_random_engine(seed));


    std::unordered_set<int> characters_set;
    for (char c: input) {
        characters_set.insert(c);
    }
    std::unordered_map<int,int> characters_map;
    for (int c : characters_set)
        characters_map[c] = 0;
    for (char c: input)
        characters_map[c] = characters_map[c] + 1;

    std::vector<int> characters;
    for (int c : characters_set)
        characters.push_back(c);
    std::shuffle(characters.begin(), characters.end(), std::default_random_engine(seed));


    std::unordered_map<int, std::vector<int>> select_permutations;
    for (auto pair: characters_map){
        std::vector<int> permutation;
        for (int i = 1; i<=pair.second; ++i)
            permutation.push_back(i);

        int seed = 0;
        std::shuffle(permutation.begin(), permutation.end(), std::default_random_engine(seed));
        select_permutations[pair.first] = permutation;
    }
/*
    for (int r = 2; r<=8; r*=2) {
        for (int max_leaf_length = 4; max_leaf_length <= 128; max_leaf_length*=2) {
            std::cout << "r=" << r << ", mll=" << max_leaf_length << std::endl << std::endl;
            BlockTree *bt = new BlockTree(input, r, max_leaf_length);
            bt->process_back_pointers();
            bt->clean_unnecessary_expansions();
            for (int c: characters)
                bt->add_rank_select_support(c);
            std::cout << "bt" << std::endl;
            bt->print_statistics();

            CBlockTree *cbt = new CBlockTree(bt);
            std::cout << "cbt" << std::endl;
            cbt->print_statistics();
            experiments(cbt, characters, permutation, select_permutations, input_permutation);
            std::cout << std::endl;

            delete bt;
            delete cbt;

            BlockTree *liberal_heuristic_bt = new BlockTree(input, r, max_leaf_length-1);
            liberal_heuristic_bt->process_back_pointers_liberal_heuristic();

            for (int c: characters)
                liberal_heuristic_bt->add_rank_select_support(c);
            std::cout << "liberal_heuristic_bt" << std::endl;
            liberal_heuristic_bt->print_statistics();

            CBlockTree *liberal_heuristic_cbt = new CBlockTree(liberal_heuristic_bt);
            std::cout << "liberal_heuristic_cbt" << std::endl;
            liberal_heuristic_cbt->print_statistics();
            experiments(liberal_heuristic_cbt, characters, permutation, select_permutations, input_permutation);


            std::cout << std::endl;
            std::cout << std::endl;

            delete liberal_heuristic_bt;
  	    delete liberal_heuristic_cbt;

        }
    }
*/

    for (int r = 2; r<=8; r*=2) {
        for (int max_leaf_length = 4; max_leaf_length <= 128; max_leaf_length*=2) {
            std::cout << "r=" << r << ", mll=" << max_leaf_length << std::endl << std::endl;
            PBlockTree *bt = new PBlockTree(input, r, max_leaf_length);
            bt->process_back_front_pointers();
            //bt->clean_unnecessary_expansions();
            for (int c: characters)
                bt->add_rank_select_support(c);
            std::cout << "padding_bt_noclean" << std::endl;
            bt->print_statistics();
            PCBlockTree *cbt = new PCBlockTree(bt);
            std::cout << "padding_cbt_noclean" << std::endl;
            cbt->print_statistics();
            experiments(cbt, characters, permutation, select_permutations, input_permutation);
            std::cout << std::endl;
            delete bt;
            delete cbt;

            for (int c = 2; c <= 16; c*=2) {
                bt = new PBlockTree(input, r, max_leaf_length);
                bt->process_back_pointers();
                bt->clean_unnecessary_expansions(c);
                for (int c: characters)
                    bt->add_rank_select_support(c);
                std::cout << "padding_pruning_"<<c<<"_bt" << std::endl;
                bt->print_statistics();
                cbt = new PCBlockTree(bt);
                std::cout << "padding_pruning_"<<c<<"_cbt" << std::endl;
                cbt->print_statistics();
                experiments(cbt, characters, permutation, select_permutations, input_permutation);
                std::cout << std::endl;
                delete bt;
                delete cbt;
            }


            bt = new PBlockTree(input, r, max_leaf_length);
            bt->process_back_pointers_heuristic();
            for (int c: characters)
                bt->add_rank_select_support(c);
            std::cout << "heuristic_bt" << std::endl;
            bt->print_statistics();
            cbt = new PCBlockTree(bt);
            std::cout << "heuristic_cbt" << std::endl;
            cbt->print_statistics();
            experiments(cbt, characters, permutation, select_permutations, input_permutation);
            std::cout << std::endl;
            delete bt;
            delete cbt;


            bt = new PBlockTree(input, r, max_leaf_length);
            bt->process_back_pointers_conservative_heuristic();
            for (int c: characters)
                bt->add_rank_select_support(c);
            std::cout << "conservative_heuristic_bt" << std::endl;
            bt->print_statistics();
            cbt = new PCBlockTree(bt);
            std::cout << "conservative_heuristic_cbt" << std::endl;
            cbt->print_statistics();
            experiments(cbt, characters, permutation, select_permutations, input_permutation);
            std::cout << std::endl;
            delete bt;
            delete cbt;

            bt = new PBlockTree(input, r, max_leaf_length);
            bt->process_back_pointers_heuristic_concatenate();
            for (int c: characters)
                bt->add_rank_select_support(c);
            std::cout << "heuristic_concatenate_bt" << std::endl;
            bt->print_statistics();
            cbt = new PCBlockTree(bt);
            std::cout << "heuristic_concatenate_cbt" << std::endl;
            cbt->print_statistics();
            experiments(cbt, characters, permutation, select_permutations, input_permutation);
            std::cout << std::endl;
            delete bt;
            delete cbt;


            bt = new PBlockTree(input, r, max_leaf_length);
            bt->process_back_pointers_conservative_optimized_heuristic();
            for (int c: characters)
                bt->add_rank_select_support(c);
            std::cout << "conservative_optimized_heuristic_bt" << std::endl;
            bt->print_statistics();
            cbt = new PCBlockTree(bt);
            std::cout << "conservative_optimized_heuristic_cbt" << std::endl;
            cbt->print_statistics();
            experiments(cbt, characters, permutation, select_permutations, input_permutation);
            std::cout << std::endl;
            delete bt;
            delete cbt;


            bt = new PBlockTree(input, r, max_leaf_length);
            bt->process_back_pointers_other_conservative_heuristic();
            for (int c: characters)
                bt->add_rank_select_support(c);
            std::cout << "other_conservative_heuristic_bt" << std::endl;
            bt->print_statistics();
            cbt = new PCBlockTree(bt);
            std::cout << "other_conservative_heuristic_cbt" << std::endl;
            cbt->print_statistics();
            experiments(cbt, characters, permutation, select_permutations, input_permutation);
            std::cout << std::endl;
            delete bt;
            delete cbt;


            bt = new PBlockTree(input, r, max_leaf_length);
            bt->process_back_pointers_liberal_heuristic();
            for (int c: characters)
                bt->add_rank_select_support(c);
            std::cout << "liberal_heuristic_bt" << std::endl;
            bt->print_statistics();
            cbt = new PCBlockTree(bt);
            std::cout << "liberal_heuristic_cbt" << std::endl;
            cbt->print_statistics();
            experiments(cbt, characters, permutation, select_permutations, input_permutation);
            std::cout << std::endl;
            delete bt;
            delete cbt;
/*
	    PBlockTree *back_front_heuristic_bt = new PBlockTree(input, r, max_leaf_length);
	    back_front_heuristic_bt->process_back_front_pointers();
	    for (int c: characters)
	    	back_front_heuristic_bt->add_rank_select_support(c);
	    std::cout << "padding_back_front_heuristic_bt" << std::endl;
	    back_front_heuristic_bt->print_statistics();
	    PCBlockTree *back_front_heuristic_cbt = new PCBlockTree(back_front_heuristic_bt);
	    std::cout << "padding_back_front_heuristic_cbt" << std::endl;
	    back_front_heuristic_cbt->print_statistics();
	    experiments(back_front_heuristic_cbt, characters, permutation, select_permutations, input_permutation);
	    std::cout << std::endl;
	    delete back_front_heuristic_bt;
	    delete back_front_heuristic_cbt;


	    PBlockTree *other_back_front_heuristic_bt = new PBlockTree(input, r, max_leaf_length);
	    other_back_front_heuristic_bt->process_other_back_front_pointers();
	    for (int c: characters)
	    	other_back_front_heuristic_bt->add_rank_select_support(c);
	    std::cout << "padding_other_back_front_heuristic_bt" << std::endl;
	    other_back_front_heuristic_bt->print_statistics();
	    PCBlockTree *other_back_front_heuristic_cbt = new PCBlockTree(other_back_front_heuristic_bt);
	    std::cout << "padding_other_back_front_heuristic_cbt" << std::endl;
	    other_back_front_heuristic_cbt->print_statistics();
	    experiments(other_back_front_heuristic_cbt, characters, permutation, select_permutations, input_permutation);
	    std::cout << std::endl;
	    delete other_back_front_heuristic_bt;
	    delete other_back_front_heuristic_cbt;
	    */
        }
    }
    return 0;
}
