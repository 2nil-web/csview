
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <random>
#include <ctime>
#include <cstdio>
#include <cerrno>
#include <system_error>
#include <expected>

#include <memory>
#include <string>
#include <stdexcept>

// https://github.com/Dyrcona/libunistdcpp/blob/devel/include/unistd/asprintf.h
template<typename T>
auto sfmt_convert(T&& t) {
  if constexpr (std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, std::string>) return std::forward<T>(t).c_str();
  else return std::forward<T>(t);
}

template<typename ... Args>
auto sfmt_impl(std::string &s, const std::string& format, Args&& ... args) {
  auto l=std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...);
  if (l < 0) return l;
  std::size_t sz=l+1;
  auto buf=std::make_unique<char[]>(sz);
  l=std::snprintf(buf.get(), sz, format.c_str(), std::forward<Args>(args)...);
  if (l > 0) s.assign(buf.get(), l);
  return l;
}

template<typename ... Args>
std::string sfmt(const std::string& format, Args&& ... args) {
  std::string s{};
  if (sfmt_impl(s, format, sfmt_convert(std::forward<Args>(args))...) == -1) throw std::runtime_error("Error during formatting.");
  return s;
}

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
// * As of 2020-10-10 epoch is 1970-01-01T00:00:00Z in GMT form, which can be checked with the followig command : date -d 1970-01-01T00:00:00Z +%s
// In c++:
//  std::time_t max_time_t=std::numeric_limits<time_t>::max();
//
std::time_t generate_time_t() {
  print_time_t(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())); // time_t for now
  print_time_t(1577833200); // time_t for 2020-01-01T00:00:00
  print_time_t(0); // time_t for epoch: 1970-01-01T01:00:00

  return 0;
}

struct s_time_info {
  tm t; // tm_year, tm_mon, tm_mday ...
  time_t gmtime, localtime; // Number of second away from an epoch
  std::string tzs; // Time zone string which can have value "GMT" or "+0100" or "-0100" or "+0205" or "-0230" and so forth, generally minus sign indicate timezone at east of GMT and plus sign indicate timezone at west
};
// Convert an iso8601 compatible string (<=> to command "date +%Y-%m-%dT%H:%M:%S%z") to a s_time_info struct 
// The string must, at least be of the form XXXX.XX.XX.XX.XX.XX where X is a digit and . might be anything
// If strict_checking is true then the strict conformance to the iso8601 standard is checked,
// where the string must be exactly of the form XXXX-XX-XXTXX:XX:XX, eventually followed by a 'Z' for GMT time or by - or + 
// followed by 4 digits which give the shift in hours and minutes from the GMT time
// If for any reason, the conversion went wrong then it return false but the there might be a result in the output variable tt
bool iso_to_time_info(std::string s, s_time_info& ti, bool strict_checking=false) {
  time_t shift_dir=0;
  ti.tzs="GMT";

  // At least must be of the form XXXX.XX.XX.XX.XX.XX
  if (s.size() >= 19) {
    ti.t.tm_year = std::stoi(s.substr(0, 4))-1900;
    ti.t.tm_mon  = std::stoi(s.substr(5, 2))-1;
    ti.t.tm_mday = std::stoi(s.substr(8, 2));
    ti.t.tm_hour = std::stoi(s.substr(11, 2));
    ti.t.tm_min  = std::stoi(s.substr(14, 2));
    ti.t.tm_sec  = std::stoi(s.substr(17, 2));
    ti.t.tm_isdst=0;

    // Get gm time (=localtime-timezone)

    ti.localtime=ti.gmtime=mktime(&ti.t)-timezone;
    // Calling localtime or gmtime feeds variables *tzname, timezone, daylight for local time zone
    localtime(&ti.localtime);

    // XXXX.XX.XX.XX.XX.XXZ GMT time representation
    if (s.size() != 20 || s[19] != 'Z') {
      // XXXX.XX.XX.XX.XX.XX-XXXX or XXXX.XX.XX.XX.XX.XXZ+XXXX local time representation
      // 2020-01-01T00:00:00+0100
      time_t shift_hour=0, shift_min=0;

      if (s.size() == 24) {
        ti.tzs=s.substr(19, 5);

        shift_hour=std::stoi(s.substr(20, 2));
        shift_min=std::stoi(s.substr(22, 2));

        if (s[19] == '-') shift_dir=1;
        else 
          if (s[19] == '+') shift_dir=-1;
            else shift_dir=0;

      }

      ti.localtime=ti.gmtime+shift_dir*(shift_hour*3600+shift_min*60);
    }
  } else return false;

  if (strict_checking) { // XXXX-XX-XXTXX:XX:XX[Z|[+|-]XXXX]
    if (s.size() >= 20 && s[4] == '-' && s[7] == '-' && s[10] == 'T' && s[13] == ':' && s[16] == ':') {
      if (s.size() == 20 && s[19] == 'Z') return true;
      else if (s.size() == 24 && shift_dir != 0) return true;
    }

    return false;
  }

  return true;
}

void print_iso_time(std::string s) {
  s_time_info ti;
  if (!iso_to_time_info(s, ti, true)) std::cout << "Problem with iso 8601 conversion, displaying anyway ..." << std::endl;

  std::cout
    << s << " ==> "
    << sfmt("length %d, year %4d, month %02d, month day %02d, week day %02d, year day %03d, time %02dh%02dm%02ds, timezone %s, gm and local time values %lld, %lld",
            s.size(), ti.t.tm_year+1900, ti.t.tm_mon+1, ti.t.tm_mday, ti.t.tm_wday, ti.t.tm_yday, ti.t.tm_hour, ti.t.tm_min, ti.t.tm_sec, ti.tzs, ti.gmtime, ti.localtime)
    << std::endl;
}

// To check :
// make mktestlog && ./mktestlog.exe 2022-01-01T00:00:00Z && date -d 2020-01-01T00:00:00Z +%s
int main(int argc, char *argv[]) { 
  std::vector<std::string> args;//, envs;
  std::string prog_basename;

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

