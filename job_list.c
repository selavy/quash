#include "job_list.h"

static unsigned int next_job_id = 1;
static job_list * tail = NULL;
static job_list * head = NULL;

void traverse_job_list (void (*fn)(job_list*)) {
  job_list * p = head;
#ifdef DEBUG
  /*  printf("JOBS:\n"); */
#endif
  while(p) {
    fn(p);
    p = p->next;
  }
}

void delete_all_jobs() {
  job_list * p = head;
  job_list * next;
  int status;
  while(p) {
    if(-1 == waitpid(p->pid, &status, 0)) {
      printf("Error in %s\n", p->command);
    }
    next = p->next;
    free (p->command);
    free (p);
    p = next;
  }
}

void check_background_processes() {
  job_list * p = head;
  while(p) {
    /*int result = kill(p->pid, 0); 
    printf("result = %d\n", result);
    if ((-1 == result) && (errno == ESRCH)) { */
    int status;
    if(waitpid(p->pid, &status, WNOHANG) > 0) {
      /* process no longer exists */
      job_list * prev = p->prev;
      job_list * next = p->next;
      printf ("[%d] %d finished %s\n", p->job_id, p->pid, p->command);
      free (p->command);
      
      if (prev) prev->next = next;
      if (next) next->prev = prev;
      if (tail == p) tail = p->prev;
      if (head == p) head = p->next;
      free (p);
      p = next;
    }
    else p = p->next;
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

      if(prev) prev->next = next;
      if(next) next->prev= prev;
      if(tail == p) tail = p->prev;
      if(head == p) head = p->next;
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
