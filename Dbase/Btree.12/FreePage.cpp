#include "stdafx.h"

const FreePageItem &FreePage::getItem(UINT i) const {
  if(i < 1 || i > m_itemCount) {
    throwSqlError(SQL_FATAL_ERROR,_T("FreePage::getItem:Invalid argument:%u. Itemcount=%u\n"),i,m_itemCount);
  }
  return(m_e[i-1]);
}

void  FreePage::setItem(UINT i, const FreePageItem &t) {
  (FreePageItem&)getItem(i) = t;
}

void FreePage::copyItems(int from, int to, const FreePage &src, int start) {
  const int amount = to - from + 1;
  if(amount == 0) {
    return;
  }
  if(amount < 0 || (start + amount - 1 > (int)src.getItemCount())) {
    throwSqlError(SQL_FATAL_ERROR,_T("FreePage::copyItems:Invalid argument(%d,%d,%d,%d)"),from,to,start,src.getItemCount());
  }

  memmove((void*)&getItem(from),&src.getItem(start),amount * sizeof(FreePageItem));
}

void FreePage::setItemCount(int value) {
  if(value < 0 || value > FREEPAGEMAXCOUNT) {
    throwSqlError(SQL_FATAL_ERROR,_T("FreePage::setItemCount:Invalid argument (=%d). maxitemcount=%d"),value,FREEPAGEMAXCOUNT);
  }
  m_itemCount = value;
}

void FreePage::itemCountIncr() {
  if(m_itemCount >= FREEPAGEMAXCOUNT) {
    throwSqlError(SQL_FATAL_ERROR,_T("FreePage::itemCountIncr:Itemcount exceeds maxitemcount (=%d)"),FREEPAGEMAXCOUNT);
  }
  m_itemCount++;
}

void FreePage::itemCountDecr() {
  if(m_itemCount == 0) {
    throwSqlError(SQL_FATAL_ERROR,_T("FreePage::itemCountDecr:Itemcount is already 0"));
  }
  m_itemCount--;
}

void FreePage::insertItem(UINT i, const FreePageItem &t) {
  if(m_itemCount >= FREEPAGEMAXCOUNT) {
    throwSqlError(SQL_FATAL_ERROR,_T("FreePage::insertItem:Page is already full"));
  }

  itemCountIncr();

  if(i < m_itemCount) {
    memmove((void*)&getItem(i+1),&getItem(i),sizeof(FreePageItem) * (m_itemCount - i));
  }

  setItem(i,t);
}

void FreePage::removeItem(UINT i) {
  if(m_itemCount == 0) {
    throwSqlError(SQL_FATAL_ERROR,_T("FreePage::removeItem:Page is already empty"));
  }
  if(i > m_itemCount) {
    throwSqlError(SQL_FATAL_ERROR,_T("FreePage::removeItem:Invalid argument. i=%d. itemcount=%d"),i,m_itemCount);
  }

  if(i < m_itemCount) {
    memmove((void*)&getItem(i),&getItem(i+1),sizeof(FreePageItem) * (m_itemCount - i));
  }

  itemCountDecr();
}

DbAddr FreePage::getChild(UINT i) const {
  return i == 0 ? m_p0 : getItem(i).m_child;
}

void FreePage::setChild(UINT i, DbAddr addr)  {
  if(i == 0) {
    m_p0 = addr;
  } else {
    ((FreePageItem&)getItem(i)).m_child = addr;
  }
}
