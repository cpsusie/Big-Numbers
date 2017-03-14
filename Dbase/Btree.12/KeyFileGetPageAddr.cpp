#include "stdafx.h"

KeyPageAddr KeyFile::getPageAddr(const KeyType &key) {
  KeyFileHeader header;
  readHead(header, false);

  KeyPage page(m_pageInfo);
  KeyPageAddr a = header.m_root;
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
    if(r > 0 && m_keydef.keyCmp(page.getKey(r),key)==0) {
      break;
    }
    a = page.getChild(r);
  }
  return a;
}
