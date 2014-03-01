#include "set.h"

void set() {
  char * var;
  char * setstr;
#ifdef DEBUG
  printf("set()\n");
#endif

  if(is_last_token()) return;

  do {
    var = parse_token ("=\n\t\r ");
    if (var) break;
  } while (!is_last_token());
  fprintf (stdout, "var = %s\n", var);

  if (!var) return;
  
  do {
    setstr = parse_token (NULL);
    if (setstr) break;
  } while (!is_last_token());

  if (!setstr) {
    free (var);
    return;
  }

  set_path (setstr);
#ifdef DEBUG
  printf("set PATH to %s\n", get_path());
#endif
}
