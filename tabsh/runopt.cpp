
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

#include "version.h"
#include "util.h"
#include "runopt.h"

#ifdef _MSC_VER
#include "editline/readline.h"
#else
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "vt_text_format.h"

#ifdef _WIN32
#include <windows.h>
bool open_console() {
  static bool console_not_opened=true;

  if (console_not_opened) {
    SetConsoleOutputCP(CP_UTF8);
    // Set output mode to handle virtual terminal sequences
    HANDLE hOut=GetStdHandle(STD_OUTPUT_HANDLE);

    if (hOut == INVALID_HANDLE_VALUE) return false;
    DWORD dwMode=0;

    if (!GetConsoleMode(hOut, &dwMode)) return false;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    console_not_opened=false;
  }

  return true;
}
#else
bool open_console() { return true; }
#endif

// See https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
std::string vt_txt(unsigned int n) {
  return "\033["+std::to_string(n)+"m";
}

// Cherche la 1ére occurence de c dans la chaine s, et la met en valeur par un souligné vert si émulation vt possible
std::string parse_vt(char c, std::string s) {
  auto pos=s.find_first_of(c);

  if (pos != std::string::npos && open_console()) {
    s.insert(pos+1, vt_txt(VT_Reset));
    s.insert(pos, vt_txt(VT_Green));
  }

  return s;
}

// name, has_arg, val, help
// has_arg : no_argument (ou 0), si l'option ne prend pas d'argument, required_argument (ou 1) si l'option prend un argument, ou optional_argument (ou 2) si l'option prend un argument optionnel.
size_t n_opt=0, longest_opname=0;
static struct option *long_options=NULL;
static opt_list my_ropts;
std::string optstr="";
bool arg_sel=true, interp_on=true, quiet=false;


size_t index_from_val (char v) {
  for (size_t i=0; i < n_opt; i++) {
    if (v == my_ropts[i].val) return i;
  }

  return n_opt+1;
}

size_t index_from_name (std::string n) {
  for (size_t i=0; i < n_opt; i++) {
    if (n == my_ropts[i].name) return i;
  }

  return n_opt+1;
}

std::string name_from_val (char v) {
  for (size_t i=0; i < n_opt; i++) {
    if (v == my_ropts[i].val) {
      return my_ropts[i].name;
    }
  }

  return "";
}

char val_from_name (std::string n) {
  for (size_t i=0; i < n_opt; i++) {
    if (n == my_ropts[i].name && my_ropts[i].val != '\0') {
      return my_ropts[i].val;
    }
  }

  return '\0';
}


std::string progpath="";
std::string intro="";
std::string copyright="";
void usage(std::ostream& out) {
  if (arg_sel) interp_on=false;

  if (!interp_on) out << "Usage: " << progpath << " [OPTIONS] ARGUMENT" << std::endl;
  out << intro << std::endl;
  if (interp_on) out << "Available commands and their shortcut, if available." << std::endl;
  else out << "Available options:" << std::endl;

  size_t rion=longest_opname+1;

  for (auto o:my_ropts) {
    std::string uname;
    uname=parse_vt(o.val, o.name);

    std::string spc="";
    spc.append(rion-o.name.size(), ' ');

    if (o.name == "" && o.has_arg == 0 && o.val == 0 && o.func == 0) {
      if (o.oi_mode == opt_itr || (interp_on && o.oi_mode == itr_only) || (!interp_on && o.oi_mode == opt_only))
        out << o.help << std::endl;
    } else if (interp_on && (o.oi_mode == opt_itr || o.oi_mode == itr_only)) {
      std::string hlp=o.help;
      if (hlp.size() > 0) hlp[0]=tolower(hlp[0]);
      //out.width(rion); out << std::left << uname;
      out << uname << spc;
      if (isprint(o.val)) out << "(" << o.val << ") ";
      else out << "   ";
      out << hlp << std::endl;
    } else if (!interp_on && (o.oi_mode == opt_itr || o.oi_mode == opt_only)) {
      if (o.val == 0) out << "   ";
      else out << '-' << o.val << ",";
      std::string hlp=o.help;
      if (hlp.size() > 0) hlp[0]=toupper(hlp[0]);
      //out.width(rion); out << std::left << " --"+uname << hlp << std::endl;
      out << " --"+uname+spc << hlp << std::endl;
    }
  }
  if (!interp_on) out << "Order in which options are used is important." << std::endl;
}

