#include "parse_token.h"

static int last_token = 0;
static int is_sentinel (char c, char * sentinels);

static int is_sentinel (char c, char * sentinels) {
  if((c=='\n') || (c=='\r') || (c==EOF)) last_token = 1;
  else last_token = 0;

  if ((sentinels==NULL) || (*sentinels=='\0')) {
    return ((c==' ') || (c=='\t') || (c=='\n') || (c=='\r') || (c==EOF));
  }
  else {
    int i;
    for (i=0; sentinels[i] != '\0'; ++i) {
      if (c==sentinels[i]) return 1;
    }
    return 0;
  }
}

int is_last_token() {
  return last_token;
}

char * parse_token (char * sentinels) {
  int c;
  char * token;
  size_t token_sz = 0, max_token_sz = INIT_TOKEN_SZ;

  token = malloc (sizeof (*token) * max_token_sz);
  if (!token) { perror("Malloc failed!\n"); return NULL; }

  for(;;) {
    c = getc (stdin);
    if (is_sentinel(c, sentinels)) break;
    
    token[token_sz++] = c;
    if (token_sz >= max_token_sz) {
      char * larger_token;
      max_token_sz *= 2;
      larger_token = malloc (sizeof (*larger_token) * max_token_sz);
      if (!larger_token) { free(token); perror("Malloc failed!\n"); return NULL; }
      strncpy (larger_token, token, token_sz);
      free (token);
      token = larger_token;
    }
  }

  if (token_sz == 0) return NULL;
  else {
    token[token_sz] = '\0';
    return token;
  }
}
