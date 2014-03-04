#include "execute_command.h"
static int STDOUT_copy = -1;
static int fd = -1;
static int old_fd = -1;
static int doing_pipe = 0;
static int exec_first_pipe_command = 0;
static int exec_second_pipe_command = 0;
static int exec_in_background = 0;
static int pipefd[2];

static void execute_abs_path (char * command);
static void execute_search_in_path (char * command);
static char** get_arguments (char * exec_name);
static void delete_arguments (char ** args);
static void execute (char ** command, char **args);
static void exec_pipe_cmd (char ** command, char ** args);
static char ** package_command (char * command);

void execute_command(char * token) {
  if (token[0] == '/') {
    execute_abs_path (token);
  } else {
    execute_search_in_path (token);
  }
}

static void exec_pipe_cmd (char ** command, char ** args) {
  char * second_command, **second_cmd, **second_args;
  /* reset the pipe flag */
  doing_pipe = 0;

  if (is_last_token()) { perror ("Second command not given!"); delete_arguments (command); delete_arguments (args); return; }
  
  second_command = parse_token (NULL);
  if (!second_command) { perror ("Second command not given!"); delete_arguments (command); delete_arguments (args); return; }

  second_cmd = package_command (second_command);
  if (!second_cmd) { perror ("Second command not given!"); delete_arguments (command); delete_arguments (args); free (second_command); return; }
  second_args = get_arguments (second_command);

  exec_first_pipe_command = 1;
  execute (command, args);
  exec_first_pipe_command = 0;

  exec_second_pipe_command = 1;
  execute (second_cmd, second_args);
  exec_second_pipe_command = 0;

  /* command is delete by function above */
  /* arguments deleted by function above */
  delete_arguments (second_cmd);
  delete_arguments (second_args);
  free (second_command);
}

static void execute_abs_path (char * command) {
  char ** args, **cmd;
  cmd = malloc(sizeof(*cmd) * 2);
  cmd[0] = command; cmd[1] = (char *) NULL;
  args = get_arguments(command);
  if(doing_pipe) exec_pipe_cmd (cmd, args);
  else execute (cmd, args);    
  free (cmd);
  delete_arguments (args);
}

static char ** package_command (char * command) {
  char *c, **cmd, *path, *pch, *ptr;
  int n_paths = 0, i = 0, max;
  size_t cmd_sz, size;

  c = malloc( strlen(command) + 2 );
  if(!c) { perror("malloc failed"); return NULL; }
  c[0] = '/';
  strcpy(c + 1, command);
  cmd_sz = strlen(c);

  path = get_path();
  if(!path || (path[0] == '\0')) { printf("Unable to find %s\n", command); free (c); return NULL; }
  pch = strchr (path, ':');
  while(pch != NULL) {
    ++n_paths;
    pch = strchr( pch + 1, ':');
  }
  ++n_paths;
  cmd = malloc (sizeof (*cmd) * (n_paths + 1));
  if(!cmd) { perror ("malloc failed"); return NULL; }

  max = strlen(path);
  size = strcspn (path, ":\0");
  ptr = path;
  max -= (size + 1);
  while(max > 0) {
    char * t = malloc (size + cmd_sz + 1);
    if(!t) { perror("malloc failed"); return NULL; }
    strncpy (t, ptr, size);
    strcpy (t + size, c);
    ptr += size + 1;
    max -= size + 1;
    size = strcspn( ptr, ":" );
    cmd[i] = t;
    ++i;
  }
  
  /* add the sentinel value to the end of the array */
  cmd[i] = (char *) NULL;
  free (c);
  return cmd;
}

static void execute_search_in_path (char * command) {
  char ** args, **cmd;
  cmd = package_command (command);
  if (!cmd) return;
  args = get_arguments(command);
  if (doing_pipe) {
    exec_pipe_cmd( cmd, args );
  } else {
    execute (cmd, args);
  }
  delete_arguments(args);
  delete_arguments(cmd);
}

static void execute(char ** command, char **args) {
  pid_t pid;

  if ((fd < 0) && (old_fd>0)) {
    close (old_fd);
    old_fd = -1;
    dup2 (STDOUT_copy, STDOUT_FILENO);
  }

  if (exec_first_pipe_command) {
    if (-1 == pipe (pipefd)) { perror ("pipe"); return; }
  }

  pid = fork();
  if (-1 == pid) {
    perror("error forking");
    delete_arguments(args);
    return;
  } else if (!pid) {
    /* child */
    char ** cmd = command;
    if (fd>0) dup2(fd, STDOUT_FILENO);
    if (exec_first_pipe_command) {
      dup2 (pipefd[1], STDOUT_FILENO);
      close (pipefd[0]); close (pipefd[1]);
    } else if (exec_second_pipe_command) {
      dup2 (pipefd[0], STDIN_FILENO);
      close (pipefd[0]); close (pipefd[1]);
    }
    while (*cmd && (-1 == execve (*cmd, args, get_env()))) ++cmd;
    fprintf (stderr, "unable to execute %s\n", args[0]);
    exit (0);
  } else {
    /* parent */
    int status;
    if (fd>0) { STDOUT_copy = dup(STDOUT_FILENO); fd = -1; old_fd = fd; }
    if (exec_second_pipe_command) { close (pipefd[0]); close (pipefd[1]); }
    if (!exec_first_pipe_command) {
      if (exec_in_background) add_job (pid, command[0]);
      else {  /* if in foreground, then wait for it to finish */
	if(-1 == waitpid(pid, &status, 0)) { fprintf (stderr, "problems executing %s\n", command[0]);}
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
    /* check if background process */
    if (is_last_token() && (0 == strcmp ("&", token))) {
      exec_in_background = 1;
      free (token);
      continue;
    }

    if (0 == strcmp(">", token)) {
      int pFile;
      char * filename = parse_token( NULL );
      if(!filename) { perror ("Must give file to redirect output!"); continue; }
      pFile = open (filename, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR );
      if (pFile<0) { perror("open"); continue; }
      fd = pFile;
      free (token);
      free (filename);
      continue;
    }

    if (0 == strcmp("|", token)) {
      args[args_sz] = (char *) NULL;
      doing_pipe = 1;
      return args;
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
