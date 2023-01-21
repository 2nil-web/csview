
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <map>

#include "readcsv.h"
#include "util.h"
#include "interp.h"

std::string cmd_parm="";
bool g_in_memory=true;


std::vector<csv::file> csvs;
size_t curr_csv_idx=0;

std::string parse_underline(std::string s) {
  std::string ret="";
  bool close_ul=false;

  for(auto c:s) {
    if (c == '_') {
      ret+="\033[4m\033[92m";
      close_ul=true;
    } else {
      ret+=c;

      if (close_ul) {
        ret+="\033[0m";
        close_ul=false;
      }
    }
  }

  return ret;
}

void help() {
  std::cout << parse_underline(R"EOF(Available commands are :
_help: display this message
_info: display various informations on the current file
_line: display lines of the current file. Without parameters it will display all the lines, an interactive warning might appear if the file has more than a 1000 lines. You can also pass a range in the form "r1-r2" or a list of line in the form "r1 r2 r3 ...". Rows indexes start to 1 and end to maximum number of lines.
_cell: Behave like the 'line' command but for cells.
_lin_col: display a cell by its line and column coordinate. By example lincol 0,0 <=> cell 0 and lincol 'lastline','lastcol' <=> cell 'lastcellindex'.
_find: Display the line where the string is found (may be a regex).
_cfind: Display the cells where the string is found.
_read : if a filename is provided then load a new file and set it as the current file else update the current file if there is.
_t_ranspose: transpose the matrix represented by the csv.
_write: save the file.
_set: without parameter list all the loaded files, else set the file whose number is passed as parameter as the current file.
_var: without argument list the actual configuration variables used to parse the csv file else expect a line of the form 'var=value' to change one of them.
!: execute a command in the current shell.
e_xit/_quit: leave interactive mode.
)EOF");
}

#define RETURN_IF_NO_LOADED_FILE  if (csvs.size() == 0) { std::cout << "No file loaded" << std::endl; return; }

void row() {
  RETURN_IF_NO_LOADED_FILE;

  if (cmd_parm == "") csvs[curr_csv_idx].list_row();
  else {
    std::vector<std::uintmax_t> parm;
    if (csvs[curr_csv_idx].parse_range(cmd_parm, parm)) {
      for (size_t i=0; i < parm.size(); i += 2) csvs[curr_csv_idx].list_row(parm[i], parm[i+1]);
    } else if (csvs[curr_csv_idx].parse_list(cmd_parm, parm)) {
      for (size_t i=0; i < parm.size(); i++) csvs[curr_csv_idx].list_row(parm[i]);
    }
  }
}

void cell() {
  RETURN_IF_NO_LOADED_FILE;

  if (cmd_parm == "") csvs[curr_csv_idx].list_cell();
  else {
    std::vector<std::uintmax_t> parm;
    if (csvs[curr_csv_idx].parse_range(cmd_parm, parm)) {
      for (size_t i=0; i < parm.size(); i += 2) csvs[curr_csv_idx].list_cell(parm[i], parm[i+1]);
    } else if (csvs[curr_csv_idx].parse_list(cmd_parm, parm)) {
      for (size_t i=0; i < parm.size(); i++) csvs[curr_csv_idx].list_cell(parm[i]);
    }
  }
}

void set() {
  RETURN_IF_NO_LOADED_FILE;

  if (cmd_parm == "") {
    for(size_t i=0; i < csvs.size(); i++) {
      if (i == curr_csv_idx) std::cout << '*';
      else std::cout << ' ';
      std::cout << ' ' << i+1 << ':' << csvs[i].get_filename() << std::endl;
    }
  } else {
    size_t n=std::stoi(cmd_parm);
    if (n > 0 && n <= csvs.size()) {
      curr_csv_idx=n-1;
      std::cout << n << ':' << csvs[curr_csv_idx].get_filename() << std::endl;
    }
  }

}

std::string get_str_var(std::string name, char value) {
  name += '=';
  if (std::isprint(value)) name += "'"+std::string(1, value)+"'";
  name += " ("+std::to_string((int)value)+")";
  return name;
}

std::string get_bool_var(std::string name, char value) {
  return "mem="+std::string(value ? "true" : "false");
}

std::string get_fmts(csv::file cf) {
  return
    "in_mem"+std::string(cf.is_in_mem() ? "true" : "false") + ", " +
    get_str_var("cell_sep", cf.cell_separator) + ", " +
    get_str_var("str_delim", cf.string_delimiter) + ", " +
    get_str_var("eol", cf.end_of_line) + ", " +
    get_str_var("esc", cf.escape);
}


void info() {
  RETURN_IF_NO_LOADED_FILE;

  std::cout << csvs[curr_csv_idx].get_filename() << " is" << (csvs[curr_csv_idx].is_csv?" ":" not ") << "a csv file." << std::endl;
  std::cout << get_fmts(csvs[curr_csv_idx]) << std::endl;

  csvs[curr_csv_idx].stat(string_to_bool(cmd_parm));
}

// Expect a line of the form var=value

