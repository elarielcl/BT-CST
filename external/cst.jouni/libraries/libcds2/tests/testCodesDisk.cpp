
#include <cstdlib>
#include <wt_coder_huff.h>
#include <wt_coder_SubOptimalMult.h>
#include <wt_coder_hutucker.h>
#include <Tuple.h>
#include "timeMeasure.h"
#include <Tuple.h>

#ifndef OUTPUT_FILE
#define OUTPUT_FILE "tmp.bin"
#endif

using namespace std;
using namespace cds_static;
using namespace cds_utils;

const uint timeFactor = 1000000000;

void usage(char *name){
  cerr << "Usage: " << name<< " collection (t|p|a|m|h|i|o) [options (s| l (t|p)| l (t|p) c )]" << endl;
  cerr << "\tt: table" << endl;
  cerr << "\tp: perm" << endl;  
  cerr << "\ta: add" << endl;
  cerr << "\tm: mult" << endl;
  cerr << "\th: hu-tucker" << endl;
  cerr << "\ti: pMin" << endl;
  cerr << "\tb: balanced" << endl;
  cerr << "\to: optimal l-restricted" << endl;
  cerr << "OPTIONS: " << endl;
  cerr << "\ts: sampling for select improvement on permutation"<<endl;
  cerr << "\tl: lmax (for add and mult)" << endl;
  cerr << "\tc: for mult or minFreq for pMin" << endl; 
}

