#include "stdafx.h"

void KeyFile::pageUpdate(KeyPageAddr a, const KeyType &key) {
  KeyPage page(m_pageInfo);
  while(a != DB_NULLADDR) {
    readPage(a, page);

    int l = 1;
    int r = page.getItemCount() + 1;
    while(l < r) {
      const int m = (l + r)/2;
      if(m_keydef.keyCmp(page.getKey(m),key) <= 0) {
        l = m + 1; 
      } else {
        r = m;
      }
    }
    r--;
    if(r > 0 && m_keydef.keyCmp(page.getKey(r),key) == 0) {
      logPage(a, page);
      page.setKey(r, key);
      writePage(a, page);
      return;
    }
    a = page.getChild(r);
  }
  throwSqlError(SQL_NOT_FOUND, _T("Key <%s> not found"), m_keydef.sprintf(key,m_keydef.getKeyFieldCount()).cstr());
}

void KeyFile::update(const KeyType &key) {
  KeyFileHeader header;
  readHead(header, false);
  pageUpdate(header.m_root, key);
}
