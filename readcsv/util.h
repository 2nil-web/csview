
#ifndef UTIL_H
#define UTIL_H

#ifdef __WIN32__
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif
double getTotalSystemMemory();
std::string trim(std::string& s);
bool swallow_file(std::string &file_path, std::string &s);
double delay(bool start=true);
bool string_to_bool(std::string s);

#endif // UTIL_H

