#include "stdafx.h"

int main(int argc, char **argv, char **env) { 
  while(*env)
    printf("EnvironString:%s\n",*(env++));
  argv++;
  int seconds;
  if(!*argv) return -1;
  if(sscanf(*argv,"%d",&seconds) == 0) return -1;
  if(seconds <= 0) return -1;
  Sleep(seconds*1000);
  return 0;
}
