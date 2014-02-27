#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#define isspace(c) (((c)==' ')||((c)=='\n')||((c)=='\t'))
#define STR_SZ 64
#define FORK_ERROR -1
#define CHILD 0

void print_environment(char **);
void process_file( int, char **);
void interactive(char **envp);
void execute_command(char * command);
char * parse_token();

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
    char * command = parse_token();
    if (0==strcmp ("exit",command)) {
      free (command);
      command = NULL;
      break;
    } else if (command[0] == '/') {
      execute_command (command);
    } else {
      int c;
      fprintf (stderr, "Unable to process input: %s\n", command);
      free (command);
      command = NULL;
      while(EOF!=(c = getc (stdin))) { if('\n'==c) break; }
    }
    if(command) free(command);
  }

  exit (0);
}

char * parse_token() {
    char * command;
    int c;
    size_t len, curr_sz;
    
    fprintf (stdout,"> ");
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
      if (isspace(c)) break;
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

void execute_command(char * command) {
  int status;
  pid_t pid;
  int c;
  size_t len = 0, curr_sz = STR_SZ;
  char * args = malloc (STR_SZ * sizeof (*args));
  if (!args) {
    perror("malloc failed!\n");
    free (command);
    command = NULL;
    exit (1);
  }
  
  /* get any arguments so they can be passed to execl */
  while (EOF!=(c = getc (stdin))) {
    if (c=='\n') break;
    args[len] = c;
    ++len;
    if (len>=curr_sz) {
      int i;
      char * new_args = malloc (curr_sz * 2 * sizeof (*new_args));
      if(!new_args) {
	perror("malloc failed!\n");
	free (command);
	command = NULL;
	free (args);
	exit (1);
      }
      for(i=0; i<len; ++i) new_args[i] = args[i];
      free (args);
      args = new_args;
    }
  }

  pid = fork();
  if (FORK_ERROR==pid) {
    perror ("Unable to fork!");
    free (command);
    exit (1);
  } else if (CHILD==pid) {
    if (len==0) {
      execl (command, command, (char *) NULL);
    } else {
      args[len] = '\0';
      ++len;
      execl (command, command, args, (char *) NULL);
    }
  } else {
    /* parent */
    /* TODO check for '&' to set as a background process */
    /* TODO change wait to waitpid() */
    if(-1 == waitpid (pid, &status, 0)) {
      fprintf (stderr, "Process encountered an error. ERROR%d", errno);
    }
    free (args);
  }
}
