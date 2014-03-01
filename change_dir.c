#include "change_dir.h"

static void change_to (char * dir);

static void change_to (char * dir) {
  if(-1 == chdir(dir)) {
    fprintf (stderr, "Unable to change to %s\n", dir);
  }
}

void change_dir() {
  char * token, *check_error;

#ifdef DEBUG
  printf ("change_dir()\n");
#endif

  token = parse_token (NULL);
  if (!token) {
    token = malloc (sizeof (*token) * strlen ("/home/peter") + 1);
    if (!token) {
      perror ("malloc failed!");
    }
    strcpy (token, "/home/peter");
    change_to (token);
  }
  check_error = parse_token (NULL);
  if (check_error) {
    perror ("too many arguments given!");
    free (token);
    free (check_error);
  }

  change_to (token);
}
