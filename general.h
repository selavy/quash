#ifndef _GENERAL_
#define _GENERAL_

#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

void set_home (char*);
void set_path (char*);
void set_env (char **);
char * get_home();
char * get_path();
char ** get_env();

extern FILE * pIn;

#endif
