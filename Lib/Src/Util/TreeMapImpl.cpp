#include "pch.h"
#include <TreeMap.h>

TreeMapImpl::TreeMapImpl(const AbstractObjectManager &keyManager, const AbstractObjectManager &dataManager, const AbstractComparator &comparator)
: TreeSetImpl(keyManager, comparator)
{
  m_dataManager = dataManager.clone(); TRACE_NEW(m_dataManager);
}

TreeMapImpl::~TreeMapImpl() {
  clear();
  SAFEDELETE(m_dataManager);
}

class TreeMapIterator : public TreeSetIterator {
public:
  TreeMapIterator(TreeSetImpl &set) : TreeSetIterator(set) {
  }
  AbstractIterator *clone() {
    return new TreeMapIterator(*this);
  }
  void *next();
};

void *TreeMapIterator::next() {
  return (AbstractEntry*)((TreeMapNode*)nextNode());
}

AbstractIterator *TreeMapImpl::getIterator() {
  return new TreeMapIterator(*this);
}

AbstractMap *TreeMapImpl::cloneMap(bool cloneData) const {
  TreeMapImpl *clone = new TreeMapImpl(*TreeSetImpl::getObjectManager(),*m_dataManager,*getComparator());
  if(cloneData) {
    AbstractIterator *it = ((TreeMapImpl*)this)->getIterator(); TRACE_NEW(it);
    while(it->hasNext()) {
      AbstractEntry *n = (AbstractEntry*)it->next();
      clone->put(n->key(), n->value());
    }
    SAFEDELETE(it);
  }
  return clone;
}

TreeSetNode *TreeMapImpl::allocateNode() const {
  TreeMapNode *n = new TreeMapNode(); TRACE_NEW(n); return n;
}

TreeSetNode *TreeMapImpl::createNode(const void *key, const void *value) const {
  TreeMapNode *n = (TreeMapNode*)TreeSetImpl::createNode(key);
  n->m_value = m_dataManager->cloneObject(value);
  return n;
}

TreeSetNode *TreeMapImpl::cloneNode(TreeSetNode *n) const {
  TreeMapNode *mn = (TreeMapNode*)n;
  return createNode(mn->key(), mn->m_value);
}

void TreeMapImpl::deleteNode(TreeSetNode *n) const {
  TreeMapNode *mn = (TreeMapNode*)n;
  m_dataManager->deleteObject(mn->m_value);
  TreeSetImpl::deleteNode(n);
}

void TreeMapImpl::swapContent(TreeSetNode *p1, TreeSetNode *p2) {
  TreeMapNode *mp1 = (TreeMapNode*)p1;
  TreeMapNode *mp2 = (TreeMapNode*)p2;

  TreeSetImpl::swapContent(p1,p2);
  void *tmp    = mp1->m_value;
  mp1->m_value = mp2->m_value;
  mp2->m_value = tmp;
}

bool TreeMapImpl::put(const void *key, void *value) {
  TreeSetNode *newNode = createNode(key, value);
  bool ret = TreeSetImpl::insertNode(newNode);
  if(!ret) {
    deleteNode(newNode);
  }
  return ret;
}

bool TreeMapImpl::put(const void *key, const void *value) {
  TreeSetNode *newNode = createNode(key, value);
  bool ret = TreeSetImpl::insertNode(newNode);
  if(!ret) {
    deleteNode(newNode);
  }
  return ret;
}

bool TreeMapImpl::remove(const void *key) {
  return TreeSetImpl::remove(key);
}

void *TreeMapImpl::get(const void *key) {
  TreeSetNode *n = TreeSetImpl::findNode(key);
  return (n != NULL) ? ((TreeMapNode*)n)->value() : NULL;
}

const void *TreeMapImpl::get(const void *key) const {
  const TreeSetNode *n = TreeSetImpl::findNode(key);
  return (n != NULL) ? ((TreeMapNode*)n)->value() : NULL;
}

AbstractEntry *TreeMapImpl::selectEntry() const {
  if(size() == 0) {
    throwException("TreeMap::select:Map is empty");
  }
  return (TreeMapNode*)findNode(select());
}

const AbstractEntry *TreeMapImpl::getMinEntry() const {
  return (TreeMapNode*)TreeSetImpl::getMinNode();
}

const AbstractEntry *TreeMapImpl::getMaxEntry() const {
  return (TreeMapNode*)TreeSetImpl::getMaxNode();
}
