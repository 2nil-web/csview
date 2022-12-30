
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <thread>

#include <windows.h>

std::vector<std::string> args, envs;
std::string prog_basename;


// Display lines of a file from the bottom
void display_from_bottom(std::string fn, size_t n) {
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

// Compute the number of lines of a files
size_t count_lines(std::string fn, bool display=false) {
  std::string ln;
  int _nl=0;
  std::ifstream fp(fn);

  if (fp.is_open()) {
    while (std::getline(fp, ln)) {
      if (display) std::cout << ln << std::endl;
      ++_nl;
    }
    fp.close();
  }

  return _nl;
}

bool is_positive_integer(const std::string &s) {
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

bool flush_cache_for_file(std::string s) {
  bool ret=false;

  HANDLE hFile=CreateFile(s.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile != INVALID_HANDLE_VALUE) {
    ret=FlushFileBuffers(hFile);
    CloseHandle(hFile);
  }

  return ret;
}


#define trc std::cout << __LINE__ << std::endl;

int main(int argc, char *argv[]) {
  args=std::vector<std::string>(argv, argv + argc);
  prog_basename=std::filesystem::path(args[0]).stem().string();
  args.erase(args.begin());

  size_t prev_count=0, curr_count, disp_count, max_nl=0;

  if (is_positive_integer(args[0])) {
    max_nl=std::stoi(args[0]);
    args.erase(args.begin());
  }


  if (max_nl == 0) prev_count=count_lines(args[0], true);
  else {
    prev_count=count_lines(args[0]);
    display_from_bottom(args[0], max_nl);
  }

  for (;;) {
    curr_count=count_lines(args[0]);

    if (curr_count > prev_count) {
      disp_count=curr_count-prev_count;
      prev_count=curr_count;
      if (disp_count > 10) disp_count=10000;
      display_from_bottom(args[0], disp_count);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    flush_cache_for_file(args[0]);
  }
  return 0;
}

