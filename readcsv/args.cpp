
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <functional>

#include "mygetopt.h"

#define OptHead std::cout<<"Shortoption "<<c<<",name "<<name<<",value ["<<val<<']'<<std::endl;

void info(char c, std::string name, std::string val) {
  OptHead
}

void row(char c, std::string name, std::string val) {
  OptHead
}

void cell(char c, std::string name, std::string val) {
  OptHead
}

void linecolumn(char c, std::string name, std::string val) {
  OptHead
}

void xy(char c, std::string name, std::string val) {
  OptHead
}

void find(char c, std::string name, std::string val) {
  OptHead
}

void transpose(char c, std::string name, std::string val) {
  OptHead
}

void read(char c, std::string name, std::string val) {
  OptHead
}

void write(char c, std::string name, std::string val) {
  OptHead
}

void set(char c, std::string name, std::string val) {
  OptHead
}

void fmt(char c, std::string name, std::string val) {
  OptHead
}

void quit(char c, std::string name, std::string val) {
  OptHead
}



std::vector<my_option> my_options = {
  { "info",      'i', opt_itrp, no_argument,       "display various informations on the current file.", info },
  { "line",      'l', opt_itrp, optional_argument, "display lines of the current file. Without parameters it will display all the lines, an interactive warning might appear if the file has more than a 1000 lines. You can also pass a range in the form 'r1-r2' or a list of line in the form 'r1 r2 r3 ...'. Rows indexes start to 1 and end to maximum number of lines.", row },
  { "cell",      'c', opt_itrp, optional_argument, "Behave like the 'line' command but for cells.", cell },
  { "lincol",   '\0', opt_itrp, required_argument, "display a cell by its line and column coordinate. By example lincol 0,0 <=> cell 0 and lincol 'lastline','lastcol' <=> cell 'lastcellindex'.", linecolumn },
  { "lc",       '\0', opt_itrp, required_argument, "shortcut for option lincol", linecolumn },
  { "xy",        'x', opt_itrp, required_argument, "display a cell by its column (x) and line (y) coordinate.", xy },
  { "find",      'f', opt_itrp, required_argument, "Display the line where the string is found (may be a regex).", find },
  { "transpose", 't', opt_itrp, no_argument,       "transpose the matrix represented by the csv.", transpose },
  { "tr",       '\0', opt_itrp, no_argument,       "shortcut for option transpose", transpose },
  { "read",      'r', opt_itrp, optional_argument, "if a filename is provided then load a new file and set it as the current file else update the current file if there is.", read },
  { "write",     'w', opt_itrp, no_argument,       "save the current file, if there is.", write },
  { "set",       's', opt_itrp, optional_argument, "without parameter list all the loaded files, else set the file whose number is passed as parameter as the current file.", set },
  { "var",       'v', opt_itrp, optional_argument, "without argument list the actual configuration variables used to parse the csv file else expect a line of the form 'var=value' to change one of them.", fmt },
  { "quit",      'q', opt_itrp, no_argument,       "leave.", quit },
  { "exit",      'x', opt_itrp, no_argument,       "leave.", quit },
  { "!",         '!', opt_itrp, required_argument, "execute a command in the current shell.",
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
  getopt_init(argc, argv, my_options, "Command line viewer and handler for csv or text file.", "0.1.0", "(c) Denis LALANNE. Provided as is. NO WARRANTY to the extent permitted by law.");

  if (argc < 2) {
    std::cerr << "Missing parameters. ";
    usage(std::cerr);
    exit(ENOTSUP);
  }

  exit(EXIT_SUCCESS);
}

