#include "stdafx.h"

/* ----------------- DEBUGGING FUNCTIONS ------------------ */

static void keypage_dump(KeyPageAddr addr, KeyPage &page, const KeyFileDefinition &keydef, int level, FILE *f, int flags) {
  _ftprintf(f,_T("Page:%3s%*.*s level %d itemcount:%d, %s")
             ,toString(addr).cstr()
             ,level,level,_T(" "),level
             ,page.getItemCount()
             ,page.getTypeStr()
           );
  if(!page.isLeafPage()) {
    _ftprintf(f,_T(" P0:%s"), toString(page.getChild(0)).cstr());
  }

  if(flags & DUMP_NEWLINE) {
    _ftprintf(f,_T("\n"));
  }
  if(flags & DUMP_ALL) {
    for(UINT i = 1; i <= page.getItemCount(); i++) {
      _ftprintf(f,_T("["));
      keydef.fprintf(f, page.getKey(i));
      if(page.isLeafPage()) {
        _ftprintf(f,_T("]"));
      } else {
        _ftprintf(f,_T("] %s "), toString(page.getChild(i)).cstr());
      }
      if(flags & DUMP_NEWLINE) {
        _ftprintf(f,_T("\n"));
      }
    }
  } else {
    _ftprintf(f,_T("["));
    keydef.fprintf(f,page.getKey(1));
    _ftprintf(f,_T("]-["));
    keydef.fprintf(f,page.getKey(page.getItemCount()));
    _ftprintf(f,_T("] "));
    if(flags & DUMP_NEWLINE) {
      _ftprintf(f,_T("\n"));
    }
  }
}

void KeyFile::dbAddrDump(KeyPageAddr addr, int level, FILE *f, int flags) {
  KeyPage page(m_pageInfo);

  if(addr == DB_NULLADDR) {
    return;
  }
  readPage(addr, page);
  keypage_dump(addr,page,m_keydef,level,f,flags);
  dbAddrDump(page.getChild(0),level+1,f,flags);
  for(UINT i = 1; i <= page.getItemCount(); i++)
    dbAddrDump(page.getChild(i),level+1,f,flags);
}

void KeyFile::dump(FILE *f, int flags) {
  KeyFileHeader header;

  readHead(header, false);
  KeyPage page(m_pageInfo);
  dbAddrDump(header.m_root, 1,f,flags);
  if(flags & DUMP_HEADER) {
    _ftprintf(f,_T("Root:%s Last:%s Freelist:("), toString(header.m_root).cstr(), toString(header.m_last).cstr());
  }
  if(flags & DUMP_FREELIST) {
    for(KeyPageAddr list = header.m_freeList;list != DB_NULLADDR; list = page.getNextFree()) {
      _ftprintf(f,_T("%s "),toString(list).cstr());
      readPage(list, page);
    }
    _ftprintf(f,_T(")\n"));
  }
}
