#include "stdafx.h"
#include <HashMap.h>

static void usage() {
  _ftprintf(stderr, _T("Usage:CheckRefCount filename\n"));
  exit(-1);
}

class PointerObject {
public:
  const int m_createLine;
  PointerObject(int createLine)
    : m_createLine(createLine)
  {}
};

class PointerMap : public UInt64HashMap<PointerObject> {
public:
  void handlePointerCommand(const String &cmd, Tokenizer &tok, int lineCount, const String &line);
  void logErrors();
};

void PointerMap::handlePointerCommand(const String &cmd, Tokenizer &tok, int lineCount, const String &line) {
  String       addrStr     = tok.next();
  UINT64       addr;
  if(_stscanf(addrStr.cstr(), _T("%I64X"), &addr) != 1) {
    throwException(_T("%s not a valid hex-address"), addrStr.cstr());
  }
  if (cmd == _T("NEW")) {
    PointerObject *obj = get(addr);
    if (obj != NULL) {
      _tprintf(_T("Line %d<%s>:%s already allocated\n")
              ,lineCount, line.cstr(), addrStr.cstr());
    }
    put(addr, PointerObject(lineCount));
  } else if (cmd == _T("DELETE")) {
    PointerObject *obj = get(addr);
    if (obj == NULL) {
      _tprintf(_T("Line %d<%s>:%s not allocated\n")
              ,lineCount, line.cstr(), addrStr.cstr());
    } else {
      remove(addr);
    }
  } else {
    _tprintf(_T("Line %d<%s>:Invalid Heap-command\n")
            ,lineCount, line.cstr());
  }
}

void PointerMap::logErrors() {
  if (!isEmpty()) {
    _tprintf(_T("Heap objects not deleted (memory leaks)\n"));
    for (Iterator<Entry<UINT64, PointerObject> > it = entrySet().getIterator(); it.hasNext();) {
      const Entry<UINT64, PointerObject> &e = it.next();
      const UINT64 addr = e.getKey();
      _tprintf(_T("%p created in line %d still allocated\n")
              ,(void*)addr
              ,e.getValue().m_createLine
              );
    }
  }
}

class RefObject : public PointerObject {
public:
  const int m_startRefCount;
  int       m_refCount;
  RefObject(int createLine, int refCount)
    : PointerObject(createLine)
    , m_startRefCount(refCount)
    , m_refCount(refCount)
  {}
};

class RefObjectMap : public UInt64HashMap<RefObject> {
public:
  void handleREFCNTCommand(Tokenizer &tok, int lineCount, const String &line);
  void logErrors();
};

