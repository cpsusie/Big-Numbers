#include "stdafx.h"
#include <HashMap.h>

static void usage() {
  _ftprintf(stderr, _T("Usage:CheckRefCount filename\n"));
  exit(-1);
}

class RefObject {
public:
  const int m_createLine;
  int       m_refCount;
  RefObject(int createLine, int refCount)
    : m_createLine(createLine)
    , m_refCount(refCount)
  {}
};

int _tmain(int argc, TCHAR **argv) {
  argv++;
  if(!*argv) usage();
  String fileName = *argv;
  try {
    FILE *f = FOPEN(fileName, _T("r"));
    String line;
    int lineCount = 0;
    UInt64HashMap<RefObject> pointerMap; 
    try {
      while (readLine(f, line)) {
        lineCount++;
        Tokenizer tok(line,_T(":"));
        if(!tok.hasNext()) continue;
        const String cmd = tok.next();
        if (cmd == _T("REFCNT")) {
          const String op = tok.next();
          String addrStr = tok.next();
          UINT64 addr;
          String refCountStr = tok.next();
          int    refCount;
          if(_stscanf(addrStr.cstr(), _T("%I64X"), &addr) != 1) {
            throwException(_T("%s not a valid hex-address"), addrStr.cstr());
          }
          if (_stscanf(refCountStr.cstr(), _T("refCount=%d"), &refCount) != 1) {
            throwException(_T("%s not a valid refCount"), refCountStr.cstr());
          }
          if (refCount < 0) {
            _tprintf(_T("Line %d<%s>:Negative refCount\n")
                    ,lineCount, line.cstr());
          }
          if (op == _T("change")) {
            RefObject *obj = pointerMap.get(addr);
            if (obj == NULL) {
              _tprintf(_T("Line %d<%s>:%s not allocated\n")
                      ,lineCount, line.cstr(), addrStr.cstr());
            } else {
              const int dr = refCount - obj->m_refCount;
              if((dr != -1) && (dr != 1)) {
                _tprintf(_T("Line %d<%s>:Change refcount=%d (should be +1 or -1)\n")
                        ,lineCount, line.cstr(), dr);
              }
              obj->m_refCount = refCount;
            }
          } else if (op == _T("create")) {
            RefObject *obj = pointerMap.get(addr);
            if (obj != NULL) {
              _tprintf(_T("Line %d<%s>:%s already allocated\n")
                      ,lineCount, line.cstr(), addrStr.cstr());
            } else {
              if (refCount != 1) {
              _tprintf(_T("Line %d<%s>:%s created with refCount=%d (should be 1)\n")
                      ,lineCount, line.cstr(), addrStr.cstr(), refCount);
              }
              pointerMap.put(addr, RefObject(lineCount,1));
            }
          } else if (op == _T("destroy")) {
            RefObject *obj = pointerMap.get(addr);
            if (obj == NULL) {
              _tprintf(_T("Line %d<%s>:%s not allocated\n")
                      ,lineCount, line.cstr(), addrStr.cstr());
            } else {
              pointerMap.remove(addr);
            }
          } else {
            _tprintf(_T("Line %d<%s>:Invalid REFCNT-command\n")
                    ,lineCount, line.cstr());
          }
        }
      }
      if (!pointerMap.isEmpty()) {
        _tprintf(_T("RefCounted objects not deleted (memory leaks)\n"));
        for (Iterator<Entry<UINT64, RefObject> > it = pointerMap.entrySet().getIterator(); it.hasNext();) {
          const Entry<UINT64, RefObject> &e = it.next();
          _tprintf(_T("%I64X created in line %d still allocated (refCount=%d)\n")
                  ,e.getKey()
                  ,e.getValue().m_createLine
                  ,e.getValue().m_refCount
                  );
        }
      }
    } catch (Exception e) {
      fclose(f);
      throwException(_T("Error in line %d:%s"), lineCount, e.what());
    }
    fclose(f);
  } catch (Exception e) {
    _ftprintf(stderr, _T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}

