
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

std::vector<std::string> args, envs;
std::string prog_basename;

// tail, see : https://stackoverflow.com/questions/11876290/c-fastest-way-to-read-only-last-line-of-text-file

void tail(std::string fn, int n) {
  std::ifstream fin(fn);

  if (fin.is_open()) {
    fin.seekg(-1, std::ios_base::end);

    while (n && fin.good()) {
      fin.seekg(-1, std::ios_base::cur);
      if (fin.peek() == '\n') n--;
    }

    fin.seekg(1, std::ios_base::cur);
    std::string ln;
    while (getline(fin, ln)) std::cout << ln << std::endl;
    fin.close();
  }
}

unsigned int nl(std::string fn) {
  std::string ln;
  unsigned int _nl=0;
  std::ifstream fp(fn);

  if (fp.is_open()) {
    while (std::getline(fp, ln)) ++_nl;
    fp.close();
  }
  return _nl;
}

int main(int argc, char *argv[]) { 
  args=std::vector<std::string>(argv, argv + argc);
  prog_basename=std::filesystem::path(args[0]).stem().string();
  args.erase(args.begin());

  if (args.size() == 1) tail(args[1], 10); 
  if (args.size() >= 2) tail(args[1], std::stoi(args[0])); 
  return 0;
}

