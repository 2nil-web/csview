#ifndef MY_GETOPT_H
#define MY_GETOPT_H

#include <getopt.h>
#include <vector>
#include <string>
#include <functional>

typedef std::function<void(char c, std::string, std::string)> OptFunc;
//typedef [](char c, std::string name, std::string val)->void LambdaOptFunc;


struct my_option {
  std::string help;
  std::string name;
  char val;
  int has_arg;
  std::function<void(char c, std::string, std::string)> func;
  //void (*func) (char c, std::string, std::string);
};

void usage(std::ostream& out = std::cout);
void getopt_init(int argc, char **argv, std::vector<my_option> pOptions, const std::string pIntro="Some introductory help message", const std::string pVersion="1.0.0", const std::string copyright="");

#endif /* MY_GETOPT_H */

