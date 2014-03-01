#ifndef _JOB_LIST_
#define _JOB_LIST_

#include "general.h"

typedef struct _job_node {
  struct _job_node * next;
  struct _job_node * prev;
  unsigned int job_id;
  pid_t pid;
  char * command;
} job_list;

void traverse_job_list (void (*fn)(job_list*));
job_list * remove_job (pid_t pid);
unsigned int add_job (pid_t pid, char * command);

#endif
