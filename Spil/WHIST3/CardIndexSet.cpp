#include "stdafx.h"
#include <Exception.h>
#include "GameTypes.h"

void CardIndexSet::init(UINT count) {
  if(count > 13) {
    throwInvalidArgumentException(__TFUNCTION__, _T("count=%d. Must be [0..13]"), count);
  }
  m_bits = (1 << count) - 1;
}

bool CardIndexSet::contains(UINT i) const {
  if(i > 12) {
    throwInvalidArgumentException(__TFUNCTION__, _T("i=%d. Must be [0..12]"), i);
  }
  return (m_bits & (1 << i)) ? true : false;
}

void CardIndexSet::add(UINT i) {
  if(i > 12) {
    throwInvalidArgumentException(__TFUNCTION__, _T("i=%d. Must be [0..12]"),i);
  }
  m_bits |= (1<<i);
}

void CardIndexSet::remove(UINT i) {
  if(i > 12) {
    throwInvalidArgumentException(__TFUNCTION__, _T("i=%d. Must be [0..12]"),i);
  }
  m_bits &= ~(1<<i);
}

int CardIndexSet::selectRandom() const {
  DEFINEMETHODNAME;
  const int count = size();
  if(count == 0) {
    throwException(_T("%s:size = 0"), method);
  }
  int n = rand() % count;
  for(int i = 0, j = 0; i < 13; i++) {
    if(contains(i)) {
      if(j == n) {
        return i;
      }
      j++;
    }
  }
  throwException(_T("%s:Dropped to the end"), method);
  return -1;
}

int CardIndexSet::size() const {
  int count = 0;
  for(USHORT x = m_bits; x; x &= (x-1)) {
    count++;
  }
  return count;
}

class CardIndexSetIterator : public AbstractIterator {
private:
  const CardIndexSet &m_set;
  int                 m_next;
  int                 m_current;
  void first();
public:
  CardIndexSetIterator(const CardIndexSet &set);
  AbstractIterator *clone()       override;
  bool  hasNext()           const override { return m_next >= 0; }
  void *next()                    override;
  void  remove()                  override;
};

Iterator<int> CardIndexSet::getIterator() const {
  return Iterator<int>(new CardIndexSetIterator(*this));
}

CardIndexSetIterator::CardIndexSetIterator(const CardIndexSet &set) : m_set(set) {
  first();
}

AbstractIterator *CardIndexSetIterator::clone() {
  return new CardIndexSetIterator(*this);
}

void CardIndexSetIterator::first() {
  m_next = -1;
  if(!m_set.m_bits) {
    return;
  }
  for(int j = 0; j < 13; j++) { // we have a set bit
    if(m_set.contains(j)) {
      m_next = j;
      break;
    }
  }
}

void *CardIndexSetIterator::next() { // throw Exception if no more
  if(m_next < 0) {
    noNextElementError(__TFUNCTION__);
  }
  m_current = m_next;
  for(m_next++; m_next < 13; m_next++) {
    if(m_set.contains(m_next)) {
      break;
    }
  }
  if(m_next == 13) {
    m_next = -1;
  }
  return &m_current;
}

void CardIndexSetIterator::remove() {
  unsupportedOperationError(__TFUNCTION__);
}

void CardCount::setAll(UINT count) {
  for(int i = 0; i < 3; i++) {
    m_cardSet[i].init(count);
  }
}

void CardCount::decrementPlayer(UINT playerId) {
  if(playerId > 2) {
    throwInvalidArgumentException(__TFUNCTION__, _T("playerId=%lu. Must be [0..2]"), playerId);
  }
  CardIndexSet &set = m_cardSet[playerId];
  set.remove(set.selectRandom());
}

bool CardCount::hasCard(UINT playerId, UINT index) const {
  if(playerId > 2) {
    throwInvalidArgumentException(__TFUNCTION__, _T("playerId=%lu. Must be [0..2]"), playerId);
  }
  return m_cardSet[playerId].contains(index);
}

int CardCount::getCount(UINT playerId) const {
  if(playerId > 2) {
    throwInvalidArgumentException(__TFUNCTION__, _T("playerId=%lu. Must be [0..2]"), playerId);
  }
  return m_cardSet[playerId].size();
}


