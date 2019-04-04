#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>

#include <compressed/PCBlockTree.h>
#include <rlcsa/rlcsa.h>
#include <compressed/PBTCLCPA.h>
#include <compressed/PBTCSA.h>
#include <compressed/PBTCISA.h>

int main(int argc, char *argv[]) {

    char* filename = argv[1];
    const int Max_Sampling_Size = 100000;

    std::istringstream iss( argv[2] );
    int version_sa;
    iss >> version_sa;

    std::istringstream iss2( argv[3] );
    int r_sa;
    iss2 >> r_sa;

    std::istringstream iss3( argv[4] );
    int mll_sa;
    iss3 >> mll_sa;

    std::istringstream iss4( argv[5] );
    int c_sa;
    iss4 >> c_sa;

    std::istringstream iss5( argv[6] );
    int version_isa;
    iss5 >> version_isa;

    std::istringstream iss6( argv[7] );
    int r_isa;
    iss6 >> r_isa;

    std::istringstream iss7( argv[8] );
    int mll_isa;
    iss7 >> mll_isa;

    std::istringstream iss8( argv[9] );
    int c_isa;
    iss8 >> c_isa;

    std::istringstream iss9( argv[10] );
    int version_lcp;
    iss9 >> version_lcp;

    std::istringstream iss10( argv[11] );
    int r_lcp;
    iss10 >> r_lcp;

    std::istringstream iss11( argv[12] );
    int mll_lcp;
    iss11 >> mll_lcp;

    std::istringstream iss12( argv[13] );
    int c_lcp;
    iss12 >> c_lcp;


    std::string input_string;
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    input_string = buffer.str();
    t.close();
    //input_string = "manuelarielcaceresreyes";

    PBTCSA* pbtcsa = new PBTCSA(input_string, version_sa, r_sa, mll_sa, c_sa);
    PBTCISA* pbtcisa = new PBTCISA(input_string, version_isa, r_isa, mll_isa, c_isa);
    PBTCLCPA* pbtclcpa = new PBTCLCPA(input_string, version_lcp, r_lcp, mll_lcp, c_lcp);



    std::cout << "==SA=="<< std::endl;
    pbtcsa->print_statistics();
    std::cout << "==ISA=="<< std::endl;
    pbtcisa->print_statistics();
    std::cout << "==LCP=="<< std::endl;
    pbtclcpa->print_statistics();
    std::vector<int> pos;
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
        pbtcsa->access(s);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    long user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    long system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total SA ACCESS Time(us): " << user_time+system_time << std::endl;
    std::cout << pos.size() << std::endl;
    /// SA ACCESS

    /// SA ACCESS
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int s : pos) {
        pbtcsa->saccess(s);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total SA SACCESS Time(us): " << user_time+system_time << std::endl;
    std::cout << pos.size() << std::endl;
    /// SA ACCESS

    /// SA ACCESS
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int s : pos) {
        pbtcsa->caccess(s);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total SA CACCESS Time(us): " << user_time+system_time << std::endl;
    std::cout << pos.size() << std::endl;
    /// SA ACCESS

    /// SA ACCESS
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int s : pos) {
        pbtcsa->csaccess(s);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total SA CSACCESS Time(us): " << user_time+system_time << std::endl;
    std::cout << pos.size() << std::endl;
    /// SA ACCESS


    /// LCP ACCESS
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int s : pos) {
        pbtclcpa->access(s);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total LCP ACCESS Time(us): " << user_time+system_time << std::endl;
    std::cout << pos.size() << std::endl;
    /// LCP ACCESS

    /// LCP ACCESS
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int s : pos) {
        pbtclcpa->saccess(s);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total LCP SACCESS Time(us): " << user_time+system_time << std::endl;
    std::cout << pos.size() << std::endl;
    /// LCP ACCESS

    /// LCP ACCESS
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int s : pos) {
        pbtclcpa->caccess(s);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total LCP CACCESS Time(us): " << user_time+system_time << std::endl;
    std::cout << pos.size() << std::endl;
    /// LCP ACCESS

    /// LCP ACCESS
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int s : pos) {
        pbtclcpa->csaccess(s);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total LCP CSACCESS Time(us): " << user_time+system_time << std::endl;
    std::cout << pos.size() << std::endl;
    /// LCP ACCESS


    /// ISA ACCESS
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int s : pos) {
        pbtcisa->access(s);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total ISA ACCESS Time(us): " << user_time+system_time << std::endl;
    std::cout << pos.size() << std::endl;
    /// ISA ACCESS

    /// ISA ACCESS
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int s : pos) {
        pbtcisa->saccess(s);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total ISA SACCESS Time(us): " << user_time+system_time << std::endl;
    std::cout << pos.size() << std::endl;
    /// ISA ACCESS

    /// ISA ACCESS
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int s : pos) {
        pbtcisa->caccess(s);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total ISA CACCESS Time(us): " << user_time+system_time << std::endl;
    std::cout << pos.size() << std::endl;
    /// ISA ACCESS

    /// ISA ACCESS
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int s : pos) {
        pbtcisa->csaccess(s);
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total ISA CSACCESS Time(us): " << user_time+system_time << std::endl;
    std::cout << pos.size() << std::endl;
    /// ISA ACCESS


    return 0;
}

