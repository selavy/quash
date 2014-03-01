#ifndef _PARSE_TOKEN_
#define _PARSE_TOKEN_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define INIT_TOKEN_SZ 32

/*
 * function: parse_token()
 * parameters: sentinels: c-string list of sentinel character for determining the end of a token
 * preconditions: none
 * postconditions: malloc'd c-string created
 * returns: c-string with the token or NULL if no token
 * notes:
 */
char * parse_token (char * sentinels);
int is_last_token();

#endif
