
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <functional>

#include "util.h"
#include "runopt.h"
#include "readcsv.h"


#define RETURN_IF_NO_LOADED_FILE  if (csvs.size() == 0) { std::cout << "No file loaded" << std::endl; return; }

bool g_in_memory=true;
std::vector<csv::file> csvs;
size_t curr_csv_idx=0;
bool g_in_mem=true, g_csv=true;
char g_sep=';', g_dlm='\0', g_eol='\n', g_esc='\\';

std::string get_str_var(std::string name, char value) {
  name += '=';
  if (std::isprint(value)) name += "'"+std::string(1, value)+"'";
  name += " ("+std::to_string((int)value)+")";
  return name;
}

std::string get_bool_var(std::string name, char value) {
  return name+"="+std::string(value ? "true" : "false");
}

std::string get_fmts(csv::file cf) {
  return
    "in_mem"+std::string(cf.is_in_mem() ? "true" : "false") + ", " +
    get_str_var("cell_sep", cf.cell_separator) + ", " +
    get_str_var("str_delim", cf.string_delimiter) + ", " +
    get_str_var("eol", cf.end_of_line) + ", " +
    get_str_var("esc", cf.escape);
}

void info(char, std::string, std::string param) {
  RETURN_IF_NO_LOADED_FILE;

  std::cout << csvs[curr_csv_idx].get_filename() << " is" << (csvs[curr_csv_idx].is_csv?" ":" not ") << "a csv file." << std::endl;
  std::cout << get_fmts(csvs[curr_csv_idx]) << std::endl;

  csvs[curr_csv_idx].stat(string_to_bool(param));
}

void row(char, std::string, std::string param) {
  RETURN_IF_NO_LOADED_FILE;
  if (param == "") csvs[curr_csv_idx].list_row();
  else {
    std::vector<std::uintmax_t> parm;
    if (csvs[curr_csv_idx].parse_range(param, parm)) {
      for (size_t i=0; i < parm.size(); i += 2) csvs[curr_csv_idx].list_row(parm[i], parm[i+1]);
    } else if (csvs[curr_csv_idx].parse_list(param, parm)) {
      for (size_t i=0; i < parm.size(); i++) csvs[curr_csv_idx].list_row(parm[i]);
    }
  }
}

void cell(char, std::string, std::string param) {
  RETURN_IF_NO_LOADED_FILE;
  if (param == "") csvs[curr_csv_idx].list_cell();
  else {
    std::vector<std::uintmax_t> parm;
    if (csvs[curr_csv_idx].parse_range(param, parm)) {
      for (size_t i=0; i < parm.size(); i += 2) csvs[curr_csv_idx].list_cell(parm[i], parm[i+1]);
    } else if (csvs[curr_csv_idx].parse_list(param, parm)) {
      for (size_t i=0; i < parm.size(); i++) csvs[curr_csv_idx].list_cell(parm[i]);
    }
  }
}

void linecolumn(char, std::string, std::string param) {
  RETURN_IF_NO_LOADED_FILE;
  uintmax_t r, c;
  if (csvs[curr_csv_idx].parse_coord(param, r, c)) {
    std::string s;
    if (csvs[curr_csv_idx].get_cell_by_rc(r, c, s)) {
      std::cout << r << ',' << c << ": " << s << std::endl;
    } else {
      std::cout << "Bad coordinate " << param << ',' << c << std::endl;
    }
  }
}

void xy(char, std::string, std::string param) {
  RETURN_IF_NO_LOADED_FILE;

  uintmax_t r, c;
  if (csvs[curr_csv_idx].parse_coord(param, r, c)) {
    std::string s;
    if (csvs[curr_csv_idx].get_cell_by_rc(c, r, s)) {
      std::cout << c << ',' << r << ": " << s << std::endl;
    } else {
      std::cout << "Bad coordinate " << param << ',' << c << std::endl;
    }
  }
}

void find(char, std::string, std::string param) {
  RETURN_IF_NO_LOADED_FILE;
  csvs[curr_csv_idx].find(param);
}

void transpose(char, std::string, std::string) {
  RETURN_IF_NO_LOADED_FILE;
  csvs[curr_csv_idx].transpose();
}

void readtab(char, std::string, std::string param) {
  if (param == "") {
    RETURN_IF_NO_LOADED_FILE;
    csvs[curr_csv_idx].load(csvs[curr_csv_idx].get_filename(), g_in_memory); 
  } else {
    csv::file cf;

    cf.is_csv=g_csv;
    cf.cell_separator=g_sep;
    cf.string_delimiter=g_dlm;
    cf.end_of_line=g_eol;
    cf.escape=g_esc;

    if (cf.load(param, g_in_memory)) {
      csvs.push_back(cf);
      curr_csv_idx=csvs.size()-1;
    }
  }
}

