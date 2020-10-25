#include "pch.h"
#include <TreeSet.h>
#include "TreeSetNode.h"

TreeSetImpl::TreeSetImpl(const AbstractObjectManager &objectManager, const AbstractComparator &comparator)
: m_deleteHelper(nullptr)
{
  m_objectManager = objectManager.clone(); TRACE_NEW(m_objectManager);
  m_comparator    = comparator.clone();    TRACE_NEW(m_comparator);
  m_root          = nullptr;
  m_size          = 0;
  m_updateCount   = 0;
}

AbstractCollection *TreeSetImpl::clone(bool cloneData) const {
  TreeSetImpl      *clone = nullptr;
  AbstractIterator *it    = nullptr;
  clone = new TreeSetImpl(*m_objectManager, *m_comparator);
  try {
    if(cloneData) {
      it = getIterator(); TRACE_NEW(it);
      while(it->hasNext()) {
        clone->add(it->next());
      }
      SAFEDELETE(it);
    }
  } catch(...) {
    TRACE_NEW( clone);
    SAFEDELETE(it   );
    SAFEDELETE(clone);
    throw;
  }
  return clone;
}

TreeSetImpl::~TreeSetImpl() {
  clear();
  SAFEDELETE(m_comparator);
  SAFEDELETE(m_objectManager);
}

TreeSetNode *TreeSetImpl::allocateNode() const {
  TreeSetNode *n = new TreeSetNode(); TRACE_NEW(n); return n;
}

TreeSetNode *TreeSetImpl::createNode(const void *key) const {
  TreeSetNode *n = allocateNode();
  n->m_balance = 0;
  n->m_left    = n->m_right = nullptr;
  n->m_key     = m_objectManager->cloneObject(key);
  return n;
}

TreeSetNode *TreeSetImpl::cloneNode(TreeSetNode *n) const {
  return createNode(n->m_key);
}

void TreeSetImpl::deleteNode(TreeSetNode *n) const {
  m_objectManager->deleteObject(n->m_key);
  n->m_key = nullptr;
  SAFEDELETE(n);
}

void TreeSetImpl::swapContent(TreeSetNode *p1, TreeSetNode *p2) {
  void *tmp = p1->m_key;
  p1->m_key = p2->m_key;
  p2->m_key = tmp;
}

// Insert n in pp. Returns true if pp needs rebalancing. false if not
bool TreeSetImpl::nodeInsert(TreeSetNode *&pp, TreeSetNode *n) {
  TreeSetNode *p = pp;

  if(!p) {
    pp = p = n;
    m_size++;
    m_updateCount++;
    return true;
  }
  const int c = m_comparator->cmp(p->m_key, n->m_key);
  if(c > 0) {                          // pp.key > n.key. Insert n in left subtree.
    if(!nodeInsert(p->m_left, n)) {
      return false;
    }

    switch(p->m_balance) {             // Left subtree has grown.
    case 1 :
      p->m_balance =  0;
      return false;
    case 0 :
      p->m_balance = -1;
      return true;
    case -1:                           // Rebalance
      { TreeSetNode *p1 = p->m_left;
        if(p1->m_balance == -1) {      // Single LL rotation
          p->m_left       = p1->m_right;
          p1->m_right     = p;
          p->m_balance    = 0;
          pp = p = p1;
        } else {                       // Double LR rotation
          TreeSetNode *p2 = p1->m_right;
          p1->m_right     = p2->m_left;
          p2->m_left      = p1;
          p->m_left       = p2->m_right;
          p2->m_right     = p;
          p->m_balance    = (p2->m_balance == -1) ?  1 : 0;
          p1->m_balance   = (p2->m_balance ==  1) ? -1 : 0;
          pp = p = p2;
        }
        p->m_balance = 0;
        break;
      }
    }
    return false;
  }

  if(c < 0) {                          // pp.key < n.key. Insert n in right subtree.
    if(!nodeInsert(p->m_right, n)) {
      return false;
    }

    switch(p->m_balance) {             // Right subtree has grown.
    case -1:
      p->m_balance = 0;
      return false;
    case  0:
      p->m_balance = 1;
      return true;
    case  1:                           // Rebalance
      { TreeSetNode *p1 = p->m_right;
        if(p1->m_balance == 1) {       // Single RR rotation
          p->m_right      = p1->m_left;
          p1->m_left      = p;
          p->m_balance    = 0;
          pp = p = p1;
        } else {                       // Double RL rotation
          TreeSetNode *p2 = p1->m_left;
          p1->m_left      = p2->m_right;
          p2->m_right     = p1;
          p->m_right      = p2->m_left;
          p2->m_left      = p;
          p->m_balance    = (p2->m_balance ==  1) ? -1 : 0;
          p1->m_balance   = (p2->m_balance == -1) ?  1 : 0;
          pp = p = p2;
        }
        p->m_balance = 0;
        break;
      }
    }
    return false;
  }
  return false;                        // Duplicate key. Dont insert.
}

