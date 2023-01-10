
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

void csv::file::list(std::uintmax_t r1, std::uintmax_t r2) {
  if (r1 > rows.size()) {
    std::cout << "row start value (" << r1 << ") is beyond last row value (" << rows.size() << ")." << std::endl;
    return;
  }

  if (r2 > rows.size()) {
    std::cout << "row end value (" << r2 << ") is beyond last row value (" << rows.size() << "), will stop on last row." << std::endl;
    r2=rows.size();
  }

  for(auto i=r1; i <= r2; i++) {
    if (loaded_in_mem) {
      std::cout << i << ": " << in_mem.substr(rows[i].start, rows[i].end-rows[i].start) << std::endl;
    } else {
    }
  }
}

void csv::file::list(std::uintmax_t r) { list(r, r+1); }

void csv::file::list() {
  if (rows.size() > 1000) {
    std::string rep;
    std::cout << "There is more than a thousand rows to display. Are you sure that you want to proceed ? (y/n)" << std::endl;
    std::cin >> rep;
    if (rep != "y") return;
  }

  uintmax_t i=0;
  for(auto row:rows) {
    //std::cout << row.start << ',' << row.end << std::endl;
    if (loaded_in_mem) {
      std::cout << i++ << ": " << in_mem.substr(row.start, row.end-row.start) << std::endl;
    } else {
    }
  }
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

void csv::file::parse(char c) {
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


void csv::file::end_parse() {
  if (curr_cell.start < curr_pos && curr_row.cells.size () > 0) {
    curr_cell.end=curr_row.end=curr_pos;
    curr_row.cells.push_back(curr_cell);
    rows.push_back(curr_row);
    curr_row.reset();
  }
}

bool swallow_file(std::string &file_path, std::string &s) {
  std::ifstream in(file_path, std::ios::in | std::ios::binary);

  if (in) {
    in.seekg(0, std::ios::end);
    s.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&s[0], s.size());
    in.close();
    return true;
  }

  return false;
}

// Put whole file in memory then process
bool csv::file::read_in_memory(std::string &file_path) {
  loaded_in_mem=true;

  if (swallow_file(file_path, in_mem)) {
    for(auto c:in_mem) parse(c);
    end_parse();
    return true;
  }

  return false;
}

// Process while reading char by char
bool csv::file::read_from_file(std::string &file_path) {
  loaded_in_mem=false;

  std::ifstream in(file_path);

  if (in.is_open()) {
    //std::uintmax_t file_length=std::filesystem::file_size(std::filesystem::path{fname});

    while(in.good()) {
      in.get(c);
      parse(c);
    }

    end_parse();
  }

  if (!in.eof() && in.fail()) {
    std::cout << "error reading " << file_path << std::endl;
    return false;
  }

  in.close();
  return true;
}

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

void usage(std::string progpath, std::ostream& out = std::cout) {
    out << "Usage: " << std::filesystem::path(progpath).stem().string() << std::endl;
    out <<
        R"EOF(CSV or text file viewer at command line
Optionnal parameters :        
  -h : display this help.
  -i : batch (non interactive) mode is the default, this paraemter give access to the interactive mode. Once done, type the "help" command for more information.
At least the name of the file to browse must be provided.
Then 3 other optional arguments might be provided in boolean form (1/0, on/off, true/false):
  Read as cvs (-on) or read as a text file (off), by default this argument is on.
  Stats line by line or notff in interactive mode and on in batch mode.
  Read file in memory or not.
In interactive mode they are off by default. In non-interactive mode they are on by default.
)EOF";
  //exit(EXIT_SUCCESS);
}


