
#include "util.h"

// Return total memory in mega bytes
double getTotalSystemMemory() {
  unsigned long long mem_in_byte;
#ifdef __WIN32__
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    mem_in_byte=status.ullTotalPhys;
#else
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    mem_in_byte=pages * page_size;
#endif
    // Return mem in mega bytes
    return (double)mem_in_byte/1048576;
}

std::string trim(std::string& s) {
  s.erase(0, s.find_first_not_of(" \n\r\t"));
  s.erase(s.find_last_not_of(" \n\r\t")+1);
  return s;
}

bool swallow_file(std::string &file_path, std::string &s) {
  std::ifstream in(file_path, std::ios::in | std::ios::binary);

  if (in) {
    in.seekg(0, std::ios::end);
    s.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&s[0], s.size());
    in.close();
    return true;
  }

  return false;
}

double delay(bool start) {
  static std::chrono::high_resolution_clock::time_point begin;

  if (start) {
    begin=std::chrono::high_resolution_clock::now();
    return 0.0;
  } else  {
    return std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-begin).count();
  }
}

bool string_to_bool(std::string s) {
  if (s == "1" || s == "on" || s == "true") return true;
  return false;
}

