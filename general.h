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

extern void set_home (char*);
extern void set_path (char*);
extern void set_env (char **);
extern char * get_home();
extern char * get_path();
extern char ** get_env();

extern FILE * pIn;

#endif
