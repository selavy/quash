#include "mykill.h"

void mykill() {
  char * sz_signum;
  char * sz_pid;
  pid_t pid;
  int job_id;
  int signum;


    
  if (is_last_token()) { perror ("No signal given"); return; }
  sz_signum = parse_token (NULL);
  if (!sz_signum) { perror ("No signal given"); return; }
  if (is_last_token()) { perror ("No process id given"); return; }
  sz_pid = parse_token (NULL);
  if (!sz_pid) { perror ("No process id given"); return; }

#ifndef _CS_POSIX_V7_WIDTH_RESTRICTED_ENVS
  printf("NOT PORTABLE TO TYPECAST LONG TO PID_T\n");
#endif

  job_id = atoi (sz_pid);
  signum = atoi (sz_signum);
  pid = get_job_by_job_id (job_id);

  if (-1 == pid) {
    fprintf (stderr, "No job with job id %d\n", job_id);
  } else {
    kill (pid, signum);
  }

  free (sz_signum);
  free (sz_pid);
}
