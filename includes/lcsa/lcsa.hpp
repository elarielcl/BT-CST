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

#ifndef LCSA_LCSA_HPP
#define LCSA_LCSA_HPP



#include "repair.hpp"

using namespace util_repair;

namespace lcsa {


    class lcsa {

    public:
        typedef uint64_t size_type;
        typedef uint32_t value_type;
        typedef util_repair::Tpair rule_type;

    private:
        size_type m_sample;
        sdsl::int_vector<> m_sampling_vector;
        sdsl::sd_vector<> m_L;
        sdsl::rank_support_sd<1> m_rank_support;
        sdsl::select_support_sd<1> m_select_support;
        int *m_c;
        size_type m_len_c;
        rule_type *m_r;
        size_type m_len_r;
        uint m_alpha;
        size_type m_size;
        sdsl::byte_alphabet::char2comp_type m_char2comp;
        sdsl::byte_alphabet::sigma_type m_sigma;

        void copy(const lcsa& csa)
        {
            m_sample= csa.m_sample;
            m_sampling_vector= csa.m_sampling_vector;
            m_L = csa.m_L;
            m_rank_support = csa.m_rank_support;
            m_rank_support.set_vector(&m_L);
            m_select_support = csa.m_select_support;
            m_select_support.set_vector(&m_L);
            m_c = csa.m_c;
            m_len_c = csa.m_len_c;
            m_r = csa.m_r;
            m_alpha = csa.m_alpha;
            m_size = csa.m_size;
            m_char2comp = csa.m_char2comp;
            m_sigma = csa.m_sigma;
        }

        int translate(const int value_to_translate, const int &last, const std::vector<int> &keys,
                      const repair &repair){
            if(value_to_translate >= repair.alpha){
                return last + (value_to_translate-repair.alpha+1);
            }else{
                return keys[value_to_translate];
            }
        }

    public:

        sdsl::byte_alphabet::char2comp_type &char2comp = m_char2comp;
        sdsl::byte_alphabet::sigma_type &sigma = m_sigma;

        lcsa(){};

        lcsa(const lcsa& csa)
        {
            copy(csa);
        }
        //! Move constructor
        lcsa(lcsa&& csa)
        {
            *this = std::move(csa);
        }

        template<class t_csa>
        lcsa(t_csa &csa, const size_type sample = 64, bool delete_csa = false, bool repair_precomputed = false,
                size_type max_mb = 30720) {

            m_size = csa.size();
            m_sigma = csa.sigma;
            m_char2comp = csa.char2comp;
            m_sample = sample;
            repair m_repair;
            sdsl::int_vector<> sampling_vector(m_size / m_sample + 1, 0);
            sampling_vector[0] = csa[0];
            std::cout << "Repair precomputed: " << repair_precomputed << std::endl;
            if(repair_precomputed){
                for (size_type i = m_sample; i < m_size; i= i+m_sample) {
                    if (i % m_sample == 0) {
                        sampling_vector[i / m_sample] = csa[i];
                    }
                }
                m_repair.run();
            }else{
                std::map<int, int> map_terminals;
                map_terminals.insert(std::pair<int, int>(0, 1));
                int* m_diffs = new int[m_size]();
                m_diffs[0] = 0;
                for (size_type i = 1; i < m_size; i++) {
                    int diff_i = csa[i] - csa[i-1];
                    m_diffs[i] = diff_i;
                    if(map_terminals.find(diff_i) == map_terminals.end()){
                        map_terminals.insert(std::pair<int, int>(diff_i, 1));
                    }
                    if (i % sample == 0) {
                        sampling_vector[i / m_sample] = csa[i];
                    }
                }
                m_repair.run(m_diffs,  m_size, map_terminals, max_mb);
            }

            if(delete_csa) sdsl::util::clear(csa);

            sdsl::util::assign(m_sampling_vector, sampling_vector);
            sdsl::int_vector<> ones = m_repair.expand();
            m_L = sdsl::sd_vector<>(ones.begin(), ones.end());
            m_c = m_repair.c;
            m_len_c = m_repair.lenC;
            m_r = m_repair.rules;
            m_len_r = m_repair.lenR;
            m_alpha = m_repair.alpha;
            sdsl::util::init_support(m_rank_support, &m_L);
            sdsl::util::init_support(m_select_support, &m_L);


        }

        lcsa& operator=(const lcsa& csa)
        {
            if (this != &csa) {
                copy(csa);
            }
            return *this;
        }

        //! Swap method
        void swap(lcsa& v)
        {
            if (this != &v) {
                std::swap(m_sample, v.m_sample);
                std::swap(m_sampling_vector, v.m_sampling_vector);
                std::swap(m_L, v.m_L);
                m_rank_support.swap(v.m_rank_support);
                m_rank_support.set_vector(&m_L);
                m_select_support.swap(v.m_select_support);
                m_select_support.set_vector(&m_L);
                std::swap(m_c, v.m_c);
                std::swap(m_len_c, v.m_len_c);
                std::swap(m_r, v.m_r);
                std::swap(m_alpha, v.m_alpha);
                std::swap(m_size, v.m_size);
                std::swap(m_char2comp, v.m_char2comp);
                std::swap(m_sigma, v.m_sigma);
            }
        }

