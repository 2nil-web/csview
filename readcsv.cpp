
#include "readcsv.h"

// Return total memory in mega bytes
double getTotalSystemMemory() {
  unsigned long long mem_in_byte;
#ifdef __WIN32__
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    mem_in_byte=status.ullTotalPhys;
#else
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    mem_in_byte=pages * page_size;
#endif
    // Return mem in mega bytes
    return (double)mem_in_byte/1048576;
}


void csv::file::setfmt(bool _is_csv,
    char _cell_separator, char _string_delimiter, char _end_of_line, char _escape,
    std::uintmax_t _max_line_count, std::uintmax_t _min_cell_size, std::uintmax_t _max_cell_size) {
  is_csv=_is_csv;
  cell_separator=_cell_separator;
  string_delimiter=_string_delimiter;
  end_of_line=_end_of_line;
  escape=_escape;
  max_line_count=_max_line_count;
  min_cell_size=_min_cell_size;
  max_cell_size=_max_cell_size;
}

bool csv::file::read(std::string &file_path) {
  std::ifstream in(file_path);
  char c;

  std::uintmax_t curr_pos=0;
  row curr_row;
  cell curr_cell;
  bool currently_escaped=false, currently_delimited=false;

  if (in.is_open()) {
    //std::uintmax_t file_length=std::filesystem::file_size(std::filesystem::path{fname});

    while(in.good()) {
      in.get(c);

      if (is_csv) {
        // Ignore current character meaning as it is escaped
        if (currently_escaped) {
          currently_escaped=false;
          curr_pos++;
          continue;
        }

        // Continue on same cell as it contains a delimited string
        // While next delimiter which marks end of delimited string is not found
        if (currently_delimited) {
          if (c == string_delimiter) currently_delimited=false;
          curr_pos++;
          continue;
        }

        // Next character is escaped
        if (c == escape) {
          currently_escaped=false;
        }

        // Current cell contains a delimited string
        if (c == string_delimiter) {
          currently_delimited=true;
        }

        // End of cell, store current cell into current row
        if (c == cell_separator) {
          curr_cell.end=curr_pos;
          curr_row.cells.push_back(curr_cell);
          curr_cell.reset();
          curr_cell.start=curr_pos+1;
        }
      }

      // End of line, store current cell into current row into file
      if (c == end_of_line) {
        curr_cell.end=curr_pos;
        curr_row.end=curr_pos;

        if (curr_cell.start != 0 || curr_row.start == 0) {
          curr_row.cells.push_back(curr_cell);
          if (curr_row.start != curr_row.end) rows.push_back(curr_row);
        }

        curr_cell.reset();
        curr_row.reset();
        curr_cell.start=curr_row.start=curr_pos+1;
      }

      curr_pos++;
    }

    if (curr_cell.start < curr_pos && curr_row.cells.size () > 0) {
      curr_cell.end=curr_row.end=curr_pos;
      curr_row.cells.push_back(curr_cell);
      rows.push_back(curr_row);
      curr_row.reset();
    }
/*
    if (curr_row.start != curr_row.end) {
      if (curr_row.end == 0) curr_row.end=curr_pos;
      rows.push_back(curr_row);
    }*/
  }

  if (!in.eof() && in.fail()) {
    std::cout << "error reading " << file_path << std::endl;
    return false;
  }

  in.close();
  return true;
}

//bool csv::file::read(std::string &file_path) { return read(file_path, cell_separator, string_delimiter, end_of_line, escape); }

void csv::file::stat(bool line_by_line) {
  std::uintmax_t 
    min_cells_in_a_row=UINT_MAX, max_cells_in_a_row=0, row_with_min_cells, row_with_max_cells,
    min_char_in_a_cell=UINT_MAX, max_char_in_a_cell=0,
    cell_with_min_char, row_with_min_char,
    cell_with_max_char, row_with_max_char;

  if (line_by_line) std::cout << "row number;number of cells;row start;row length;cell number;cell start;cell length" << std::endl;
  std::uintmax_t nr=0, nc=0, ncr;
  for(auto row:rows) {
    if (line_by_line) std::cout << nr << cell_separator << row.cells.size() << cell_separator << row.start << cell_separator << row.end-row.start;

    if (row.cells.size() < min_cells_in_a_row) {
      min_cells_in_a_row=row.cells.size();
      row_with_min_cells=nr;
    }

    if (row.cells.size() > max_cells_in_a_row) {
      max_cells_in_a_row=row.cells.size();
      row_with_max_cells=nr;
    }

    ncr=0;
    for(auto cell:row.cells) {
      std::uintmax_t cell_len=cell.end-cell.start;

      if (line_by_line) std::cout << cell_separator << ncr << cell_separator << cell.start-row.start << cell_separator << cell_len;

      if (cell_len < min_char_in_a_cell) {
        min_char_in_a_cell=cell_len;
        cell_with_min_char=ncr;
        row_with_min_char=nr;
      }

      if (cell_len > max_char_in_a_cell) {
        max_char_in_a_cell=cell_len;
        cell_with_max_char=ncr;
        row_with_max_char=nr;
      }

      ncr++;
      nc++;
    }

    if (line_by_line) std::cout << std::endl;
    nr++;
  }

  std::cout << "Number of rows  " << rows.size() << std::endl;
  if (is_csv) {
    std::cout << "Number of cells " << nc << std::endl;
    std::cout << "Min number of cells in a row " << min_cells_in_a_row << " first found at row " << row_with_min_cells << std::endl;
    std::cout << "Max number of cells in a row " << max_cells_in_a_row << " first found at row " << row_with_max_cells << std::endl;

    std::cout << "Min number of chars in a cell " << min_char_in_a_cell << " first found at rc(" << row_with_min_char << ',' << cell_with_min_char << ')' << std::endl;
    std::cout << "Max number of chars in a cell " << max_char_in_a_cell << " first found at rc(" << row_with_max_char << ',' << cell_with_max_char << ')' << std::endl;
  } else {
    std::cout << "Min number of chars in a row " << min_char_in_a_cell << " first found at row " << row_with_min_char << std::endl;
    std::cout << "Max number of chars in a row " << max_char_in_a_cell << " first found at row " << row_with_max_char << std::endl;
  }
}

double delay(bool start=true) {
  static std::chrono::high_resolution_clock::time_point begin;

  if (start) {
    begin=std::chrono::high_resolution_clock::now();
    return 0.0;
  } else  {
    return std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-begin).count();
  }
}

std::vector<std::string> args;
std::string prog_basename;

bool string_to_bool(std::string s) {
  if (s == "1" || s == "on" || s == "true") return true;
  return false;
}

int main(int argc, char *argv[]) {
  args=std::vector<std::string>(argv, argv + argc);
  prog_basename=std::filesystem::path(args[0]).stem().string();
  args.erase(args.begin());

  bool is_csv;
  if (args.size() > 1) {
    is_csv=string_to_bool(args[1]);
  }

  bool line_by_line=true;
  if (args.size() > 2) {
    line_by_line=string_to_bool(args[2]);
  }

  if (args.size() > 0) {
    csv::file cf;
    cf.setfmt(is_csv);

    delay();
    cf.read(args[0]);
    double dl=delay(false);
    std::cout << "Delay to read file " << args[1] << ' ' << dl << " seconds." << std::endl;

    delay();
    cf.stat(line_by_line);
    dl=delay(false);
    std::cout << "Delay to compute stats " << dl << " seconds." << std::endl;

    std::cout << "Total mem " << getTotalSystemMemory() << std::endl;
  }

  return 0;
}