bool TreeSetImpl::balanceL(TreeSetNode *&pp) {
  TreeSetNode *p = pp;

  switch(p->m_balance) {
  case -1:
    p->m_balance = 0;
    return true;
  case  0:
    p->m_balance = 1;
    return false;
  case  1:                             // Rebalance
    { TreeSetNode *p1 = p->m_right;
      char b = p1->m_balance;
      if(b >= 0) {                     // Single RR rotation
        p->m_right = p1->m_left;
        p1->m_left = p;
        if(b == 0) {                   // p->right->balance == 0
          p->m_balance  =  1;
          p1->m_balance = -1;
          pp = p1;
          return false;
        } else {                       // p->right->balance > 0
          p->m_balance  = 0;
          p1->m_balance = 0;
          pp = p1;
          return true;
        }
      } else {                         // p->right->balance < 0. Double RL rotation
        TreeSetNode *p2 = p1->m_left;
        b               = p2->m_balance;
        p1->m_left      = p2->m_right;
        p2->m_right     = p1;
        p->m_right      = p2->m_left;
        p2->m_left      = p;
        p->m_balance    = (b ==  1) ? -1 : 0;
        p1->m_balance   = (b == -1) ?  1 : 0;
        p2->m_balance   = 0;
        pp              = p2;
        return true;
      }
    }
  }
  throwException(_T("%s:Dropped to the end"), __TFUNCTION__);
  return true;
}

bool TreeSetImpl::balanceR(TreeSetNode *&pp) {
  TreeSetNode *p = pp;

  switch(p->m_balance) {
  case  1:
    p->m_balance =  0;
    return true;
  case  0:
    p->m_balance = -1;
    return false;
  case -1:                             // Rebalance
    { TreeSetNode *p1 = p->m_left;
      char b = p1->m_balance;
      if(b <= 0) {                     // Single LL rotation
        p->m_left   = p1->m_right;
        p1->m_right = p;
        if(b == 0) {                   // p->left->balance == 0
          p->m_balance  = -1;
          p1->m_balance =  1;
          pp = p1;
          return false;
        } else {                       // p->left->balance < 0
          p->m_balance  = 0;
          p1->m_balance = 0;
          pp = p1;
          return true;
        }
      } else {                         // p->left->balance > 0. Double LR rotation
        TreeSetNode *p2 = p1->m_right;
        b               = p2->m_balance;
        p1->m_right     = p2->m_left;
        p2->m_left      = p1;
        p->m_left       = p2->m_right;
        p2->m_right     = p;
        p->m_balance    = (b == -1) ?  1 : 0;
        p1->m_balance   = (b ==  1) ? -1 : 0;
        p2->m_balance   = 0;
        pp              = p2;
        return true;
      }
    }
  }
  throwException(_T("%s:dropped to the end"), __TFUNCTION__);
  return true;
}


