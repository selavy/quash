#include "execute_command.h"
static int exec_in_background = 0;
static void execute_abs_path (char * command);
static void execute_search_in_path (char * command);
static char** get_arguments(char * exec_name);
static void delete_arguments(char ** args);
static void execute(char ** command, char **args);
static int fd = -1;
static int old_fd = -1;

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
  char ** args, **cmd;
  cmd = malloc(sizeof(*cmd) * 2);
  cmd[0] = command; cmd[1] = (char *) NULL;
  args = get_arguments(command);
  execute(cmd, args);
  free (cmd);
  delete_arguments(args);
}

static void execute_search_in_path (char * command) {
  char ** args, **cmd, *path, *pch, *c, *ptr;
  int n_paths = 0, i = 0, max;
  size_t cmd_sz, size;
  
  c = malloc( strlen(command) + 2 );
  if(!c) { perror("malloc failed"); return; }
  c[0] = '/';
  strcpy(c + 1, command);
  cmd_sz = strlen(c);

  path = get_path();
  if(!path || (path[0] == '\0')) { printf("Unable to find %s\n", command); free (c); return; }
  pch = strchr (path, ':');
  while(pch != NULL) {
    ++n_paths;
    pch = strchr( pch + 1, ':');
  }
  ++n_paths;
  cmd = malloc (sizeof (*cmd) * (n_paths + 1));
  if(!cmd) { perror ("malloc failed"); return; }

  max = strlen(path);
  size = strcspn (path, ":\0");
  ptr = path;
  max -= (size + 1);
  while(max > 0) {
    char * t = malloc (size + cmd_sz + 1);
    if(!t) { perror("malloc failed"); exit (0); }
    strncpy (t, ptr, size);
    strcpy (t + size, c);
    ptr += size + 1;
    max -= size + 1;
    size = strcspn( ptr, ":" );
    cmd[i] = t;
    ++i;
  }
  
  cmd[i] = (char *) NULL; /* last argument must be NULL */
  args = get_arguments(command);
  execute (cmd, args);
  delete_arguments(args);
  delete_arguments(cmd);
  free (c);
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
    if(fd>0) { dup2(fd, STDOUT_FILENO); fd = -1; old_fd = fd; }
    while(*cmd && (-1 == execve (*cmd, args, get_env()))) ++cmd;
    fprintf (stderr, "unable to execute %s\n", args[0]);
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

static void delete_arguments(char ** args) {
  char ** p = args;
  while(*p) {
    free(*p);
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
      free (token);
      continue;
    }

    if (0 == strcmp(">", token)) {
      char * filename = parse_token( NULL );
      int pFile;
      if(!filename) { perror ("Must give file to redirect output!"); continue; }
      pFile = open (filename, O_WRONLY | O_CREAT, S_IRWXU);
      if (pFile<0) { perror("open"); continue; }
      fd = pFile;
      printf("New file descriptor created %s => %d\n", filename, fd);
      free (token);
      free (filename);
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
