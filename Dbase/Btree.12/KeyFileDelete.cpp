#include "stdafx.h"

void KeyFile::pageUnderflow(KeyPageAddr a, KeyPage &c, int s, bool &h) {

  KeyPage apage(m_pageInfo);
  KeyPage bpage(m_pageInfo);

  const int mc = c.getItemCount();
  KeyPageItem tmpItem;

  if(s < mc) {
    s++;
    KeyPageAddr b = c.getChild(s);
    readPage(b, bpage);
    logPage( b, bpage);
    readPage(a, apage);
    logPage( a, apage);
    int mb = bpage.getItemCount();
    const int halfSize = bpage.getHalfSize();
    const int k = (mb - halfSize + 1) / 2;
                                       // ITEM(a,n) = ITEM(c,s);
    if(k > 0) { // move k items from b to a
      apage.setItemCount(halfSize - 1 + k);
      if(apage.isLeafPage()) {
        apage.setKey(halfSize, c.getKey(s));
      } else {
        c.getItem(s, tmpItem).m_child = bpage.getChild(0);
        apage.setItem(halfSize, tmpItem);
      }
      apage.copyItems(halfSize + 1,halfSize - 1 + k,bpage,1);
      c.setItem(s, bpage.getItem(k, tmpItem));
      c.setChild(s,b);
      if(!bpage.isLeafPage()) {
        bpage.setChild(0, bpage.getChild(k));
      }
      mb -= k;
      bpage.copyItems(1, mb, bpage, k + 1);
      bpage.setItemCount(mb);
      h = false;

      writePage(a, apage);
      writePage(b, bpage);
    } else { // k == 0. merge pages a and b
      apage.setItemCount(apage.getMaxItemCount());
      if(apage.isLeafPage()) {
        apage.setKey(halfSize, c.getKey(s));
      } else {
        c.getItem(s, tmpItem).m_child = bpage.getChild(0);
        apage.setItem(halfSize, tmpItem);
      }
      apage.copyItems(halfSize + 1, apage.getMaxItemCount(),bpage,1);
      c.copyItems(s, mc - 1, c, s + 1);
      h = c.itemCountDecr();

      writePage(a, apage);
      releasePage(b);
    }
  } else { // s >= mc. b := page to the left of a
    KeyPageAddr b = c.getChild(s - 1);

    readPage(b, bpage);
    logPage( b, bpage);
    readPage(a, apage);
    logPage( a, apage);

    const int halfSize = bpage.getHalfSize();
    int mb = bpage.getItemCount() + 1;
    int k  = (mb - halfSize) / 2;
    if(k > 0) { // move k items from page b to a
      apage.copyItems(k + 1, halfSize - 1 + k, apage, 1);
      if(apage.isLeafPage()) {
        apage.setKey(k, c.getKey(s));
      } else {
        c.getItem(s, tmpItem).m_child = apage.getChild(0);
        apage.setItem(k, tmpItem);
      }
      mb -= k;
      apage.copyItems(1, k - 1, bpage, mb + 1);
      if(!apage.isLeafPage()) {
        apage.setChild(0, bpage.getChild(mb));
      }
      c.setItem( s, bpage.getItem(mb, tmpItem));
      c.setChild(s, a);
      bpage.setItemCount(mb - 1);
      apage.setItemCount(halfSize - 1 + k);
      h = false;

      writePage(a, apage);
      writePage(b, bpage);
    } else { // k == 0. merge pages a and b
      bpage.setItemCount(bpage.getMaxItemCount());
      if(bpage.isLeafPage()) {
        bpage.setKey(mb, c.getKey(s));
      } else {
        c.getItem(s, tmpItem).m_child = apage.getChild(0);
        bpage.setItem(mb, tmpItem);
      }
      bpage.copyItems(mb + 1, mb + halfSize - 1, apage, 1);
      h = c.itemCountDecr();
      releasePage(a);
      writePage(b, bpage);
    }
  }
}

void KeyFile::pageDel(KeyPageAddr p, KeyPage &a, int r, bool &h) {
  KeyPage ppage(m_pageInfo);

  readPage(p, ppage);
  const KeyPageAddr q = ppage.getLastChild(); // get the rightmost child
  if(q != DB_NULLADDR) {
    pageDel(q, a, r, h);
    if(h) {
      logPage(p, ppage);
      pageUnderflow(q, ppage, ppage.getItemCount(), h);
      writePage(p, ppage);
    }
  } else { // q == NULL => we are at a leafpage. a.key[r] = ppage.lastkey, and remove lastkey from ppage
    logPage(p, ppage);
    a.setKey(r, ppage.getLastKey());
    h = ppage.itemCountDecr();
    writePage(p, ppage);
  }
}

void KeyFile::pageDelete(KeyPageAddr a, const KeyType &key, bool &h) {
  if(a == DB_NULLADDR) { // key not in tree
    h = false;
    throwSqlError(SQL_NOT_FOUND,_T("Key <%s> not found"), m_keydef.sprintf(key, m_keydef.getKeyFieldCount()).cstr());
  } else {
    KeyPage apage(m_pageInfo);
    readPage(a, apage);

    int l = 1;
    int r = apage.getItemCount() + 1; // binary search
    while(l < r) {
      const int m = (l + r) / 2;
      if(m_keydef.keyCmp(apage.getKey(m),key) < 0) {
        l = m + 1;
      } else {
        r = m;
      }
    }

    const KeyPageAddr q = apage.getChild(r - 1);

    if(r <= (int)apage.getItemCount() && m_keydef.keyCmp(apage.getKey(r), key) == 0) { // found, now delete
      logPage(a, apage);

      if(q == DB_NULLADDR) { // a is a leafpage. Can remove the key with no need to find a substitute
        h = apage.removeItem(r);
      } else {
        pageDel(q, apage, r, h);
        if(h) {
          pageUnderflow(q, apage, r-1, h);
        }
      }
      writePage(a, apage);
    } else {
      pageDelete(q, key, h);
      if(h) {
        logPage(a, apage);
        pageUnderflow(q, apage, r-1, h);
        writePage(a, apage);
      }
    }
  }
}

void KeyFile::remove(const KeyType &key) {
  bool          h;
  KeyFileHeader header;

  readHead(header, true);

  KeyPageAddr rootAddr = header.m_root;

  pageDelete(rootAddr, key, h);

  if(h) { // base page size reduced
    KeyPage rootPage(m_pageInfo);
    readPage(rootAddr, rootPage);
    if(rootPage.getItemCount() == 0) {
      logPage(rootAddr, rootPage);
      KeyPageAddr q = rootAddr;
      setRoot(rootPage.getChild(0),-1);
      releasePage(q);
      return;
    }
  }
  setRoot(header.m_root, -1);
}
