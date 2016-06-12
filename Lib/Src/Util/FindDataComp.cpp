#include "pch.h"
#include <Scandir.h>

CompoundFindDataComparator::CompoundFindDataComparator(FindDataField field, ...) {
  va_list argptr;
  va_start(argptr, field);

  bool ascending = va_arg(argptr, bool);
  m_sortDefinition.add(FindDataFieldSortDefinition(field,ascending));
  for(FindDataField f = va_arg(argptr,FindDataField); f != -1; f = va_arg(argptr,FindDataField)) {
    ascending = va_arg(argptr, bool);
    m_sortDefinition.add(FindDataFieldSortDefinition(f,ascending));
  }
}

int CompoundFindDataComparator::compare(const DirListEntry &e1, const DirListEntry &e2) {
  for(int i = 0; i < (int)m_sortDefinition.size(); i++) {
    FindDataFieldSortDefinition &s = m_sortDefinition[i];
    long long c;
    switch(s.m_field) {
    case FILEATTRIB:
      { const int is1dir = (e1.attrib & _A_SUBDIR) ? 0 : 1;
        const int is2dir = (e2.attrib & _A_SUBDIR) ? 0 : 1;
        c = is1dir - is2dir;
      }
      break;
    case FILENAME  :
      c = _tcsicmp(e1.name,e2.name);
      break;
    case FILEWRITETIME  :
      c = e1.time_write - e2.time_write;
      break;
    case FILESIZE:
      c = e1.size - e2.size;
      break;
    }
    if(c) { 
      return s.m_ascending ? sign(c) : -sign(c);
    }
  }
  return 0;
}
