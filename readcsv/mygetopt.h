#ifndef MY_GETOPT_H
#define MY_GETOPT_H

#include <getopt.h>
#include <vector>
#include <string>
#include <functional>

typedef std::function<void(char c, std::string, std::string)> OptFunc;
//typedef [](char c, std::string name, std::string val)->void LambdaOptFunc;

enum io_mode { OPTION_INTERP=0, OPTION=1, INTERP=2 };
#define opt_itrp 0
#define opt      1
#define itrp     2

struct my_option {
  std::string name;
  char val;
  int oi_mode;
  int has_arg;
  std::string help;
  std::function<void(char c, std::string, std::string)> func;
  //void (*func) (char c, std::string, std::string);
};

void usage(std::ostream& out = std::cout);
void getopt_init(int argc, char **argv, std::vector<my_option> pOptions, const std::string pIntro="Some introductory help message", const std::string pVersion="1.0.0", const std::string copyright="");

#endif /* MY_GETOPT_H */

