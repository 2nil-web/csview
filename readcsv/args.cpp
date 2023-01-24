
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <functional>

#include "mygetopt.h"

void info(char c, std::string , std::string val) { }
void row(char c, std::string , std::string val) { }
void cell(char c, std::string , std::string val) { }
void linecolumn(char c, std::string , std::string val) { }
void xy(char c, std::string , std::string val) { }
void find(char c, std::string , std::string val) { }
void transpose(char c, std::string , std::string val) { }
void read(char c, std::string , std::string val) { }
void write(char c, std::string , std::string val) { }
void set(char c, std::string , std::string val) { }
void fmt(char c, std::string , std::string val) { }
void quit(char c, std::string , std::string val) { }


std::vector<my_option> my_options = {
  { "", "info", 'i', no_argument, info },
  { "", "line", 'l', optional_argument, row },
  { "", "cell", 'c', optional_argument, cell },
  { "", "lincol", '\0', required_argument, linecolumn },
  { "", "lc", '\0', required_argument, linecolumn },
  { "", "xy", 'x', required_argument, xy },
  { "find", 'f', required_argument, find   },
  { "", "transpose", 't', no_argument, transpose },
  { "", "tr", '\0', no_argument, transpose },
  { "", "read", 'r', optional_argument, read },
  { "", "write", 'w', no_argument, write },
  { "", "set", 's', optional_argument, set },
  { "", "var",  'v', optional_argument, fmt },
  { "", "quit", 'q', no_argument, quit },
  { "", "exit", 'x', no_argument, quit },
  { "", "!", '!', required_argument,
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
  getopt_init(argc, argv, my_options, "Set environment variables and run commands.", "1.1.0", "(c) Denis LALANNE. Provided as is. NO WARRANTY to the extent permitted by law.");

  if (argc < 2) {
    std::cerr << "Missing parameters. ";
    usage(std::cerr);
    exit(ENOTSUP);
  }

  exit(EXIT_SUCCESS);
}

