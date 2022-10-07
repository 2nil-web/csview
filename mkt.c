
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print_iso_time(char *msg, struct tm* t) {
  if (msg) printf("%s ", msg);
  printf("year %4d, mon %02d, mday %02d, wday %d, yday %d isdst %2d, time %02dh%02dm%02ds, ", t->tm_year, t->tm_mon, t->tm_mday, t->tm_wday, t->tm_yday, t->tm_isdst, t->tm_hour, t->tm_min, t->tm_sec);

  char daybuf[30];
  strftime(daybuf, sizeof(daybuf), "%Y-%m-%dT%H:%M:%S", t);
  time_t tt=mktime(t);

  printf("daylight %d, timezone=%ld, tzname %s ==> ", daylight, timezone, *tzname);
  printf("%lld ==> %s\n", tt, daybuf);
}

int main(int argc, char **argv) {
  struct tm t;
  time_t tt;

  time (&tt);

  // Print current gmt time
  gmtime_s(&t, &tt);
  print_iso_time("Current gmt time         ==> ", &t);

  // Print current local time
  localtime_s(&t, &tt);
  print_iso_time("Current local time       ==> ", &t);

  if (argc != 7) return 1;

  // Print time passed as parameter from 1 to 6 as yyyy mm dd hh MM ss
  t.tm_year  = atoi(argv[1])-1900;
  t.tm_mon   = atoi(argv[2])-1;
  t.tm_mday  = atoi(argv[3]);
  t.tm_hour  = atoi(argv[4]);
  t.tm_min   = atoi(argv[5]);
  t.tm_sec   = atoi(argv[6]);
//  t.tm_isdst = -1;
  print_iso_time("Parameters as local time ==> ", &t);

  return 0;
}

