
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

std::vector<std::string> args, envs;
std::string prog_basename;

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
  for(auto f: args) std::cout << nl(f) << std::endl;
  return 0;
}

