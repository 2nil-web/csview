
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <algorithm>

std::vector<std::string> args, envs;
std::string prog_basename;

// tail, see : https://stackoverflow.com/questions/11876290/c-fastest-way-to-read-only-last-line-of-text-file

void tail(std::string fn, int n) {
  std::ifstream fin(fn);

  if (fin.is_open()) {
    if (n > 0) {
      fin.seekg(-1, std::ios_base::end);

      while (n && fin.good()) {
        fin.seekg(-1, std::ios_base::cur);
        if (fin.peek() == '\n') n--;
      }

      fin.seekg(1, std::ios_base::cur);
    }

    std::string ln;
    while (getline(fin, ln)) std::cout << ln << std::endl;
    fin.close();
  }
}

bool is_positive_integer(const std::string &s) {
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

int main(int argc, char *argv[]) { 
  args=std::vector<std::string>(argv, argv + argc);
  prog_basename=std::filesystem::path(args[0]).stem().string();
  args.erase(args.begin());

  int nl=10;

  if (args.size() == 1) tail(args[0], nl);
  else if (args.size() > 1) {
    if (is_positive_integer(args[0])) {
      nl=std::stoi(args[0]);
      args.erase(args.begin());
    }

    for(auto f: args) tail(f, nl);
  }
  return 0;
}

