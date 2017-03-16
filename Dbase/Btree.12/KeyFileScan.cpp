#include "stdafx.h"

/* Recursive ascending scan of dbf, start with first dbf.key where
   keyncmprelop(beginRelOp,dbf.key,beginKey,beginFieldCount) = true
   Call func for each key. Stop when func returns false, (or eof).
*/

bool KeyFile::pageScanAsc(KeyPageAddr addr, RelationType beginRelOp, const KeyType *beginKey, UINT beginFieldCount, KeyFileScanner &scanner ) {
  int index;

  if(addr == DB_NULLADDR ) {
    return true; /* return true to continue scan */
  }

  KeyPage page(m_pageInfo);
  readPage(addr, page);

  switch(beginRelOp) {
  case RELOP_TRUE:
    index = 1;
    break;
  case RELOP_GE:
  case RELOP_EQ:
    index = page.searchMinIndex(*beginKey, m_keydef, beginFieldCount);
    break;
  case RELOP_GT:
    index = page.searchMaxIndex(*beginKey, m_keydef, beginFieldCount);
    index++;
    break;
  default:
    throwSqlError(SQL_FATAL_ERROR,_T("KeyFile::pageScanAsc:Invalid beginRelOp:%d"),beginRelOp);
    break;
  }

  if(index <= (int)page.getItemCount()) {
    if(!pageScanAsc(page.getChild(index-1), beginRelOp, beginKey, beginFieldCount, scanner))
      return false;

    for(int i = index; i <= (int)page.getItemCount(); i++) {
      if(!scanner.handleKey(page.getKey(i))) {
        return false;
      }
      if(!pageScanAsc(page.getChild(i), beginRelOp, beginKey, beginFieldCount, scanner)) {
        return false;
      }
    }
    return true;
  } else {
    return pageScanAsc(page.getChild(page.getItemCount()), beginRelOp, beginKey, beginFieldCount, scanner);
  }
}

/* Recursive descending scan of dbf, start with first dbf.key where
   keyncmprelop(beginRelOp,dbf.key,beginKey,beginFieldCount) = true
   Call func for each key. Stop when func returns false, (or eof).
*/
bool KeyFile::pageScanDesc(KeyPageAddr addr, RelationType beginRelOp, const KeyType *beginKey, UINT beginFieldCount, KeyFileScanner &scanner ) {
  int index;

  if(addr == DB_NULLADDR ) {
    return true; /* return true to continue scan */
  }

  KeyPage page(m_pageInfo);
  readPage(addr, page);

  switch(beginRelOp) {
  case RELOP_TRUE:
    index = page.getItemCount();
    break;
  case RELOP_LE:
  case RELOP_EQ:
    index = page.searchMaxIndex(*beginKey, m_keydef, beginFieldCount);
    break;
  case RELOP_LT:
    index = page.searchMinIndex(*beginKey, m_keydef, beginFieldCount);
    index--;
    break;
  default:
    throwSqlError(SQL_FATAL_ERROR,_T("KeyFile::pageScanDesc:Invalid beginRelOp:%d"),beginRelOp);
    break;
  }

  if(index > 0) {
    int i;
    for(i = index; i > 0; i--) {
      if(!pageScanDesc(page.getChild(i), beginRelOp, beginKey, beginFieldCount, scanner)) {
        return false;
      }

      if(!scanner.handleKey(page.getKey(i))) {
        return false;
      }
    }
    return pageScanDesc(page.getChild(i), beginRelOp, beginKey, beginFieldCount, scanner);
  } else {
    return pageScanDesc(page.getChild(0), beginRelOp, beginKey, beginFieldCount, scanner);
  }
}

void KeyFile::scanAsc(RelationType beginRelOp, const KeyType *beginKey, UINT beginFieldCount, KeyFileScanner &scanner) {
  KeyFileHeader kh;

  readHead(kh, false);

  m_keydef.checkKeyFieldCount(beginFieldCount, __TFUNCTION__);

  if(kh.m_root == DB_NULLADDR) {
    return;
  }

  pageScanAsc(kh.m_root, beginRelOp, beginKey, beginFieldCount, scanner);
}

void KeyFile::scanDesc(RelationType beginRelOp, const KeyType *beginKey, UINT beginFieldCount, KeyFileScanner &scanner) {
  KeyFileHeader kh;

  readHead(kh, false);

  m_keydef.checkKeyFieldCount(beginFieldCount, __TFUNCTION__);

  if(kh.m_root == DB_NULLADDR) {
    return;
  }

  pageScanDesc(kh.m_root, beginRelOp, beginKey, beginFieldCount, scanner);
}

bool KeyFile::pageScan(KeyPageAddr addr, int level, KeyFilePageScanner &scanner, int maxLevel) {
  if(addr == DB_NULLADDR ) {
    return true; /* return true to continue scan */
  }

  KeyPage page(m_pageInfo);
  readPage(addr, page);
  if(!scanner.handlePage(addr, level, page)) {
    return false;
  } else if(!page.isLeafPage() && ((maxLevel < 0) || (level < maxLevel))) {
    for(int i = 0; i <= (int)page.getItemCount(); i++) {
      if(!pageScan(page.getChild(i), level+1, scanner, maxLevel)) {
        return false;
      }
    }
  }
  return true;
}
