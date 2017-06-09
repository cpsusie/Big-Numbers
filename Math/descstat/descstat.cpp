#include "stdafx.h"
#include <Math/Statistic.h>

int main(int argc, char **argv) {
  try {
    argv++;
    FILE *input = *argv ? FOPEN(*argv,"r") : stdin;
    char line[1000];
    int linecount = 0;
    Array<Real> dataArray;
    while(fgets(line,sizeof(line),input)) {
      linecount++;
      double x;
      if(sscanf(line,"%le",&x) != 1) {
        fprintf(stderr,"Error in input in line %d:%s\n",linecount,line);
        continue;
      }
      dataArray.add(x);
    }
    DescriptiveStatistics desc(dataArray);
    _tprintf(_T("%s\n"), desc.toString().cstr());
  } catch(Exception e) {
    _ftprintf(stderr,_T("%s\n"),e.what());
    exit(-1);
  }
  return 0;
}
