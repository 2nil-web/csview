
#ifndef UTIL_H
#define UTIL_H


#include <functional>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif
double getTotalSystemMemory();
std::string trim(std::string& s);
std::vector<std::string> split(const std::string&, char);
bool swallow_file(std::string &file_path, std::string &s);
double delay(bool start=true);
bool string_to_bool(std::string s);
char string_to_ascii(std::string s);
bool any_of_ctype(const std::string, std::function<int(int)>);
bool all_of_ctype(const std::string, std::function<int(int)>);

#endif // UTIL_H

