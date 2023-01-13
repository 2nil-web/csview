

#include "readcsv.h"
#include "inter.h"
#include "util.h"

std::vector<std::string> args;
std::string prog_basename;

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

int main(int argc, char *argv[]) {
  args=std::vector<std::string>(argv, argv + argc);
  prog_basename=std::filesystem::path(args[0]).stem().string();
  args.erase(args.begin());

//  if (args.size() == 0) { usage(prog_basename, std::cerr); return 1; }

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

  bool in_memory=true;
  if (args.size() > 3) {
    in_memory=string_to_bool(args[3]);
  }

  csv::file cf;

  if (args.size() > 0) {
    cf.setfmt(is_csv);
    delay();
    cf.load(args[0], in_memory);
    double dl1=delay(false);
    std::cout << "Delay to read file " << args[1] << ' ' << dl1 << " seconds." << std::endl;
  } else {
    interactive=true;
  }

  if (interactive) inter(cf, in_memory);
  else {
    if (cf.get_filename() != "") {
      delay();
      cf.stat(line_by_line);
      double dl2=delay(false);
      std::cout << "Delay to compute stats " << dl2 << " seconds." << std::endl;
      std::cout << "Total mem " << getTotalSystemMemory() << std::endl;
    } else usage(prog_basename, std::cerr);
  }

  return 0;
}