bool g_in_mem=true, g_csv=true;
char g_sep=';', g_dlm='\0', g_eol='\n', g_esc='\\';
void fmt() {
  if (cmd_parm == "") {
    std::cout <<
      get_bool_var("mem=", g_in_memory) + ", " +
      get_bool_var("csv=", g_csv) + ", " +
      get_str_var("sep", g_sep) + ", " +
      get_str_var("dlm", g_dlm) + ", " +
      get_str_var("eol", g_eol) + ", " +
      get_str_var("esc", g_esc);
    std::cout << std::endl << "Variable 'mem' and 'csv' expect a boolean value (true/false, on/off, 1/0) and the other a character or an ascii code value (0-255)." << std::endl;
  } else {
    std::string var, val;
    std::vector<std::string> v=split(cmd_parm, '=');

    trim(v[0]);
    trim(v[1]);
         if (v[0] == "sep") g_sep=string_to_ascii(v[1]);
    else if (v[0] == "dlm") g_dlm=string_to_ascii(v[1]);
    else if (v[0] == "eol") g_eol=string_to_ascii(v[1]);
    else if (v[0] == "esc") g_esc=string_to_ascii(v[1]);
    else if (v[0] == "mem") g_in_memory=string_to_bool(v[1]);
    else if (v[0] == "csv") g_csv=string_to_bool(v[1]);
    else std::cout << "Unknown variable " << v[0] << std::endl;
  }
}

void read() {
  if (cmd_parm == "") {
    RETURN_IF_NO_LOADED_FILE;
    csvs[curr_csv_idx].load(csvs[curr_csv_idx].get_filename(), g_in_memory); 
  } else {
    csv::file cf;

    cf.is_csv=g_csv;
    cf.cell_separator=g_sep;
    cf.string_delimiter=g_dlm;
    cf.end_of_line=g_eol;
    cf.escape=g_esc;

    if (cf.load(cmd_parm, g_in_memory)) {
      csvs.push_back(cf);
      curr_csv_idx=csvs.size()-1;
    }
  }
}

void quit () {
  exit(0);
}

void find () {
  RETURN_IF_NO_LOADED_FILE;
  csvs[curr_csv_idx].find(cmd_parm);
}

void lincol () {
  RETURN_IF_NO_LOADED_FILE;

  uintmax_t r, c;
  if (csvs[curr_csv_idx].parse_coord(cmd_parm, r, c)) {
    std::string s;
    if (csvs[curr_csv_idx].get_cell_by_rc(r, c, s)) {
      std::cout << r << ',' << c << ": " << s << std::endl;
    } else {
      std::cout << "Bad coordinate " << cmd_parm << ',' << c << std::endl;
    }
  }
}


void xy () {
  RETURN_IF_NO_LOADED_FILE;

  uintmax_t r, c;
  if (csvs[curr_csv_idx].parse_coord(cmd_parm, r, c)) {
    std::string s;
    if (csvs[curr_csv_idx].get_cell_by_rc(c, r, s)) {
      std::cout << c << ',' << r << ": " << s << std::endl;
    } else {
      std::cout << "Bad coordinate " << cmd_parm << ',' << c << std::endl;
    }
  }
}

void transp () {
  RETURN_IF_NO_LOADED_FILE;

  csvs[curr_csv_idx].transpose();
}

void write () {
  RETURN_IF_NO_LOADED_FILE;

  csvs[curr_csv_idx].save_from_memory();
}


std::map<std::string, std::function<void()>> cmd_funcs = {
  { "help",      help   }, { "h",    help   },
  { "info",      info   }, { "inf",  info   }, { "i", info }, { "stat", info },
  { "line",      row    }, { "l",    row    },
  { "cell",      cell   }, { "c",    cell   },
  { "lincol",    lincol }, { "lc",   lincol },
  { "xy",        xy     },
  { "find",      find   }, { "f",    find   },
  { "transpose", transp }, { "tr",   transp },
  { "read",      read   }, { "r",    read   },
  { "write",     write  }, { "w",    write  },
  { "set",       set    }, { "s",    set    },
  { "var",       fmt    }, { "v",    fmt    },
  { "quit",      quit   }, { "exit", quit   }, { "q", quit }, { "x", quit },
  { "!", []() { std::system(cmd_parm.c_str()); } },
};

#ifdef _WIN32
bool EnableVTMode()
{
  // Set output mode to handle virtual terminal sequences
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE) return false;
  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode)) return false;
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  if (!SetConsoleMode(hOut, dwMode)) return false;
  return true;
}
#endif


void inter(csv::file _cf, bool _in_memory) {
  g_in_memory=_in_memory;

  if (_cf.get_filename() != "") csvs.push_back(_cf);
  std::string ln, prompt="> ";
  std::string cmd;
  std::string::size_type pos;

#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  EnableVTMode();
//  SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_PROCESSED_OUTPUT  | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN | ENABLE_VIRTUAL_TERMINAL_INPUT);
#endif
  std::cout << prompt << std::flush;

  while (std::getline(std::cin, ln)) {
    trim(ln);

    if (ln[0] == '!') {
      cmd="!";
      cmd_parm=ln.substr(1);
      trim(cmd_parm);
    } else {
      pos=ln.find_first_of(' ');

      if (pos == std::string::npos) {
        cmd=ln;
        cmd_parm="";
      } else {
        cmd=ln.substr(0, pos);
        cmd_parm=ln.substr(pos);
        trim(cmd_parm);
      }
    }

    if (cmd_funcs.contains(cmd)) cmd_funcs.at(cmd)();
    else if (ln != "" && any_of_ctype(ln, isgraph)) {
      std::cout << "Unknown command ["<< cmd << ']' << std::endl;
    }

    std::cout << prompt << std::flush;
  }
}

