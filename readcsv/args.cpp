
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <functional>

#include "mygetopt.h"


#define OptHead std::cout<<"Short command "<<c<<", command: "<<name<<", parameter ["<<param<<']'<<std::endl

void info(char c, std::string name, std::string param) {
  OptHead;
}

void row(char c, std::string name, std::string param) {
  OptHead;
}

void cell(char c, std::string name, std::string param) {
  OptHead;
}

void linecolumn(char c, std::string name, std::string param) {
  OptHead;
}

void xy(char c, std::string name, std::string param) {
  OptHead;
}

void find(char c, std::string name, std::string param) {
  OptHead;
}

void transpose(char c, std::string name, std::string param) {
  OptHead;
}

void read(char c, std::string name, std::string param) {
  OptHead;
}

void write(char c, std::string name, std::string param) {
  OptHead;
}

void set(char c, std::string name, std::string param) {
  OptHead;
}

void fmt(char c, std::string name, std::string param) {
  OptHead;
}

void quit(char, std::string, std::string) {
  extern bool no_quit;
  no_quit=false;
}

std::vector<my_option> my_options = {
  { "info",      'n', opt_itr, no_argument,       "display various informations on the current file.", info },
  { "line",      'l', opt_itr, optional_argument, "display lines of the current file. Without parameters it will display all the lines, an interactive warning might appear if the file has more than a 1000 lines. You can also pass a range in the form 'r1-r2' or a list of line in the form 'r1 r2 r3 ...'. Rows indexes start to 1 and end to maximum number of lines.", row },
  { "cell",      'c', opt_itr, optional_argument, "behave like the 'line' command but for cells.", cell },
  { "lincol",   '\0', opt_itr, required_argument, "display a cell by its line and column coordinate. By example lincol 0,0 <=> cell 0 and lincol 'lastline','lastcol' <=> cell 'lastcellindex'.", linecolumn },
  { "lc",       '\0', opt_itr, required_argument, "shortcut for option lincol", linecolumn },
  { "xy",        'x', opt_itr, required_argument, "display a cell by its column (x) and line (y) coordinate.", xy },
  { "find",      'f', opt_itr, required_argument, "display the line where the string is found (may be a regex).", find },
  { "transpose", 't', opt_itr, no_argument,       "transpose the matrix represented by the csv.", transpose },
  { "tr",       '\0', opt_itr, no_argument,       "shortcut for option transpose", transpose },
  { "read",      'r', opt_itr, optional_argument, "if a filename is provided then load a new file and set it as the current file else update the current file if there is.", read },
  { "write",     'w', opt_itr, no_argument,       "save the current file, if there is.", write },
  { "set",       's', opt_itr, optional_argument, "without parameter list all the loaded files, else set the file whose number is passed as parameter as the current file.", set },
  { "var",       'v', opt_itr, optional_argument, "without argument list the actual configuration variables used to parse the csv file else expect a line of the form 'var=value' to change one of them.", fmt },
  { "quit",      'q', itr_only,     no_argument,       "leave.", quit },
  { "exit",      'x', itr_only,     no_argument,       "leave.", quit },
  { "!",         '!', itr_only,     required_argument, "execute a command in the current shell.",
    [] (char , std::string , std::string val) -> void
    {
      std::system(val.c_str());
    }
  },

  { "", '\0', 0, 0, "\nAdditionnal help message.", NULL },
  { "", '\0', 0, 0, "", NULL },

  { "", '\0', 0, 0, "\n2nd Additional message.", NULL }
};


int main(int argc, char **argv, char **) {
  getopt_init(argc, argv, my_options, "Command line viewer and handler for csv or text file.", "0.1.0", "(c) Denis LALANNE. Provided as is. NO WARRANTY of any kind.");

//  for(auto myopt:my_options) {
//    std::cout << "name " << myopt.name << ", val " << myopt.val << ", oi_mode " << myopt.oi_mode << ", has_arg " << myopt.has_arg << ", help " << myopt.help << std::endl;
//  }

  if (!interp() && argc < 2) {
    std::cerr << "Missing parameters. ";
    usage(std::cerr);
    exit(ENOTSUP);
  }


  exit(EXIT_SUCCESS);
}

