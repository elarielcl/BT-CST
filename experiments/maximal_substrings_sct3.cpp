#include <string>
#include <sdsl/construct.hpp>
#include <sdsl/cst_sct3.hpp>


void find_maximal_substrings(sdsl::cst_sct3<sdsl::csa_wt<sdsl::wt_huff<sdsl::rrr_vector<>>,32,64>,
        sdsl::lcp_support_sada<sdsl::rrr_vector<>>>& sct3T, std::string& S) {
    int64_t i = 0;
    int64_t j = -1;
    sdsl::bp_interval<uint64_t> v = sct3T.root();
    int64_t l = 0;
    int64_t string_depth_v = 0;
    sdsl::bp_interval<uint64_t> u = sct3T.root();
    int64_t string_depth_u = 0;
    int64_t max_maximal = -1;

    while (true) {
        // Try to descend
        bool uncomplete_match = false;
        bool advance = false;
        if (l != 0) {

            while (string_depth_v+l < string_depth_u) {
                if (sct3T.edge(u, (string_depth_v+l)+1) != S[j+1]) {
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
                u = sct3T.child(v, S[j+1]);
                if (u == sct3T.root()) {
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
                string_depth_u = sct3T.depth(u);
                bool uncomplete_match = false;
                while (string_depth_v+l < string_depth_u) {
                    if (sct3T.edge(u, (string_depth_v+l)+1) != S[j+1]) {
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
            sdsl::bp_interval<uint64_t> u_p = sct3T.sl(v);
            v = u = u_p;
            string_depth_v = string_depth_u = sct3T.depth(u_p);
        } else {

            sdsl::bp_interval<uint64_t> u_p = sct3T.sl(u);
            int64_t string_depth_u_p = sct3T.depth(u_p);
            int64_t sdepth = string_depth_u_p - (string_depth_u-string_depth_v-l);
            sdsl::bp_interval<uint64_t> node = u_p;
            while (node != sct3T.root()) {
                sdsl::bp_interval<uint64_t> parent = sct3T.parent(node);
                if (sct3T.depth(parent) < sdepth) break;
                node = parent;
            }
            if (node == sct3T.root()) {
                string_depth_u = string_depth_v = l = 0;
                v = u = sct3T.root();
            } else {
                u = node;
                string_depth_u = sct3T.depth(u);
                if (string_depth_u == sdepth) {
                    string_depth_v = string_depth_u;
                    v = u;
                    l = 0;
                } else {
                    v = sct3T.parent(node);
                    string_depth_v = sct3T.depth(v);
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
    sdsl::cst_sct3<sdsl::csa_wt<sdsl::wt_huff<sdsl::rrr_vector<>>,32,64>,
            sdsl::lcp_support_sada<sdsl::rrr_vector<>>> sct3T;
    sdsl::construct_im(sct3T, T, 1);

    int rep_times = 100;
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int i = 0; i < rep_times; ++i)
        find_maximal_substrings(sct3T, S);
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    long user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    long system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total Time(us): " << user_time+system_time << std::endl;
}