int main(int argc, char ** argv) {

  if (argc<3){
    usage(argv[0]);
    exit(0);
  }

  int samplingSelect = 0;
  char *bufferFreqs  = new char[strlen(argv[1])+128];
      strcpy(bufferFreqs,argv[1]);
      strcat(bufferFreqs,".freqs");
      ifstream in(bufferFreqs);  

      uint *freqs;
      uint len;
      uint max_v;
      in.read((char*)&len,sizeof(uint));
      in.read((char*)&max_v,sizeof(uint));
      freqs = new uint[max_v+1];
      in.read((char*)freqs,sizeof(uint)*(max_v+1));
      in.close();

  // double H0 = 0.0;
  // for (uint i=0;i<=max_v;i++){
  //   assert(freqs[i]>0);
  //   double pi = freqs[i]*1.0/len;
  //   H0 += pi * log(pi)/log(2);
  // }
  // cerr << "H0: "<< H0 << endl;
  
  uint *input = new uint[len];
  uint *inputCodes = new uint[len];
  ifstream inputf(argv[1]);
  inputf.read((char*)input,sizeof(uint)*len);
  inputf.close();

  uint newmaxv = 0;
  for (uint i=0;i<len;i++)
    newmaxv = max(newmaxv,input[i]);
  uint *newCounter = new uint[newmaxv+1];
  for (uint i=0;i<=newmaxv;i++)
    newCounter[i]=0;
  for (uint i=0;i<len;i++)
    newCounter[input[i]]++;
  // H0 = 0.0;
  // for (uint i=0;i<=newmaxv;i++){
  //   double pi = newCounter[i]*1.0/len;
  //   H0 += pi * log(pi)/log(2);
  // }
  // cerr << "H0: " << -H0 << endl;
  struct stat sdata;
  stat(argv[1],&sdata);
  uint seq_len = sdata.st_size /sizeof (uint);
  assert((sdata.st_size&sizeof(uint))==0);

  pair<uint,uint> *codeLen = new pair<uint,uint>[max_v+1];

  uint REP = 1;
  char option = argv[2][0];
  float optimallity=0.0;
  if (option=='t' || option=='p'){
    if (option=='p' && argc<4){
      usage(argv[0]);
      exit(0);
    }
    //MODEL STORED IN A TALBLE (t) or PERMUTATION (p)
    char *buffer  = new char[strlen(argv[1])+128];
    strcpy(buffer,argv[1]);
    strcat(buffer,".huffman.model");


    ifstream in(buffer);
    //wt_coder_huff * wc = wt_coder_huff::load(in);
    wt_coder_huff * wc = new wt_coder_huff(input, len, new MapperNone());
    in.close();
    uint max_v = wc->getMaxv();
   // cerr << "max_v: " << max_v << ", newmaxv: "<< newmaxv << endl;
    wt_coder_huff_can *can = new wt_coder_huff_can(wc,max_v);
    

    cds_static::tuple *table_symbols_codes = can->getCopyTableSymbols();
    
    uint maxCode = 0;
    uint minCode = ~0;
    float avgCode=0.0;

    for (uint i=0;i<=max_v; i++){
      avgCode+=table_symbols_codes[i].len;
      if (table_symbols_codes[i].len > maxCode) maxCode = table_symbols_codes[i].len;
      if (table_symbols_codes[i].len < minCode) minCode = table_symbols_codes[i].len;
    }

  // uint *H0_L = new uint[maxCode+1];
  // for (uint i=0;i<=maxCode;i++)
  //   H0_L[i]=0;
  // double acum_h0l =0.0;
  // for (uint i=0;i<=max_v; i++){
  //   H0_L[table_symbols_codes[i].len]++;
  // }
  
  // for (uint i=0;i<=maxCode;i++){
  //   if (H0_L[i]){
  //     double pi = (double)H0_L[i]/(max_v+1);
  //     acum_h0l+= pi*log(pi)/log(2);
  //   }
  // }
  // cerr << "acum_h0l: " << -acum_h0l << endl;


  double pi_li_optimal = 0.0;
  for (uint i=0;i<=max_v;i++){
    double pi = freqs[i]*1.0/len;
    pi_li_optimal += pi * log(pi)/log(2);
  }
  cerr << "pi_li_optimal: "<< pi_li_optimal << endl;

    avgCode=avgCode/(max_v+1);
    MappingSymbols *ms;
    if (option=='t'){
      ms = new MappingTable(table_symbols_codes,(size_t)max_v+1);
    }else{
      BitSequenceBuilder *bsb;
      samplingSelect = atoi(argv[3]);
      
      if (samplingSelect<0){
        BitSequencePlain::BuilderSequencePlain builder = BitSequencePlain::BuilderSequencePlain::createBuilderSequencePlain(nullptr,0,33)[0];//->s1factor(samplingSelect).s0factor(samplingSelect);
        bsb  = new BitSequenceBuilderPlain(&builder);
      }else{
        BitSequencePlain::BuilderSequencePlain builder = BitSequencePlain::BuilderSequencePlain::createBuilderSequencePlain(nullptr,0,33)->s1factor(samplingSelect).s0factor(samplingSelect);
        bsb  = new BitSequenceBuilderPlain(&builder);
      }
      
      // MappingBuilder *mb = new MappingPermutationsBuilder(table_symbols_codes,max_v+1,bsb);
      ms = new MappingPermutations(table_symbols_codes, (size_t)max_v+1,bsb);
    }
    assert(seq_len==len);
    
    double optimallity=0;
    for (uint i=0;i<=max_v;i++){
      uint leaf;
      ms->getCode(i,codeLen[i].second,codeLen[i].first,leaf);
      double pi = (double)freqs[i]/seq_len;
      assert(pi>0);
      optimallity += freqs[i]*1.0/seq_len*codeLen[i].first; 
    }
    optimallity = -optimallity;
    for (uint i=0;i<len;i++)
      inputCodes[i]=codeLen[input[i]].second;

    //cerr << "optimallity: " << optimallity << endl;
    double t0,t1;
    
    size_t total=0;
    size_t next_write=32;
    
     ofstream out("tmp.out.bin");
    inputf.open(argv[1]);
    t0=getTime();
      for (uint j=0;j<REP;j++){
        for (uint i=0;i<len;i++){
          uint len, code, leaf;
          uint symb;
          inputf.read((char*)&symb, sizeof(uint));
          ms->getCode(symb,code,len,leaf);
          total+=len;
          if (total>next_write){
            next_write+=32;
            out.write((char*)input,sizeof(uint));
            #if FFLUSH
            out.flush();
            #endif
          }

        }
      }
    t1 = getTime();
    inputf.close();
    out.close();
   cout << "Collection: " << argv[1] << ";samplingSelect: "<< samplingSelect;
    cout << ";Option " << argv[2];
        cout << ";sigma: " << max_v;
        cout << ";minle: " << minCode;
        cout << ";maxle: " << maxCode;
        cout << ";avgCo: " << avgCode;
        cout << ";timeCode: "<< (t1-t0)*timeFactor/((len*REP));
        cout << ";size: "<<ms->getSize()*8.0/(max_v+1);
        cout << ";optimallity: "<< optimallity;
    ofstream salida(OUTPUT_FILE);
    // out.open(OUTPUT_FILE);
    salida.write((char*)inputCodes,sizeof(uint)*len);
    salida.close();
    //out.open(OUTPUT_FILE);
    
    //inputf.open(argv[1]);

    ofstream out2("tmp2.bin");

    t0=getTime();
      for (uint j=0;j<REP;j++){
        ifstream inCodeLen(OUTPUT_FILE);
        for (uint i=0;i<len;i++){
          uint p;
          inCodeLen.read((char*)&p, sizeof(uint));
          // if (inputCodes[i]!=p) 
          //   cerr << endl;

          uint s = ms->getSymbol(p);//, codeLen[input[i]].first);
          out2.write((char*)&s,sizeof(uint));
          #if FFLUSH
            out2.flush();
          #endif
        }
        inCodeLen.close();
      }
    t1 = getTime();
    //inputf.close();
    out.close();
    out2.close();
   // cout << "Collection: " << argv[1] << ";samplingSelect: "<< samplingSelect;
   //  cout << ";Option " << argv[2];
   //      cout << ";sigma: " << max_v;
   //      cout << ";minle: " << minCode;
   //      cout << ";maxle: " << maxCode;
   //      cout << ";avgCo: " << avgCode;
        cout << ";timeDeco: "<< (t1-t0)*timeFactor/((len*REP))<<endl;
        // cout << ";size: "<<ms->getSize()*8.0/(max_v+1);
        // cout << ";optimallity: "<< optimallity << endl;

    delete [] buffer;
  }else{
    uint lmax;
    uint c;
    //CODER
    wt_coder *coder;

     if (option=='a' || option=='m' || option=='i' || option=='b' || option=='o'){
    //ADDITIVE HUFFMAN MODEL (a) or MULTIPLICATIVE (m)
      char *buffer  = new char[strlen(argv[1])+128];
      strcpy(buffer,argv[1]);
      strcat(buffer,".huffman.model");

      if (option=='a' && argc<5){
        usage(argv[0]);
        exit(0);
      }
      if ((option=='m' || option=='i') && argc<7){
        usage(argv[0]);
        exit(0);
      }    
      samplingSelect = atoi(argv[3]);
      BitSequenceBuilder *bsb;
      if (samplingSelect<0){
        BitSequencePlain::BuilderSequencePlain builder = BitSequencePlain::BuilderSequencePlain::createBuilderSequencePlain(nullptr,0,33)[0];//->s1factor(samplingSelect).s0factor(samplingSelect);
        bsb  = new BitSequenceBuilderPlain(&builder);
      }else{
        BitSequencePlain::BuilderSequencePlain builder = BitSequencePlain::BuilderSequencePlain::createBuilderSequencePlain(nullptr,0,33)->s1factor(samplingSelect).s0factor(samplingSelect);
        bsb  = new BitSequenceBuilderPlain(&builder);
      }

      // MappingBuilder *mb = new MappingTableBuilder();//new MappingPermutationsBuilder(bsb);
      MappingBuilder *mb;// = new MappingPermutationsBuilder(bsb);

      if (option=='a' || option=='b' || option=='o'){
        if (argv[5][0]=='p'){
          mb = new MappingPermutationsBuilder(bsb);
        }else{
          mb = new MappingTableBuilder();
        }
      }else if (option=='m' || option=='i'){
        cerr << argv[6] << endl;
        if (argv[6][0]=='p'){
          mb = new MappingPermutationsBuilder(bsb);
        }else{
          mb = new MappingTableBuilder();
        }
      }
      
      ifstream in(buffer);
      wt_coder_huff * wc = wt_coder_huff::load(in);
      in.close();
      uint max_v = wc->getMaxv()-1;
      //wt_coder_huff_can *wc_can = new wt_coder_huff_can(wc,wc->getMaxv());
      lmax = atoi(argv[4]);

      
      if (option=='a'){
        coder = new wt_coder_SubOptimalAdd(wc,wc->getMaxv(),lmax,mb);
      }else if (option=='m'){
        c = atoi(argv[5]);
        //a multiplicative coder is built on an additive one
        wt_coder_SubOptimalAdd *wc_add = new wt_coder_SubOptimalAdd(wc,wc->getMaxv(),lmax,mb);
        coder = new wt_coder_SubOptimalMult(wc_add,wc->getMaxv(),c,lmax);  
      }else if (option=='i'){
        c = atoi(argv[5]);
        coder = new wt_coder_Pmin(input, len, lmax, c, mb);
      }else if (option=='b'){
        coder = new wt_coder_Balanced(wc,wc->getMaxv(),lmax,mb);
      }else if (option=='o'){
        //cerr << "suboptimal" << endl;
        coder = new wt_coder_Suboptimal(input,len,lmax,mb);
      }else{
        cerr << "option " << option << " not known" << endl;
        return 0;
      }


      //cout << "size: "<<coder->getSize()*8.0/(max_v+1)<<endl;

      delete [] buffer;
        
    }else if (option=='h'){
      //hutucker
      char *inout  = new char[strlen(argv[1])+128];
      strcpy(inout,argv[1]);
      strcat(inout,".hutucker");
      cerr << "inout: "<< inout << endl;
      ifstream in(inout);
      //coder = wt_coder_hutucker::load(in);
    uint sigma_ht = loadValue<uint>(in);
    uint len_ht = loadValue<uint>(in);
    uint *par;
    par = loadValue<uint>(in,uint_len(len_ht,1));

    
    cds_static::tuple *table_symbols_codes = new cds_static::tuple[sigma_ht];
    table_symbols_codes = loadValue<cds_static::tuple>(in,sigma_ht);

      uint costRank = 50;
      uint costFindClose = 250;
      size_t totalCostCompr = 0;
      size_t totalCostDecompr = 0;
      uint *unos = new uint[sigma_ht];
      for (uint i=0;i<sigma_ht;i++){
          unos[i]=0;
          for (uint j=0;j<table_symbols_codes[i].len;j++){
            if (bitget(&(table_symbols_codes[i].code),j)){
              unos[i]++;
            }
          }
      }

      for (uint i=0;i<len;i++){
        totalCostDecompr = totalCostDecompr + (2*costRank + costFindClose)* unos[input[i]];
        totalCostCompr = totalCostCompr + (2*costRank * table_symbols_codes[input[i]].len ) + costFindClose*unos[input[i]];
      }
      cerr << "Time(decomp): " << totalCostDecompr * 1.0 / len << endl;
      cerr << "Time(comp): " << totalCostCompr * 1.0 / len << endl;
      // if (access(inout,F_OK)){
      //   cerr << "loading hu-tucker" << endl;
      //   ifstream in(inout);
      //   coder = wt_coder_hutucker::load(in);
      //   in.close();
      // }else{
        
          //coder = new wt_coder_hutucker(freqs,max_v+1);   
        // ofstream of(inout);
        // coder->save(of);
        // of.close();
        // cerr << "hu-tucker saved" << endl;
        //delete [] freqs;
        
      // }
      delete [] inout;



    }else{
      usage(argv[0]);
      exit(0);
    }
    // float optimallity = 0.0;
    //  for (uint i=0;i<max_v;i++){
    //     codeLen[i].first = coder->getCode(i,codeLen[i].second);
    //     optimallity += freqs[i]*1.0/seq_len*codeLen[i].first; 
    //   }
    uint mclen = 0;
    double optimallity=0;
    for (uint i=0;i<=max_v;i++){
      uint leaf;
      codeLen[i].first = coder->getCode(i,codeLen[i].second);
      mclen = max(mclen,codeLen[i].first);
      double pi = (double)freqs[i]/seq_len;
      assert(pi>0);
      optimallity += freqs[i]*1.0/seq_len*codeLen[i].first; 
    }
   //cerr << "mclen: " << mclen << endl;
    //cerr << "optimallity: " << optimallity << endl;
    for (uint i=0;i<len;i++)
      inputCodes[i]=codeLen[input[i]].second;

    double t0,t1;

    size_t total=0;
    size_t next_write=32;
    
    ofstream out("tmp2.out.bin");
    inputf.open(argv[1]);
    t0=getTime();
      for (uint j=0;j<REP;j++){
        for (uint i=0;i<len;i++){
          //read symbol from disk
          uint symb;
          inputf.read((char*)&symb, sizeof(uint));
          
          //encode symbol
          uint len, code;
            len = coder->getCode(symb,code);
          total+=len;
          //write it to disk 
          if (total>next_write){
            next_write+=32;
            out.write((char*)input,sizeof(uint));
            #if FFLUSH
              out.flush();
            #endif
          }

        }
      }
    t1 = getTime();
    inputf.close();
    out.close();

     cout << "Collection: " << argv[1] << ";samplingSelect: "<< samplingSelect;
      cout << ";Option " << argv[2];
      if (option=='a' || option=='i' || option=='b' || option=='o'){
        cout << ";lmax:"<<lmax;
      }else if (option=='m'){
        cout << ";lmax:"<<lmax<<";c:"<<c;
      }
      coder->avgCodeLength();
      cout << ";timeCode: "<< (t1-t0)*timeFactor/((len*REP));
      cout << ";size: "<<coder->getSize()*8.0/(max_v+1);
      cout << ";optimallity: "<< optimallity;


    ofstream salida(OUTPUT_FILE);
    salida.write((char*)inputCodes,sizeof(uint)*len);
    salida.close();
    //out.open(OUTPUT_FILE);
    ifstream inCodeLen(OUTPUT_FILE);
    inputf.open(argv[1]);
    ofstream out2("tmp2.bin");

     if (option=='h'){
      t0=getTime();
        for (uint j=0;j<REP;j++){
          for (uint i=0;i<len;i++){
            uint p;
            inCodeLen.read((char*)&p,sizeof(uint));
            uint s = coder->getSymbol(p,codeLen[input[i]].first);
            out2.write((char*)&s,sizeof(uint));
            #if FFLUSH
              out2.flush();
            #endif
          }
        }
      t1 = getTime();
    }else{
      t0=getTime();
        for (uint j=0;j<REP;j++){
          for (uint i=0;i<len;i++){
            uint p;
            inCodeLen.read((char*)&p,sizeof(uint));
            uint s = coder->getSymbol(p);
            out2.write((char*)&s,sizeof(uint));
            #if FFLUSH
              out2.flush();
            #endif
          }
        }
      t1 = getTime();
    }
    inCodeLen.close();
    out2.close();
    inputf.close();

     // cout << "Collection: " << argv[1] << ";samplingSelect: "<< samplingSelect;
     //  cout << ";Option " << argv[2];
     //  coder->avgCodeLength();
      cout << ";timeDeco: "<< (t1-t0)*timeFactor/((len*REP))<<endl;

  }
  delete [] input;
  delete [] inputCodes;
  delete [] bufferFreqs;
  delete [] freqs;
  delete [] codeLen;
  return 0;

}

