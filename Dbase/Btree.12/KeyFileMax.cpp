#include "stdafx.h"

/*
#define DEBUG
*/

// returns max of dbf.key where keynCmp(dbf.key,key,fieldCount) <= 0
// false if none found 
bool KeyFile::pageSearchMaxLE(KeyPageAddr addr, KeyType &key, UINT fieldCount) {

  if(addr == DB_NULLADDR) {
    return false;
  }

  KeyPage page(m_pageInfo);
  readPage(addr, page);


  const int i = page.searchMaxIndex(key, m_keydef, fieldCount);

#ifdef DEBUG
  _tprintf(_T("maxle:page %s i:%d\n"),toString(addr).cstr(),i);
#endif

  if(pageSearchMaxLE(page.getChild(i), key, fieldCount)) {
    return true;
  }
  if(i > 0) {
    key = page.getKey(i);
    return true;
  }
  return false;
}

bool KeyFile::pageSearchMaxLT(KeyPageAddr addr, KeyType &key, UINT fieldCount) {

  if(addr == DB_NULLADDR) {
    return false;
  }

  KeyPage page(m_pageInfo);
  readPage(addr, page);

  // first find the biggest page.item < key
  int i = page.searchMinIndex(key, m_keydef, fieldCount);
  if(i > 0) i--;

#ifdef DEBUG
  _tprintf(_T("maxlt:page %s:%d\n"),toString(addr).cstr(),i);
#endif

  if(pageSearchMaxLT(page.getChild(i), key, fieldCount)) {
    return true;
  }
  if(i > 0) {
    key = page.getKey(i);
    return true;
  }
  return false;
}

// find the last key in KeyFile returns false if none found. else true
bool KeyFile::pageSearchLast(KeyPageAddr addr, KeyType &key) {
  KeyPage page(m_pageInfo);
  bool ret;
  
  for(ret = false; addr != DB_NULLADDR; addr = page.getLastChild()) {
    readPage(addr, page);
    key = page.getLastKey();
    ret = true;
  }
  return ret;
}

bool KeyFile::searchMax(RelationType relop, KeyType &key, UINT fieldCount) {
  KeyFileHeader header;
  KeyType       tmpKey;
  bool          ret;

  readHead(header, false);

  m_keydef.checkKeyFieldCount(fieldCount);

  switch(relop) {
  case RELOP_GE: // find the biggest key in file and check it
    ret = pageSearchLast(header.m_root, tmpKey);
    if(!ret) {
      return false;
    }
    if(fieldCount == 0 || m_keydef.keynCmp(tmpKey,key,fieldCount) >= 0) {
      key = tmpKey;
      return true;
    } else {
      return false;
    }

  case RELOP_GT: // find the biggest key in file and check it
    ret = pageSearchLast(header.m_root, tmpKey);
    if(!ret) {
      return false;
    }
    if(fieldCount == 0 || m_keydef.keynCmp(tmpKey,key,fieldCount) > 0) {
      key = tmpKey;
      return true;
    } else {
      return false;
    }
  
  case RELOP_LE:
    if(fieldCount == 0) {
      return pageSearchLast(header.m_root, key);
    } else {
      return pageSearchMaxLE(header.m_root, key, fieldCount);
    }

  case RELOP_LT:
    if(fieldCount == 0) {
      return pageSearchLast(header.m_root, key);
    } else {
      return pageSearchMaxLT(header.m_root, key, fieldCount);
    }

  case RELOP_EQ:
    if(fieldCount == 0) {
      return pageSearchLast(header.m_root, key);
    } else {
      tmpKey = key;
      ret = pageSearchMaxLE(header.m_root, tmpKey, fieldCount);
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
    return pageSearchLast(header.m_root, key);

  case RELOP_FALSE:
    return false;

  default:
    throwSqlError(SQL_FATAL_ERROR,_T("KeyFile::searchMax:Invalid relop:%d\n"),relop);
    return false;
  }
}
