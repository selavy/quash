#include "execute_command.h"
static int exec_in_background = 0;
static void execute_abs_path (char * command);
static void execute_search_in_path (char * command);
static char** get_arguments(char * exec_name);
static void delete_arguments(char ** args);
static void execute(char ** command, char **args);

void execute_command(char * token) {
  if (token[0] == '/') {
    execute_abs_path (token);
  } else {
    execute_search_in_path (token);
  }
}

/* TODO */
/* add support for pipe */
static void execute_abs_path (char * command) {
  char ** args;
  char ** cmd = malloc(sizeof(*cmd) * 2);
  cmd[0] = command; cmd[1] = (char *) NULL;
  args = get_arguments(command);
  execute(cmd, args);
  free (cmd);
}

static void execute(char ** command, char **args) {
  pid_t pid;
  pid = fork();
  if(-1 == pid) {
    perror("error forking");
    delete_arguments(args);
    return;
  } else if(!pid) {
    /* child */
    char ** cmd = command;
    while(*cmd && (-1 == execve (*cmd, args, get_env()))) ++cmd;
    fprintf (stderr, "unable to execute %s\n", command[0]);
    exit (0);
    
  } else {
    /* parent */
    int status;
    
    if(exec_in_background) {
      /* in background, so don't wait for it, just add it to the jobs list */
      add_job (pid, command[0]);
    } else {
      /* if in foreground, then wait for it to finish */
      if(-1 == waitpid(pid, &status, 0)) {
	fprintf (stderr, "problems executing %s\n", command[0]);
      }
    }
  }
}

static void execute_search_in_path (char * command) {

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

  /* by default execute in foreground */
  exec_in_background = 0;
  while (!is_last_token() && (NULL != (token = parse_token (NULL)))) {
    if(0 == strcmp("&", token)) {
      exec_in_background = 1;
      continue;
    }
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
