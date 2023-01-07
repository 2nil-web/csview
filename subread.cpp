
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
 
std::vector<std::string> args;
std::string prog_basename;

bool readsubfile(std::string fname, std::uintmax_t start_read=0, std::uintmax_t length_read=0) {
  std::uintmax_t file_length=std::filesystem::file_size(std::filesystem::path{fname});
  if (start_read > file_length) {
    std::cerr << "Trying to read starting from character " << start_read << ", which is beyond file length of " << file_length << " characters" << std::endl;
  }

  std::ifstream in(fname);

  if (in.is_open()) {
    if (start_read > 0) in.seekg(start_read);

    std::uintmax_t l=0;
    char c;
    while(in.good()) {
      in.get(c);
      std::cout << c;
      if (length_read > 0 && l > length_read) break;
      l++;
    }
   
    std::cout << std::endl;
    in.close();
  } else {
    std::cerr << "Could not open " << fname << std::endl;
    return false;
  }

  return true;
}

int main(int argc, char *argv[]) {
  args=std::vector<std::string>(argv, argv + argc);
  prog_basename=std::filesystem::path(args[0]).stem().string();
  args.erase(args.begin());

  // filename in args[0]
  if (args.size() < 1) {
    std::cerr << "At least filename" << std::endl;
    std::cerr << "Optionnally start read and length read" << std::endl;
    return 1;
  }
  // start char in args[1] optionnale, default 0
  // read length in args[2] optionnal, default to end
  std::uintmax_t start_read=0, length_read=0;
  if (args.size() >= 2) start_read=std::stoi(args[1]);
  if (args.size() >= 3) length_read=std::stoi(args[2]);
  if (readsubfile(args[0], start_read, length_read)) return 0;

  return 1;
}

