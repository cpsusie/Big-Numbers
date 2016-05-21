#include "pch.h"
#include <TreeSet.h>

DEFINECLASSNAME(TreeSetIterator);

AbstractIterator *TreeSetIterator::clone() {
  return new TreeSetIterator(*this);
}

enum IteratorState {
  LEFT_DONE
 ,LEFT_FOR
 ,RIGHT_FOR
 ,RIGHT_DONE
};

void TreeSetIterator::push(TreeSetNode *node, char state) { 
  m_stack.push(TreeSetIteratorStackElement(node,state));
}

TreeSetIteratorStackElement *TreeSetIterator::top() { 
  return m_stack.isEmpty() ? NULL : &m_stack.top();
}

TreeSetIterator::TreeSetIterator(TreeSetImpl &set) : m_set(set) {
  m_updateCount = m_set.m_updateCount;
  m_next        = findFirst();
  m_current     = NULL;
}

bool TreeSetIterator::hasNext() const { 
  return m_next != NULL;
}

TreeSetNode *TreeSetIterator::nextNode() {
  if(m_next == NULL) {
    noNextElementError(s_className);
  }
  checkUpdateCount();
  m_current = m_next;
  m_next    = findNext();
  return m_current;
}

TreeSetNode *TreeSetIterator::findFirst() {
  m_stack.clear();
  if(m_set.m_root) {
    push(m_set.m_root, LEFT_FOR);
  }
  return findNext();
}

void TreeSetIterator::findPath(const void *key) {
  m_stack.clear();
  for(TreeSetNode *p = m_set.m_root; p;) {
    int c = m_set.m_comparator->cmp(p->key(), key);
    if(c > 0) {                    // p->key > key
      push(p,LEFT_DONE);
      p = p->left();
    } else if (c < 0) {            // p->key < key
      push(p,RIGHT_DONE);
      p = p->right();
    } else {
      push(p,RIGHT_FOR);
      return;
    }
  }
  throwException(_T("%s::findPath:Key not found"), s_className);
}

TreeSetNode *TreeSetIterator::findNext() {
  for(TreeSetIteratorStackElement *sp = top(); sp; sp = top()) {
    TreeSetNode *n = sp->m_node;
    switch(sp->m_state) {
    case LEFT_DONE :
      sp->m_state = RIGHT_FOR;
      return n;

    case LEFT_FOR  :
      if(n->left()) {
        sp->m_state = LEFT_DONE;
        push(n->left(), LEFT_FOR);
      } else {
        sp->m_state = RIGHT_FOR;
        return n;
      }
      break;

    case RIGHT_FOR :
      if(n->right()) {
        sp->m_state = RIGHT_DONE;
        push(n->right(), LEFT_FOR);
      } else {
        pop();
      }
      break;

    case RIGHT_DONE:
      pop();
      break;

    default:
      throwException(_T("%s::findNext:Illegal state:%d"), s_className, sp->m_state);
    }
  }
  return NULL;
}

void TreeSetIterator::remove() {
  if(m_current == NULL) {
    noCurrentElementError(s_className);
  }
  checkUpdateCount();
  m_set.remove(m_current->key());
  m_current = NULL;
  if(m_next != NULL) {
    findPath(m_next->key());
  }
  m_updateCount = m_set.m_updateCount;
}

void TreeSetIterator::checkUpdateCount() const {
  if(m_updateCount != m_set.m_updateCount) {
    concurrentModificationError(s_className);
  }
}
