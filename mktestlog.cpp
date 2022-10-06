
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <random>
#include <ctime>

std::vector<std::string> args, envs;
std::string prog_basename;

int generate_number() {
  std::random_device rd; // Seed
  std::default_random_engine generator(rd()); // Random number generator

  /* Distribution on which to apply the generator */
  std::uniform_int_distribution<long long unsigned> distribution(0,0xFFFFFFFFFFFFFFFF);

  for (int i = 0; i < 10; i++) {
      std::cout << distribution(generator) << std::endl;
  }

  return 0;
}

void print_time_t(std::time_t t, std::string time_format="%Y-%m-%dT%H:%M:%S") {
  std::cout << std::put_time(std::localtime(&t), time_format.c_str()) << " comes from time_t value " << t << '.' << std::endl;
}


// In bash:
// * To get a specific time_t : date -d 2020-01-01T00:00:00 +%s ==> 1577833200
// * To get time_t for now    : date +%s ==> ...
// In c++:
//  std::time_t max_time_t=std::numeric_limits<time_t>::max();
//
std::time_t generate_time_t() {
  print_time_t(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())); // time_t for now
  print_time_t(1577833200); // time_t for 2020-01-01T00:00:00
  print_time_t(0); // time_t for epoch: 1970-01-01T01:00:00

  return 0;
}

/*
time_t iso_to_time_t(std::string s) {
  struct tm t;
  strptime(s.c_str(), "%Y-%m-%dT%H:%M:%S", &t);
  return mktime(&t);
}  
*/
time_t iso_to_time_t(std::string s) {
  size_t i=0, i0=0;

  for (;;) {
    if (!isdigit(s[i])) {
      std::cout << "i0 " << i0 << ", i " << i << std::endl;
      if (i > s.size()-4) break;
      std::cout << s.substr(i0, i-i0) << std::endl;
      i0=i+1;
    }
    i++;
  }
  /*
    struct tm t;
    int success = sscanf(s.c_str(), "%d-%d-%dT%d:%d:%d", &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
    if (success != 5) return 0;

    t.tm_mon = t.tm_mon - 1;
    t.tm_wday = 0;
    t.tm_yday = 0;
    t.tm_isdst = 0;

    time_t localTime = mktime(&t);
    time_t utcTime = localTime - timezone;
    return utcTime;*/
}

int main(int argc, char *argv[]) { 
  args=std::vector<std::string>(argv, argv + argc);
  prog_basename=std::filesystem::path(args[0]).stem().string();
  args.erase(args.begin());

//  generate_number();

//  generate_time_t();

  iso_to_time_t("1970-01-01T00:00:00");
  iso_to_time_t("2022-10-06T00:00:00");
  return 0;
}