void getUsage(char , std::string , std::string ) {
  usage();
  if (!interp_on) exit(EXIT_SUCCESS);
}

bool already_v=false;

void getVersion(char ='\0', std::string ="", std::string ="") {
  if (!already_v) already_v=true;
  std::string ppath=progpath;
  if (ppath.size() > 0) ppath[0]=toupper(ppath[0]);

  if (commit != "") version+='+'+commit;
  std::cout << ppath << ' ' << version << ", build for " << getBuild();
  if (copyright.size() > 0) std::cout << ", " << copyright << std::endl;

  if (!interp_on) exit(EXIT_SUCCESS);
}

void set_options () {
  n_opt=my_ropts.size();
  long_options=new option[n_opt+1];
  char v;

  for (size_t i=0; i < n_opt; i++) {
    //std::cout << "myv val " << my_ropts[i].val << ", name " << my_ropts[i].name << std::endl;
    if (my_ropts[i].name.size() > longest_opname) longest_opname=my_ropts[i].name.size();

    if (my_ropts[i].name.size() > 0) long_options[i].name=my_ropts[i].name.c_str();
    else long_options[i].name=NULL;

    long_options[i].has_arg=my_ropts[i].has_arg;
    long_options[i].val=my_ropts[i].val;
    long_options[i].flag=NULL;

    //std::cout << "[[[ lopt val " << (char)long_options[i].val << ", name " << long_options[i].name << ", has_arg " << long_options[i].has_arg << "]]]" << std::endl;

    if (my_ropts[i].val == 0) v=val_from_name(my_ropts[i].name);
    else v=my_ropts[i].val;

    if (v && optstr.find(v) == std::string::npos) {
      optstr+=(char)v;

      switch (my_ropts[i].has_arg) {
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
  // Do nothing if val or name already exist
  if (index_from_val(val) > n_opt && index_from_name(name) > n_opt) {
    my_ropts.insert(my_ropts.begin(), { name, val, oi_mode, has_a, help, func });
    n_opt++;
    return true;
  }

  return false;
}

bool interp_done=false;
void interp_quit (char, std::string, std::string) {
  interp_done=true;
}

int int_width(int n) {
  int l=1;

  while (n >= 10) {
    n /= 10;
    l++;
  }

  return l;
}

bool rdlnpp(std::string prompt, std::string& line) {
  char *_line=readline(prompt.c_str());

  if (_line) {
    line=_line;
    return true;
  }

  return false;
}

int histo_len() {
#ifdef _MSC_VER
  return history_length();
#else
  return history_length;
#endif
}

HIST_ENTRY **get_histo(int& l) {
  l=histo_len();
  return history_list();
}

void dispHisto(char , std::string , std::string ) {
  int l;
  HIST_ENTRY **h=get_histo(l);
  int w=int_width(l);

  for(int i=0; i < l; i++) {
    if (h[i]) std::cout << std::setw(w) << i+1 << ':' << h[i]->line << std::endl;
  }
}

#ifdef _MSC_VER

bool get_int(std::string s, int& n) {
  trim(s);

  int neg=1;

  if (s[0] == '-') {
    s=s.substr(1);
    neg=-1;
  }

  if (all_of_ctype(s, isdigit)) {
    n=neg*std::stoi(s);
    return true;
  }

  return false;
}

// Recall cmd from history by its number from the end if < 0 or from the start
std::string recall_cmd_by_num(int n) {
  int l;
  HIST_ENTRY **h=get_histo(l);
  std::string s="";

  // Rappel à partir de la fin
  if (n < 0) s=h[l+n]->line;
  // Rappel à partir du début
  else s=h[n]->line;

  return s;
}

#define trc std::cout << __LINE__ << ", SH " << sh << std::endl;
// Recall cmd from history starting from end finding string corresponding to expr
std::string recall_cmd_by_expr(std::string expr) {
  int l;
  HIST_ENTRY **h=get_histo(l);
  std::string s="";

  if (expr[0] == '?') {
    expr.erase(0,1);
    bool at_end=true;

    if (expr.back() == '?') {
      expr.pop_back();
      at_end=false;
    }

    std::string sh;
    while (--l) {
      sh=h[l]->line;

      if (at_end) {trc;
        if (sh.ends_with(expr)) return sh;
      } else {trc;
        if (sh.find(expr) != std::string::npos) return sh;
      }
    }
  } else {
    std::string sh;
    while (--l) {
      sh=h[l]->line;
      if (sh.starts_with(expr)) {
        return sh;
      }
    }
  }

  return s;
}

std::string recall_cmd_by_subst(std::string subst) {
  int l;
  HIST_ENTRY **h=get_histo(l);

  //if (subst.back() == '^') subst.pop_back();
  auto v=split(subst, '^');
  size_t p;
  std::string sh="";

  while (--l) {
    sh=h[l]->line;
    p=sh.find(v[0]);
    if (p != std::string::npos) return sh.replace(p, v[0].size(), v[1]);
  }

  return "";
}


int history_expand(char *_hs, char **_out) {
  std::string hs=_hs;
  trim(hs);
  if (hs[0] == '!')  {
    hs.erase(0, 1);
    int n;
    if (get_int(hs, n)) { // Rappel de commande numérique
      hs=recall_cmd_by_num(n);
    } else { // Rappel de commande par chaine
      // !! <=> !-1
      if (hs[0] == '!') hs=recall_cmd_by_num(-1);
      else hs=recall_cmd_by_expr(hs);
    }
  } if (hs[0] == '^')  {
    hs.erase(0, 1);
    hs=recall_cmd_by_subst(hs);
  }

  *_out=_strdup(hs.c_str());
  if (hs == "") return 1;
  return 0;
}
#endif

bool expand(std::string& line) {
  trim(line);
  char *expansion;
  int result=history_expand((char *)line.c_str(), &expansion);

  if (result) std::cerr << "Bad expansion [" << line << "] gives [" << expansion << ']' << std::endl;

  if (result < 0 || result == 2) {
    delete expansion;
    return false;
  }

  //add_history(expansion);
  if (line != expansion && expansion != "")
    std::cout << '[' << line << "] expanded to [" << expansion << ']' << std::endl;
  line=expansion;
  delete expansion;
  return true;
}

bool interp () {
  arg_sel=false;
  if (!interp_on) return false;

  if (!quiet && !already_v) {
    getVersion();
    already_v=false;
  }

  std::string ln;
  int idx=1;
  std::string cmd, param;
  std::string::size_type pos;
  bool found_cmd;

  interp_done=false;
  read_history(".tabsh_history");
  while (!interp_done) {
    if (!rdlnpp(std::to_string(idx)+" >", ln)) break;
    trim(ln);
    if (ln != "" && !expand(ln)) continue;

    if (ln.ends_with("#EOF")) {
      ln.erase(ln.size()-4);
      interp_done=true;
    }
/*
    if (ln == "#EOF") {
      ln="";
      interp_done=true;
    }
*/
    // Cas particulier du : qui n'a pas forcément besoin d'espace après ses paramétres
    if (ln[0] == ':') {
      cmd=":";
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

    if (cmd.size() > 0 && cmd != "") {
      found_cmd=false;
      for(auto myopt:my_ropts) {
        if (myopt.oi_mode != opt_only && (myopt.name == cmd || (cmd.size() == 1 && myopt.val == cmd[0]))) {
          found_cmd=true;
          if (myopt.func != NULL) {
            myopt.func(myopt.val, myopt.name, param);
            add_history((char *)ln.c_str());
            idx++;
          }
        }
      }

      if (!found_cmd && !ln.empty() && ln != "" && any_of_ctype(ln, isgraph)) {
        std::cout << "Unknown command "<< cmd;
        if (param.size() > 0) std::cout << ", with parameter(s) " << '[' << param <<']';
        std::cout << std::endl;
      }
    }
  }

  write_history(".tabsh_history");

  return true;
}

int getopt_init(int argc, char **argv, opt_list pOptions, const std::string pIntro, const std::string pVersion, const std::string pCopyright) {
  progpath=std::filesystem::path(argv[0]).stem().string();
  intro=pIntro;
  if (pVersion != "") version=pVersion;
  copyright=pCopyright;
  for (auto vo:pOptions) {
    my_ropts.push_back(vo);
    //std::cout << "val " << vo.val << ", name " << vo.name << ", help [[" << vo.help << "]]" << std::endl;
  }

  // Try to insert --help and --version if not already done
  insert_arg_if_missing("history", 'H', itr_only, no_argument, "Display history.", dispHisto);
  insert_arg_if_missing("quiet", 'q', opt_only, no_argument, "Run silently and do not display a banner in interactive mode.", [] (char , std::string , std::string) -> void { quiet=true; });
  insert_arg_if_missing("batch", 'b', opt_only, no_argument, "work in batch mode default is to work in interactive mode if -h or -V are not provided.", [] (char , std::string , std::string) -> void { interp_on=false; });
  insert_arg_if_missing("inter", 'i', opt_only, no_argument, "work in interactive mode, this is the default mode if -h or -V are not provided.", [] (char , std::string , std::string) -> void { arg_sel=false; interp_on=true; });
  insert_arg_if_missing("version", 'V', opt_itr, no_argument, "display version information.", getVersion);
  insert_arg_if_missing("help", 'h', opt_itr, no_argument, "print this message and exit if interactive mode is not explicitly specified.", getUsage);
//  for (auto vo:my_ropts) std::cout << "val " << vo.val << ", name " << vo.name << ", help [[" << vo.help << "]]" << std::endl;

  set_options();
  //std::cout << "optstr " << optstr << std::endl;

  int option_index=0, c;

  //for (int i=0; i < argc; i++) std::cout << "argv[" << i << "]=" << argv[i] << std::endl;

  size_t idx;
  std::string oarg;

  while ((c=getopt_long_only(argc, argv, optstr.c_str(), long_options, &option_index)) != -1) {
    //std::cout << "LOOP val [" << (char)c << "], name [" << long_options[option_index].name << "], idx " << option_index << ", n_opt " << n_opt << std::endl;
    //std::cout << "argc " << argc << ", option_index " << option_index << std::endl;

    if (c == '?') {
      usage(std::cerr);
      if (!interp_on) exit(ENOTSUP);
    } else {
      idx=index_from_val(c);

      if (idx < n_opt) {
        //std::cout << "Activating option '" << (char)c << "', name \"" << my_ropts[idx].name << "', arg ? " << my_ropts[idx].val << ", optarg [[" << (optarg?optarg:"") << "]]" << std::endl;

        if (my_ropts[idx].func != nullptr) {
          switch (my_ropts[idx].has_arg) {
          case required_argument:
            oarg="";
            if (optarg) oarg=optarg;
            if (oarg == "" || (oarg[0] == '-' && oarg[1] != 0)) {
              std::cerr << "Missing argument for option -" << my_ropts[idx].val << "/--" << my_ropts[idx].name << ")" << std::endl;
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

          //std::cout << "Calling func for arg " << my_ropts[idx].val << "|" << my_ropts[idx].name << "|" << oarg << std::endl;
          my_ropts[idx].func(c, my_ropts[idx].name, oarg);
        }
      }
    }
  }

  return optind;
}

