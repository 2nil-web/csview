
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <map>

#include "readcsv.h"
#include "util.h"
#include "interp.h"

std::string cmd_parm="";
bool in_memory=true;


std::vector<csv::file> csvs;
size_t curr_csv_idx=0;

void help() {
  std::cout << R"EOF(Available commands are :
help: display this message
info: display various informations on the current file
line: display lines of the current file. Without parameters it will display all the lines, an interactive warning might appear if the file has more than a 1000 lines. You can also pass a range in the form "r1-r2" or a list of line in the form "r1 r2 r3 ...". Rows indexes start to 1 and end to maximum number of lines.
cell: Behave like the 'line' command but for cells.
linecol: display a cell by its line and column coordinate. By example linecol 0,0 <=> cell 0 and linecol 'lastline','lastcol' <=> cell 'lastcellindex'.
find: Display the line where the string is found (may be a regex).
cfind: Display the cells where the string is found.
read : if a filename is provided then load a new file and set it as the current file else update the current file if there is.
transpose: transpose the matrix represented by the csv.
write: save the file.
set: without parameter list all the loaded files, else set the file whose number is passed as parameter as the current file.
!: execute a command in the current shell.
exit/quit/x/q: leave interactive mode.
)EOF";
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

std::string get_fmt(std::string name, char value) {
  name += '=';
  if (std::isprint(value)) name += "'"+std::string(1, value)+"'";
  name += " ("+std::to_string((int)value)+")";
  return name;
}

void info() {
  RETURN_IF_NO_LOADED_FILE;

  std::cout << csvs[curr_csv_idx].get_filename() << " is" << (csvs[curr_csv_idx].is_csv?" ":" not ") << "a csv file." << std::endl;
  std::cout << 
    get_fmt("cell_sep", csvs[curr_csv_idx].cell_separator) << ", " <<
    get_fmt("str_delim", csvs[curr_csv_idx].string_delimiter) << ", " <<
    get_fmt("eol", csvs[curr_csv_idx].end_of_line) << ", " <<
    get_fmt("esc", csvs[curr_csv_idx].escape) <<
  std::endl;

  csvs[curr_csv_idx].stat(string_to_bool(cmd_parm));
}

void fmt() {
}

void read() {
  if (cmd_parm == "") {
    RETURN_IF_NO_LOADED_FILE;
    csvs[curr_csv_idx].load(csvs[curr_csv_idx].get_filename(), in_memory); 
  } else {
    csv::file cf;

    if (cf.load(cmd_parm, in_memory)) {
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
  { "help",      help   },  { "h",    help   },
  { "info",      info   },  { "inf",  info   }, { "i", info }, { "stat", info },
  { "line",      row    },  { "l",    row    },
  { "cell",      cell   },  { "c",    cell   },
  { "lincol",    lincol },  { "lc",   lincol },
  { "xy",        xy     },
  { "find",      find   },  { "f",    find   },
  { "transpose", transp },  { "tr",   transp },
  { "read",      read   },  { "r",    read   },
  { "write",     write  },  { "w",    write  },
  { "set",       set    },  { "s",    set    },
  { "fmt",       fmt    },
  { "quit",      quit   },  { "exit", quit   }, { "q", quit }, { "x", quit },
  { "!", []() { std::system(cmd_parm.c_str()); } },
};

void inter(csv::file _cf, bool _in_memory) {
  in_memory=_in_memory;

  if (_cf.get_filename() != "") csvs.push_back(_cf);
  std::string ln, prompt="> ";
  std::string cmd;
  std::string::size_type pos;

#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
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

