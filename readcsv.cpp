
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

std::string trim(std::string& s) {
  s.erase(0, s.find_first_not_of(" \n\r\t"));
  s.erase(s.find_last_not_of(" \n\r\t")+1);
  return s;
}

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

// List range of rows
// Rows are passed from 1 to size but converted to 0 to size-1
void csv::file::list(std::uintmax_t r1, std::uintmax_t r2) {
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
void csv::file::list(std::uintmax_t r) { list(r, r); }

// List the whole file
void csv::file::list() {
  if (rows.size() > 1000) {
    std::string rep;
    std::cout << "There is more than a thousand rows to display. Are you sure that you want to proceed ? (y/n)" << std::endl;
    std::cin >> rep;
    if (rep != "y") return;
  }

  list(1, rows.size());
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
bool csv::file::read_in_memory() {
  loaded_in_mem=true;

  if (swallow_file(filename, in_mem)) {
    for(auto c:in_mem) parse_file(c);
    end_parse_file();
    return true;
  }

  return false;
}

// Process while reading char by char
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

void csv::file::reset() {
  curr_pos=0;
  curr_row.reset();
  curr_cell.reset();
  rows.clear();
  in_mem.clear();
  filename="";
}

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
Foolowing, at least the name of the file to browse must be provided.
Then 3 other optional arguments might be provided in boolean form (1/0, on/off, true/false):
  Read as cvs (on) or read as a text file (off), by default this argument is on.
  Stats line by line or not, by default in interactive mode this is off and in batch mode this is on.
  Read file in memory or not, by default this is on.
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

// Cherche une ou plusieurs sous-chaine de la forme "r1-r2 r3-r4 ..." qui définisse des plages numérique et les range par paires dans le vector
// Le vector résultant doit donc avoir une taille paire
bool csv::file::parse_row_range(std::string s, std::vector<std::uintmax_t>& parm) {
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

bool csv::file::parse_row_list(std::string s, std::vector<std::uintmax_t>& parm) {
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
// Cherche une ou plusieurs sous-chaine de la forme "r1-r2 r3-r4 ..." qui définisse des plages numérique et les range par paires dans le vector
// Le vector résultant doit donc avoir une taille paire
bool csv::file::parse_cell_range(std::string s, std::vector<std::uintmax_t>& parm) {
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

bool csv::file::parse_cell_list(std::string s, std::vector<std::uintmax_t>& parm) {
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


csv::file cf;
std::string cmd_parm="";

void row() {
  if (cmd_parm == "") cf.list();
  else {
    std::vector<std::uintmax_t> parm;
    if (cf.parse_row_range(cmd_parm, parm)) {
      for (size_t i=0; i < parm.size(); i += 2) cf.list(parm[i], parm[i+1]);
    } else if (cf.parse_row_list(cmd_parm, parm)) {
      for (size_t i=0; i < parm.size(); i++) cf.list(parm[i]);
    }
  }
}

void cell() {
  if (cmd_parm == "") cf.list();
  else {
    std::vector<std::uintmax_t> parm;
    if (cf.parse_cell_range(cmd_parm, parm)) {
      for (size_t i=0; i < parm.size(); i += 2) cf.list(parm[i], parm[i+1]);
    } else if (cf.parse_cell_list(cmd_parm, parm)) {
      for (size_t i=0; i < parm.size(); i++) cf.list(parm[i]);
    }
  }
}

void quit () {
  exit(0);
}

bool in_memory=true;

std::map<std::string, std::function<void()>> cmd_funcs = {
  { "help", help },
  { "stat", []() { cf.stat(string_to_bool(cmd_parm)); } },
  { "row", row },
  { "cell", cell },
  { "load", []() { cf.load(cmd_parm, in_memory); } },
  { "quit", quit },
  { "q", quit },
  { "x", quit },
  { "exit", quit }
};


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

  if (args.size() > 3) {
    in_memory=string_to_bool(args[3]);
  }

  if (args.size() > 0) {
    cf.setfmt(is_csv);
    delay();
    cf.load(args[0], in_memory);
    double dl1=delay(false);

    if (interactive) {
      std::string ln, prompt="> ";
      std::cout << prompt << std::flush;
      std::string cmd;
      std::string::size_type pos;
      while (std::getline(std::cin, ln)) {
        trim(ln);
        pos=ln.find_first_of(' ');
        if (pos == std::string::npos) {
          cmd=ln;
          cmd_parm="";
        } else {
          cmd=ln.substr(0, pos);
          cmd_parm=ln.substr(pos);
          trim(cmd_parm);
        }
        if (cmd_funcs.contains(cmd)) cmd_funcs.at(cmd)();
        else if (ln != "") std::cerr << "Unknown command ["<< cmd << ']' << std::endl;
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

