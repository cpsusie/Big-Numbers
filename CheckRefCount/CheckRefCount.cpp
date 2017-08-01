#include "stdafx.h"
#include <HashMap.h>

static void usage() {
  _ftprintf(stderr, _T("Usage:CheckRefCount filename\n"));
  exit(-1);
}

class PointerObject {
public:
  const int m_createLine;
  String    m_text;
  PointerObject(int createLine, const String text)
    : m_createLine(createLine)
    , m_text(text)
  {
  }
};

typedef enum {
  KREFCOUNT
 ,KCREATE
 ,KADDREF
 ,KRELEASE
 ,KTRACE
 ,KCHANGE
 ,KDESTROY
 ,KNEW
 ,KDELETE
 ,KOTHER
} Keyword;

typedef struct {
  const TCHAR *m_str;
  Keyword      m_keyword;
} KeywordName;

static const KeywordName keywordTable[] = {
  _T("REFCNT" ), KREFCOUNT
 ,_T("create" ), KCREATE
 ,_T("addref" ), KADDREF
 ,_T("release"), KRELEASE
 ,_T("trace"  ), KTRACE
 ,_T("change" ), KCHANGE
 ,_T("destroy"), KDESTROY
 ,_T("NEW"    ), KNEW
 ,_T("DELETE" ), KDELETE
};

class KeywordMap : public StrHashMap<Keyword> {
public:
  KeywordMap();
  inline Keyword findKeyword(const String &str) const {
    const Keyword *kw = get(str.cstr());
    return kw ? *kw : KOTHER;
  }
};

KeywordMap::KeywordMap() {
  for (int i = 0; i < ARRAYSIZE(keywordTable); i++) {
    const KeywordName &kn = keywordTable[i];
    put(kn.m_str, kn.m_keyword);
  }
}

static const KeywordMap keywords;

class PointerMap : public UInt64HashMap<PointerObject> {
public:
  void handlePointerCommand(Keyword kw, Tokenizer &tok, int lineCount, const String &line);
  void logErrors();
};

void PointerMap::handlePointerCommand(Keyword kw, Tokenizer &tok, int lineCount, const String &line) {
  String       addrStr     = tok.next();
  UINT64       addr;
  if(_stscanf(addrStr.cstr(), _T("%I64X"), &addr) != 1) {
    throwException(_T("%s not a valid hex-address"), addrStr.cstr());
  }
  switch(kw) {
  case KNEW:
    { PointerObject *obj = get(addr);
      if(obj != NULL) {
        _tprintf(_T("Line %d<%s>:%s already allocated\n")
                ,lineCount, line.cstr(), addrStr.cstr());
      }
      put(addr, PointerObject(lineCount, tok.getRemaining()));
    }
    break;
  case KDELETE:
    { PointerObject *obj = get(addr);
      if(obj == NULL) {
        _tprintf(_T("Line %d<%s>:%s not allocated\n")
                ,lineCount, line.cstr(), addrStr.cstr());
      } else {
        remove(addr);
      }
    }
    break;
  default:
    _tprintf(_T("Line %d<%s>:Invalid Heap-command\n"), lineCount, line.cstr());
  }
}

void PointerMap::logErrors() {
  if(!isEmpty()) {
    _tprintf(_T("Heap objects not deleted (memory leaks)\n"));
    for(Iterator<Entry<UINT64, PointerObject> > it = entrySet().getIterator(); it.hasNext();) {
      const Entry<UINT64, PointerObject> &e = it.next();
      const UINT64         addr = e.getKey();
      const PointerObject &obj  = e.getValue();
      _tprintf(_T("%p created in line %5d still allocated (%s)\n")
              ,(void*)addr
              ,obj.m_createLine
              ,obj.m_text.cstr()
              );
    }
  }
}

class RefObject : public PointerObject {
public:
  const int m_startRefCount;
  int       m_refCount;
  RefObject(int createLine, int refCount, const String &text)
    : PointerObject(createLine, text)
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
  const Keyword kw          = keywords.findKeyword(tok.next());
  String        addrStr     = tok.next();
  UINT64        addr;
  String        refCountStr = tok.next();
  int           refCount;
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
  switch(kw) {
  case KCREATE:
    { RefObject *obj = get(addr);
      if(obj != NULL) {
        _tprintf(_T("Line %d<%s>:%s already allocated\n")
                ,lineCount, line.cstr(), addrStr.cstr());
      } else if(refCount != 1) {
        _tprintf(_T("Line %d<%s>:%s created with refCount=%d (should be 1)\n")
                ,lineCount, line.cstr(), addrStr.cstr(), refCount);
      }
      put(addr, RefObject(lineCount,refCount, tok.getRemaining()));
    }
    break;
  case KADDREF:
    { RefObject *obj = get(addr);
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
    }
    break;
  case KRELEASE:
    { RefObject *obj = get(addr);
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
    }
    break;
  case KTRACE:
    { RefObject *obj = get(addr);
      if(obj != NULL) {
        if(obj->m_refCount != refCount) {
          _tprintf(_T("Warning:Line %d<%s>:%s refcount has changed from %d to %d\n")
                  ,lineCount, line.cstr(), addrStr.cstr(), obj->m_refCount, refCount);
          obj->m_refCount = refCount;
        }
      } else if(refCount == 1) {
        _tprintf(_T("Line %d<%s>:%s traced with refCount=%d (should be > 1)\n")
                ,lineCount, line.cstr(), addrStr.cstr(), refCount);
      }
      put(addr, RefObject(lineCount,refCount, tok.getRemaining()));
    }
    break;
  case KCHANGE:
    { RefObject *obj = get(addr);
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
    }
    break;
  case KDESTROY:
    { RefObject *obj = get(addr);
      if(obj == NULL) {
        _tprintf(_T("Line %d<%s>:%s not allocated\n")
                ,lineCount, line.cstr(), addrStr.cstr());
      } else {
        remove(addr);
      }
    }
    break;
  default:
    _tprintf(_T("Line %d<%s>:Invalid REFCNT-command\n")
            ,lineCount, line.cstr());
  }
}

void RefObjectMap::logErrors() {
  if(!isEmpty()) {
    _tprintf(_T("RefCounted objects not deleted (memory leaks)\n"));
    for (Iterator<Entry<UINT64, RefObject> > it = entrySet().getIterator(); it.hasNext();) {
      const Entry<UINT64, RefObject> &e = it.next();
      const UINT64     addr = e.getKey();
      const RefObject &obj  = e.getValue();
      _tprintf(_T("%p created in line %5d still allocated (refCount=%d) (%s)\n")
              ,(void*)addr
              ,obj.m_createLine
              ,obj.m_refCount
              ,obj.m_text.cstr()
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
      while(readLine(f, line)) {
        lineCount++;
        Tokenizer tok(line,_T(":"));
        if(!tok.hasNext()) continue;
        const Keyword cmd = keywords.findKeyword(tok.next());
        switch(cmd) {
        case KREFCOUNT:
          refPointerMap.handleREFCNTCommand(tok, lineCount, line);
          break;
        case KNEW:
        case KDELETE:
          pointerMap.handlePointerCommand(cmd, tok, lineCount, line);
          break;
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

