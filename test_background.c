#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
  int c;
  pid_t pid2, pid = fork();

  if(pid == -1) {
    perror("fork");
  } else if(!pid) {
    /* child */
    
    pid2 = fork();
    if(-1 == pid2) {
      perror("fork2");
    } else if(!pid2) {
      if(-1 == execl("/bin/sleep", "sleep", "3", (char *) NULL)) {
	perror("execl");
	exit(1);
      }
    } else {
      int status;
      if(-1 == waitpid(pid, &status, 0)) {
	perror("problem with child1");
	exit(1);
      }
      printf("Child finished!\n");
      exit(1);
    }
  }


  /* parent */
  printf("Parent keeps going\n");
  printf("More\n");
  c = getc(stdin);
  printf("got %c\n", (char) c);
  return 0;
}
