#include "general.h"
#include "parse_token.h"
#include "jobs.h"
#include "change_dir.h"
#include "set.h"
#include "execute_command.h"

#define print_prompt() fprintf(pOut, "> ");

FILE * pIn;
FILE * pOut;

int main(int argc, char **argv, char **envp) {
  char * token;
  pOut = stdout;
  if(argc > 2) {
    if (0 == strcmp( argv[1],  "<")) {
      if (argc > 3) {
	pIn = fopen (argv[2], "r");
	if (!pIn) { perror ("could not find input file!"); exit (0); }
	pOut = fopen("/dev/null", "w");
      } else { perror ("no file specified");	exit (0); }
    }
    else { perror ("incorrect arugments");      exit (0); }
  } else { pIn = stdin; }

  set_env(envp);
  
  while (!feof (pIn)) {
    check_background_processes();
    print_prompt();
    token = parse_token( NULL );
    if(!token) { print_prompt(); continue; }
    else if (0 == strcmp (token, "quit")) break;
    else if (0 == strcmp (token, "exit")) break;
    else if (0 == strcmp (token, "jobs")) jobs();
    else if (0 == strcmp (token, "set" )) set();
    else if (0 == strcmp (token, "cd"  )) change_dir();
    else                                 execute_command( token );
    free (token);
    fflush (stdin);
  }

  delete_all_jobs();
  if (token) free (token);
  return 0;
}
