#include "stdafx.h"
#include <MyUtil.h>

void *operator new(size_t size, const TCHAR *file, int line, const TCHAR *method) {
  void *p = ::operator new(size);
  debugLog(_T("NEW:%p:%s(%d):%s (%zu bytes)\n"), p, file,line,method,size);
  return p;
}


#undef TRACE_NEW

#define TRACE_NEW    new(   __TFILE__,__LINE__, __TFUNCTION__)

#define new    TRACE_NEW

class List {
public:
  int   m_data;
  List *m_next;
public:
  List(int data, List *next) : m_data(data), m_next(next) {
  }
  ~List() {
    if(m_next) {
      delete m_next;
    }
  }
};

static List *first;
int main() {

  for (int i = 0; i < 10; i++) {
    first = new List(i,first);
  }

  for (List *p = first; p; p = p->m_next) {
    printf("%d\n", p->m_data);
  }
  delete first;
  return 0;
}
