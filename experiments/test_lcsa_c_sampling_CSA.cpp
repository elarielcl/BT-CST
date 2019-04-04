#include <chrono>
#include <fstream>
#include <iostream>
#include <sdsl/csa_bitcompressed.hpp>
#include <lcsa/repair_differential_array_c_sampling.hpp>
#include <sdsl/lcp_bitcompressed.hpp>

int main(int argc, char **argv) {


    //Paso 1

    std::ifstream ifs(argv[1]);
    std::string content( (std::istreambuf_iterator<char>(ifs) ),
                         (std::istreambuf_iterator<char>()    ) );

    const int Max_Sampling_Size = 100000;


    sdsl::csa_bitcompressed<> csa;
    sdsl::construct_im(csa, content, 1);
    sdsl::lcp_bitcompressed<> lcp;
    sdsl::construct_im(lcp, content, 1);

    repair_differential_array_c_sampling::repair_differential_array_c_sampling dsa(csa.sa_sample);

    sdsl::int_vector<> isa(csa.size(),0);
    for (int i = 0; i < isa.size(); ++i) {
        isa[csa[i]] = i;
    }
    repair_differential_array_c_sampling::repair_differential_array_c_sampling disa(isa);
    repair_differential_array_c_sampling::repair_differential_array_c_sampling dlcp(lcp);


    std::cout << "Bytes LCSA(SA) " << dsa.size_in_bytes() << std::endl;
    std::cout << "Bytes LCSA(ISA) " << disa.size_in_bytes() << std::endl;
    std::cout << "Bytes LCSA(LCP) " << dlcp.size_in_bytes() << std::endl;

    std::vector<int> pos;

    for (int i = 0; i < Max_Sampling_Size; ++i) {
        int r = (rand() % content.size());
        pos.push_back(r);
    }


    /// SA ACCESS
    struct rusage usage;
    struct timeval ru_start, rs_start, ru_end, rs_end;
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int s : pos) {
        dsa[s];
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
        dlcp[s];
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total LCP ACCESS Time(us): " << user_time+system_time << std::endl;
    std::cout << pos.size() << std::endl;
    /// LCP ACCESS

    /// ISA ACCESS
    getrusage(RUSAGE_SELF, &usage);
    rs_start = usage.ru_stime;
    ru_start = usage.ru_utime;
    for (int s : pos) {
        disa[s];
    }
    getrusage(RUSAGE_SELF, &usage);
    rs_end = usage.ru_stime;
    ru_end = usage.ru_utime;
    user_time = (ru_end.tv_sec - ru_start.tv_sec)*1000000 + (ru_end.tv_usec - ru_start.tv_usec);
    system_time = (rs_end.tv_sec - rs_start.tv_sec)*1000000 + (rs_end.tv_usec - rs_start.tv_usec);
    std::cout << "Total ISA ACCESS Time(us): " << user_time+system_time << std::endl;
    std::cout << pos.size() << std::endl;
    /// ISA ACCESS


    return 0;

}

