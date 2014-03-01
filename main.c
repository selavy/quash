#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parse_token.h"
#include "jobs.h"
#include "change_dir.h"
#include "set.h"
#include "execute_command.h"

#define print_prompt() printf("> ");

int main(int argc, char **argv, char **envp) {
  char * token;

  print_prompt();
  while ((token = parse_token( NULL )) != NULL) {
#ifdef DEBUG
    printf ("parsed %s\n", token);
#endif

    if     (0 == strcmp (token, "quit")) break;
    else if(0 == strcmp (token, "exit")) break;
    else if(0 == strcmp (token, "jobs")) jobs();
    else if(0 == strcmp (token, "set" )) set();
    else if(0 == strcmp (token, "cd"  )) change_dir();
    else                                 execute_command( token );
    free (token);
    /* fflush (stdin); */
    print_prompt();
  }

  if (token) free (token);
  return 0;
}
