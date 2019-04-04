#ifndef UTILS_RLCSA_H
#define UTILS_RLCSA_H

#include <fstream>
#include <vector>

#include "definitions.h"


namespace TextIndexes
{



struct Triple
{
  usint first;
  usint second;
  usint third;

  Triple();
  Triple(usint a, usint b, usint c);
};


std::streamoff fileSize(std::ifstream& file);
std::streamoff fileSize(std::ofstream& file);


std::ostream& operator<<(std::ostream& stream, pair_type data);


void readRows(std::ifstream& file, std::vector<std::string>& rows, bool skipEmptyRows);
void readPizzaChili(std::ifstream& file, std::vector<std::string>& patterns);

double readTimer();


template<class T>
void
removeDuplicates(std::vector<T>* vec)
{
  if(vec == 0) { return; }
  std::sort(vec->begin(), vec->end());
  vec->resize(std::unique(vec->begin(), vec->end()) - vec->begin());
}


} // namespace TextIndexes


#endif // UTILS_H
