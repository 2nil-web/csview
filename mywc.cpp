
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

std::vector<std::string> args, envs;
std::string prog_basename;

// Compute the number of lines of a files
int nl(std::string fn) {
  std::string ln;
  int _nl=0;
  std::ifstream fp(fn);

  if (fp.is_open()) {
    while (std::getline(fp, ln)) ++_nl;
    fp.close();
  }
  return _nl;
}

// Compute the density of a file.
// The density is the average of characters by lines.
// This parameter may have an influence during the load of a text file
uintmax_t density (std::string fn) {
  // Get file size
  std::uintmax_t fs=std::filesystem::file_size(fn);
  return fs/nl(fn);
}

int main(int argc, char *argv[]) { 
  args=std::vector<std::string>(argv, argv + argc);
  prog_basename=std::filesystem::path(args[0]).stem().string();
  args.erase(args.begin());
  for(auto f: args) std::cout << f << ':' << nl(f) << " lines, " << density(f) << " chars per line." << std::endl;
  return 0;
}

