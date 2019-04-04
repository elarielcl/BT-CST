
#include <cstdlib>

#include <libcdsBasics.h>
#include <BitSequence.h>
#include <Mapper.h>
#include <Sequence.h>
#include <wt_coder.h>

using namespace std;
using namespace cds_static;


void testSequence(Array & a, Sequence & bs) {
    ofstream outfs("sequence.tmp");
    bs.save(outfs);
    outfs.close();
    ifstream infs("sequence.tmp");
    Sequence * seq = Sequence::load(infs);
    infs.close();
    uint maxv = a.getMax();
    size_t count[maxv+1];
    for(size_t i=0;i<=maxv;i++)
        count[i] = 0;
    for(size_t i=0;i<a.getLength();i++) {
        count[a[i]]++;
        if(a[i]!=seq->access(i)) {
            cerr << "ERROR ACCESS" << endl;
            cerr << "(pos: " << i << "), Got:" << seq->access(i) << " Expected:" << a[i] << endl;
            exit(-3);
        }
        for(uint j=a[i];j<=a[i];j++) {
            if(seq->rank(j,i)!=count[j]) {
                cerr << "ERROR RANK " << endl;
                cerr << " Rank result: " << bs.rank(j,i) << " count=" << count[j] << endl;
                cerr << " symbol=" << j << " position=" << i << endl;
                exit(-1);
            }
        }
        if(seq->select(a[i],count[a[i]])!=i) {
            cerr << "ERROR SELECT " << endl;
            cerr << "select(" << a[i] << "," << count[a[i]] << ")"<< endl;
            cerr << "a[i]=" << a[i] << " maxv=" << maxv << endl;
            cerr << "bs.select=" << bs.select(a[i],count[a[i]]) << " i=" << i << endl;
            exit(-2);
        }

    }
    cout << "Test OK" << endl;
    delete seq;
}


void testSequence2(uint *a, uint n, Sequence & bs) {
    ofstream outfs("sequence.tmp");
    bs.save(outfs);
    outfs.close();
    ifstream infs("sequence.tmp");
    Sequence * seq = Sequence::load(infs);
    infs.close();
    uint maxv = 0;
    for (uint i=0;i<n;i++) if (a[i]>maxv) maxv = a[i];
    size_t count[maxv+1];
    for(size_t i=0;i<=maxv;i++)
        count[i] = 0;
    for(size_t i=0;i<n;i++) {
        count[a[i]]++;
        //~ cout << i << endl;
        if(a[i]!=seq->access(i)) {
            cerr << "ERROR ACCESS" << endl;
            cerr << "(pos: " << i << "), Got:" << seq->access(i) << " Expected:" << a[i] << endl;
            exit(-3);
        }
        for(uint j=a[i];j<=a[i];j++) {
            if(seq->rank(j,i)!=count[j]) {
                cerr << "ERROR RANK " << endl;
                cerr << " Rank result: " << bs.rank(j,i) << " count=" << count[j] << endl;
                cerr << " symbol=" << j << " position=" << i << endl;
                exit(-1);
            }
        }
        if(seq->select(a[i],count[a[i]])!=i) {
            cerr << "ERROR SELECT " << endl;
            cerr << "a[i]=" << a[i] << " maxv=" << maxv << endl;
            cerr << "bs.select=" << bs.select(a[i],count[a[i]]) << " i=" << i << endl;
            exit(-2);
        }

    }
    delete seq;
}


void testSequenceRange(Array & a, WaveletMatrix & bs, int rngx, int rngy, int n) {
    ofstream outfs("sequence.tmp");
    bs.save(outfs);
    outfs.close();
    ifstream infs("sequence.tmp");
    Sequence * seq = Sequence::load(infs);
    infs.close();
    uint maxv = a.getMax();
    for (int i=0;i<n;i++){
		int xs,xe;
		xs = (int)(rand()%a.getLength());
		xe=xs+rngx;
		if (xe>=a.getLength()) xe=a.getLength()-1;
		
		int ys,ye;
		ys=(int)(rand()%maxv);
		ye=ys+rngy;
		if (ye>=maxv) ye=maxv-1; 
		int cnt = 0;
		for (int j=xs;j<=xe;j++){
			if (a.getField(j)>=ys && a.getField(j)<=ye) cnt++;
		}
		vector<pair<int,int> > *res = new vector<pair<int,int> > ();
		bs.range(xs,xe,ys,ye,res);
		//~ cerr << "xs="<<xs<<",xe="<<xe<<",ys="<<ys<<",ye="<<ye<<endl;
		//~ for (int j=0;j<res->size();j++) cout << "("<<(*res)[j].first<<","<<(*res)[j].second<<")-";
		//~ cout << endl;
		for (int j=0;j<res->size();j++){
			assert((*res)[j].second>=xs && (*res)[j].second<=xe);
			//~ assert((*res)[j].first>=ys && (*res)[j].first<=ye);
		}
		if (cnt!=bs.rangeCount(xs,xe,ys,ye)||(cnt!=res->size())){
			cerr << "ERROR RANGE COUNT" << endl;
			cerr << "Real=" << cnt << ", count=" << bs.rangeCount(xs,xe,ys,ye)<< ", range="<<res->size() << endl;
			cerr << "xs="<<xs<<",xe="<<xe<<",ys="<<ys<<",ye="<<ye<<endl;
			exit(-3);
		}
		delete res;
	}
	cerr << "TEST RANGE OK" << endl;
    
}

int main(int argc, char ** argv) {

  if(argc!=4) {
    cout << "Checks the array class generating <length> elements between 0 and <maxv> using <seed> as seed for the numbers generation" << endl << endl;
    cout << "usage: " << argv[0] << " <seed> <length> <maxv>" << endl;
    return 0;
  }

  srand(transform(string(argv[1])));
  uint len = transform(string(argv[2]));
  uint maxv = transform(string(argv[3]));

  len = 13;
  maxv=6;
  uint *data = new uint[len];
  Array a(len,maxv);

  a.setField(0,2);a.setField(1,1);a.setField(2,4);a.setField(3,0);
  a.setField(4,2);a.setField(5,6);a.setField(6,5);a.setField(7,3);
  a.setField(8,3);a.setField(9,0);a.setField(10,5);a.setField(11,0);
  a.setField(12,4);
 
  
  Mapper * mapper = new MapperCont(a, BitSequenceBuilderRG(20));
  mapper->use();
   wt_coder *wc = new wt_coder_binary(a,mapper);

	WaveletMatrix wm(a,new BitSequenceBuilderRRR(32),new MapperNone());
	testSequenceRange(a,wm,4,3,10);

  //~ cout << "Test 1 : Wavelet tree with pointers" << endl;
  //~ WaveletTree wt1(a, wc, new BitSequenceBuilderRRR(32), mapper);
  //~ for (uint i=0;i<len;i++){
	  //~ cout << "a[" << i << "]: " << a.getField(i) << endl;
  //~ }
  //~ testSequence(a, wt1);
  //~ testSequenceRange(a,wt1,4,3,10);

  //~ Mapper * mapper = new MapperCont(a, BitSequenceBuilderRG(20));
  //~ mapper->use();
  //~ cout << "Test 1 : Wavelet tree with pointers" << endl;
  //~ WaveletTreeNoptrs wt1(a,new BitSequenceBuilderRRR(32), mapper);
  //~ for (int i=0;i<len;i++){
	  //~ cout << "a[" << i << "]: " << a.getField(i) << endl;
  //~ }
  //~ testSequence(a, wt1);
  //~ testSequenceRange(a,wt1,4,3,10);
	
	delete [] data;
  return 0;
}

