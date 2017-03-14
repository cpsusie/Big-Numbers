#include "stdafx.h"
#include <stddef.h>

KeyPageInfo::KeyPageInfo(USHORT keySize) {
  init(keySize);
}

void KeyPageInfo::init(USHORT keySize) {
  if(keySize == 0) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage:Keysize=0"));
  }
  m_keySize          = keySize;
  m_halfMaxKeyCount  = KEY_DATASIZE/m_keySize/2;
  m_maxKeyCount      = 2*m_halfMaxKeyCount;
  m_pageSize         = sizeof(KeyPageHeader) + m_maxKeyCount*m_keySize;
  m_itemSize         = offsetof(KeyPageItem,m_key.m_data) + m_keySize;
  m_halfMaxItemCount = (m_maxKeyCount*m_keySize-sizeof(DbAddrFileFormat))/m_itemSize/2;
  m_maxItemCount     = 2*m_halfMaxItemCount;
  if(m_maxItemCount < 2) {
    throwSqlError(SQL_KEY_TOO_BIG,_T("KeyPageInfo:Keysize too big. (=%d). Max. itemcount=%d. Max. keycount=%d")
                                 ,m_keySize, m_maxItemCount, m_maxKeyCount);
  }
}

#define ISLEAFPAGE(page) (page)->m_h.m_leafPage
 
KeyPage::KeyPage(USHORT keySize) : m_pageInfo(keySize) {
  setItemCount(0);
}

KeyPage::KeyPage(const KeyPageInfo &pageInfo) : m_pageInfo(pageInfo) {
  setItemCount(0);
}

void KeyPage::init() {
  memset(m_items,0,sizeof(m_items));
  memset(&m_h,0,sizeof(m_h));
}

void KeyPage::setLeafPage(bool leafPage) {
  m_h.m_leafPage = leafPage ? 1 : 0;
}

bool KeyPage::isLeafPage() const {
  return ISLEAFPAGE(this) ? true : false;
}

UINT KeyPage::getItemSize() const {
  return m_pageInfo.m_itemSize;
}

UINT KeyPage::getKeySize() const {
  return m_pageInfo.m_keySize;
}

UINT KeyPage::getHalfSize() const {
  return ISLEAFPAGE(this) ? m_pageInfo.m_halfMaxKeyCount : m_pageInfo.m_halfMaxItemCount;
}

UINT KeyPage::getPageSize() const {
  return m_pageInfo.m_pageSize;
}

UINT KeyPage::getMaxItemCount() const {
  return ISLEAFPAGE(this) ? m_pageInfo.m_maxKeyCount : m_pageInfo.m_maxItemCount;
}

USHORT KeyPage::getItemCount() const { 
  return m_h.m_itemCount;
}

#define NONLEAFPAGE_ITEM(page,i) ((KeyPageItem*)(&((page)->m_items[sizeof(DbAddrFileFormat) + (page)->m_pageInfo.m_itemSize * ((i)-1)])))
#define LEAFPAGE_KEY(page,i)     ((KeyType*)&(page)->m_items[(page)->m_pageInfo.m_keySize*((i)-1)])

void KeyPage::setItem(UINT i, const KeyPageItem &t) {
#ifdef _DEBUG
  if(i < 1 || i > m_h.m_itemCount) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::getItem:Invalid argument:%d. Itemcount=%d\n")
                                 ,i, m_h.m_itemCount);
  }
#endif

  if(ISLEAFPAGE(this)) {
#ifdef _DEBUG
    if(t.m_child != DB_NULLADDR) {
      throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::setItem in leafpage with non-null child"));
    }
#endif
    setKey(i,t.m_key);
  } else {
    memcpy(NONLEAFPAGE_ITEM(this,i), &t, m_pageInfo.m_itemSize);
  }
}

KeyPageItem &KeyPage::getItem(UINT i, KeyPageItem &t) const {
#ifdef _DEBUG
  if(i < 1 || i > m_h.m_itemCount) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::getItem:Invalid argument:%d. Itemcount=%d\n")
                                 ,i, m_h.m_itemCount);
  }
