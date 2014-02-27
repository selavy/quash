#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#define isspace(c) (((c)==' ')||((c)=='\t'))
#define STR_SZ 64
#define LIST_SZ 16
#define FORK_ERROR -1
#define CHILD 0

void print_environment(char **);
void process_file( int, char **);
void interactive(char **);
void execute_command(char *, int);
char * parse_token(int*);

int main(int argc, char **argv, char **envp) {
  if( argc > 1 ) {
    process_file( argc, argv );
  } else {
    interactive(envp);
  }
  return 0;
}

void print_environment(char **env) {
  while(*env) {
    printf("%s\n", *env);
    ++env;
  }
}

void process_file( int argc, char **argv) {
}

void interactive(char **envp) {
  for(;;) {
    int last_token;
    char * command;

    printf("> ");
    command = parse_token(&last_token);
    if (0==strcmp ("exit",command)) {
      free (command);
      command = NULL;
      break;
    } else if (command[0] == '/') {
      execute_command (command, last_token);
    } else {
      int c;
      fprintf (stderr, "Unable to process input: %s\n", command);
      free (command);
      command = NULL;
      if(!last_token)
	while(EOF!=(c = getc (stdin))) { if('\n'==c) break; }
    }
    if(command) free(command);
  }

  exit (0);
}

char * parse_token(int * last_token) {
    char * command;
    int c;
    size_t len, curr_sz;
    
    *last_token = 0;
    command = malloc (STR_SZ * sizeof (*command));
    if (!command) {
      perror ("Unable to allocate memory for string!\n");
      exit (1);
    }
    curr_sz = STR_SZ;

    /* get the first word, then decide where to go from there */
    len = 0;
    do {
      c = getc (stdin);
      if (c == '\n') {
	*last_token = 1;
	break;
      }
      else if (isspace(c)) {
	break;
      }
      command[len] = c;
      ++len;
      if (len>=curr_sz) {
	char * new_command = malloc (curr_sz * 2 * sizeof(*new_command));
	size_t i;
	if(!new_command) {
	  free (command);
	  perror ("Unable to allocate memory for string!\n");
	  exit (1);
	}
	curr_sz *= 2;
	for(i=0; i<len; ++i) {
	  new_command[i] = command[i];
	}
	free (command);
	command = new_command;
      }
    } while (EOF!=c);
    command[len] = '\0';
    ++len;
    return command;
}

void execute_command(char * command, int last_token) {
  int status;
  pid_t pid;
  int i;
  size_t list_sz = 1, curr_list_sz = LIST_SZ;
  char * name = strcpy(malloc (strlen (command) + 1), command);
  char ** args_list = malloc (LIST_SZ * sizeof (*args_list));
  char * token;
  int last = 0;

  if(!args_list) {
    perror("malloc failed!\n");
    free (command);
    command = NULL;
    exit (1);
  }
  
  args_list[0] = name;
  if (!last_token) {
    while (!last) {
      token = parse_token (&last);
      args_list[list_sz] = token;
      ++list_sz;
      if (list_sz>=(curr_list_sz-1)) {
	int i;
	char ** new_args_list = malloc (curr_list_sz * 2 * sizeof (*new_args_list));
	curr_list_sz *= 2;
	if(!new_args_list) {
	  perror ("Malloc failed!\n");
	  exit (1);
	}

	for(i=0; i<list_sz; ++i) {
	  new_args_list[i] = args_list[i];
	}

	free (args_list);
	args_list = new_args_list;
      }
    }
    args_list[list_sz] = (char *) NULL;
  }

  pid = fork();
  if (FORK_ERROR==pid) {
    perror ("Unable to fork!");
    free (command);
    exit (1);
  } else if (CHILD==pid) {
    if (last_token) {
      execl (command, command, (char *) NULL);
    } else {
      execv (command, args_list);
    }
  } else {
    /* parent */
    /* TODO check for '&' to set as a background process */
    /* TODO change wait to waitpid() */
    if(-1 == waitpid (pid, &status, 0)) {
      fprintf (stderr, "Process encountered an error. ERROR%d", errno);
    }

    /* free arguments list */
    for(i=0; i<list_sz; ++i) {
      free (args_list[i]);
    }
    free (args_list);
  }
}
