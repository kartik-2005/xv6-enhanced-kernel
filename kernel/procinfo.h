// kernel/procinfo.h
#ifndef PROCINFO_H
#define PROCINFO_H

#define MAX_NAME_LEN 16

struct procinfo {
  int pid;
  int state;
  int ticks;
  char name[MAX_NAME_LEN];
};

#endif