#endif

  if(ISLEAFPAGE(this)) {
    memcpy(&t.m_key, &getKey(i), getKeySize());
    t.m_child = DB_NULLADDR;
  } else {
    memcpy(&t, NONLEAFPAGE_ITEM(this,i), getItemSize());
  }
  return t;
}

KeyPageItem &KeyPage::getLastItem(KeyPageItem &t)  const {
  return getItem(getItemCount(), t);
}

KeyType &KeyPage::setKey(UINT i, const KeyType &key) {
  if(ISLEAFPAGE(this)) {
    return *(KeyType*)memcpy(LEAFPAGE_KEY(this, i), &key, m_pageInfo.m_keySize);
  } else {
    return *(KeyType*)memcpy(&NONLEAFPAGE_ITEM(this, i)->m_key, &key, m_pageInfo.m_keySize);
  }
}

const KeyType &KeyPage::getKey(UINT i) const  {
  if(ISLEAFPAGE(this)) {
    return *LEAFPAGE_KEY(this,i);
  } else {
    return NONLEAFPAGE_ITEM(this,i)->m_key;
  }
}

const KeyType &KeyPage::getLastKey() const  {
  return getKey(getItemCount());
}

void KeyPage::copyItems(int from, int to, const KeyPage &src, int start) {
  const int amount = to - from + 1;
  if(amount == 0) {
    return;
  }

#ifdef _DEBUG
  if(ISLEAFPAGE(this) != ISLEAFPAGE(&src)) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::copyItems:copying items from %s to %s")
                                 ,src.getTypeStr(), getTypeStr());
  }
  if(amount < 0 || (start + amount - 1 > src.getItemCount())) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::copyItems:Invalid argument(%d,%d,%d,%d)")
                                 ,from, to, start, src.getItemCount());
  }
  if(m_pageInfo.m_keySize != src.m_pageInfo.m_keySize) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::copyItems:Keysize != src.keysize (=%d,%d)")
                                 ,getKeySize(), src.getKeySize());
  }
#endif

  if(ISLEAFPAGE(this)) {
    memmove(LEAFPAGE_KEY(this, from), LEAFPAGE_KEY(&src, start), amount * m_pageInfo.m_keySize);
  } else {
    memmove(NONLEAFPAGE_ITEM(this, from), NONLEAFPAGE_ITEM(&src, start), amount * m_pageInfo.m_itemSize);
  }
}

void KeyPage::setItemCount(UINT value) {
#ifdef _DEBUG
  if(value > getMaxItemCount()) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::setItemCount:Invalid argument (=%d). Max. itemcount=%d")
                                 ,value, getMaxItemCount());
  }
#endif
  m_h.m_itemCount = value;
}

void KeyPage::itemCountIncr() {
#ifdef _DEBUG
  if(isFull()) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::itemCountIncr:Page is full"));
  }
#endif
  m_h.m_itemCount++;
}

bool KeyPage::itemCountDecr() {
#ifdef _DEBUG
  if(m_h.m_itemCount == 0) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::itemCountDecr:Itemcount=0"));
  }
#endif
  return --m_h.m_itemCount < getHalfSize();
}

void KeyPage::insertItem(UINT i, const KeyPageItem &t) { 
#ifdef _DEBUG
  if(isFull()) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::insertItem:Page is full"));
  }
#endif

  if(ISLEAFPAGE(this)) {
#ifdef _DEBUG
    if(t.m_child != DB_NULLADDR) {
      throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::insertItem in leafpage with non-null-child"));
    }
#endif
    itemCountIncr();
    if(i < m_h.m_itemCount) {
      memmove(LEAFPAGE_KEY(this, i+1), LEAFPAGE_KEY(this, i), m_pageInfo.m_keySize * (m_h.m_itemCount - i));
    }
    setKey(i, t.m_key);
  } else {
    itemCountIncr();
    if(i < m_h.m_itemCount) {
      memmove(NONLEAFPAGE_ITEM(this, i+1), NONLEAFPAGE_ITEM(this, i), m_pageInfo.m_itemSize * (m_h.m_itemCount - i));
    }
    setItem(i, t);
  }
}

