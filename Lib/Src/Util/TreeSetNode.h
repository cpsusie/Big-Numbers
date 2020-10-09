#pragma once

#include <Set.h>

class TreeSetNode : public AbstractKey {
  friend class TreeSetImpl;
private:
  void *m_key;
  char  m_balance;
  TreeSetNode *m_left, *m_right;
public:
  const void *key() const override {
    return m_key;
  }
  inline TreeSetNode *left() {
    return m_left;
  }
  inline TreeSetNode *right() {
    return m_right;
  }
};
