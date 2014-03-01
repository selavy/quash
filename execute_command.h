#ifndef _EXECUTE_COMMAND_
#define _EXECUTE_COMMAND_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include "parse_token.h"
#define INIT_ARGS_LIST_SZ 8

void execute_command(char * token);

#endif