bool KeyPage::removeItem(UINT i) {
#ifdef _DEBUG
  if(m_h.m_itemCount == 0) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::removeItem:Page is empty"));
  }
  if(i > m_h.m_itemCount) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::removeItem: Invalid argument. i=%d. m_itemCount=%d"), i, m_h.m_itemCount);
  }
#endif

  if(i < m_h.m_itemCount) {
    if(ISLEAFPAGE(this)) {
      memmove(LEAFPAGE_KEY(this, i), LEAFPAGE_KEY(this, i+1), m_pageInfo.m_keySize * (m_h.m_itemCount - i));
    } else {
      memmove(NONLEAFPAGE_ITEM(this, i), NONLEAFPAGE_ITEM(this, i+1), m_pageInfo.m_itemSize * (m_h.m_itemCount - i));
    }
  }
  return itemCountDecr();
}

bool KeyPage::isFull() const {
  return m_h.m_itemCount >= getMaxItemCount();
}

void KeyPage::setChild(UINT i, KeyPageAddr addr) {
#ifdef _DEBUG
  if(i > getMaxItemCount()) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::setChild: Invalid argument. i=%d. Max. itemCount=%d")
                                 ,i, getMaxItemCount());
  }
#endif
  if(ISLEAFPAGE(this)) {
#ifdef _DEBUG
    if(addr != DB_NULLADDR) {
      throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::setChild to non-null in leafpage"));
    }
#endif
    return;
  }

  if(i == 0) {
    setP0(addr);
  } else {
    NONLEAFPAGE_ITEM(this,i)->m_child = addr;
  }
}

KeyPageAddr KeyPage::getChild(UINT i) const {
#ifdef _DEBUG
  if(i > getMaxItemCount()) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::getChild: Invalid argument. i=%d. Max. itemCount=%d")
                                 ,i, getMaxItemCount());
  }
#endif

  return ISLEAFPAGE(this) ? DB_NULLADDR : (i ? NONLEAFPAGE_ITEM(this,i)->m_child : getP0());
}

KeyPageAddr KeyPage::getLastChild() const {
  return getChild(m_h.m_itemCount);
}

KeyPageAddr KeyPage::getNextFree() const {
  return getP0();
}

void KeyPage::setNextFree(KeyPageAddr addr) {
  init();
  setLeafPage(false);
  setP0(addr);
}

KeyPageAddr KeyPage::getP0() const {
#ifdef _DEBUG
  if(ISLEAFPAGE(this)) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::getP0. Page is a leafPage"));
  }
#endif

  DbAddrFileFormat &addrff = *(DbAddrFileFormat*)m_items;
  return addrff;
}

void KeyPage::setP0(const KeyPageAddr &addr) {
#ifdef _DEBUG
  if(ISLEAFPAGE(this)) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::setP0. Page is a leafPage"));
  }
#endif

  DbAddrFileFormat addrff;
  addrff = addr;
  memcpy(m_items, &addrff, sizeof(DbAddrFileFormat));
}

/* Returns min index of page.item where keynCmp(item,key,fieldCount) >= 0. 
   If none returns itemcount+1 
*/
int KeyPage::searchMinIndex(const KeyType &key, const KeyFileDefinition &keydef, UINT fieldCount) {
  int l = 1;
  int r = getItemCount() + 1;
  while(l<r) {
    const int m = (l + r) / 2;
    if(keydef.keynCmp(getKey(m), key, fieldCount) < 0) {
      l = m + 1;
    } else {
      r = m;
    }
  }
  return r;
}

/* Returns max index of page.item where keynCmp(item,key,fieldCount) <= 0. 
   If none returns 0 
*/
int KeyPage::searchMaxIndex(const KeyType &key, const KeyFileDefinition &keydef, UINT fieldCount) {
  int l = 1;
  int r = getItemCount() + 1;

  while(l<r) {
    const int m = (l + r) / 2;
    if(keydef.keynCmp(getKey(m), key, fieldCount) <= 0) {
      l = m + 1;
    } else {
      r = m;
    }
  }
  return r - 1;
}

const TCHAR *KeyPage::getTypeStr() const {
  return ISLEAFPAGE(this) ? _T("leafpage") : _T("non leafpage");
}
