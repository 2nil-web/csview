
#include <stdio.h>
#include <stdlib.h>
#include <time.h>



int main(int argc, char **argv) {
  if (argc != 7) return 1;

  printf("daylight %d tz=%ld, tzname %c%c\n", daylight, timezone, (int)(*tzname)[0], (int)(*tzname)[1]);
  struct tm t;
  char daybuf[30];
  t.tm_year  = atoi(argv[1])-1900;
  t.tm_mon   = atoi(argv[2])-1;
  t.tm_mday  = atoi(argv[3]);
  t.tm_hour  = atoi(argv[4]);
  t.tm_min   = atoi(argv[5]);
  t.tm_sec   = atoi(argv[6]);
  t.tm_isdst = daylight;

  time_t tt=mktime(&t);

  printf("daylight %d tz=%ld, tzname %c%c\n", daylight, timezone, (int)(*tzname)[0], (int)(*tzname)[1]);
  printf("year %4d, mon %02d, mday %02d, time %02dh%02dm%02ds (tz=%ld)\n", t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, timezone);
  tt += timezone;

  if (tt == -1) puts("-unknown-");
  else {
    strftime(daybuf, sizeof(daybuf), "%Y-%m-%dT%H:%M:%S", &t);
    printf("%lld ==> %s\n", tt, daybuf);
  }
  return 0;
}

