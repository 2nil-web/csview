
#include <iostream>
#include <fstream>
#include <string>
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

