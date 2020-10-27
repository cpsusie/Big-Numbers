#include "stdafx.h"


static void checkIntegrity(KeyFile &file, const KeyFileInfo &info) {
  _tprintf(_T("Checking integrity of keyfile...\n"));

  KeyCursor cursor(file, RELOP_TRUE, nullptr, 0, RELOP_TRUE, nullptr, 0, SORT_ASCENDING);
  const KeyFileDefinition keydef(file);
  KeyType lastKey;
  unsigned __int64 keyCounter = 0;
  bool ok = true;
  while(cursor.hasNext()) {
    KeyType key;
    cursor.next(key);
    keyCounter++;
    if(keyCounter % 50000 == 0) {
      _tprintf(_T("read %s (%.2lf%%)\r"), format1000(keyCounter).cstr(), ((double)(__int64)keyCounter)/((double)(__int64)info.m_header.m_keyCount)*100);
    }
    if(keyCounter > 1) {
      const int cmpResult = keydef.keyCmp(key, lastKey);
      if(cmpResult <= 0) {
        _tprintf(_T("keyCompare(\"%s\",\"%s\") = %d. Should be > 0\n")
                ,keydef.sprintf(key).cstr()
                ,keydef.sprintf(lastKey).cstr()
                ,cmpResult
                );
        const KeyPageAddr keyAddr     = file.getPageAddr(key);
        const KeyPageAddr lastKeyAddr = file.getPageAddr(lastKey);
        _tprintf(_T("Page address(\"%s\")=%s (offset=%s). Page address(\"%s\")=%s (offset=%s)\n")
                ,keydef.sprintf(key).cstr()    , format1000(keyAddr).cstr()    , format1000(file.getPageOffset(keyAddr    )).cstr()
                ,keydef.sprintf(lastKey).cstr(), format1000(lastKeyAddr).cstr(), format1000(file.getPageOffset(lastKeyAddr)).cstr()
              );;
        ok = false;
      }
    }
    lastKey = key;
  }

  if(keyCounter != info.m_header.m_keyCount) {
    _tprintf(_T("Number of keys read=%s != Number of keys specified in header=%s\n")
            ,format1000(keyCounter).cstr()
            ,format1000(info.m_header.m_keyCount).cstr());
    ok = false;
  }
  KeyPage page(info.m_header.m_keydef.getSize());
  unsigned long freePageCount = 0;
  for(DbAddrFileFormat addr = info.m_header.m_freeList; addr != DB_NULLADDR; addr = page.getNextFree()) {
    file.readPage(addr, page);
    freePageCount++;
  }
  if(freePageCount != info.m_header.m_freeListSize) {
    _tprintf(_T("Number of free pages=%s != Number of free pages specified in header=%s\n")
            ,format1000(freePageCount).cstr()
            ,format1000(info.m_header.m_freeListSize).cstr());
    ok = false;
  }
  if(ok) {
    _tprintf(_T("Keyfile ok\n"));
  } else {
    _tprintf(_T("Keyfile corrupted\n"));
  }
}

static void usage() {
  _ftprintf(stderr,_T("Usage:KeyFileInfo [-ikltv] [-E|-I] keyfile\n"
                      "      -i:Print information of btree-structure.\n"
                      "      -k:Print keydefinition.\n"
                      "      -l:Print number of pages on each level.\n"
                      "      -t:Test integrity of the keyfile. Scan the file and check the itegrity of all pages in the file.\n"
                      "      -v:verbose.\n"
                      "      -E:Export data from keyfile in textformat to stdout.\n"
                      "      -I:Import data from stdin.\n")
           );
  exit(-1);
}

int _tmain(int argc, TCHAR **argv) {
  bool printInfo     = false;
  bool printKeydef   = false;
  bool integrityTest = false;
  bool levelCount    = false;
  bool verbose       = false;
  bool exportData    = false;
  bool importData    = false;
  TCHAR *cp;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
      case 'i': printInfo     = true; continue;
      case 'k': printKeydef   = true; continue;
      case 'l': levelCount    = true; continue;
      case 't': integrityTest = true; continue;
      case 'v': verbose       = true; continue;
      case 'E': exportData    = true; continue;
      case 'I': importData    = true; continue;
      default : usage();
      }
      break;
    }
  }

  if(importData && exportData) {
    usage();
  }

  if(!*argv) {
    usage();
  }

  try {
    const String fileName = *argv;
    const DbFileMode fileMode = importData ? DBFMODE_READWRITE : DBFMODE_READONLY;

    KeyFile file(fileName, fileMode, nullptr);
    KeyFileInfo info = file.getInfo();
    if(printInfo) {
      _tprintf(_T("%s\n"),info.toString().cstr());
    }
    if(printKeydef) {
      _tprintf(_T("%s"),info.m_header.m_keydef.toString().cstr());
    }
    if(levelCount) {
      const CompactIntArray &pageCounts = info.m_pageCountPerLevel;
      _tprintf(_T("Level   Pagecount\n"));
      for(size_t l = 0; l < pageCounts.size(); l++) {
        _tprintf(_T("%5zd %11s\n"), l, format1000(pageCounts[l]).cstr());
      }
    }
    if(integrityTest) {
      checkIntegrity(file, info);
    }
    if(exportData) {
      KeyCursor cursor(file,RELOP_TRUE,nullptr,0,RELOP_TRUE,nullptr,0,SORT_ASCENDING);
      const KeyFileDefinition &keydef = info.m_header.m_keydef;
      const __int64 totalKeyCount = info.m_header.m_keyCount;
            __int64 lineCount     = 0;
      while(cursor.hasNext()) {
        KeyType key;
        cursor.next(key);
        _tprintf(_T("%s\n"), keydef.sprintf(key).cstr());
        if(verbose) {
          if(lineCount++ % 50000 == 0) {
            _ftprintf(stderr,_T("%.2lf%%\r"),(double)lineCount/totalKeyCount*100.0);
          }
        }
      }
      if(verbose) {
        _ftprintf(stderr,_T("                      \r"));
      }
    } else if(importData) {
      if(isatty(stdin)) {
        throwException(_T("Cannot read data from keyboard."));
      }
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("Exception:%s\n"), e.what());
    return -1;
  } catch(sqlca ca) {
    ca.dump(stderr);
    return -1;
  }
  return 0;
}