void help() {
  std::cout << R"EOF(Available commands are :
help: display this message
stat: display various statistics on the current file
row: display rows of the current file. Without parameters it will display all the rows, an interactive warning might appear if the file has more than a 1000 lines. You can also pass a range in the form "r1-r2" or a list of row in the form "r1 r2 r3 ...".
cell: displays cells of the current file. Parameters are mandatory. They are of the form r1,c1. They might be provided in range form "r1,c1-r2,c2" or in list form "r1,c1 r2,c2 r3,c3 ...".
reload: reload the current file. This might be useful if the file has been modified.
load filename: load a new file and set it as the current file.
ls: list all the currently loaded file.
set n: set the file numbered n as the current file.
exit: leave interactive mode.
)EOF";
}

std::string trim(std::string& s) {
  s.erase(0, s.find_first_not_of(" \n\r\t"));
  s.erase(s.find_last_not_of(" \n\r\t")+1);
  return s;
}

// Cherche une ou plusieurs sous-chaine de la forme "r1-r2 r3-r4 ..." qui définisse des plages numérique et les range par paires dans le vector
// Le vector résultant doit donc avoir une taille paire
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

  //std::cout << "from rng" << std::endl;
  return true;
}

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
/*
  for (auto p:parm) std::cout << p << ',';
  std::cout << std::endl;
  std::cout << "from lst" << std::endl;*/
  return true;
}


void row(std::string ln, csv::file& cf) {
  ln.erase(ln.begin(), ln.begin()+3);
  trim(ln);
  if (ln == "") cf.list();
  else {
    std::vector<std::uintmax_t> parm;
    if (cf.parse_range(ln, parm)) {
      for (size_t i=0; i < parm.size(); i += 2) cf.list(parm[i], parm[i+1]);
    } else if (cf.parse_list(ln, parm)) {
      for (size_t i=0; i < parm.size(); i++) cf.list(parm[i]);
    }
  }
}

int main(int argc, char *argv[]) {
  args=std::vector<std::string>(argv, argv + argc);
  prog_basename=std::filesystem::path(args[0]).stem().string();
  args.erase(args.begin());

  if (args.size() == 0) {
    usage(prog_basename, std::cerr);
    return 1;
  }

  if (args[0] == "-h") {
    usage(prog_basename);
    return 0;
  }

  bool interactive=false;

  if (args[0] == "-i") {
    interactive=true;
    args.erase(args.begin());
  }

  bool is_csv=true;
  if (args.size() > 1) is_csv=string_to_bool(args[1]);

  bool line_by_line;
  if (interactive) line_by_line=false;
  else line_by_line=true;

  if (args.size() > 2) {
    line_by_line=string_to_bool(args[2]);
  }

  bool read_in_memory=true;
  if (args.size() > 3) {
    read_in_memory=string_to_bool(args[3]);
  }

  if (args.size() > 0) {
    csv::file cf;
    cf.setfmt(is_csv);

    delay();
    if (read_in_memory) {
      cf.read_in_memory(args[0]);
      std::cout << "File " << args[0] << " loaded in memory." << std::endl;
    } else {
      cf.read_from_file(args[0]);
      std::cout << "File " << args[0] << " parsed." << std::endl;
    }
    double dl1=delay(false);

    if (interactive) {
      std::string ln, prompt="> ";
      std::cout << prompt << std::flush;
      while (std::getline(std::cin, ln)) {
        trim(ln);

        if (ln == "help") help();
        else if (ln == "q" || ln == "x" || ln == "exit" || ln == "quit") break;
        else if (ln == "stat") cf.stat(false);
        else if (ln.starts_with("row")) row(ln, cf);
        else if (ln != "") std::cerr << "Uknown command ["<< ln << ']' << std::endl;

        std::cout << prompt << std::flush;
      }
    } else {
      delay();
      cf.stat(line_by_line);
      double dl2=delay(false);
      std::cout << "Delay to read file " << args[1] << ' ' << dl1 << " seconds." << std::endl;
      std::cout << "Delay to compute stats " << dl2 << " seconds." << std::endl;
      std::cout << "Total mem " << getTotalSystemMemory() << std::endl;
    }
  }

  return 0;
}

