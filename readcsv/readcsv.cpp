
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

#include "util.h"
#include "readcsv.h"

void csv::file::parse_file(char c) {
  if (is_csv) {
    // Ignore current character meaning as it is escaped
    if (currently_escaped) {
      currently_escaped=false;
      curr_pos++;
      return;
    }

    // Continue on same cell as it contains a delimited string
    // While next delimiter which marks end of delimited string is not found
    if (currently_delimited) {
      if (c == string_delimiter) currently_delimited=false;
      curr_pos++;
      return;
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


void csv::file::end_parse_file() {
  if (curr_cell.start < curr_pos && curr_row.cells.size () > 0) {
    curr_cell.end=curr_row.end=curr_pos;
    curr_row.cells.push_back(curr_cell);
    rows.push_back(curr_row);
    curr_row.reset();
  }
}

void csv::file::reset() {
  curr_pos=0;
  curr_row.reset();
  curr_cell.reset();
  rows.clear();
  in_mem.clear();
  filename="";
}

// Parse a file char by char without storing it
bool csv::file::read_from_file() {
  loaded_in_mem=false;

  std::ifstream in(filename);

  if (in.is_open()) {
    //std::uintmax_t file_length=std::filesystem::file_size(std::filesystem::path{fname});

    while(in.good()) {
      in.get(c);
      parse_file(c);
    }

    end_parse_file();
  }

  if (!in.eof() && in.fail()) {
    std::cout << "error reading " << filename << std::endl;
    return false;
  }

  in.close();
  return true;
}

// Put whole file in memory then parse it char by char
bool csv::file::read_in_memory() {
  loaded_in_mem=true;

  if (swallow_file(filename, in_mem)) {
    for(auto c:in_mem) parse_file(c);
    end_parse_file();
    return true;
  }

  return false;
}

// Load a file in memory or not
bool csv::file::load(std::string _filename, bool in_memory) {
  reset();
  filename=trim(_filename);

  bool ret;
  if (in_memory) ret=read_in_memory();
  else ret=read_from_file();

  if (ret) {
    if (loaded_in_mem)
      std::cout << "File " << filename << " loaded in memory and parsed." << std::endl;
    else
      std::cout << "File " << filename << " parsed." << std::endl;
  } else std::cout << "Problem loading file " << _filename << "." << std::endl;

  return ret;
}

// Display various infos on a file optionally preceded by each of its metadata lines
void csv::file::stat(bool md_lines) {
  std::uintmax_t 
    min_cells_in_a_row=UINT_MAX, max_cells_in_a_row=0,
    row_with_min_cells=0, row_with_max_cells=0,
    min_char_in_a_cell=UINT_MAX, max_char_in_a_cell=0,
    cell_with_min_char=0, row_with_min_char=0,
    cell_with_max_char=0, row_with_max_char=0;

  if (md_lines) std::cout << "row number;number of cells;row start;row length;cell number;cell start;cell length" << std::endl;
  std::uintmax_t nr=0, nc=0, ncr;
  for(auto row:rows) {
    if (md_lines) std::cout << nr << cell_separator << row.cells.size() << cell_separator << row.start << cell_separator << row.end-row.start;

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

      if (md_lines) std::cout << cell_separator << ncr << cell_separator << cell.start-row.start << cell_separator << cell_len;

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

    if (md_lines) std::cout << std::endl;
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

// Read a substring from a file
std::string csv::file::read_substring_from_file(std::uintmax_t start_read, std::uintmax_t length_read) {
  std::uintmax_t file_length=std::filesystem::file_size(std::filesystem::path{filename});

  if (start_read > file_length) {
    std::cerr << "Trying to read starting from character " << start_read << ", which is beyond file length of " << file_length << " characters" << std::endl;
  }

  std::ifstream in(filename);
  std::string s="";

  if (in.is_open()) {
    if (start_read > 0) in.seekg(start_read);

    std::uintmax_t l=0;
    char c;
    while(in.good()) {
      in.get(c);
      s+=c;
      if (length_read > 0 && l > length_read) break;
      l++;
    }
   
    std::cout << std::endl;
    in.close();
  } else {
    std::cerr << "Could not open " << filename << std::endl;
    return s;
  }

  return s;
}

// Cherche une ou plusieurs sous-chaine numériques de la forme "n1-n2 n3-n4 ..." et les range dans le vector
// Le vector résultant doit donc avoir une taille paire et chaque paire définie une plage
bool csv::file::parse_range(std::string s, std::vector<std::uintmax_t>& parm) {
  parm.clear();
  trim(s);
  std::string sn="";

  for(auto c:s) {
    if (std::isdigit(c)) sn+=c;
    else if (c == '-' || (isspace(c) && parm.size() % 2)) {
      parm.push_back(stoi(sn));
      sn="";
    } else return false;
  }

  if (sn != "") parm.push_back(stoi(sn));
  // Si il n'y a pas de dernier chiffre alors on en déduit qu'il faut aller jusqu'au bout
  if (s.back() == '-') parm.push_back(rows.size());
  if (parm.size() % 2) return false;

  return true;
}

// Cherche une ou plusieurs sous-chaine de la forme "n1 n2 n3 ..." et les range dans le vector
bool csv::file::parse_list(std::string s, std::vector<std::uintmax_t>& parm) {
  parm.clear();
  trim(s);
  std::string sn="";

  for(auto c:s) {
    if (std::isdigit(c)) sn+=c;
    else if (isspace(c)) {
      parm.push_back(stoi(sn));
      sn="";
    } else return false;
  }

  if (sn != "") parm.push_back(stoi(sn));
  return true;
}

// List a range of rows that are passed from 1 to size but converted to 0 to size-1
void csv::file::list_row(std::uintmax_t r1, std::uintmax_t r2) {
  if (r1 < 1) {
    std::cout << "row start value (" << r1 << ") is below 1, starting from 1." << std::endl;
  }

  if (r1 > rows.size()) {
    std::cout << "row start value (" << r1 << ") is beyond last row index (" << rows.size()-1 << "), starting from last row index." << std::endl;
    r1=rows.size();
  }

  r1--;

  if (r2 > rows.size()) {
    std::cout << "row end value (" << r2 << ") is beyond last row index (" << rows.size()-1 << "), will stop on last index." << std::endl;
    r2=rows.size();
  }

  r2--;

  for(auto i=r1; i <= r2; i++) {
    if (loaded_in_mem) {
      std::cout << i+1 << ": " << in_mem.substr(rows[i].start, rows[i].end-rows[i].start) << std::endl;
    } else {
      std::cout << i+1 << ": " << read_substring_from_file(rows[i].start, rows[i].end-rows[i].start) << std::endl;
    }
  }
}

// List only one row
void csv::file::list_row(std::uintmax_t r) { list_row(r, r); }

// List the whole file by rows
void csv::file::list_row() {
  if (rows.size() > 1000) {
    std::string rep;
    std::cout << "You are about to display more than a thousant rows. Are you sure that you want to proceed ? (y/n)" << std::endl;
    std::cin >> rep;
    if (rep != "y") return;
  }

  list_row(1, rows.size());
}

std::uintmax_t csv::file::cell_count() {
  std::uintmax_t nc=0;
  for(auto row:rows) nc+=row.cells.size();
  return nc;
}

// Retourne une cellule en fonction de son index
bool csv::file::get_cell(uintmax_t ic, csv::cell& c) {
  std::uintmax_t nc=0;

  for(auto row:rows) {
    if (nc+row.cells.size() > ic) {
      c=row.cells[ic-nc];
      return true;
    }

    nc+=row.cells.size();
  }

  return false;
}

// Retourne un vecteur de cellule de l'index ic1 jusqu'à l'index ic2
bool csv::file::get_cells(std::uintmax_t ic1, std::uintmax_t ic2, std::vector<csv::cell>& c) {
  return false;

  std::uintmax_t nc=0;

  std::vector<csv::cell> cells;
  for(auto row:rows) {
  } 

  return false;
}

// List a range of cells that are passed from 1 to size but converted to 0 to size-1
void csv::file::list_cell(std::uintmax_t ic1, std::uintmax_t ic2) {
  std::cout << "List cell " << ic1;

  cell c1;
  get_cell(ic1, c1);

  if (ic1 == ic2) {
    std::cout << "Display cell " << ic1 << ", starting at char " << c1.start << " and ending at char " << c1.end;
  } else {
    if (ic2-ic1 > 100) {
      std::string rep;
      std::cout << "You are about to display more than a thousant rows. Are you sure that you want to proceed ? (y/n)" << std::endl;
      std::cin >> rep;
      if (rep != "y") return;
    }

    cell c2;
    get_cell(ic2, c2);
    std::cout << "Display from cell " << ic1 << " until cell " << ic2 << ", starting at char " << c1.start << " and ending at char " << c2.end;
  }

  std::cout << std::endl;
}

// List only one cell
void csv::file::list_cell(std::uintmax_t ic) {
  list_cell(ic, ic);
}

// List the whole file by cells
void csv::file::list_cell() {
  list_cell(0, cell_count()-1);
}

