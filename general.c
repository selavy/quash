#include "general.h"

char * HOME = NULL;
char * PATH = NULL;

void set_home (char * str) {
  if (HOME) free (HOME);
  HOME = str;
}

void set_path (char * str) {
  if (PATH) free (PATH);
  PATH = str;
}

char * get_home() {
  return HOME;
}

char * get_path() {
  return PATH;
}
