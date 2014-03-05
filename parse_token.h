#ifndef _PARSE_TOKEN_
#define _PARSE_TOKEN_

#include "general.h"

#define INIT_TOKEN_SZ 32

/*
 * function: parse_token()
 * parameters: sentinels: c-string list of sentinel character for determining the end of a token
 * preconditions: none
 * postconditions: malloc'd c-string created
 * returns: c-string with the token or NULL if no token
 * notes:
 */
extern char * parse_token (char * sentinels);

/*
 * function: is_last_token()
 * parameters: none
 * preconditions: none
 * postconditions: none
 * returns: TRUE or FALSE as to whether last token was the end of a line or file
 * notes:
 */
extern int is_last_token();

#endif
