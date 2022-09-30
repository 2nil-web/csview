
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

/*
#include <iterator>
#include <algorithm>


struct TestEOL {
  bool operator()(char c) {
    last    = c;
    return last == '\n';
  }

  char    last;
};

int cl(std::string fn) {
  std::fstream  file(fn);
  TestEOL       test;
  std::size_t   count=std::count_if(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), test);
  // If the last character checked is not '\n' then the last line in the file has not been  counted. So increement the count so we count the last line even if it is not '\n' terminated.
  if (test.last != '\n') ++count;
  return count;
}

int main(int argc, char *argv[]) {
  if (argc >= 2) std::cout << cl(argv[1]) << std::endl;
  return 0;
}
*/
