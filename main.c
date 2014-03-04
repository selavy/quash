#include "general.h"
#include <ctype.h>
#include "parse_token.h"
#include "jobs.h"
#include "change_dir.h"
#include "set.h"
#include "execute_command.h"
#include "mykill.h"

#define print_prompt() fprintf(pOut, "> ");

FILE * pIn;
FILE * pOut;

int is_comment (char * token);

int main(int argc, char **argv, char **envp) {
  char * token;
  pIn = stdin;
  pOut = stdout;
  set_env(envp);
  while (!feof (pIn)) {
    check_background_processes();
    print_prompt();
    token = parse_token( NULL );
    if(!token)                            continue;
    else if (0 == strcmp (token, "quit")) { free (token); token = NULL; break; }
    else if (0 == strcmp (token, "exit")) { free (token); token = NULL; break; }
    else if (0 == strcmp (token, "jobs")) jobs();
    else if (0 == strcmp (token, "set" )) set();
    else if (0 == strcmp (token, "cd"  )) change_dir();
    /* else if (0 == strcmp (token, "kill" )) mykill(); */
    else                                 execute_command( token );
    free (token);
    fflush (pIn);
  }

  delete_all_jobs();
  if (token) free (token);
  return 0;
}

int is_comment (char * token) {
  char * c = token;
  while((*c != '\0') && isspace (*c)) {
    ++c;
  }

  return (*c == '#') ? 1 : 0;
}
