#include "general.h"

char ** envp = NULL;

void set_home (char * str) {
  char ** p = envp;

  while(*p) {
    char * tok = strtok (*p, "=");
    if (0 == strcmp(tok, "PATH")) {
      char * newpath = malloc (sizeof (*newpath) * (strlen (str) + 6));
      char * addspot;
      if(!newpath) { perror ("malloc failed!"); return; }
      strcpy (newpath, "HOME=");
      addspot = newpath + 5;
      strcpy (addspot, str);
      *p = newpath;
      break;
    }
    ++p;
  }
}

void set_path (char * str) {
  char ** p = envp;

  while(*p) {
    char * tok = strtok (*p, "=");
    if (0 == strcmp(tok, "PATH")) {
      char * newpath = malloc (sizeof (*newpath) * (strlen (str) + 6));
      char * addspot;
      if(!newpath) { perror ("malloc failed!"); return; }
      strcpy (newpath, "PATH=");
      addspot = newpath + 5;
      strcpy (addspot, str);
      *p = newpath;
      break;
    }
    ++p;
  }
}

void set_env(char ** e) {
  envp = e;
}

char * get_home() {
  return getenv ("HOME");
}

char * get_path() {
  return getenv ("PATH");
}

char ** get_env() {
  return envp;
}
