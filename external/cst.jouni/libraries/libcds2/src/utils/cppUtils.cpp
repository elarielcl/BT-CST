
#include <utils/libcdsBasics.h>
#include <utils/cppUtils.h>
#include <cstring>
#include <cerrno>
#include <stdexcept>

namespace cds_utils
{

	uint transform(const string & s) {
		stringstream ss;
		ss << s;
		uint ret;
		ss >> ret;
		return ret;
	}
	
	void tokenize(string str, vector<string> &tokens, char delim) {
		string::size_type last_pos = 0;
		string::size_type pos = str.find_first_of(delim);
		while(pos!=string::npos) {
			tokens.push_back(str.substr(last_pos,pos-last_pos));
			last_pos = pos+1;
			if(last_pos >= str.length()) break;
			pos = str.find_first_of(delim,pos+1);
		}
		if(last_pos<str.length())
			tokens.push_back(str.substr(last_pos));
	}

	char *fileFromFd(uint fd){
		char *path = new char [2048];
		strcpy(path,FILE_DESCRIPTORS_LOCATION);
		sprintf(path,"%s%u",path,fd);
		char *buf = new char[4096];
		uint r;
		if ((r=readlink(path, buf, 4096))==-1){
			std::cerr << "Error while trying to open " << path << std::endl;
			throw std::runtime_error(strerror(errno));
 		}
 		buf[r]='\0';
 		return buf;
	}

	ifstream *ifstreamFromFd(uint fd){
		ifstream *in = new ifstream(fileFromFd(fd));
		return in;
	}
	
	ofstream *ofstreamFromFd(uint fd){
		ofstream *of = new ofstream(fileFromFd(fd));
		return of;
	}
};
