/***
BSD 2-Clause License

Copyright (c) 2018, Adrián
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/


//
// Created by Adrián on 19/11/2018.
//

#ifndef LCSA_REPAIR_HPP
#define LCSA_REPAIR_HPP


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <sdsl/int_vector.hpp>
#include <repair_gn/include/irepair.h>

namespace util_repair {


    typedef struct {
        int left, right;
    } Tpair;

    class repair {

    public:
        Tpair* rules;
        uint lenR;
        int alpha;
        int* c;
        uint lenC;

        sdsl::sd_vector<> m_translate;
        sdsl::rank_support_sd<1> m_rank_support;
        sdsl::select_support_sd<1> m_select_support;

    private:

        void read_rules_repair(const char *fileR) {
            FILE *Tf;
            struct stat s;

            if (stat(fileR, &s) != 0) {
                fprintf(stderr, "Error: cannot stat file %s\n", fileR);
                exit(1);
            }
            uint64_t n = s.st_size;
            Tf = fopen(fileR, "r");
            if (Tf == NULL) {
                fprintf(stderr, "Error: cannot open file %s for reading\n", fileR);
                exit(1);
            }

            if (fread(&alpha, sizeof(int), 1, Tf) != 1) {
                fprintf(stderr, "Error: cannot read file %s\n", fileR);
                exit(1);
            }
            lenR = (n - sizeof(int)) / sizeof(Tpair);
            rules = (Tpair *) malloc(lenR * sizeof(Tpair));
            if (fread(rules, sizeof(Tpair), lenR, Tf) != lenR) {
                fprintf(stderr, "Error: cannot read file %s\n", fileR);
                exit(1);
            }
            fclose(Tf);
        }

        void read_c_repair(const char *fileC) {
            FILE *Cf;
            struct stat s;

            if (stat(fileC, &s) != 0) {
                fprintf(stderr, "Error: cannot stat file %s\n", fileC);
                exit(1);
            }
            lenC = s.st_size / sizeof(int);
            Cf = fopen(fileC, "r");
            if (Cf == NULL) {
                fprintf(stderr, "Error: cannot open file %s for reading\n", fileC);
                exit(1);
            }
            c = (int *) malloc(lenC * sizeof(int));
            if (fread(c, sizeof(int), lenC, Cf) != lenC) {
                fprintf(stderr, "Error: cannot read file %s\n", fileC);
                exit(1);
            }
            fclose(Cf);
        }

        int* read_keys_repair(const char *fileC, uint &lenK) {
            FILE *Cf;
            struct stat s;

            if (stat(fileC, &s) != 0) {
                fprintf(stderr, "Error: cannot stat file %s\n", fileC);
                exit(1);
            }
            lenK = s.st_size / sizeof(int);
            Cf = fopen(fileC, "r");
            if (Cf == NULL) {
                fprintf(stderr, "Error: cannot open file %s for reading\n", fileC);
                exit(1);
            }
            int* k = (int *) malloc(lenK * sizeof(int));
            if (fread(k, sizeof(int), lenK, Cf) != lenK) {
                fprintf(stderr, "Error: cannot read file %s\n", fileC);
                exit(1);
            }
            fclose(Cf);
            return k;
        }

        void write_keys(const char* file, std::vector<int>& keys){
            FILE* f = fopen (file,"w");
            for(uint64_t i = 0; i<keys.size(); i++){
                int value = keys[i];
                fwrite(&value,1,sizeof(int),f);
            }
            fclose(f);
        }

        int translate(const int value_to_translate, const int &last, const std::vector<int> &keys){
            if(value_to_translate >= alpha){
                return last + (value_to_translate-alpha+1);
            }else{
                return keys[value_to_translate];
            }
        }

        int translate(const int value_to_translate, const int &last, const int* keys){
            if(value_to_translate >= alpha){
                return last + (value_to_translate-alpha+1);
            }else{
                return keys[value_to_translate];
            }
        }

        uint encode(int x) const {
            bool neg = x < 0;
            if (neg) x = -x;
            return (x<<1) + ((neg)?0:1);
        }


        int decode(uint x) const {
            bool pos = x%2;
            return ((pos)?1:-1) * (x>>1);
        }

    public:


        repair() = default;

        void run(int* &repair_ints, uint64_t len, std::map<int,int>& map_terminals, const int ram_mb){

            std::string repair_file = "repair";
            std::string fileR = repair_file + ".R";
            std::string fileC = repair_file + ".C";

            std::vector<uint> keys;
            for(auto it = map_terminals.begin(); it != map_terminals.end(); ++it) {
                keys.push_back(encode(it->first));
            }
            map_terminals.clear();

            // std::cout << "Keys: " << keys.size() << std::endl;
            //Sort the keys
            std::sort(keys.begin(), keys.end());

            m_translate = sdsl::sd_vector<>(keys.begin(), keys.end());
            sdsl::util::init_support(m_rank_support, &m_translate);
            sdsl::util::init_support(m_select_support, &m_translate);

            //Translate values
            for(uint64_t i = 0; i < len; i++){
                int diff_i = repair_ints[i];
                uint value_translated = m_rank_support(encode(diff_i));
                repair_ints[i] = value_translated;
            }

            std::cout << "Repair-ing gn" << std::endl;
            repair_gn::build_grammar_irepair(repair_ints, len, ram_mb, repair_file.c_str());

            std::cout << "Repair reading "<< fileR << std::endl;
            read_rules_repair(fileR.c_str());
            std::cout << "Repair reading "<< fileC << std::endl;
            read_c_repair(fileC.c_str());

            /*
            int last =  keys[keys.size()-1];
            //std::cout << "Alpha: " << last+1 << std::endl;
            //Unmapping values
            for(uint64_t i = 0; i < lenC; i++){
                c[i] = translate(c[i], last, keys);
            }
             */
            /*for(size_type i = 0; i < m_len_c; i++){
                std::cout << "c[" << i << "]=" << m_c[i] << std::endl;
            }*/
            /*
            for(uint64_t i = 0; i < lenR; i++){
                rules[i].left = translate(rules[i].left, last, keys);
                rules[i].right = translate(rules[i].right, last, keys);
            }
            alpha = last+1;//last+1
*/


        }

        void run(){
            std::string fileR = "repair.R";
            std::string fileC = "repair.C";
            std::string fileK = "repair.K";
            std::cout << "Repair reading "<< fileR << std::endl;
            read_rules_repair(fileR.c_str());
            std::cout << "Repair reading "<< fileC << std::endl;
            read_c_repair(fileC.c_str());
            std::cout << "Repair reading " << fileK << std::endl;
            uint lenK = 0;
            int* keys = read_keys_repair(fileK.c_str(), lenK);
            int last =  keys[lenK-1];
            //Unmapping values
            for(uint64_t i = 0; i < lenC; i++){
                c[i] = translate(c[i], last, keys);
            }
            for(uint64_t i = 0; i < lenR; i++){
                rules[i].left = translate(rules[i].left, last, keys);
                rules[i].right = translate(rules[i].right, last, keys);
            }
            alpha = last+1;//last+1

        }


        sdsl::int_vector<> expand(){
            std::cout << "Alpha: " << alpha << std::endl;
            std::cout << "lenC: " << lenC << std::endl;
            std::cout << "lenR: " << lenR << std::endl;
            sdsl::int_vector<> results(lenC);
            uint a = 0;
            for(uint64_t i = 0; i < lenC; i++){
                // std::cout << "c[" << i << "]=" << c[i] << std::endl;
                a += expand(c[i]);
                //std::cout << "expandido: " << a << std::endl;
                results[i] = a;
                //std::cout << "engadido a results" << results.size() << std::endl;
            }
            return results;
        }

        int expand(int i){
            int ret = 1;
            while (i >= alpha) {
                // std::cout << "i-alpha: " << i-alpha << std::endl;
                ret+= expand(rules[i-alpha].left);
                i = rules[i-alpha].right;
            }
            return ret;
        }


    };


}


#endif //LCSA_REPAIR_HPP
