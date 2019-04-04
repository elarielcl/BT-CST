#include <string>
#include <sdsl/construct.hpp>
#include <compressed/CST_SCT3_RLCSA.h>


void find_maximal_substrings(CST_SCT3_RLCSA* sct3_rlcsa, std::string& S) {
    int64_t i = 0;
    int64_t j = -1;
    sdsl::bp_interval<uint64_t> v = sct3_rlcsa->root();
    int64_t l = 0;
    int64_t string_depth_v = 0;
    sdsl::bp_interval<uint64_t> u = sct3_rlcsa->root();
    int64_t string_depth_u = 0;
    int64_t max_maximal = -1;

    while (true) {
        // Try to descend
        bool uncomplete_match = false;
        bool advance = false;
        if (l != 0) {

            while (string_depth_v+l < string_depth_u) {
                if (sct3_rlcsa->string(u, (string_depth_v+l)) != S[j+1]) {
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
                u = sct3_rlcsa->labeled_child(v, S[j+1]);
                if (u == sct3_rlcsa->root()) {
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
                string_depth_u = sct3_rlcsa->string_depth(u);
                bool uncomplete_match = false;
                while (string_depth_v+l < string_depth_u) {
                    if (sct3_rlcsa->string(u, (string_depth_v+l)) != S[j+1]) {
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
            sdsl::bp_interval<uint64_t> u_p = sct3_rlcsa->suffix_link(v);
            v = u = u_p;
            string_depth_v = string_depth_u = sct3_rlcsa->string_depth(u_p);
        } else {

            sdsl::bp_interval<uint64_t> u_p = sct3_rlcsa->suffix_link(u);
            int64_t string_depth_u_p = sct3_rlcsa->string_depth(u_p);
            int64_t sdepth = string_depth_u_p - (string_depth_u-string_depth_v-l);
            sdsl::bp_interval<uint64_t> node = u_p;
            while (node != sct3_rlcsa->root()) {
                sdsl::bp_interval<uint64_t> parent = sct3_rlcsa->parent(node);
                if (sct3_rlcsa->string_depth(parent) < sdepth) break;
                node = parent;
            }
            if (node == sct3_rlcsa->root()) {
                string_depth_u = string_depth_v = l = 0;
                v = u = sct3_rlcsa->root();
            } else {
                u = node;
                string_depth_u = sct3_rlcsa->string_depth(u);
                if (string_depth_u == sdepth) {
                    string_depth_v = string_depth_u;
                    v = u;
                    l = 0;
                } else {
                    v = sct3_rlcsa->parent(node);
                    string_depth_v = sct3_rlcsa->string_depth(v);
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

    //S = "ACTAGCAAAAAAAGATGAGTCTTCTAACCGAGGTCGAAACGTACGTTCTCTCTATCGTCCCGTCAGGCCCCCTCAAAGCCGAGATCGCACAGAGA";
    std::string base_name = filenameT;
    CST_SCT3_RLCSA* sct3_rlcsa = new CST_SCT3_RLCSA(base_name, T);

    int rep_times = 100;
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int i = 0; i < rep_times; ++i)
        find_maximal_substrings(sct3_rlcsa, S);
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    long user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    long system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total Time(us): " << user_time+system_time << std::endl;
}
