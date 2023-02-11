#ifndef MY_GETOPT_H
#define MY_GETOPT_H

#include <getopt.h>
#include <vector>
#include <string>
#include <functional>

typedef std::function<void(char c, std::string, std::string)> OptFunc;
//typedef [](char c, std::string name, std::string val)->void LambdaOptFunc;

enum io_mode { OPTION_INTERP=0, OPTION=1, INTERP=2 };
#define opt_itr  0
#define opt_only 1
#define itr_only 2

struct run_opt {
  std::string name;
  char val;
  int oi_mode;
  int has_arg;
  std::string help;
  OptFunc func;
//  std::function<void(char c, std::string, std::string)> func;
  //void (*func) (char c, std::string, std::string);
};

typedef std::vector<run_opt> opt_list;
bool interp ();
void interp_quit (char, std::string, std::string);
void usage(std::ostream& out = std::cout);
int getopt_init(int argc, char **argv, opt_list pOptions, const std::string pIntro="Some introductory help message", const std::string pVersion="1.0.0", const std::string copyright="");

#endif /* MY_GETOPT_H */

