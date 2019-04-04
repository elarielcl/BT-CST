#include <algorithm>
#include <cstdlib>

#include <rlcsa/utils.h>

#ifdef MULTITHREAD_SUPPORT
#include <omp.h>
#endif


namespace TextIndexes
{

//--------------------------------------------------------------------------

Triple::Triple() :
  first(0), second(0), third(0)
{
}

Triple::Triple(usint a, usint b, usint c) :
  first(a), second(b), third(c)
{
}

//--------------------------------------------------------------------------

std::streamoff
fileSize(std::ifstream& file)
{
  std::streamoff curr = file.tellg();

  file.seekg(0, std::ios::end);
  std::streamoff size = file.tellg();
  file.seekg(0, std::ios::beg);
  size -= file.tellg();

  file.seekg(curr, std::ios::beg);
  return size;
}

std::streamoff
fileSize(std::ofstream& file)
{
  std::streamoff curr = file.tellp();

  file.seekp(0, std::ios::end);
  std::streamoff size = file.tellp();
  file.seekp(0, std::ios::beg);
  size -= file.tellp();

  file.seekp(curr, std::ios::beg);
  return size;
}

//--------------------------------------------------------------------------

std::ostream&
operator<<(std::ostream& stream, pair_type data)
{
  return stream << "(" << data.first << ", " << data.second << ")";
}

//--------------------------------------------------------------------------

void
readRows(std::ifstream& file, std::vector<std::string>& rows, bool skipEmptyRows)
{
  while(file)
  {
    std::string buf;
    std::getline(file, buf);
    if(skipEmptyRows && buf.length() == 0) { continue; }
    rows.push_back(buf);
  }
}

void
readPizzaChili(std::ifstream& file, std::vector<std::string>& patterns)
{
  std::string header;
  std::getline(file, header);

  std::size_t start = header.find("number=");
  if(start == std::string::npos) { return; }
  int temp = std::atoi(header.substr(start + 7).c_str());
  if(temp <= 0) { return; }
  usint n = temp;

  start = header.find("length=");
  if(start == std::string::npos) { return; }
  temp = std::atoi(header.substr(start + 7).c_str());
  if(temp <= 0) { return; }
  usint l = temp;

  char buffer[l];
  for(usint i = 0; i < n; i++)
  {
    file.read(buffer, l);
    patterns.push_back(std::string(buffer, l));
  }
}

//--------------------------------------------------------------------------

double
readTimer()
{
  #ifdef MULTITHREAD_SUPPORT
  return omp_get_wtime();
  #else
  return clock() / (double)CLOCKS_PER_SEC;
  #endif
}

} // namespace TextIndexes
