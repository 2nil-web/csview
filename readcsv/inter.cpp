
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <map>

#include "readcsv.h"
#include "util.h"
#include "inter.h"

csv::file cf;
std::string cmd_parm="";
bool in_memory=true;


std::vector<csv::file> csvs;
size_t curr_csv_idx=0;

void help() {
  std::cout << R"EOF(Available commands are :
help: display this message
inf: display various informations on the current file
row: display rows of the current file. Without parameters it will display all the rows, an interactive warning might appear if the file has more than a 1000 lines. You can also pass a range in the form "r1-r2" or a list of row in the form "r1 r2 r3 ...".
cell: displays cells of the current file. Parameters are mandatory. They are of the form r1,c1. They might be provided in range form "r1,c1-r2,c2" or in list form "r1,c1 r2,c2 r3,c3 ...".
reload: reload the current file. This might be useful if the file has been modified.
load filename: load a new file and set it as the current file.
set n: without parameter list all the loaded files, else set the file whose number is passed as parameter as the current file.
!: execute a command in the current shell.
exit/quit/x/q: leave interactive mode.
)EOF";
}

void row() {
  if (cmd_parm == "") csvs[curr_csv_idx].list();
  else {
    std::vector<std::uintmax_t> parm;
    if (csvs[curr_csv_idx].parse_row_range(cmd_parm, parm)) {
      for (size_t i=0; i < parm.size(); i += 2) csvs[curr_csv_idx].list(parm[i], parm[i+1]);
    } else if (csvs[curr_csv_idx].parse_row_list(cmd_parm, parm)) {
      for (size_t i=0; i < parm.size(); i++) csvs[curr_csv_idx].list(parm[i]);
    }
  }
}

void cell() {
  if (cmd_parm == "") csvs[curr_csv_idx].list();
  else {
    std::vector<std::uintmax_t> parm;
    if (csvs[curr_csv_idx].parse_cell_range(cmd_parm, parm)) {
      for (size_t i=0; i < parm.size(); i += 2) csvs[curr_csv_idx].list(parm[i], parm[i+1]);
    } else if (csvs[curr_csv_idx].parse_cell_list(cmd_parm, parm)) {
      for (size_t i=0; i < parm.size(); i++) csvs[curr_csv_idx].list(parm[i]);
    }
  }
}

void set() {
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

void inf() {
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


void quit () {
  exit(0);
}


std::map<std::string, std::function<void()>> cmd_funcs = {
  { "help", help },
  { "inf", inf },
  { "row", row },
  { "cell", cell },
  { "load", []() {
       csv::file cf;
       cf.load(cmd_parm, in_memory);
       csvs.push_back(cf);
       curr_csv_idx=csvs.size()-1;
     }
  },
  { "reload", []()
    {
      csvs[curr_csv_idx].load(cmd_parm, in_memory);
    }
  },
  { "set", set },
  { "fmt", fmt },
  { "!", []() { std::system(cmd_parm.c_str()); } },
  { "quit", quit },
  { "q", quit },
  { "x", quit },
  { "exit", quit }
};

void inter(csv::file _cf, bool _in_memory) {
  in_memory=_in_memory;

  csvs.push_back(_cf);
  std::string ln, prompt="> ";
  std::cout << prompt << std::flush;
  std::string cmd;
  std::string::size_type pos;

#ifdef __WIN32
  SetConsoleOutputCP(CP_UTF8);
#endif

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
    else if (ln != "") std::cerr << "Unknown command ["<< cmd << ']' << std::endl;
    std::cout << prompt << std::flush;
  }
}

