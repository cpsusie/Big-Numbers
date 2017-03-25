#include "stdafx.h"

int main(int argc, char **argv) {
  argv++;
  if(*argv == NULL) {
    for(char **cpp = _environ; *cpp; cpp++)
      printf("%s\n",*cpp);
  } else {
    char *env = getenv(*argv);
    if(env == NULL) {
      printf("Environment variable %s not defined\n",*argv);
      return -1;
    } else {
      printf("%s\n",env);
    }
  }
  return 0;
}
