#include "execute_command.h"
static int exec_in_background = 0;
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
  int sync_pipe[2];
  char ** args;
  unsigned int jobid;

  if(-1 == pipe(sync_pipe)) {
    perror("pipe()");
  }

  args = get_arguments(command);
  pid = fork();
  if(-1 == pid) {
    perror("error forking");
    delete_arguments(args);
    return;
  } else if(!pid) {
    /* child */
    close (sync_pipe[0]);
    close (sync_pipe[1]);
    if (!exec_in_background) {
      /* execute in foreground */
      if (-1 == execv (command, args)) {
	fprintf (stderr, "unable to execute %s\n", command);
	exit (0);
      }
    } else {
      /* execute in background */
      pid_t pid2;
      close (sync_pipe[0]);
      pid2 = fork();
      jobid = add_job (pid2, command);
      if(jobid) {
	printf ("[%d] %d running in background\n", jobid, pid2);
      }
      if (-1 == pid2) {
	close (sync_pipe[1]);
	perror("error forking\n");
	exit (1);
      } else if(!pid2) {
	/* child - child */
	close (sync_pipe[1]);
	if (-1 == execv (command, args)) {
	  fprintf (stderr, "unable to execute %s\n", command);
	  exit (0);
	}
      } else {
	/* child - parent */
	int status;

	status = 1;
	write( sync_pipe[1], &status, sizeof(int));
	close (sync_pipe[1]);

	if(-1 == waitpid(pid, &status, 0)) {
	  fprintf (stderr, "problems executing %s\n", command);
	} else {
	  if(jobid) {
	    job_list * job = remove_job (pid2);
	    if(job) { free (job->command); free (job); }
	    printf ("[%d] %d finished %s\n", jobid, pid2, command);
	  }
	}
	exit (0);
      }
      exit (0);
    }
  } else {
    /* parent */
    int status;
    close (sync_pipe[1]);
    if(!exec_in_background) {
      if(-1 == waitpid(pid, &status, 0)) {
	fprintf (stderr, "problems executing %s\n", command);
      }
    } else {
      while( read( sync_pipe[0], &status, sizeof(int) ) == -1);
    }
    close (sync_pipe[0]);

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

