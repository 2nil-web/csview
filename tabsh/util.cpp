
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <functional>

#include "util.h"

// Return total memory in mega bytes
double getTotalSystemMemory() {
  unsigned long long mem_in_byte;
#ifdef _WIN32
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

std::vector<std::string> split(const std::string &str, char delim) {
 std::vector<std::string> sv;
 std::stringstream ss(str);
 std::string s;
 while (getline(ss, s, delim)) sv.push_back(s);

 return sv;
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

char string_to_ascii(std::string s) {
  if (s.size() == 1) return s[0];
  if (all_of_ctype(s, isdigit)) return (char)std::stoi(s);
  std::cout << "Undefined character or ascii code, returning space" << std::endl;
  return ' ';
}

bool string_to_bool(std::string s) {
  if (s == "1" || s == "on" || s == "true") return true;
  if (s == "0" || s == "off" || s == "false") return false;
  std::cout << "Unknown boolean value, returning false" << std::endl;
  return false;
}

bool any_of_ctype(const std::string s, std::function<int(int)> istype) {
  return std::any_of(s.begin(), s.end(), [istype](char c) { return istype(c); } );
}

bool all_of_ctype(const std::string s, std::function<int(int)> istype) {
  return std::all_of(s.begin(), s.end(), [istype](char c) { return istype(c); } );
}


// From Freak, see : https://stackoverflow.com/questions/152016/detecting-cpu-architecture-compile-time
std::string getBuild() {
  #if defined(__x86_64__) || defined(_M_X64)
  return "x86_64";
  #elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
  return "x86_32";
  #elif defined(__ARM_ARCH_2__)
  return "ARM2";
  #elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
  return "ARM3";
  #elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
  return "ARM4T";
  #elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
  return "ARM5"
  #elif defined(__ARM_ARCH_6T2_) || defined(__ARM_ARCH_6T2_)
  return "ARM6T2";
  #elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
  return "ARM6";
  #elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
  return "ARM7";
  #elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
  return "ARM7A";
  #elif defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
  return "ARM7R";
  #elif defined(__ARM_ARCH_7M__)
  return "ARM7M";
  #elif defined(__ARM_ARCH_7S__)
  return "ARM7S";
  #elif defined(__aarch64__) || defined(_M_ARM64)
  return "ARM64";
  #elif defined(mips) || defined(__mips__) || defined(__mips)
  return "MIPS";
  #elif defined(__sh__)
  return "SUPERH";
  #elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
  return "POWERPC";
  #elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
  return "POWERPC64";
  #elif defined(__sparc__) || defined(__sparc)
  return "SPARC";
  #elif defined(__m68k__)
  return "M68K";
  #else
  return "UNKNOWN";
  #endif
}

