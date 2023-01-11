
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
#include <cerrno>
#include <tuple>

#ifdef __WIN32__
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
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

    char c;
    std::uintmax_t curr_pos=0;
    row curr_row;
    cell curr_cell;
    bool currently_escaped=false, currently_delimited=false, loaded_in_mem;
    std::string filename;
    std::string in_mem="";

    void parse_file(char c);
    void end_parse_file();
    
    public:
    file (std::string _filename="")  { filename=_filename; };
    ~file ()  { };
    void setfmt(
      bool _is_csv=true,
      char _cell_separator=';',
      char _string_delimiter='\0',
      char _end_of_line='\n',
      char _escape='\\',
      size_t _max_line_count=256,
      size_t _min_cell_size=8,
      size_t _max_cell_size=256
    );

    bool read_from_file();
    bool read_in_memory();
    void reset();
    bool load(std::string _filename, bool in_memory=true);
    void stat(bool line_by_line=true);
    std::string read_substring_from_file(std::uintmax_t start_read=0, std::uintmax_t length_read=0);
    void list(std::uintmax_t r1, std::uintmax_t r2);
    void list(std::uintmax_t r);
    void list();
    bool parse_row_range(std::string, std::vector<std::uintmax_t>&);
    bool parse_row_list(std::string, std::vector<std::uintmax_t>&);
    bool parse_cell_range(std::string, std::vector<std::uintmax_t>&);
    bool parse_cell_list(std::string, std::vector<std::uintmax_t>&);
  };
}

#endif // READCSV_H

