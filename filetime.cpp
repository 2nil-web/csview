
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

// Displaying the type of a variable with name 'var'
//  std::cout << typeid(var).name() << std::endl;


template <typename TP>
std::time_t to_time_t(TP tp) {
  auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(tp-TP::clock::now()+std::chrono::system_clock::now());
  return std::chrono::system_clock::to_time_t(sctp);
}

// Return last write time of a file in time_t type
std::time_t last_write_time(std::string filename) {
  auto p=std::filesystem::path(filename);
  std::filesystem::file_time_type ftime=std::filesystem::last_write_time(p);
  return to_time_t(ftime);
}


// std::filesystem::file_time_type is defined as std::chrono::time_point<std::chrono::file_clock>,
// Display the last write time of a file with specific time format (default format is ISO8601)
void print_last_write_time(std::string file_path, std::string time_format="%Y-%m-%dT%H:%M:%S") {
  auto ftime_t=last_write_time(file_path);
  std::cout << "Last write time " << std::put_time(std::localtime(&ftime_t), time_format.c_str()) << " for " << file_path << std::endl;
}

int main(int argc, char *argv[]) {
  std::vector<std::string> args, envs;
  //std::string prog_basename;

  args=std::vector<std::string>(argv, argv + argc);
  //prog_basename=std::filesystem::path(args[0]).stem().string();
  args.erase(args.begin());
  for(auto f: args) print_last_write_time(f);
  return 0;
}

