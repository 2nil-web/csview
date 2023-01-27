
#ifdef __unix__
#include <unistd.h>
#include <sys/types.h>
#include <linux/limits.h>
#ifndef MAX_PATH
#define MAX_PATH PATH_MAX
#endif
#elif defined(_WIN32) || defined(WIN32)
#include <winsock2.h>
#include <windows.h>
#include <shlobj.h>
#include <io.h>
#include <tchar.h>
#include <stdio.h>
#endif

#include <cstdlib>
#include <vector>
#include <filesystem>
#include <string>
#include <fstream>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

#include "util.h"
#include "mygetopt.h"

// name, has_arg, val, help
// has_arg : no_argument (ou 0), si l'option ne prend pas d'argument, required_argument (ou 1) si l'option prend un argument, ou optional_argument (ou 2) si l'option prend un argument optionnel.
size_t n_opt=0, longest_opname=0;
static struct option *long_options=NULL;
static std::vector<my_option> myOptions;
std::string optstr="";
bool interp_on=true, quiet=false, no_quit=false;


size_t index_from_val (char v) {
  for (size_t i=0; i < n_opt; i++) {
    if (v == myOptions[i].val) return i;
  }

  return n_opt+1;
}

size_t index_from_name (std::string n) {
  for (size_t i=0; i < n_opt; i++) {
    if (n == myOptions[i].name) return i;
  }

  return n_opt+1;
}

std::string name_from_val (char v) {
  for (size_t i=0; i < n_opt; i++) {
    if (v == myOptions[i].val) {
      return myOptions[i].name;
    }
  }

  return "";
}

char val_from_name (std::string n) {
  for (size_t i=0; i < n_opt; i++) {
    if (n == myOptions[i].name && myOptions[i].val != '\0') {
      return myOptions[i].val;
    }
  }

  return '\0';
}

// From Freak, see : https://stackoverflow.com/questions/152016/detecting-cpu-architecture-compile-time
std::string getBuild() {
  #if defined(__x86_64__) || defined(_M_X64)
  return "x86_64";
  #elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
  return "x86_32";
  #elif defined(__ARM_ARCH_2__)
  return "ARM2";
  #elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
  return "ARM3";
  #elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
  return "ARM4T";
  #elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
  return "ARM5"
  #elif defined(__ARM_ARCH_6T2_) || defined(__ARM_ARCH_6T2_)
  return "ARM6T2";
  #elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
  return "ARM6";
  #elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
  return "ARM7";
  #elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
  return "ARM7A";
  #elif defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
  return "ARM7R";
  #elif defined(__ARM_ARCH_7M__)
  return "ARM7M";
  #elif defined(__ARM_ARCH_7S__)
  return "ARM7S";
  #elif defined(__aarch64__) || defined(_M_ARM64)
  return "ARM64";
  #elif defined(mips) || defined(__mips__) || defined(__mips)
  return "MIPS";
  #elif defined(__sh__)
  return "SUPERH";
  #elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
  return "POWERPC";
  #elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
  return "POWERPC64";
  #elif defined(__sparc__) || defined(__sparc)
  return "SPARC";
  #elif defined(__m68k__)
  return "M68K";
  #else
  return "UNKNOWN";
  #endif
}

std::string progpath="";
std::string intro="";
std::string version="1.0.0";
std::string copyright="";
void usage(std::ostream& out) {
  if (interp_on) out << "Available commands" << std::endl;
  else out << "Usage: " << progpath << " [OPTIONS] ARGUMENT" << std::endl;
  out << intro << std::endl;
  size_t rion=longest_opname+3;

  for (auto o:myOptions) {
    //if (o.has_arg == a_comment) out << o.help << std::endl;
    if (o.name == "" && o.has_arg == 0 && o.val == 0 && o.func == 0) {
      out << o.help << std::endl;
    } else if (interp_on && (o.oi_mode == opt_itr || o.oi_mode == itr_only)) {
      std::string hlp=o.help;
      if (hlp.size() > 0) hlp[0]=tolower(hlp[0]);
      out.width(rion); out << std::left << o.name;
      if (isprint(o.val)) out << "(" << o.val << ") ";
      std::cout << hlp << std::endl;
    } else if (!interp_on && (o.oi_mode == opt_itr || o.oi_mode == opt_only)) {
      if (o.val == 0) out << "   ";
      else out << '-' << o.val << ",";
      std::string hlp=o.help;
      if (hlp.size() > 0) hlp[0]=toupper(hlp[0]);
      out.width(rion); out << std::left << " --"+o.name << hlp << std::endl;
    }
  }
}

