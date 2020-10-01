#pragma once

#include <TreeSet.h>

class TreeSetNode : public AbstractKey {
private:
  void *m_key;
  char  m_balance;
  TreeSetNode *m_left, *m_right;
public:
  const void *key() const {
    return m_key;
  }

  TreeSetNode *left() {
    return m_left;
  }

  TreeSetNode *right() {
    return m_right;
  }

  friend class TreeSetImpl;
};

class TreeSetIteratorStackElement {
public:
  TreeSetNode *m_node;
  char         m_state;

  inline TreeSetIteratorStackElement(TreeSetNode *node, char state) {
    m_node  = node;
    m_state = state;
  }

  inline TreeSetIteratorStackElement() {
  }
};

class TreeSetIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  TreeSetImpl                              &m_set;
  CompactStack<TreeSetIteratorStackElement> m_stack;
  TreeSetNode                              *m_next, *m_current;
  size_t                                    m_updateCount;

  inline void push(TreeSetNode *node, char state) {
    m_stack.push(TreeSetIteratorStackElement(node,state));
  }
  inline void pop() {
    m_stack.pop();
  }
  inline TreeSetIteratorStackElement *top() {
    return m_stack.isEmpty() ? nullptr : &m_stack.top();
  }
  TreeSetNode      *findFirst();
  void              findPath(const void *key);
  TreeSetNode      *findNext();
  void              checkUpdateCount() const;
protected:
  TreeSetNode      *nextNode();
public:
  TreeSetIterator(TreeSetImpl &set);
  AbstractIterator *clone()                  override;
  bool              hasNext()          const override;
  void             *next()                   override {
    return (void*)(nextNode()->key());
  }
  void              remove()                 override;
};
