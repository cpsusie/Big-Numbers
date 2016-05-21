#pragma once

#include "TreeSet.h"

class LineTracer {
private:
  IntTreeSet m_lineDeclared;
  IntTreeSet m_lineDone;
  char *m_fileName;
  bool  m_enabled;
public:
  LineTracer(char *fileName, bool enabled = true);
  inline void add(int line) { if(m_enabled) m_lineDone.add(line); }
  ~LineTracer();
};

#define TRACEFILE   static LineTracer lineTracer(__FILE__);
#define NOTRACEFILE static LineTracer lineTracer(__FILE__,false);

#define TRACELINE { lineTracer.add(__LINE__); }

