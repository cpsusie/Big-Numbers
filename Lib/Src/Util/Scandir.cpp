#include "pch.h"
#include <Scandir.h>

DirList scandir(const TCHAR *pattern, FindDataSelector *selector, FindDataComparator *comparator) {
  intptr_t      nameIterator;
  int           done;
  DirListEntry  entry;
  DirList       list;

  try {
    for(done = 0, nameIterator = _tfindfirst64((TCHAR*)pattern, &entry);
        nameIterator >= 0 && done == 0; done = _tfindnext64(nameIterator, &entry)) {
      if(selector == NULL || selector->select(entry)) {
        list.add(entry);
      }
    }
    if(comparator != NULL) {
      list.sort(*comparator);
    }
    if(nameIterator >= 0)  {
      _findclose(nameIterator);
    }
  } catch(...) {
    if(nameIterator >= 0)  {
      _findclose(nameIterator);
    }
    throw;
  }
  return list;
}

DirList scandir(const String &pattern, FindDataSelector *selector, FindDataComparator *comparator) {
  return scandir(pattern.cstr(), selector, comparator);
}
