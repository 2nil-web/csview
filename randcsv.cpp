
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <cmath>

char cell_separator=';', string_delimiter='"', end_of_line='\n', escape='\\';
size_t max_line_count=256, min_cell_size=8, max_cell_size=256;

int main(int argc, char *argv[]) {
  std::vector<std::string> args=std::vector<std::string>(argv, argv + argc);
  std::string prog_basename=std::filesystem::path(args[0]).stem().string();
  args.erase(args.begin());

  std::cout << prog_basename << std::endl;
  for(auto arg:args) {
    std::cout << arg << std::endl;
  }

  std::cout << "SizeOf Char " << sizeof(char) << std::endl;
  std::cout << "SizeOf wchar_t " << sizeof(wchar_t) << std::endl;
  std::cout << "SizeOf String " << sizeof(std::string) << std::endl;
  std::cout << "SizeOf Int " << sizeof(int) << std::endl;
  std::cout << "SizeOf Long " << sizeof(long) << std::endl;
  std::cout << "SizeOf Long Long " << sizeof(long long) << std::endl;
  std::cout << "MAX number of string char " << (unsigned long)pow(2, sizeof(std::string)*sizeof(char)*8) << std::endl;
  std::cout << "MAX char " << (char)1114110 << std::endl;
  std::cout << "MAX char " << (char)18446744073709551614 << std::endl;
  for (size_t i=0; i < 65535; i++) {
    if (std::isprint(i)) std::cout << (char)i;
  }
}

