int do_testLZEndPerformance(int argc, char **argv){
  
  if (argc<3){
    cerr << "Usage: " << argv[0] << " <input-file.txt> <beta> [<lzen parse input-file.txt>]" << endl;
    cerr << "\t<beta> is the parameter to decide which phrase goes to the base sequence." << endl;
    cerr << "\tIt has different meanings depending on the algorithm used." << endl;
    return 0;
  }

  struct stat sdata;
  stat(argv[1],&sdata);
  uint len = sdata.st_size;//it is adding a blank space at the end..i do not want it-->i use it for the $.
  char *input_text = new char[len];
  
  ifstream iff(argv[1]);
  iff.read((char*)input_text,len);
  iff.close();

  uint *input = new uint[len];//
  for (uint i=0;i<len;i++)
    input[i] = (uint)input_text[i];
  //len-1? at the very end position,the editor is adding something. We do not 
  //want that something. 
  Mapper *mp = new MapperCont(input, len-1, BitSequenceBuilderRG(32));
  uint dic[256];
  for (uint i=0;i<256;i++)
    dic[i]=0;
  for (uint i=0;i<len-1;i++){
    uint v = mp->map(input[i]);
    dic[input[i]]=v;
    input[i]= v;
    assert(input[i]>0);
  }
  input[len-1]=0;
  // Repair *rp = new Repair((int*)input, len);
  // cerr << "Repair rules: " << rp->getNRules() << endl;

  uint beta = atoi(argv[2]);
  uint sampleC = 1;
  uint superSampleC = 4;

  // {
  //   //create lzend parse to save
  //   char * buffer = new char[strlen(argv[1])+1024];
  //   strcpy(buffer,argv[1]);
  //   strcat(buffer,".lzend");
  //   Sequence *lz = new SequenceLZEnd(input, len, dic);
  //   ofstream fp (buffer);
  //   lz->save(fp);
  //   fp.close();
  //   exit(0);
  // }
  //LZBaseStrategy *strategy = new LZBaseLimitHops(beta);
  // LZBaseStrategy *strategy = new LZBaseLength(beta);
  // LZBaseStrategy *strategy = new LZBaseSampleOnN(beta, 8);
  LZBaseStrategy *strategy = new LZBaseCopyOptimal(beta);

  Sequence *seq;
  if (argc==4){
    ifstream fp(argv[3]);
    Sequence *initial_parse = Sequence::load(fp);
    fp.close();

    SequenceBuilder *seqFirstClasses = new SequenceBuilderSequenceRepairSC(new BitSequenceBuilderRRR(32),
             new MapperNone(),new PermutationBuilderMRRR(5,new BitSequenceBuilderRG(10)),8,4,8u);
    SequenceBuilder *sb = new SequenceBuilderWaveletTree(new BitSequenceBuilderRG(32),new MapperNone());
    // SequenceBuilder *rpsn = new SequenceBuilderRPSN(new BitSequenceBuilderRRR(32), 4,2,4);
    seq  = new SequenceLZEndBase(input,len, new BitSequenceBuilderDelta(128),sb, strategy, sampleC, superSampleC, (SequenceLZEnd*)initial_parse,false);
  }else{
    seq  = new SequenceLZEndBase(input,len, dic, new BitSequenceBuilderDelta(128),new SequenceBuilderWaveletTree(new BitSequenceBuilderRG(32),new MapperNone()), strategy, sampleC, superSampleC);
  }

  cerr << "Seq size (bps): " << seq->getSize()*8.0 / len << endl;
  uint nqueries = min((uint)len-1,1000u);
  double t0,t1;
  t0 = getTime();
  for (uint i=0;i<nqueries;i++){
    // cerr << "i: " << i << endl;
    uint acc = seq->access(i);
    assert(acc == (input[i]-1));
  }
  t1 = getTime();
  cerr << "Test access OK!" << endl;
  cerr << "Time per query: " << (t1-t0)*1000000.0/(nqueries) << endl;
  uint *counter = new uint[256];
  for (uint i=0;i<256;i++){
    counter[i]=0;
  }
  
  uint *queries = new uint[nqueries];
  for (uint i=0;i<nqueries;i++){
    queries[i] = rand()%(len-1);
  }

  t0 = getTime();
  for (uint i=0;i<nqueries;i++){
    uint acc = seq->access(queries[i]);
  }
  t1 = getTime();
  cerr << "Time per query: " << (t1-t0)*1000000.0/(nqueries) << endl;
  t0 = getTime();
  for (uint i=0;i<nqueries;i++){
    seq->rank(input[queries[i]]-1, queries[i]);
    // seq->rank(input[i]-1, i);
  }

    // for (uint i=0;i<nqueries;i++){
      // counter[input[i]]++;
       // cerr << i << endl;
     
      // uint r = seq->rank(input[i]-1, i);
      // if (!(r==counter[input[i]]))
      //   cerr << "(i:" << i << "), obtained: " << r << ", expected: " << counter[input[i]] << endl;
      // assert(r==counter[input[i]]);
    // }
  t1 = getTime();
  delete [] queries;
  cerr << "Time per query: " << (t1-t0)*1000000.0/nqueries << endl;
  cerr << "Test rank OK!"<< endl;
  delete [] input_text;
  delete [] input;
  return 0;  
}
int do_testLZEndCorrectness(int argc, char **argv){
  
  if (argc<3){
    cerr << "Usage: " << argv[0] << " <input-file.txt> <beta> [<lzen parse input-file.txt>]" << endl;
    cerr << "\t<beta> is the parameter to decide which phrase goes to the base sequence." << endl;
    cerr << "\tIt has different meanings depending on the algorithm used." << endl;
    return 0;
  }

  struct stat sdata;
  stat(argv[1],&sdata);
  uint len = sdata.st_size;//it is adding a blank space at the end..i do not want it-->i use it for the $.
  char *input_text = new char[len];
  
  ifstream iff(argv[1]);
  iff.read((char*)input_text,len);
  iff.close();

  uint *input = new uint[len];//
  for (uint i=0;i<len;i++)
    input[i] = (uint)input_text[i];
  //len-1? at the very end position,the editor is adding something. We do not 
  //want that something. 
  Mapper *mp = new MapperCont(input, len-1, BitSequenceBuilderRG(32));
  uint dic[256];
  for (uint i=0;i<256;i++)
    dic[i]=0;
  for (uint i=0;i<len-1;i++){
    uint v = mp->map(input[i]);
    dic[input[i]]=v;
    input[i]= v;
    assert(input[i]>0);
  }
  input[len-1]=0;

  uint beta = atoi(argv[2]);
  uint sampleC = 4;
  uint superSampleC = 2;

  // {
  //   //create lzend parse to save
  //   char * buffer = new char[strlen(argv[1])+1024];
  //   strcpy(buffer,argv[1]);
  //   strcat(buffer,".lzend");
  //   Sequence *lz = new SequenceLZEnd(input, len, dic);
  //   ofstream fp (buffer);
  //   lz->save(fp);
  //   fp.close();
  //   exit(0);
  // }
  //LZBaseStrategy *strategy = new LZBaseLimitHops(beta);
  // LZBaseStrategy *strategy = new LZBaseLength(beta);
  // LZBaseStrategy *strategy = new LZBaseSampleOnN(beta, 8);
  LZBaseStrategy *strategy = new LZBaseCopyOptimal(beta);

  Sequence *seq;
  if (argc==4){
    ifstream fp(argv[3]);
    Sequence *initial_parse = Sequence::load(fp);
    fp.close();
    seq  = new SequenceLZEndBase(input,len, new BitSequenceBuilderRRR(32),new SequenceBuilderWaveletMatrix(new BitSequenceBuilderRG(32),new MapperNone()), strategy, sampleC, superSampleC, (SequenceLZEnd*)initial_parse,false);
  }else{
    seq  = new SequenceLZEndBase(input,len, dic, new BitSequenceBuilderRRR(32),new SequenceBuilderWaveletMatrix(new BitSequenceBuilderRG(32),new MapperNone()), strategy, sampleC, superSampleC);
  }
  // saveLoadSequence(seq);
  cerr << "Seq size (bps): " << seq->getSize()*8.0 / len << endl;
  uint nqueries = min((uint)len-1,100000u);
  
  for (uint i=0;i<nqueries;i++){
    // cerr << "i: " << i << endl;
    uint acc = seq->access(i);
    assert(acc == (input[i]-1));
  }
  
  cerr << "Test access OK!" << endl;
  
  uint *counter = new uint[256];
  for (uint i=0;i<256;i++){
    counter[i]=0;
  }
  
  for (uint i=0;i<nqueries;i++){
    counter[input[i]]++;
    uint r = seq->rank(input[i]-1, i);
    assert(r==counter[input[i]]);
  }

  cerr << "Test rank OK!"<< endl;
  delete [] input_text;
  delete [] input;
  return 0;  
}

