
#include <cstdlib>

#include <libcdsBasics.h>
#include <BitSequence.h>
#include <Mapper.h>
#include <Sequence.h>

using namespace std;
using namespace cds_static;

double getTime (void){
	
   double usertime;
   struct rusage usage;

   getrusage (RUSAGE_SELF, &usage);

   usertime = (double) usage.ru_utime.tv_sec +
       (double) usage.ru_utime.tv_usec / 1000000.0;

   return (usertime );
}

void testDirectAccess(uint *a, uint n, DirectAccess & bs) {
 	ofstream outfs("sequence.tmp");
    bs.save(outfs);
    outfs.close();
    ifstream infs("sequence.tmp");
    DirectAccess *newda = DirectAccess::load(infs);
    infs.close();
	
	double t0,t1;
	t0=getTime();
    for(size_t i=0;i<n;i++) {
		newda->access(i);
        //~ if(a[i]!=bs.access(i)) {
            //~ cerr << "ERROR ACCESS" << endl;
            //~ cerr << "(pos: " << i << "), Got:" << bs.access(i) << " Expected:" << a[i] << endl;
            //~ exit(-3);
        //~ }
    }
    t1=getTime();
    cerr << "Time (micro/access): " << (t1-t0)*1000000/n << endl;
}

int main(int argc, char ** argv) {

  if(argc!=5 && argc!=3) {
    cout << "usage: " << argv[0] << " <seed> <length> <maxv> <0:DAC,1:DACe>" << endl;
    cout << "OR"<< endl << "usage: " << argv[0] << " <collection> (where collection is a binary sequence with one int (32 bits) for each word" << endl;
    return 0;
  }
	uint len,maxv=0,*data;
  #if 0
	  //bitmap i used to drive the development of the GCT 
	  len=66;
	  maxv=1;
	  data[66]={1,1,0,1,1,1,0,1,0,0,1,0,0,1,1,0,1,1,0,1,0,0,1,1,0,1,0,0,1,1,0,1,0,0,1,1,0,1,0,0,0,1,1,0,1,0,0,1,0,1,1,0,1,1,0,1,0,0,0,1,1,0,1,0,0,0};
  #endif 
  
  uint structure=0;
  if (argc==5){
	len = transform(string(argv[2]));
	maxv = transform(string(argv[3]));

	data = new uint[len];
	uint i;
	for(i=0;i<len/3;i++){
		data[i] = rand()%64;
	}
	for (;i<len/2;i++)
		data[i] = 64+rand()%512;
	for (;i<len;i++)
		data[i] = 512+rand()%(maxv-512);
	structure = atoi(argv[4]);
  }else if (argc==3){
		streampos begin,end;
		ifstream myfile (argv[1], ios::binary);
		begin = myfile.tellg();
		myfile.seekg (0, ios::end);
		end = myfile.tellg();
		myfile.seekg(0);
		len = end-begin;
		data = new uint[len/sizeof(uint)];
		myfile.read((char*)data,len);
		myfile.close();
		len /=sizeof(uint);
		len = min(len,len);
		MapperCont *mc = new MapperCont(data,len,BitSequenceBuilderRG(32));
		for (uint i=0;i<len;i++){
			data[i]=mc->map(data[i])-1;
			assert((int)data[i]>=0);
		}
		for (uint i=0;i<len;i++)
			maxv=max(maxv,data[i]);
	structure = atoi(argv[2]);
  }
  
  DirectAccess *da;
  
	if (structure==0){
		cerr << "DAC" << endl;
		da = new DAC(data,len,false);
	}else{
		cerr << "DACe" << endl;
		da = new DACe(data,len,false);
	}
  len = 10000;
  testDirectAccess(data,len,*da);
  delete da;
  delete [] data;
  cerr << "Test Direct Access OK!" << endl;
  return 0;
}

