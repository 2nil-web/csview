
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

  for (int i=0; i < 10; i++) {
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
// * As of 2020-10-10 epoch is 1970-01-01T00:00:00Z in UTC form, which can be checked with the followig command : date -d 1970-01-01T00:00:00Z +%s
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
// Convert an iso8601 compatible string to time_t which the number of second away from an epoch
// The string must, at least be of the form XXXX.XX.XX.XX.XX.XX where X is a digit and . is anything
// If strict_checking is true then the strict conformance to the iso8601 standard is checked, where the string must be exactly of the form XXXX-XX-XXTXX:XX:XX, followed by a 'Z' for UTC time or by - or + followed by 4 digit which gives the shift in hours and minute from the UTC time
// If the conversion went wrong then return false
bool iso_to_time_t(std::string s, time_t& utc_time, bool strict_checking=false) {
  tm t;
  time_t shift_dir=0;

  // At least must be of the form XXXX.XX.XX.XX.XX.XX
  if (s.size() >= 19) {
    t.tm_year=std::stoi(s.substr(0, 4))-1900;
    t.tm_mon=std::stoi(s.substr(5, 2))-1;
    t.tm_mday=std::stoi(s.substr(8, 2));
    t.tm_hour=std::stoi(s.substr(11, 2));
    t.tm_min=std::stoi(s.substr(14, 2));
    t.tm_sec=std::stoi(s.substr(17, 2));

    //t.tm_wday=0;
    //t.tm_yday=0;
    t.tm_isdst=-1;

    utc_time=mktime(&t);

    // XXXX.XX.XX.XX.XX.XXZ GMT/UTC time representation
    if (s.size() >= 20 && s[19] == 'Z') utc_time=utc_time+timezone;
    else {
      // XXXX.XX.XX.XX.XX.XX-XXXX or XXXX.XX.XX.XX.XX.XXZ+XXXX local time representation
      time_t shift_hour=0, shift_min=0;

      if (s.size() >= 25) {
        shift_hour=std::stoi(s.substr(22, 2));
        shift_min=std::stoi(s.substr(24, 2));

        if (s[21] == '+') shift_dir=1;
        else 
          if (s[21] == '-') shift_dir=-1;
            else shift_dir=0;

      }

      utc_time=utc_time+shift_dir*(shift_hour*3600+shift_min*60);
    }
  } else return false;

  std::cout << "year " << t.tm_year << ", mon " << t.tm_mon << ", mday " << t.tm_mday << ", time " << t.tm_hour << 'h' << t.tm_min << 'm' << t.tm_sec << "s, shift " << shift_dir << " ==> " << utc_time << std::endl;

  if (strict_checking) {
    if (s.size() >= 20 && s[4] == '-' && s[7] == '-' && s[10] == 'T' && s[13] == ':' && s[16] == ':') {
      if (s.size() == 20 && s[19] == 'Z') return true;
      else if (s.size() == 25 && shift_dir != 0) return true;
    }

    return false;
  }

  return true;
}

void print_iso_time(std::string s) {
  time_t t;
  iso_to_time_t(s, t);
  std::cout << s << " ==> " << t << std::endl;
}

// To check :
// make mktestlog && ./mktestlog.exe 2022-01-01T00:00:00Z && date -d 2020-01-01T00:00:00Z +%s
int main(int argc, char *argv[]) { 
  args=std::vector<std::string>(argv, argv + argc);
  prog_basename=std::filesystem::path(args[0]).stem().string();
  args.erase(args.begin());

//  generate_number();
//  generate_time_t();


  if (args.size() > 0) {
    for (auto s:args) print_iso_time(s);
  } else {
    std::string iso_time="1970-01-01T00:00:00Z";
    print_iso_time(iso_time);
    iso_time="2022-10-06T00:00:00Z";
    print_iso_time(iso_time);
  }

  return 0;
}


