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

String KeyPageInfo::toString() const {
  return format(_T("KeyPageInfo:\n"
                   "   KeySize          : %u\n"
                   "   HalfMaxKeyCount  : %u\n"
                   "   MaxKeyCount      : %u\n"
                   "   ItemSize         : %u\n"
                   "   HalfMaxItemCount : %u\n"
                   "   MaxItemCount     : %u\n"
                   "   PageSize         : %u\n")
               ,m_keySize
               ,m_halfMaxKeyCount
               ,m_maxKeyCount
               ,m_itemSize
               ,m_halfMaxItemCount
               ,m_maxItemCount
               ,m_pageSize
               );
}

String KeyType::toString(const KeyFileDefinition &keydef) const {
  return keydef.sprintf(*this);
}

String KeyPageHeader::toString() const {
  return format(_T("KeyPageHeader:%s %u items"), m_leafPage?_T("Leaf   "):_T("Nonleaf"), m_itemCount);
}

String KeyPageItem::toString(const KeyFileDefinition &keydef) const {
  return format(_T("KeyPageItem:<Key:%s, Child:%s>"), m_key.toString(keydef).cstr(), ::toString(m_child).cstr());
}

KeyPage::KeyPage(USHORT keySize) : m_pageInfo(keySize) {
  init(false);
}

KeyPage::KeyPage(const KeyPageInfo &pageInfo) : m_pageInfo(pageInfo) {
  init(false);
}

void KeyPage::init(bool leafPage) {
  memset(m_items,0,sizeof(m_items));
  m_header.m_itemCount = 0;
  m_header.m_leafPage  = leafPage ? 1 : 0;
}

#define NONLEAFPAGE_ITEM(page,i) ((KeyPageItem*)(&((page)->m_items[sizeof(DbAddrFileFormat) + (page)->m_pageInfo.m_itemSize * ((i)-1)])))
#define LEAFPAGE_KEY(page,i)     ((KeyType*)&(page)->m_items[(page)->m_pageInfo.m_keySize*((i)-1)])

void KeyPage::setItem(UINT i, const KeyPageItem &t) {
#ifdef _DEBUG
  if(i < 1 || i > m_header.m_itemCount) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::getItem:Invalid argument:%d. Itemcount=%d\n")
                                 ,i, m_header.m_itemCount);
  }
#endif

  if(isLeafPage()) {
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
  if(i < 1 || i > m_header.m_itemCount) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::getItem:Invalid argument:%d. Itemcount=%d\n")
                                 ,i, m_header.m_itemCount);
  }
#endif

  if(isLeafPage()) {
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
  if(isLeafPage()) {
    return *(KeyType*)memcpy(LEAFPAGE_KEY(this, i), &key, m_pageInfo.m_keySize);
  } else {
    return *(KeyType*)memcpy(&NONLEAFPAGE_ITEM(this, i)->m_key, &key, m_pageInfo.m_keySize);
  }
}

const KeyType &KeyPage::getKey(UINT i) const  {
  if(isLeafPage()) {
    return *LEAFPAGE_KEY(this,i);
  } else {
    return NONLEAFPAGE_ITEM(this,i)->m_key;
  }
}

void KeyPage::copyItems(int from, int to, const KeyPage &src, int start) {
  const int amount = to - from + 1;
  if(amount == 0) {
    return;
  }

#ifdef _DEBUG
  if(isLeafPage() != src.isLeafPage()) {
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

  if(isLeafPage()) {
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
  m_header.m_itemCount = value;
}

void KeyPage::itemCountIncr() {
#ifdef _DEBUG
  if(isFull()) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::itemCountIncr:Page is full"));
  }
#endif
  m_header.m_itemCount++;
}

bool KeyPage::itemCountDecr() {
#ifdef _DEBUG
  if(m_header.m_itemCount == 0) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::itemCountDecr:Itemcount=0"));
  }
#endif
  return --m_header.m_itemCount < getHalfSize();
}

void KeyPage::insertItem(UINT i, const KeyPageItem &t) {
#ifdef _DEBUG
  if(isFull()) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::insertItem:Page is full"));
  }
#endif

  if(isLeafPage()) {
#ifdef _DEBUG
    if(t.m_child != DB_NULLADDR) {
      throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::insertItem in leafpage with non-null-child"));
    }
#endif
    itemCountIncr();
    if(i < m_header.m_itemCount) {
      memmove(LEAFPAGE_KEY(this, i+1), LEAFPAGE_KEY(this, i), m_pageInfo.m_keySize * (m_header.m_itemCount - i));
    }
    setKey(i, t.m_key);
  } else {
    itemCountIncr();
    if(i < m_header.m_itemCount) {
      memmove(NONLEAFPAGE_ITEM(this, i+1), NONLEAFPAGE_ITEM(this, i), m_pageInfo.m_itemSize * (m_header.m_itemCount - i));
    }
    setItem(i, t);
  }
}

bool KeyPage::removeItem(UINT i) {
#ifdef _DEBUG
  if(m_header.m_itemCount == 0) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::removeItem:Page is empty"));
  }
  if(i > m_header.m_itemCount) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::removeItem: Invalid argument. i=%d. m_itemCount=%d"), i, m_header.m_itemCount);
  }
#endif

  if(i < m_header.m_itemCount) {
    if(isLeafPage()) {
      memmove(LEAFPAGE_KEY(this, i), LEAFPAGE_KEY(this, i+1), m_pageInfo.m_keySize * (m_header.m_itemCount - i));
    } else {
      memmove(NONLEAFPAGE_ITEM(this, i), NONLEAFPAGE_ITEM(this, i+1), m_pageInfo.m_itemSize * (m_header.m_itemCount - i));
    }
  }
  return itemCountDecr();
}

void KeyPage::setChild(UINT i, KeyPageAddr addr) {
#ifdef _DEBUG
  if(i > getMaxItemCount()) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::setChild: Invalid argument. i=%d. Max. itemCount=%d")
                                 ,i, getMaxItemCount());
  }
#endif
  if(isLeafPage()) {
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

  return isLeafPage() ? DB_NULLADDR : (i ? NONLEAFPAGE_ITEM(this,i)->m_child : getP0());
}

KeyPageAddr KeyPage::getNextFree() const {
  return getP0();
}

void KeyPage::setNextFree(KeyPageAddr addr) {
  init(false);
  setP0(addr);
}

KeyPageAddr KeyPage::getP0() const {
#ifdef _DEBUG
  if(isLeafPage()) {
    throwSqlError(SQL_FATAL_ERROR,_T("KeyPage::getP0. Page is a leafPage"));
  }
#endif

  DbAddrFileFormat &addrff = *(DbAddrFileFormat*)m_items;
  return addrff;
}

void KeyPage::setP0(const KeyPageAddr &addr) {
#ifdef _DEBUG
  if(isLeafPage()) {
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
  return isLeafPage() ? _T("leafpage") : _T("non leafpage");
}

String KeyPage::toString(const KeyFileDefinition &keydef) const {
  String result = m_header.toString();
  if(!isLeafPage()) {
    result += format(_T("  P0  :%8s\n"), ::toString(getP0()).cstr());
  }
  for(UINT i = 1; i <= getItemCount(); i++) {
    if(isLeafPage()) {
      result += format(_T("  %3d:%s\n"), i, getKey(i).toString(keydef).cstr());
    } else {
      KeyPageItem item;
      getItem(i, item);
      result += format(_T("  P%-3d:%8s:%s\n"), i, ::toString(item.m_child).cstr(), item.m_key.toString(keydef).cstr());
    }
  }
  return result;
}
