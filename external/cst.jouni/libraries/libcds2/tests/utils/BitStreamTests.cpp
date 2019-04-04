//
// Created by alberto on 3/17/15.
//
#include <gtest/gtest.h>
#include <utils/BitStreamIterator.h>
using namespace cds_utils;

void checkBitStreams(BitStreamIterator &bsi, BitStreamIteratorDisk &bsi_disk,ulong len, uint read){
    uint out_disk,out;
    out=out_disk=0;
    bsi_disk.readAndShift(32,out_disk);
    bsi.readAndShift(32,out);
    ASSERT_EQ(out,out_disk);
    ulong current = 32;
    for (uint i=0;current+read<len;i++,current+=read){
        bsi_disk.readAndShift(read,out_disk);
        bsi.readAndShift(read,out);
        ASSERT_EQ(out,out_disk);
    }
    bsi.reset();
    bsi_disk.reset();
}

TEST(BITSTREAM, Test1){

    uint *bmp;
    ulong len = 2048;
    createEmptyBitmap(&bmp,len);
    ulong n_ints = len / W + ((len%W!=0)?1:0);
    for (ulong i=0;i<len;i++){
        if (i%5==0)bitset(bmp,i);
    }
    ofstream fp_out("tmp.bin");
//    fp_out.write((char*)&len,sizeof(ulong));
//    fp_out.write((char*)&n_ints,sizeof(ulong));
    fp_out.write((char*)bmp,n_ints*sizeof(uint));
    fp_out.close();

    BitStreamIteratorDisk bsi_disk(string("tmp.bin"));
    BitStreamIterator bsi(bmp,len);
    for (uint read=1;read<31;read++){
        uint out_disk,out;
        out=out_disk=0;
        bsi_disk.readAndShift(32,out_disk);
        bsi.readAndShift(32,out);
        ASSERT_EQ(out,out_disk);
        ulong current = 32;
        for (uint i=0;current+read<len;i++,current+=read){
            bsi_disk.readAndShift(read,out_disk);
            bsi.readAndShift(read,out);
            ASSERT_EQ(out,out_disk);
        }
        bsi.reset();
        bsi_disk.reset();
    }

    delete [] bmp;
}

TEST(BITSTREAM, TestOutStream){

    uint *bmp;
    ulong len = 2048;
    createEmptyBitmap(&bmp,len);
    ulong n_ints = len / W + ((len%W!=0)?1:0);
    for (ulong i=0;i<len;i++){
        if (i%5==0)bitset(bmp,i);
    }
    ofstream fp_out("tmp.bin");
    fp_out.write((char*)bmp,n_ints*sizeof(uint));
    fp_out.close();


    BitStreamIteratorDisk bsi_disk(string("tmp.bin"));
    BitStreamOutputWriter *bsow = new BitStreamOutputWriter (string("tmp2.bin"));

    uint out_disk,out;
    out=out_disk=0;
    bsi_disk.readAndShift(32,out_disk);
    bsow->write(out_disk,32);
    uint read = 7;
    ulong current = 32;
    for (uint i=0;current+read<len;i++,current+=read){
        bsi_disk.readAndShift(read,out_disk);
        bsow->write(out_disk,read);
    }
    bsi_disk.reset();
    delete bsow;

    BitStreamIteratorDisk bsi_disk2(string("tmp2.bin"));
    for (uint i=1;i<31;i++){
        for (uint read=1;read<31;read++){
            uint out_disk,out;
            out=out_disk=0;
            bsi_disk.readAndShift(32,out_disk);
            bsi_disk2.readAndShift(32,out);
            ASSERT_EQ(out,out_disk);
            ulong current = 32;
            for (uint i=0;current+read<len;i++,current+=read){
                bsi_disk.readAndShift(read,out_disk);
                bsi_disk2.readAndShift(read,out);
                ASSERT_EQ(out,out_disk);
            }
            bsi_disk2.reset();
            bsi_disk.reset();
        }
    }

    bsi_disk.reset();
    bsi_disk2.reset();
    delete [] bmp;
}