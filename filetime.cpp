
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <utility>
#include <algorithm>
#include <cctype>
#include <locale>
#include <chrono>
#include <iomanip>
#include <vector>
#include <string>

using namespace std::chrono_literals;
 
std::vector<std::string> args, envs;
std::string prog_basename;

template <typename TP>
std::time_t to_time_t(TP tp) {
  using namespace std::chrono;
  auto sctp = time_point_cast<system_clock::duration>(tp-TP::clock::now()+system_clock::now());
  return system_clock::to_time_t(sctp);
}

#define TIME_FORMAT "%Y-%m-%dT%H:%M:%S"
// std::filesystem::file_time_type is defined as std::chrono::time_point<std::chrono::file_clock>,
void wr_time(std::string pth) {
  auto p=std::filesystem::path(pth);
  std::filesystem::file_time_type ftime=std::filesystem::last_write_time(p);
  auto ftime_t=to_time_t(ftime);
  std::cout << "last modif time of " << pth << " is " << std::put_time(std::localtime(&ftime_t), TIME_FORMAT) << std::endl;
}

int main(int argc, char *argv[]) {
  args=std::vector<std::string>(argv, argv + argc);
  prog_basename=std::filesystem::path(args[0]).stem().string();
  args.erase(args.begin());
  for(auto f: args) wr_time(f);
  return 0;
}

