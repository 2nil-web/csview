
#ifndef READCSV_H
#define READCSV_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <iterator>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <tuple>
#ifdef __WIN32__
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace csv {
  class cell {
    public:
    std::uintmax_t start, end;
    void reset () { start=end=0; }
    cell () { start=end=0; }
  };

  class row {
    public:
    std::vector<cell> cells;
    std::uintmax_t start, end;
    void reset () { cells.clear(); start=end=0; }
    row () { start=end=0; }
  };

  class file {
    private:
    bool is_csv=true;
    char cell_separator=';', string_delimiter='"', end_of_line='\n', escape='\\';
    size_t max_line_count=256, min_cell_size=8, max_cell_size=256;
    std::vector<row> rows;

    public:
    file ()  { };
    ~file ()  { };
    void setfmt(
      bool _is_csv=true,
      char _cell_separator=';',
      char _string_delimiter='"',
      char _end_of_line='\n',
      char _escape='\\',
      size_t _max_line_count=256,
      size_t _min_cell_size=8,
      size_t _max_cell_size=256
    );

    bool read(std::string &file_path);
    void stat(bool line_by_line=true);
  };
}

#endif // READCSV_H

