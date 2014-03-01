#include "change_dir.h"

static void change_to (char * dir);

static void change_to (char * dir) {
  if(-1 == chdir(dir)) {
    fprintf (stderr, "Unable to change to %s\n", dir);
  }
}

void change_dir() {
  char * token;

  if (is_last_token()) {
    char * home = getenv("HOME");
    token = malloc (sizeof (*token) * strlen (home) + 1);
    if (!token) {
      perror ("malloc failed!");
    }
    strcpy (token, home);
    change_to (token);
    return;
  }
  else {
    token = parse_token (NULL);
    if (!is_last_token()) {
      perror ("too many arguments given!");
      free (token);
    }
    
    change_to (token);
  }
}