// Replace del_help with the rightmost node in pp
bool TreeSetImpl::nodeDel(TreeSetNode *&pp) {
  if(pp->m_right) {
    if(nodeDel(pp->m_right)) {
      return balanceR(pp);
    } else {
      return false;
    }
  }
  swapContent(m_deleteHelper,pp);
  m_deleteHelper = pp;
  pp = m_deleteHelper->m_left;
  return true;
}

// Remove key from pp. Returns true if pp needs rebalancing. false if not
bool TreeSetImpl::nodeDelete(TreeSetNode *&pp, const void *key) {
  TreeSetNode *p = pp;

  if(p == nullptr) {
    return false;                      // key not found
  }

  const int c = m_comparator->cmp(p->m_key, key);
  if(c > 0) {                          // pp.key > key. Continue search in left subtree
    if(nodeDelete(p->m_left, key)) {
      return balanceL(pp);
    } else {
      return false;
    }
  }
  if(c < 0) {                          // pp.key < key. Continue search in right subtree
    if(nodeDelete(p->m_right, key)) {
      return balanceR(pp);
    } else {
      return false;
    }
  }

  bool result = false;
  if(p->m_right == nullptr) {
    pp = p->m_left;
    result = true;
  } else if(p->m_left == nullptr) {
    pp = p->m_right;
    result = true;
  } else {
    m_deleteHelper = p;
    if(nodeDel(p->m_left)) {
      result = balanceL(pp);
    }
    p = m_deleteHelper;
  }

  deleteNode(p);

  m_size--;
  m_updateCount++;

  return result;
}

void TreeSetImpl::deleteNodeRecurse(TreeSetNode *n) {
  if(n->m_left) {
    deleteNodeRecurse(n->m_left);
  }
  if(n->m_right) {
    deleteNodeRecurse(n->m_right);
  }
  deleteNode(n);
}

void TreeSetImpl::clear() {
  if(m_root != nullptr) {
    deleteNodeRecurse(m_root);
    m_root = nullptr;
    m_size = 0;
    m_updateCount++;
  }
}

bool TreeSetImpl::insertNode(TreeSetNode *n) {
  const size_t size = m_size;
  nodeInsert(m_root, n);
  return size != m_size;
}

bool TreeSetImpl::add(const void *key) {
  TreeSetNode *newNode = createNode(key);
  const bool result = insertNode(newNode);
  if(!result) {
    deleteNode(newNode);
  }
  return result;
}

bool TreeSetImpl::remove(const void *key) {
  const size_t size = m_size;
  nodeDelete(m_root, key);
  return size != m_size;
}

TreeSetNode *TreeSetImpl::findNode(const void *key) const {
  TreeSetNode *p = m_root;

  while(p) {
    const int c = m_comparator->cmp(p->m_key, key);
    if(c > 0) {                        // p->key > key
      p = p->m_left;
    } else if(c < 0) {                 // p->key < key
      p = p->m_right;
    } else {
      return p;
    }
  }
  return nullptr;
}

bool TreeSetImpl::contains(const void *key) const {
  return findNode(key) != nullptr;
}

TreeSetNode *TreeSetImpl::getMinNode() const {
  TreeSetNode *result = nullptr;
  for(auto p = m_root; p; result = p, p = p->m_left);
  if(result == nullptr) {
    throwEmptySetException(__TFUNCTION__);
  }
  return result;
}

TreeSetNode *TreeSetImpl::getMaxNode() const {
  TreeSetNode *result = nullptr;
  for(auto p = m_root; p; result = p, p = p->m_right);
  if(result == nullptr) {
    throwEmptySetException(__TFUNCTION__);
  }
  return result;
}

void *TreeSetImpl::getMin() const {
  return (void*)(getMinNode()->key());
}

void *TreeSetImpl::getMax() const {
  return (void*)(getMaxNode()->key());
}