void writetab(char, std::string, std::string) {
  RETURN_IF_NO_LOADED_FILE;
  csvs[curr_csv_idx].save_from_memory();
}

void set(char, std::string, std::string param) {
  RETURN_IF_NO_LOADED_FILE;
  if (param == "") {
    for(size_t i=0; i < csvs.size(); i++) {
      if (i == curr_csv_idx) std::cout << '*';
      else std::cout << ' ';
      std::cout << ' ' << i+1 << ':' << csvs[i].get_filename() << std::endl;
    }
  } else {
    size_t n=std::stoi(param);
    if (n > 0 && n <= csvs.size()) {
      curr_csv_idx=n-1;
      std::cout << n << ':' << csvs[curr_csv_idx].get_filename() << std::endl;
    }
  }
}

void fmt(char, std::string, std::string param) {
  RETURN_IF_NO_LOADED_FILE;
  if (param == "") {
    std::cout <<
      get_bool_var("mem", g_in_memory) + ", " +
      get_bool_var("csv", g_csv) + ", " +
      get_str_var("sep", g_sep) + ", " +
      get_str_var("dlm", g_dlm) + ", " +
      get_str_var("eol", g_eol) + ", " +
      get_str_var("esc", g_esc);
    std::cout << std::endl << "Variable 'mem' and 'csv' expect a boolean value (true/false, on/off, 1/0) and the other a character or an ascii code value (0-255)." << std::endl;
  } else {
    std::string var, val;
    std::vector<std::string> v=split(param, '=');

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

void quit(char, std::string, std::string) {
  extern bool no_quit;
  no_quit=false;
}

std::vector<run_opt> r_opts = {
  { "i_nfo",      'n', opt_itr,  no_argument,       "display various informations on the current file.", info },
  { "_line",      'l', opt_itr,  optional_argument, "display lines of the current file. Without parameters it will display all the lines, an interactive warning might appear if the file has more than a 1000 lines. You can also pass a range in the form 'r1-r2' or a list of line in the form 'r1 r2 r3 ...'. Rows indexes start to 1 and end to maximum number of lines.", row },
  { "_cell",      'c', opt_itr,  optional_argument, "behave like the 'line' command but for cells.", cell },
  { "lincol",    '\0', opt_itr,  required_argument, "display a cell by its line and column coordinate. By example lincol 0,0 <=> cell 0 and lincol 'lastline','lastcol' <=> cell 'lastcellindex'.", linecolumn },
  { "lc",        '\0', opt_itr,  required_argument, "shortcut for option lincol", linecolumn },
  { "_xy",        'x', opt_itr,  required_argument, "display a cell by its column (x) and line (y) coordinate.", xy },
  { "_find",      'f', opt_itr,  required_argument, "display the line where the string is found (may be a regex).", find },
  { "_transpose", 't', opt_itr,  no_argument,       "transpose the matrix represented by the csv.", transpose },
  { "_tr",       '\0', opt_itr,  no_argument,       "another shortcut for option transpose", transpose },
  { "_read",      'r', opt_itr,  optional_argument, "if a filename is provided then load a new file and set it as the current file else update the current file if there is.", readtab },
  { "_write",     'w', opt_itr,  no_argument,       "save the current file, if there is.", writetab },
  { "_set",       's', opt_itr,  optional_argument, "without parameter list all the loaded files, else set the file whose number is passed as parameter as the current file.", set },
  { "_var",       'v', opt_itr,  optional_argument, "without argument list the actual configuration variables used to parse the csv file else expect a line of the form 'var=value' to change one of them.", fmt },
  { "_quit",      'q', itr_only, no_argument,       "leave.", quit },
  { "e_xit",      'x', itr_only, no_argument,       "leave.", quit },
  { "!",          '!', itr_only, required_argument, "execute a command in the current shell.",
    [] (char , std::string , std::string val) -> void
    {
      std::system(val.c_str());
    }
  },
//  { "", '\0', 0, 0, "\nAdditionnal help message.", NULL },
//  { "", '\0', 0, 0, "", NULL },
//  { "", '\0', 0, 0, "\n2nd Additional message.", NULL }
};


int main(int argc, char **argv, char **) {
  getopt_init(argc, argv, r_opts, "Command line viewer and handler for csv or text file.", "0.1.0", "(c) Denis LALANNE. Provided as is. NO WARRANTY of any kind.");

  if (!interp() && argc < 2) {
    std::cerr << "Missing parameters. ";
    usage(std::cerr);
    exit(ENOTSUP);
  }


  exit(EXIT_SUCCESS);
}

