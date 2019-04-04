#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>

#include <compressed/PCBlockTree.h>
#include <rlcsa/rlcsa.h>
#include <compressed/PBTRLCSACST.h>

int main(int argc, char *argv[]) {

    char* filename = argv[1];
    const int Max_Sampling_Size = 100000;

    std::istringstream iss( argv[2] );
    int block_size;
    iss >> block_size;

    std::istringstream iss2( argv[3] );
    int sa_sampling_rate;
    iss2 >> sa_sampling_rate;


    std::string input_string;
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    input_string = buffer.str();
    t.close();
    std::string extension = ".csa_" + std::to_string(block_size) + "_" + std::to_string(sa_sampling_rate);
    //input_string = "manuelarielcaceresreyes";

    std::string base_name = filename;
    TextIndexRLCSA* index = (TextIndexRLCSA*)TextIndexRLCSA::load((base_name+extension).c_str());
    TextIndexes::RLCSA* rlcsa;
    rlcsa = index->rlcsa;
    ifstream lcp_in(base_name+".lcp_4");
    cds_static::LCP_RLCSA* lcp_rlcsa = LCP_RLCSA::load(lcp_in);
    lcp_in.close();

    std::vector<int> pos;

    std::cout << "RLCSA Size: " << rlcsa->getSize() << std::endl;
    rlcsa->reportSize(true);
    std::cout << "LCP Size: " << lcp_rlcsa->getSize() << std::endl;


    for (int i = 0; i < Max_Sampling_Size; ++i) {
        int r = (rand() % input_string.length());
        pos.push_back(r);
    }


    /// SA ACCESS
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int s : pos) {
        rlcsa->locate(s);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    long user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    long system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total SA ACCESS Time(us): " << user_time+system_time << std::endl;
    std::cout << pos.size() << std::endl;
    /// SA ACCESS


    /// LCP ACCESS
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int s : pos) {
        lcp_rlcsa->get_LCP(s, index);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total LCP ACCESS Time(us): " << user_time+system_time << std::endl;
    std::cout << pos.size() << std::endl;
    /// LCP ACCESS


    /// PSI ACCESS
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int s : pos) {
        rlcsa->psi(s);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total PSI ACCESS Time(us): " << user_time+system_time << std::endl;
    std::cout << pos.size() << std::endl;
    /// PSI ACCESS


    return 0;
}

