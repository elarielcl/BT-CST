#include <string>
#include <sdsl/construct.hpp>
#include <sdsl/cst_sada.hpp>
#include <compressed/CST_SADA_RLCSA.h>


void find_maximal_substrings(CST_SADA_RLCSA* sada_rlcsa, std::string& S, std::string& T) {
    int64_t i = 0;
    int64_t j = -1;
    int64_t v = sada_rlcsa->root();
    int64_t l = 0;
    int64_t string_depth_v = 0;
    int64_t u = sada_rlcsa->root();
    int64_t string_depth_u = 0;
    int64_t max_maximal = -1;

    while (true) {
        // Try to descend
        bool uncomplete_match = false;
        bool advance = false;
        if (l != 0) {
            auto& m_bp_rank10 = *(sada_rlcsa->pm_bp_rank10);
            int lr = m_bp_rank10(u);
            int index = sada_rlcsa->rlcsa_->locate(lr);
            while (string_depth_v+l < string_depth_u) {

                char c;
                if (index+string_depth_v+l >= T.length()) c = '$';
                else c = T[index+string_depth_v+l];
                //psi_ = rlcsa_->getPsi(lr,i);
                //int r = rlcsa_->getT(psi_);
                //if (r == 0) return '$';
                //return r;
                //c = sada_rlcsa->string(u, (string_depth_v+l));
                if (c != S[j+1]) {
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
                u = sada_rlcsa->labeled_child(v, S[j+1]);
                //std::cout << u << std::endl;
                if (u == sada_rlcsa->root()) {
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
                string_depth_u = sada_rlcsa->string_depth(u);
                bool uncomplete_match = false;
                auto& m_bp_rank10 = *(sada_rlcsa->pm_bp_rank10);
                int lr = m_bp_rank10(u);
                int index = sada_rlcsa->rlcsa_->locate(lr);
                while (string_depth_v+l < string_depth_u) {

                    char c;
                    if (index+string_depth_v+l >= T.length()) c = '$';
                    else c = T[index+string_depth_v+l];
                    //psi_ = rlcsa_->getPsi(lr,i);
                    //int r = rlcsa_->getT(psi_);
                    //if (r == 0) return '$';
                    //return r;
                    //c = sada_rlcsa->string(u, (string_depth_v+l));
                    if (c != S[j+1]) {
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
            int64_t u_p = sada_rlcsa->suffix_link(v);
            v = u = u_p;
            string_depth_v = string_depth_u = sada_rlcsa->string_depth(u_p);
        } else {
            int64_t u_p = sada_rlcsa->suffix_link(u);
            int64_t string_depth_u_p = sada_rlcsa->string_depth(u_p);
            int64_t sdepth = string_depth_u_p - (string_depth_u-string_depth_v-l);
            int64_t node = sada_rlcsa->string_ancestor(u_p, sdepth);
            if (node == sada_rlcsa->root()) {
                string_depth_u = string_depth_v = l = 0;
                v = u = sada_rlcsa->root();
            } else {
                u = node;
                string_depth_u = sada_rlcsa->string_depth(u);
                if (string_depth_u == sdepth) {
                    string_depth_v = string_depth_u;
                    v = u;
                    l = 0;
                } else {
                    v = sada_rlcsa->parent(node);
                    string_depth_v = sada_rlcsa->string_depth(v);
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
    //S = "ACTAGCAAAAGCAGGTAGATGTTGAAAGATGAGTCTTCTAACCGAGGTCGAAACGTACGTTCTCTCTATCGTCCCGTCAGGCCCCCTCAAAGCCGAGATCGCACAGAGA";
    std::string base_name = filenameT;
    std::cout << base_name << std::endl;
    CST_SADA_RLCSA * sada_rlcsa = new CST_SADA_RLCSA(base_name, T);

    int rep_times = 100;
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    //for (int i = 0; i < rep_times; ++i)
        find_maximal_substrings(sada_rlcsa, S, T);
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    long user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    long system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total Time(us): " << user_time+system_time << std::endl;
}
