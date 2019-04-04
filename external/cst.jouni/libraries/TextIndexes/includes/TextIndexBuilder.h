/**
*    TextIndex.h
*    Copyright (C) 2014 Alberto Ordóñez
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef TEXT_INDEX_BUILDER_H_
#define TEXT_INDEX_BUILDER_H_

#include <utils/libcdsBasics.h>
#include <TextIndex.h>

using namespace cds_utils;
namespace TextIndexes
{
    class TextIndexBuilder
    {
    public:
        virtual ~TextIndexBuilder(){};
        virtual TextIndex *build(uint *input, size_t len)=0;
        virtual TextIndex *build(uchar *input, size_t len)=0;
    protected:
        uchar *copyInput(uint *input, size_t len, bool delete_input){
            for (size_t i=0;i<len;i++){
                if (input[i]>255) return nullptr;
            }
            uchar *in = new uchar[len];
            for (size_t i=0;i<len;i++){
                in[i] = static_cast<uchar>(input[i]);
            }
            if (delete_input)delete [] input;
            return in;
        }

    };

    class TextIndexBuilderRLCSA : public TextIndexBuilder
    {
    public:
        virtual ~TextIndexBuilderRLCSA(){

        };

        TextIndexBuilderRLCSA(ulong _block_size, ulong _sa_sample_rate, bool _multiple_sequences, bool _delete_data):
                block_size(_block_size),sa_sample_rate(_sa_sample_rate),
                multiple_sequences(_multiple_sequences),delete_data(_delete_data){}

        virtual TextIndex *build(uint *input, size_t len){
            uchar *in = copyInput(input, len, delete_data);
            if (in==nullptr) return nullptr;
            return new TextIndexRLCSA(in,len,block_size,sa_sample_rate,multiple_sequences, true);
        }

        virtual TextIndex *build(uchar *input, size_t len){
            return new TextIndexRLCSA(input,len,block_size,sa_sample_rate,multiple_sequences, delete_data);
        }

    protected:
        ulong block_size;
        ulong sa_sample_rate;
        bool multiple_sequences;
        bool delete_data;
    };

    class TextIndexBuilderFMI : public TextIndexBuilder
    {
    public:
        virtual ~TextIndexBuilderFMI(){
            seq_builder->unuse();
        };

        TextIndexBuilderFMI(cds_static::SequenceBuilder *_seq_builder, uint _samplePos, uint _sampleSuff,bool _free_text):
                seq_builder(_seq_builder),samplePos(_samplePos),sampleSuff(_sampleSuff),free_text(_free_text){
            seq_builder->use();
        }

        virtual TextIndex *build(uint *in, size_t len){
            uchar *input = copyInput(in, len, free_text);
            if (input==nullptr) return nullptr;
            return new TextIndexFMI(input,len,seq_builder,samplePos,sampleSuff,free_text);
        }

        virtual TextIndex *build(uchar *input, size_t len){
            return new TextIndexFMI(input,len,seq_builder,samplePos,sampleSuff,free_text);
        }

    protected:
        cds_static::SequenceBuilder *seq_builder;
        uint samplePos;
        uint sampleSuff;
        bool free_text;
    };


    class TextIndexBuilderRLFMI : public TextIndexBuilder
    {
    public:
        virtual ~TextIndexBuilderRLFMI(){
            seq_builder->unuse();
            bsb->unuse();
        };

        TextIndexBuilderRLFMI(cds_static::SequenceBuilder *_seq_builder, cds_static::BitSequenceBuilder *_bsb,
                uint _samplePos, uint _sampleSuff,bool _free_text):
                seq_builder(_seq_builder),bsb(_bsb),samplePos(_samplePos),sampleSuff(_sampleSuff),free_text(_free_text){
            seq_builder->use();
            bsb->use();
        }

        virtual TextIndex *build(uint *in, size_t len){
            uchar *input = copyInput(in, len, free_text);
            if (input==nullptr) return nullptr;
            return new TextIndexRLFMI(input,len,seq_builder,bsb,samplePos,sampleSuff,true);
        }

        virtual TextIndex *build(uchar *input, size_t len){
            return new TextIndexRLFMI(input,len,seq_builder,bsb,samplePos,sampleSuff,true);
        }

    protected:
        cds_static::SequenceBuilder *seq_builder;
        cds_static::BitSequenceBuilder *bsb;
        uint samplePos;
        uint sampleSuff;
        bool free_text;
    };
};
#endif
