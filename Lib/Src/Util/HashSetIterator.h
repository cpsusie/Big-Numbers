#pragma once

#include "HashSetTable.h"

class HashSetIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  HashSetImpl        *m_set;
  HashSetNode        *m_next, *m_current;
  size_t              m_updateCount;

  void checkUpdateCount() const;
protected:
  HashSetImpl &getSet() {
    return *m_set;
  }
  HashSetNode *nextNode();
public:
  HashSetIterator(HashSetImpl &set);
  AbstractIterator *clone()         override {
    return new HashSetIterator(*this);
  }
  bool              hasNext() const override {
    return m_next != nullptr;
  }
  void             *next()          override {
    return (void*)(nextNode()->key());
  }
  void remove()                     override;
};
