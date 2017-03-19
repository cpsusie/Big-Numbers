#include "stdafx.h"

int _tmain(int argc, TCHAR **argv) {
  try {
    FileHashMap<int,int> map(_T("c:\\temp\\fisk.key"));

    map.clear();
    for(int i = 0; i < 1000000; i++) {
      if(i % 10000 == 0) {
        _tprintf(_T("%d\r"),i);
      }
      map.put(i,i*i);
    }
    return 0;
    for(Iterator<Entry<int,int> > it = map.getEntryIterator(); it.hasNext();) {
      Entry<int,int> &entry = it.next();
      _tprintf(_T("%d:%d\n"),entry.getKey(), entry.getValue());
      if(entry.getKey() == 99) {
        entry.getValue()++;
        break;
      }
    }
    int *e = map.get(67);
    if(e) {
      *e = 55;
    }
    map.remove(68);
    for(Iterator<int> it1 = map.getKeyIterator(); it1.hasNext();) {
      _tprintf(_T("%d "), it1.next());
    }
    _tprintf(_T("\n"));

  } catch(Exception e) {
    _tprintf(_T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}
