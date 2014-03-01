#include "execute_command.h"

static void execute_abs_path (char * command);
static void execute_search_in_path (char * command);
static char** get_arguments(char * exec_name);
static void delete_arguments(char ** args);

void execute_command(char * token) {
  if (token[0] == '/') {
    execute_abs_path (token);
  } else {
    execute_search_in_path (token);
  }
}

/* TODO */
/* add support for fork */
static void execute_abs_path (char * command) {
  pid_t pid;
  char ** args;

  args = get_arguments(command);
  pid = fork();
  if(-1 == pid) {
    perror("error forking");
    delete_arguments(args);
    return;
  } else if(!pid) {
    /* child */
    if(-1 == execv (command, args)) {
      fprintf (stderr, "unable to execute %s\n", command);
      exit(0);
    }
  } else {
    /* parent */
    /* TODO */
    /* Implement background/foreground */
    int status;
    if(-1 == waitpid(pid, &status, 0)) {
      fprintf (stderr, "problems executing %s\n", command);
    }
  }
}

static void execute_search_in_path (char * command) {
#ifdef DEBUG
  printf("execute_search_in_path()\n");
#endif
}

static void delete_arguments(char ** args) {
  char * p = *args;
  while(p) {
    free(p);
    ++p;
  }
  free(args);
}

static char ** get_arguments(char * exec_name) {
  char **args, *token;
  size_t args_sz = 1, max_args_sz = INIT_ARGS_LIST_SZ;
  
  args = malloc (sizeof (*args) * max_args_sz);
  if (!args) {
    perror("malloc failed!");
    return NULL;
  }

  args[0] = malloc (sizeof (**args) * (strlen(exec_name) + 1)); /* first argument is always the executable name */
  if (!args[0]) {
    perror("malloc failed!");
    free (args);
    return NULL;
  }
  strcpy (args[0], exec_name);

  while (!is_last_token() && (NULL != (token = parse_token (NULL)))) {
    args[args_sz] = malloc (sizeof (**args) * (strlen (token) + 1));
    if (!args[args_sz]) {
      int i;
      perror("malloc failed!");
      for(i=0; i<args_sz; ++i)
	free(args[i]);
      free (args);
      return NULL;
    }
    strcpy (args[args_sz], token);

    ++args_sz;
    if (args_sz >= max_args_sz) {
      /* make list larger */
      int i;
      char ** new_args;
      max_args_sz *= 2;
      new_args = malloc (sizeof (*new_args) * max_args_sz);
      if (!new_args) {
	int i;
	perror ("malloc failed!");
	for (i=0; i<args_sz; ++i)
	  free (args[i]);
	free (args);
      }

      for(i=0; i<args_sz; ++i)
	new_args[i] = args[i];
      free(args);
      args = new_args;
    }
  }

  args[args_sz] = (char *) NULL; /* last argument is always NULL */
  return args;
}