void getUsage(char , std::string , std::string ) {
//  if (!no_quit) interp_on=false;
  usage();
  if (!interp_on) exit(EXIT_SUCCESS);
}

void getVersion(char ='\0', std::string ="", std::string ="") {
//  if (!no_quit) interp_on=false;
  std::string ppath=progpath;
  if (ppath.size() > 0) ppath[0]=toupper(ppath[0]);

  std::cout << ppath << ' ' << version << ", build for " << getBuild();
  if (copyright.size() > 0) std::cout << ", " << copyright << std::endl;

  if (!interp_on) exit(EXIT_SUCCESS);
}

void set_options () {
  n_opt=myOptions.size();
  long_options=new option[n_opt+1];
  char v;

  for (size_t i=0; i < n_opt; i++) {
    //std::cout << "myv val " << myOptions[i].val << ", name " << myOptions[i].name << std::endl;
    if (myOptions[i].name.size() > longest_opname) longest_opname=myOptions[i].name.size();

    if (myOptions[i].name.size() > 0) long_options[i].name=myOptions[i].name.c_str();
    else long_options[i].name=NULL;

    long_options[i].has_arg=myOptions[i].has_arg;
    long_options[i].val=myOptions[i].val;
    long_options[i].flag=NULL;

    //std::cout << "[[[ lopt val " << (char)long_options[i].val << ", name " << long_options[i].name << ", has_arg " << long_options[i].has_arg << "]]]" << std::endl;

    if (myOptions[i].val == 0) v=val_from_name(myOptions[i].name);
    else v=myOptions[i].val;

    if (v && optstr.find(v) == std::string::npos) {
      optstr+=(char)v;

      switch (myOptions[i].has_arg) {
        case required_argument:
          optstr+=':';
          break;
        case optional_argument:
          optstr+="::";
          break;
        default:
          break;
      }
    }
  }

  long_options[n_opt]= { 0, 0, 0, 0 };
}

// Add arg with val and name if not already exists, return true if done else false.
bool insert_arg_if_missing(const std::string name, const char val, int oi_mode, int has_a=no_argument, const std::string help="", OptFunc func=nullptr) {
  //std::cout << "insert_arg_if_missing, name " << name << std::endl;
  // Do nothing if val or name already exist
  if (index_from_val(val) > n_opt && index_from_name(name) > n_opt) {
//    std::cout << "Adding " << name << std::endl;
    myOptions.insert(myOptions.begin(), { name, val, oi_mode, has_a, help, func });
    n_opt++;
    return true;
  }

  return false;
}

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

bool interp () {
  if (!interp_on) return false;

  if (!quiet) getVersion();

  std::string ln, prompt="> ";
  std::string cmd, param;
  std::string::size_type pos;
  bool found_cmd;

#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  EnableVTMode();
#endif
  std::cout << prompt << std::flush;

  no_quit=true;
  while (no_quit && std::getline(std::cin, ln)) {
    trim(ln);

    // Cas particulier du ! qui n'a pas forcément besoin d'espace après ses paramétres
    if (ln[0] == '!') {
      cmd="!";
      param=ln.substr(1);
      trim(param);
    } else {
      pos=ln.find_first_of(' ');

      if (pos == std::string::npos) {
        cmd=ln;
        param="";
      } else {
        cmd=ln.substr(0, pos);
        param=ln.substr(pos);
        trim(param);
      }
    }

    found_cmd=false;
    for(auto myopt:myOptions) {
      if (myopt.oi_mode != opt_only && (myopt.name == cmd || (cmd.size() == 1 && myopt.val == cmd[0]))) {
        found_cmd=true;
        myopt.func(myopt.val, myopt.name, param);
      }
    }

    if (!found_cmd && ln != "" && any_of_ctype(ln, isgraph)) {
      std::cout << "Unknown command "<< cmd;
      if (param.size() > 0) std::cout << ", with parameter(s) " << '[' << param <<']';
      std::cout << std::endl;
    }

    if (no_quit) std::cout << prompt << std::flush;
  }

  return true;
}