void RefObjectMap::handleREFCNTCommand(Tokenizer &tok, int lineCount, const String &line) {
  const String op          = tok.next();
  String       addrStr     = tok.next();
  UINT64       addr;
  String       refCountStr = tok.next();
  int          refCount;
  if(_stscanf(addrStr.cstr(), _T("%I64X"), &addr) != 1) {
    throwException(_T("%s not a valid hex-address"), addrStr.cstr());
  }
  if(_stscanf(refCountStr.cstr(), _T("refCount=%d"), &refCount) != 1) {
    throwException(_T("%s not a valid refCount"), refCountStr.cstr());
  }
  if(refCount < 0) {
    _tprintf(_T("Line %d<%s>:Negative refCount\n")
            ,lineCount, line.cstr());
  }
  if(op == _T("create")) {
    RefObject *obj = get(addr);
    if(obj != NULL) {
      _tprintf(_T("Line %d<%s>:%s already allocated\n")
              ,lineCount, line.cstr(), addrStr.cstr());
    } else {
      if(refCount != 1) {
      _tprintf(_T("Line %d<%s>:%s created with refCount=%d (should be 1)\n")
              ,lineCount, line.cstr(), addrStr.cstr(), refCount);
      }
      put(addr, RefObject(lineCount,refCount));
    }
  } else if(op == _T("addref")) {
    RefObject *obj = get(addr);
    if(obj == NULL) {
      _tprintf(_T("Line %d<%s>:%s not allocated\n")
              ,lineCount, line.cstr(), addrStr.cstr());
    } else {
      const int dr = refCount - obj->m_refCount;
      if(dr != 1) {
        _tprintf(_T("Line %d<%s>:change in refcount=%d (should be 1)\n")
                ,lineCount, line.cstr(), dr);
      }
      obj->m_refCount = refCount;
    }
  } else if(op == _T("release")) {
    RefObject *obj = get(addr);
    if(obj == NULL) {
      _tprintf(_T("Line %d<%s>:%s not allocated\n")
              ,lineCount, line.cstr(), addrStr.cstr());
    } else {
      const int dr = refCount - obj->m_refCount;
      if(dr != -1) {
        _tprintf(_T("Line %d<%s>:Change in refcount=%d (should -1)\n")
                ,lineCount, line.cstr(), dr);
      }
      obj->m_refCount += dr;
      if((obj->m_startRefCount > 1) && (refCount < obj->m_startRefCount)
        || (obj->m_refCount == 0)) {
        remove(addr);
      }
    }
  } else if(op == _T("trace")) {
    RefObject *obj = get(addr);
    if(obj != NULL) {
      if(obj->m_refCount != refCount) {
        _tprintf(_T("Warning:Line %d<%s>:%s refcount has changed from %d to %d\n")
                ,lineCount, line.cstr(), addrStr.cstr(), obj->m_refCount, refCount);
        obj->m_refCount = refCount;
      }
    } else {
      if(refCount == 1) {
      _tprintf(_T("Line %d<%s>:%s traced with refCount=%d (should be > 1)\n")
              ,lineCount, line.cstr(), addrStr.cstr(), refCount);
      }
      put(addr, RefObject(lineCount,refCount));
    }
  } else if(op == _T("change")) {
    RefObject *obj = get(addr);
    if(obj == NULL) {
      _tprintf(_T("Line %d<%s>:%s not allocated\n")
              ,lineCount, line.cstr(), addrStr.cstr());
    } else {
      const int dr = refCount - obj->m_refCount;
      if((dr != -1) && (dr != 1)) {
        _tprintf(_T("Line %d<%s>:Change in refcount=%d (should be +1 or -1)\n")
                ,lineCount, line.cstr(), dr);
      }
      obj->m_refCount = refCount;
    }
  } else if(op == _T("destroy")) {
    RefObject *obj = get(addr);
    if(obj == NULL) {
      _tprintf(_T("Line %d<%s>:%s not allocated\n")
              ,lineCount, line.cstr(), addrStr.cstr());
    } else {
      remove(addr);
    }
  } else {
    _tprintf(_T("Line %d<%s>:Invalid REFCNT-command\n")
            ,lineCount, line.cstr());
  }
}

void RefObjectMap::logErrors() {
  if (!isEmpty()) {
    _tprintf(_T("RefCounted objects not deleted (memory leaks)\n"));
    for (Iterator<Entry<UINT64, RefObject> > it = entrySet().getIterator(); it.hasNext();) {
      const Entry<UINT64, RefObject> &e = it.next();
      const UINT64 addr = e.getKey();
      _tprintf(_T("%p created in line %d still allocated (refCount=%d)\n")
              ,(void*)addr
              ,e.getValue().m_createLine
              ,e.getValue().m_refCount
              );
    }
  }
}

int _tmain(int argc, TCHAR **argv) {
  argv++;
  if(!*argv) usage();
  String fileName = *argv;
  try {
    FILE *f = FOPEN(fileName, _T("r"));
    String line;
    int lineCount = 0;
    RefObjectMap refPointerMap;
    PointerMap   pointerMap;
    try {
      while (readLine(f, line)) {
        lineCount++;
        Tokenizer tok(line,_T(":"));
        if(!tok.hasNext()) continue;
        const String cmd = tok.next();
        if (cmd == _T("REFCNT")) {
          refPointerMap.handleREFCNTCommand(tok, lineCount, line);
        } else if((cmd == _T("NEW")) || (cmd == _T("DELETE"))) {
          pointerMap.handlePointerCommand(cmd, tok, lineCount, line);
        }
      }
      refPointerMap.logErrors();
      pointerMap.logErrors();
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

