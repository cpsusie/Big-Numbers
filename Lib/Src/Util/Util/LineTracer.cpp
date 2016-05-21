#include "pch.h"
#include <String.h>
#include <LineTracer.h>

LineTracer::LineTracer(char *fileName, bool enabled) : m_enabled(enabled) {
  if(!m_enabled) return;
  m_fileName = fileName;
  FILE *f = fopen(fileName,"r");
  if(f == NULL) {
    return;
  }
  char line[4000];
  int lineno = 0;
  while(fgets(line,sizeof(line),f)) {
    lineno++;
    if(strstr(line,"TRACELINE") != NULL && strstr(line,"#define") == NULL) {
      m_lineDeclared.add(lineno);
    }
  }
  fclose(f);
}

LineTracer::~LineTracer() {
  if(!m_enabled) return;
  m_lineDeclared.removeAll(m_lineDone);
  Iterator<int> it = m_lineDeclared.getIterator();
  if(it.hasNext()) {
    printf("\n");
    while(it.hasNext()) {
      int &l = it.next();
      printf("%s : line %d not executed\n",m_fileName,l);
    }
  }
}
