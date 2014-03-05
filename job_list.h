#ifndef _JOB_LIST_
#define _JOB_LIST_

#include "general.h"
#include "limits.h"

typedef struct _job_node {
  struct _job_node * next;
  struct _job_node * prev;
  unsigned int job_id;
  pid_t pid;
  char * command;
} job_list;

extern void traverse_job_list (void (*fn)(job_list*));
extern void delete_all_jobs();
extern void check_background_processes();
extern job_list * remove_job (pid_t pid);
extern unsigned int add_job (pid_t pid, char * command);
extern pid_t get_job_by_job_id (int job_id);

#endif
