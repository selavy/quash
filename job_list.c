#include "job_list.h"

static unsigned int next_job_id = 1;
static job_list * tail = NULL;
static job_list * head = NULL;

void traverse_job_list (void (*fn)(job_list*)) {
  job_list * p = head;
#ifdef DEBUG
  printf("JOBS:\n");
#endif
  while(p) {
    fn(p);
    p = p->next;
  }
}

job_list * remove_job (pid_t pid) {
  job_list *p = head, *retVal = NULL;
  
  while(p) {
    if(p->pid == pid) {
      /* found node */
      job_list * prev = p->prev;
      job_list * next = p->next;
      retVal = p;

      if(prev) {
	prev->next = next;
      }
      if(next) {
	next->prev= prev;
      }
      if(tail == p) {
	tail = p->prev;
      }
      if(head == p) {
	head = p->next;
      }

      p = p->prev;
    }
  }
  return retVal;
}

unsigned int add_job (pid_t pid, char * command) {
  job_list * node = malloc (sizeof (*node));
  if (!node) {
    perror("malloc failed!\n");
    return 0;
  }

  node->next = NULL;
  node->job_id = next_job_id++;
  node->pid = pid;
  node->command = malloc (sizeof (*(node->command)) * (strlen (command) + 1));
  if(!node->command) {
    perror("malloc failed!\n");
    free (node);
    return 0;
  }
  strcpy (node->command, command);

  /* inserting at the *BACK* of the list */
  if(!head) head = node;
  node->prev = tail;
  if(tail) tail->next = node;
  tail = node;

  return node->job_id;
}
