
#include "util.h"
#include "readcsv.h"

csv::file cf;
std::string cmd_parm="";
bool in_memory=true;

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

void inter(csv::file& _cf, bool _in_memory) {
  in_memory=_in_memory;
  cf=_cf;
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
}

