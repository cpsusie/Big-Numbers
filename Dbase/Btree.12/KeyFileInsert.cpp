#include "stdafx.h"

void KeyFile::pageInsert(KeyPageAddr    a  , 
                         const KeyType &key, 
                         bool          &h  ,
                         KeyPageItem   &v  ) {
  if(a == DB_NULLADDR) {
    h = true;
    v.m_child = DB_NULLADDR;
    v.m_key   = key;
  } else {
    KeyPage apage(m_pageInfo);
    readPage(a, apage);

    int l = 1;
    int r = apage.getItemCount() + 1;
    while(l < r) {
      const int m = (l + r)/2;
      if(m_keydef.keyCmp(apage.getKey(m),key) <= 0) {
        l = m + 1; 
      } else {
        r = m;
      }
    }
    r--;
    if(r > 0 && m_keydef.keyCmp(apage.getKey(r),key) == 0) { // key found => DUPKEY
      throwSqlError(SQL_DUPKEY,_T("Duplicate key:<%s>"), m_keydef.sprintf(key, m_keydef.getKeyFieldCount()).cstr());
      // dont want the addr to be listet
    }

    KeyPageItem u;
    pageInsert(apage.getChild(r), key, h, u);

    if(h) { // insert u to the right of item[r]

      logPage(a, apage);

      if(!apage.isFull()) { // check for pageoverflow
        h = false;
        apage.insertItem(r+1, u);
      } else { // overflow. split page a into a,b and pass middle item to v
        KeyPageAddr b = fetchNewPage(apage.isLeafPage());
        if(b == DB_NULLADDR) {
          throwSqlError(SQL_FETCHNEWPAGE_FAILED, _T("Cannot fetch new page in keyfile %s"), getName().cstr());
        }
        KeyPage bpage(m_pageInfo);
        bpage.init();
        bpage.setLeafPage(apage.isLeafPage());
        const int halfSize = bpage.getHalfSize();
        bpage.setItemCount(halfSize); // will receive this amount of keyvalues

        if(r <= halfSize) {
          if(r == halfSize) {
            v = u;
          } else { // r < KEYPAGE_HALFSIZE
            apage.getItem(halfSize, v);
            apage.copyItems(r+2, halfSize, apage, r+1);
            apage.setItem(r+1, u);
          }
          bpage.copyItems(1, halfSize, apage, halfSize+1); // copy upper half of a to lower half of b
        } else { // r > halfSize. Insert in right page
          r -= halfSize;
          apage.getItem(halfSize+1, v);
          bpage.copyItems(1, r-1, apage, halfSize+2);
          bpage.setItem(r, u);
          bpage.copyItems(r+1, halfSize, apage, halfSize+r+1);
        }
        apage.setItemCount(halfSize);
        bpage.setChild(0, v.m_child);
        v.m_child = b;
        writePage(b, bpage);
      }
      writePage(a, apage);
    }
  }
}

void KeyFile::insert(const KeyType &key) {
  KeyFileHeader header;

  readHead(header, true);

  bool        h;
  KeyPageItem u;
  pageInsert(header.m_root, key, h, u);

  if(h) { // we got item u back. Allocate new rootpage
    KeyPage newRootPage(m_pageInfo);
    newRootPage.init();
    newRootPage.setLeafPage(header.m_root == DB_NULLADDR);
    newRootPage.setItemCount(1);
    newRootPage.setChild(0, header.m_root);
    newRootPage.setItem(1, u);

    KeyPageAddr rootAddr = fetchNewPage(newRootPage.isLeafPage());
    writePage(rootAddr, newRootPage);
    setRoot(rootAddr, 1);
  } else {
    setRoot(header.m_root, 1);
  }
}