int do_testLZEndPerformance(int argc, char **argv){
  
  if (argc<3){
    cerr << "Usage: " << argv[0] << " <input-file.txt> <beta> [<lzen parse input-file.txt>]" << endl;
    cerr << "\t<beta> is the parameter to decide which phrase goes to the base sequence." << endl;
    cerr << "\tIt has different meanings depending on the algorithm used." << endl;
    return 0;
  }

  struct stat sdata;
  stat(argv[1],&sdata);
  uint len = sdata.st_size;//it is adding a blank space at the end..i do not want it-->i use it for the $.
  char *input_text = new char[len];
  
  ifstream iff(argv[1]);
  iff.read((char*)input_text,len);
  iff.close();

  uint *input = new uint[len];//
  for (uint i=0;i<len;i++)
    input[i] = (uint)input_text[i];
  //len-1? at the very end position,the editor is adding something. We do not 
  //want that something. 
  Mapper *mp = new MapperCont(input, len-1, BitSequenceBuilderRG(32));
  uint dic[256];
  for (uint i=0;i<256;i++)
    dic[i]=0;
  for (uint i=0;i<len-1;i++){
    uint v = mp->map(input[i]);
    dic[input[i]]=v;
    input[i]= v;
    assert(input[i]>0);
  }
  input[len-1]=0;
  // Repair *rp = new Repair((int*)input, len);
  // cerr << "Repair rules: " << rp->getNRules() << endl;

  uint beta = atoi(argv[2]);
  uint sampleC = 1;
  uint superSampleC = 4;

  // {
  //   //create lzend parse to save
  //   char * buffer = new char[strlen(argv[1])+1024];
  //   strcpy(buffer,argv[1]);
  //   strcat(buffer,".lzend");
  //   Sequence *lz = new SequenceLZEnd(input, len, dic);
  //   ofstream fp (buffer);
  //   lz->save(fp);
  //   fp.close();
  //   exit(0);
  // }
  //LZBaseStrategy *strategy = new LZBaseLimitHops(beta);
  // LZBaseStrategy *strategy = new LZBaseLength(beta);
  // LZBaseStrategy *strategy = new LZBaseSampleOnN(beta, 8);
  LZBaseStrategy *strategy = new LZBaseCopyOptimal(beta);

  Sequence *seq;
  if (argc==4){
    ifstream fp(argv[3]);
    Sequence *initial_parse = Sequence::load(fp);
    fp.close();

    SequenceBuilder *seqFirstClasses = new SequenceBuilderSequenceRepairSC(new BitSequenceBuilderRRR(32),
             new MapperNone(),new PermutationBuilderMRRR(5,new BitSequenceBuilderRG(10)),8,4,8u);
    SequenceBuilder *sb = new SequenceBuilderWaveletTree(new BitSequenceBuilderRG(32),new MapperNone());
    // SequenceBuilder *rpsn = new SequenceBuilderRPSN(new BitSequenceBuilderRRR(32), 4,2,4);
    seq  = new SequenceLZEndBase(input,len, new BitSequenceBuilderDelta(128),sb, strategy, sampleC, superSampleC, (SequenceLZEnd*)initial_parse,false);
  }else{
    seq  = new SequenceLZEndBase(input,len, dic, new BitSequenceBuilderDelta(128),new SequenceBuilderWaveletTree(new BitSequenceBuilderRG(32),new MapperNone()), strategy, sampleC, superSampleC);
  }

  cerr << "Seq size (bps): " << seq->getSize()*8.0 / len << endl;
  uint nqueries = min((uint)len-1,1000u);
  double t0,t1;
  t0 = getTime();
  for (uint i=0;i<nqueries;i++){
    // cerr << "i: " << i << endl;
    uint acc = seq->access(i);
    assert(acc == (input[i]-1));
  }
  t1 = getTime();
  cerr << "Test access OK!" << endl;
  cerr << "Time per query: " << (t1-t0)*1000000.0/(nqueries) << endl;
  uint *counter = new uint[256];
  for (uint i=0;i<256;i++){
    counter[i]=0;
  }
  
  uint *queries = new uint[nqueries];
  for (uint i=0;i<nqueries;i++){
    queries[i] = rand()%(len-1);
  }

  t0 = getTime();
  for (uint i=0;i<nqueries;i++){
    uint acc = seq->access(queries[i]);
  }
  t1 = getTime();
  cerr << "Time per query: " << (t1-t0)*1000000.0/(nqueries) << endl;
  t0 = getTime();
  for (uint i=0;i<nqueries;i++){
    seq->rank(input[queries[i]]-1, queries[i]);
    // seq->rank(input[i]-1, i);
  }

    // for (uint i=0;i<nqueries;i++){
      // counter[input[i]]++;
       // cerr << i << endl;
     
      // uint r = seq->rank(input[i]-1, i);
      // if (!(r==counter[input[i]]))
      //   cerr << "(i:" << i << "), obtained: " << r << ", expected: " << counter[input[i]] << endl;
      // assert(r==counter[input[i]]);
    // }
  t1 = getTime();
  delete [] queries;
  cerr << "Time per query: " << (t1-t0)*1000000.0/nqueries << endl;
  cerr << "Test rank OK!"<< endl;
  delete [] input_text;
  delete [] input;
  return 0;  
}
