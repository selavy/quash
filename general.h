#ifndef _GENERAL_
#define _GENERAL_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void set_home (char*);
void set_path (char*);
char * get_home();
char * get_path();

#endif
