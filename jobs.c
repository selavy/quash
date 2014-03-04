#include "jobs.h"

static void printer(job_list * job);

static void printer(job_list * job) {
  fprintf(stdout, "[%d] %d %s\n", job->job_id, job->pid, job->command);
}

void jobs() {
#ifdef DEBUG
  /*  printf("jobs()\n"); */
#endif
  check_background_processes();
  traverse_job_list(&printer);
}
