
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <functional>

#include "mygetopt.h"

void info(char, std::string , std::string ) { }
void row(char, std::string , std::string ) { }
void cell(char, std::string , std::string ) { }
void linecolumn(char, std::string , std::string ) { }
void xy(char, std::string , std::string ) { }
void find(char, std::string , std::string ) { }
void transpose(char, std::string , std::string ) { }
void read(char, std::string , std::string ) { }
void write(char, std::string , std::string ) { }
void set(char, std::string , std::string ) { }
void fmt(char, std::string , std::string ) { }
void quit(char, std::string , std::string ) { }


std::vector<my_option> my_options = {
  { "display various informations on the current file.", "info", 'i', no_argument, info },
  { "display lines of the current file. Without parameters it will display all the lines, an interactive warning might appear if the file has more than a 1000 lines. You can also pass a range in the form 'r1-r2' or a list of line in the form 'r1 r2 r3 ...'. Rows indexes start to 1 and end to maximum number of lines.", "line", 'l', optional_argument, row },
  { "Behave like the 'line' command but for cells.", "cell", 'c', optional_argument, cell },
  { "display a cell by its line and column coordinate. By example lincol 0,0 <=> cell 0 and lincol 'lastline','lastcol' <=> cell 'lastcellindex'.", "lincol", '\0', required_argument, linecolumn },
  { "shortcut for option lincol", "lc", '\0', required_argument, linecolumn },
  { "display a cell by its column (x) and line (y) coordinate.", "xy", 'x', required_argument, xy },
  { "Display the line where the string is found (may be a regex).", "find", 'f', required_argument, find },
  { "transpose the matrix represented by the csv.", "transpose", 't', no_argument, transpose },
  { "shortcut for option transpose", "tr", '\0', no_argument, transpose },
  { "if a filename is provided then load a new file and set it as the current file else update the current file if there is.", "read", 'r', optional_argument, read },
  { "save the current file, if there is.", "write", 'w', no_argument, write },
  { "without parameter list all the loaded files, else set the file whose number is passed as parameter as the current file.", "set", 's', optional_argument, set },
  { "without argument list the actual configuration variables used to parse the csv file else expect a line of the form 'var=value' to change one of them.", "var",  'v', optional_argument, fmt },
  { "leave.", "quit", 'q', no_argument, quit },
  { "leave.", "exit", 'x', no_argument, quit },
  { "execute a command in the current shell.", "!", '!', required_argument,
    [] (char , std::string , std::string val) -> void
    {
      std::system(val.c_str());
    }
  },

  { "\nAdditionnal help message.", "", '\0', 0, NULL },
  { "", "", '\0', 0, NULL }, // Empty line

  { "\n2nd Additional message.", "", '\0', 0, NULL }
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