void getopt_init(int argc, char **argv, std::vector<my_option> pOptions, const std::string pIntro, const std::string pVersion, const std::string pCopyright) {
  progpath=std::filesystem::path(argv[0]).stem().string();
  intro=pIntro;
  version=pVersion;
  copyright=pCopyright;
  for (auto vo:pOptions) {
    myOptions.push_back(vo);
    //std::cout << "val " << vo.val << ", name " << vo.name << ", help [[" << vo.help << "]]" << std::endl;
  }

  // Try to insert --help and --version if not already done
  insert_arg_if_missing("quiet", 'q', opt_only, no_argument, "Run silently and do not display a banner in interactive mode.", [] (char , std::string , std::string) -> void { quiet=true; });
  insert_arg_if_missing("batch", 'b', opt_only, no_argument, "work in batch mode default is to work in interactive mode if -h or -V are not provided.", [] (char , std::string , std::string) -> void { interp_on=false; });
  insert_arg_if_missing("inter", 'i', opt_only, no_argument, "work in interactive mode, this is the default mode if -h or -V are not provided.", [] (char , std::string , std::string) -> void { interp_on=true; });
  insert_arg_if_missing("version", 'V', opt_itr, no_argument, "display version information and exit if not in interactive mode.", getVersion);
  insert_arg_if_missing("help", 'h', opt_itr, no_argument, "print this message and exit if not in interactive mode.", getUsage);
//  for (auto vo:myOptions) std::cout << "val " << vo.val << ", name " << vo.name << ", help [[" << vo.help << "]]" << std::endl;

  set_options();
  //std::cout << "optstr " << optstr << std::endl;

  int option_index = 0, c;

  //for (int i=0; i < argc; i++) std::cout << "argv[" << i << "]=" << argv[i] << std::endl;

  size_t idx;
  std::string oarg;

  while ((c = getopt_long_only(argc, argv, optstr.c_str(), long_options, &option_index)) != -1) {
    //std::cout << "LOOP val [" << (char)c << "], name [" << long_options[option_index].name << "], idx " << option_index << ", n_opt " << n_opt << std::endl;

    if (c == '?') {
      usage(std::cerr);
      if (!interp_on) exit(ENOTSUP);
    } else {
      idx=index_from_val(c);

      if (idx < n_opt) {
        //std::cout << "Activating option '" << (char)c << "', name \"" << myOptions[idx].name << "', arg ? " << myOptions[idx].val << ", optarg [[" << (optarg?optarg:"") << "]]" << std::endl;

        if (myOptions[idx].func != nullptr) {
          switch (myOptions[idx].has_arg) {
          case required_argument:
            oarg="";
            if (optarg) oarg=optarg;
            if (oarg == "" || (oarg[0] == '-' && oarg[1] != 0)) {
              std::cerr << "Missing argument for option -" << myOptions[idx].val << "/--" << myOptions[idx].name << ")" << std::endl;
              usage(std::cerr);
              if (!interp_on) exit(ENOTSUP);
            }
            break;
          case optional_argument:
            if (!optarg && argv[optind] != NULL && argv[optind][0] != '-') {
              oarg=argv[optind++];
            }
            break;
          default:
            oarg="";
            break;
          }

          //std::cout << "Calling func for arg " << myOptions[idx].val << "|" << myOptions[idx].name << "|" << oarg << std::endl;
          myOptions[idx].func(c, myOptions[idx].name, oarg);
        }
      }
    }
  }
}

