#ifndef _SYS_TIME_H
#define _SYS_TIME_H

struct timeval {
  long long tv_sec;
  long tv_usec;
};

int gettimeofday(struct timeval *tv, void *tz);

#endif