        lcsa& operator=(lcsa&& v)
        {
            if (this != &v) {
                m_sample = std::move(v.m_sample);
                m_sampling_vector = std::move(v.m_sampling_vector);
                m_L = std::move(v.m_L);
                m_rank_support = std::move(v.m_rank_support);
                m_rank_support.set_vector(&m_L);
                m_select_support = std::move(v.m_select_support);
                m_select_support.set_vector(&m_L);
                m_c = v.m_c;
                m_len_c = v.m_len_c;
                m_r = v.m_r;
                m_alpha = std::move(v.m_alpha);
                m_size = std::move(v.m_size);
                m_char2comp = std::move(v.m_char2comp);
                m_sigma = std::move( v.m_sigma);
            }
            return *this;
        }


        void expand_left(long value, std::vector<long> &vector, long &actual, size_type start, size_type end) const {
            while (value >= m_alpha) {
                expand_left(m_r[value - m_alpha].left, vector, actual, start, end);
                if(actual > end) return;
                value = m_r[value - m_alpha].right;
            }
            if (start <= actual && actual <= end) {
                vector.push_back(value);
            }
            actual++;
        }

        void expand_right(long value, std::vector<long> &vector, long &actual, size_type start, size_type end) const{
            while (value >= m_alpha) {
                expand_right(m_r[value - m_alpha].right, vector, actual, start, end);
                if(actual < start) return;
                value = m_r[value - m_alpha].left;
            }
            if (start <= actual && actual <= end) {
                vector.push_back(value);
            }
            actual--;
        }

        void expand_interval_left(size_type i, size_type j, long &actual, size_type start, size_type end,
                std::vector<long> &result) const {
            for (size_type p = i; p <= j; p++) {
                expand_left(m_c[p], result, actual, start, end);
            }
        }

        void expand_interval_right(size_type i, size_type j, long &actual, size_type start, size_type end,
                std::vector<long> &result) const{
            for (size_type p = j; p >= i; p--) {
                expand_right(m_c[p], result, actual, start, end);
            }
        }



        size_type decompress_value(size_type i) const{
            //calculate i' and j'
            size_type i1 = (i / m_sample) * m_sample;
            size_type j1 = (i / m_sample+1) * m_sample;
            //interval i'-j
            if(j1 >= m_size || i-i1 < j1-i){
                size_type c_start = m_rank_support(i1 + 1);//non-inclusive rank
                //std::cout << "c-start: " << c_start << std::endl;
                size_type c_end = m_rank_support(i + 1);
                //std::cout << "c-end: " << c_end << std::endl;
                long p_start = 0;
                if (c_start > 0) p_start = m_select_support(c_start);
                //std::cout << "p-start " << p_start << std::endl;
                std::vector<long > diffs;
                expand_interval_left(c_start, c_end, p_start, i1, i, diffs);
                size_type value = m_sampling_vector[i / m_sample];
                for (uint32_t p_diff = 1; p_diff < diffs.size(); p_diff++) {
                    value += diffs[p_diff];
                }
                return value;
            }else{
                size_type c_start = m_rank_support(i + 1);//non-inclusive rank
                //std::cout << "c-start: " << c_start << std::endl;
                size_type c_end = m_rank_support(j1 + 1);
                //std::cout << "c-end: " << c_end << std::endl;
                long p_end = m_select_support(c_end+1)-1;
                //std::cout << "p-start " << p_start << std::endl;
                std::vector<long > diffs;
                expand_interval_right(c_start, c_end, p_end, i, j1, diffs);
                size_type value = m_sampling_vector[i / m_sample+1];
                for (uint32_t p_diff = 0; p_diff < diffs.size()-1; p_diff++) {
                    value -=  diffs[p_diff];
                }
                return value;
            }

        }

        inline size_type operator[](const size_type &i) const {
            return decompress_value(i);
        }

        size_type size() const{
            return m_size;
        }

        void print() {
            std::cout << "Sampling" << std::endl;
            for (uint i = 0; i < m_sampling_vector.size(); i++) {
                std::cout << m_sampling_vector[i] << " ";
            }
            std::cout << std::endl;
            std::cout << "L" << std::endl;
            for(uint i = 0; i < m_L.size(); i++){
                std::cout << m_L[i] << " ";
            }
            std::cout << std::endl;
        }

        size_type size_in_bytes() {
            size_type size = 0;
            size += sdsl::size_in_bytes(m_L);
            size += sdsl::size_in_bytes(m_sampling_vector);
            size += sdsl::size_in_bytes(m_rank_support);
            size += sdsl::size_in_bytes(m_select_support);
            size += sizeof(Tpair) * m_len_r;
            size += sizeof(int) * m_len_c;
            size += sizeof(int) * 2;
            return size;
        }

        //! Serialize to a stream.
        /*! \param out Output stream to write the data structure.
         *  \return The number of written bytes.
         */
        /*size_type serialize(std::ostream& out, sdsl::structure_tree_node* v=nullptr, std::string name="")const
        {
            sdsl::structure_tree_node* child = sdsl::structure_tree::add_child(v, name, sdsl::util::class_name(*this));
            size_type written_bytes = 0;
            written_bytes += m_sa.serialize(out, child, "m_sa");
            written_bytes += m_isa.serialize(out, child, "m_isa");
            written_bytes += m_alphabet.serialize(out, child, "m_alphabet");
            structure_tree::add_size(child, written_bytes);
            return written_bytes;
        }*/

    };
}


#endif //RCT_LCSA_HPP
