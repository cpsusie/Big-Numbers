#include "stdafx.h"

/*
#define DEBUG
*/

// returns min of dbf.key where keynCmp(dbf.key,key,fieldcount) >= 0
// false if none found
bool KeyFile::pageSearchMinGE(KeyPageAddr addr, KeyType &key, UINT fieldCount) {

  if(addr == DB_NULLADDR) {
    return false;
  }

  KeyPage page(m_pageInfo);
  readPage(addr, page);

  // first find the smallet page.item >= key
  const int i = page.searchMinIndex(key, m_keydef, fieldCount);

#ifdef DEBUG
  _tprintf(_T("minge:page %s i:%d\n"),toString(addr).cstr(),i);
#endif

  if(i <= (int)page.getItemCount()) { // first try child
    if(pageSearchMinGE(page.getChild(i-1),key,fieldCount)) {
      return true;
    }
    key = page.getKey(i);
    return true;
  } else { // try biggest child
    return pageSearchMinGE(page.getChild(page.getItemCount()),key,fieldCount);
  }
}

bool KeyFile::pageSearchMinGT(KeyPageAddr addr, KeyType &key, UINT fieldCount) {

  if(addr == DB_NULLADDR) {
    return false;
  }

  KeyPage page(m_pageInfo);
  readPage(addr, page);

  // first find the smallest page.item > key
  int i = page.searchMaxIndex( key, m_keydef, fieldCount);

#ifdef DEBUG
  _tprintf(_T("mingt:page %s index:%d\n"),toString(addr).cstr(),i);
#endif

  if(i >= 0 && i < (int)page.getItemCount()) { // first try child
    if(pageSearchMinGT( page.getChild(i), key, fieldCount)) {
      return true;
    }
    key = page.getKey(i+1);
    return true;
  } else { // try biggest child
    return pageSearchMinGT(page.getChild(i),key,fieldCount);
  }
}

// find the first key in KeyFile. returns false if none found. else true
bool KeyFile::pageSearchFirst(KeyPageAddr addr, KeyType &key) {
  KeyPage page(m_pageInfo);
  bool ret;

  for(ret = false; addr != DB_NULLADDR; addr = page.getChild(0)) {
    readPage(addr, page);
    key = page.getKey(1);
    ret = true;
  }
  return ret;
}

bool KeyFile::searchMin(RelationType relop, KeyType &key, UINT fieldCount) {
  KeyFileHeader header;
  KeyType       tmpKey;
  bool          ret;

  readHead(header, false);

  m_keydef.checkKeyFieldCount(fieldCount, __TFUNCTION__);

  switch(relop) {
  case RELOP_LE: // find the smallest key in file and check it
    ret = pageSearchFirst(header.m_root, tmpKey);
    if(!ret) {
      return false;
    }
    if(fieldCount == 0 || m_keydef.keynCmp(tmpKey,key,fieldCount) <= 0) {
      key = tmpKey;
      return true;
    } else {
      return false;
    }

  case RELOP_LT:
    ret = pageSearchFirst(header.m_root, tmpKey);
    if(!ret) {
      return false;
    }
    if(fieldCount == 0 || m_keydef.keynCmp(tmpKey,key,fieldCount) < 0) {
      key = tmpKey;
      return true;
    } else {
      return false;
    }

  case RELOP_GE:
    if(fieldCount == 0) {
      return pageSearchFirst(header.m_root, key);
    } else {
      return pageSearchMinGE(header.m_root, key, fieldCount);
    }

  case RELOP_GT:
    if(fieldCount == 0) {
      return pageSearchFirst(header.m_root, key);
    } else {
      return pageSearchMinGT(header.m_root, key, fieldCount);
    }

  case RELOP_EQ:
    if(fieldCount == 0) {
      return pageSearchFirst(header.m_root, key);
    } else {
      tmpKey = key;
      ret = pageSearchMinGE(header.m_root, tmpKey, fieldCount);
      if(!ret) {
        return false;
      }
      if(m_keydef.keynCmp(tmpKey, key, fieldCount) == 0) {
        key = tmpKey;
        return true;
      } else {
        return false;
      }
    }

  case RELOP_TRUE:
    return pageSearchFirst(header.m_root, key);

  case RELOP_FALSE:
    return false;

  default:
    throwSqlError(SQL_FATAL_ERROR,_T("KeyFile::searchMin:Invalid relop:%d\n"),relop);
    return false;
  }
}
