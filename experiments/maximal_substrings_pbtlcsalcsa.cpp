#include <string>
#include <sdsl/construct.hpp>
#include <sdsl/cst_sada.hpp>
#include <compressed/PBTLCSALCSACST.h>


void find_maximal_substrings(PBTLCSALCSACST* pbtrlcsacst, std::string& S) {
    int64_t i = 0;
    int64_t j = -1;
    int64_t v = pbtrlcsacst->root();
    int64_t l = 0;
    int64_t string_depth_v = 0;
    int64_t u = pbtrlcsacst->root();
    int64_t string_depth_u = 0;
    int64_t max_maximal = -1;

    while (true) {
        // Try to descend
        bool uncomplete_match = false;
        bool advance = false;
        if (l != 0) {

            while (string_depth_v+l < string_depth_u) {
                if (pbtrlcsacst->string(u, (string_depth_v+l)) != S[j+1]) {
                    uncomplete_match = true;
                    break;
                }
                ++j;
                advance = true;
                if (j == S.length()-1) {
                    if (j-i+1 > max_maximal) max_maximal = j-i+1;
                    std::cout << max_maximal << std::endl;
//                    std::cout << "Maximal: " << i << ", " << j << std::endl;
                    return;
                }
                ++l;
            }
            if (uncomplete_match) {
                if (advance)
                    if (j-i+1 > max_maximal) max_maximal = j-i+1;
//                    std::cout << "Maximal: " << i << ", " << j << std::endl;

                if (j < i) ++j;
                ++i;
                if (i == S.length()) {
                    std::cout << max_maximal << std::endl;
                    return;}
            }
        }

        if (!uncomplete_match) {
            while (true) {

                u = pbtrlcsacst->labeled_child(v, S[j+1]);
                //if (215250 == u)
                //     std::cout << std::endl;
                //std::cout << u << std::endl;
                if (u == -1) {
                    if (advance)
                        if (j-i+1 > max_maximal) max_maximal = j-i+1;
//                    std::cout << "Maximal: " << i << ", " << j << std::endl;

                    if (j < i) ++j;
                    ++i;
                    if (i == S.length()) {
                        //std::cout << i << ", " << j << std::endl;
                        std::cout << max_maximal << std::endl;
                        return;
                    }
                    break;
                }
                advance = true;
                ++j;

                ++l;
                if (j == S.length()-1) {
                    if (j-i+1 > max_maximal) max_maximal = j-i+1;
                    std::cout << max_maximal << std::endl;
//                    std::cout << "Maximal: " << i << ", " << j << std::endl;
                    return;
                }
                string_depth_u = pbtrlcsacst->string_depth(u);
                bool uncomplete_match = false;
                while (string_depth_v+l < string_depth_u) {
                    if (pbtrlcsacst->string(u, (string_depth_v+l)) != S[j+1]) {
                        uncomplete_match = true;
                        break;
                    }
                    ++j;
                    if (j == S.length()-1) {
                        if (j-i+1 > max_maximal) max_maximal = j-i+1;
                        std::cout << max_maximal << std::endl;
//                    std::cout << "Maximal: " << i << ", " << j << std::endl;
                        return;
                    }
                    ++l;
                }
                if (uncomplete_match) {
                    if (advance)
                        if (j-i+1 > max_maximal) max_maximal = j-i+1;
//                    std::cout << "Maximal: " << i << ", " << j << std::endl;
                    if (j < i) ++j;
                    ++i;
                    if (i == S.length()) {
                        std::cout << max_maximal << std::endl;
                        return;}
                    break;
                }

                l = 0;
                v = u;
                string_depth_v = string_depth_u;

            }
        }
        // Try to descend

        // Take suffix link
        if (l == 0) {
            int64_t u_p = pbtrlcsacst->suffix_link(v);
            v = u = u_p;
            string_depth_v = string_depth_u = pbtrlcsacst->string_depth(u_p);
        } else {
            int64_t u_p = pbtrlcsacst->suffix_link(u);
            int64_t string_depth_u_p = pbtrlcsacst->string_depth(u_p);
            int64_t sdepth = string_depth_u_p - (string_depth_u-string_depth_v-l);
            int64_t node = pbtrlcsacst->string_ancestor(u_p, sdepth);
            if (node == -1) {
                string_depth_u = string_depth_v = l = 0;
                v = u = pbtrlcsacst->root();
            } else {
                u = node;
                string_depth_u = pbtrlcsacst->string_depth(u);
                if (string_depth_u == sdepth) {
                    string_depth_v = string_depth_u;
                    v = u;
                    l = 0;
                } else {
                    v = pbtrlcsacst->parent(node);
                    string_depth_v = pbtrlcsacst->string_depth(v);
                    l = sdepth - string_depth_v;
                }
            }
        }
        // Take suffix link
    }


}

int main(int argc, char *argv[]) {

    char* filenameT = argv[1];
    char* filenameS = argv[2];

    std::string T;
    std::ifstream t(filenameT);
    std::stringstream bufferT;
    bufferT << t.rdbuf();
    T = bufferT.str();
    t.close();

    std::string S;
    std::ifstream s(filenameS);
    std::stringstream bufferS;
    bufferS << s.rdbuf();
    S = bufferS.str();
    s.close();

    std::istringstream iss( argv[3] );
    int block_size;
    iss >> block_size;

    std::istringstream iss2( argv[4] );
    int sa_sampling_rate;
    iss2 >> sa_sampling_rate;

    //S = "ACTAGCAAAAGCAGGTAGATGTTG";//AAAGATGAGTCTTCTAACCGAGGTCGAAACGTACGTTCTCTCTATCGTCCCGTCAGGCCCCCTCAAAGCCGAGATCGCACAGAGA";

    std::string base_name = filenameT;
    std::cout << base_name << std::endl;
    PBTLCSALCSACST * pbtrlcsacst = new PBTLCSALCSACST(base_name, T, 0, 2, 128, 2, block_size, sa_sampling_rate);

    int rep_times = 100;
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    //for (int i = 0; i < rep_times; ++i)
    find_maximal_substrings(pbtrlcsacst, S);
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    long user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    long system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total Time(us): " << user_time+system_time << std::endl;
}
