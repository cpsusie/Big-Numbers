#include "stdafx.h"
#include <LineTracer.h>
#include "MemBtree.h"

// ------------------------------------BTreePage----------------------------------------------------

void BTreePage::setItemCount(int value) {
  if(value < 0 || value > MAXITEMCOUNT) {
    throwException(_T("BTreePage::setItemCount(%d):Invalid argument. MAXITEMCOUNT=%d"), value, MAXITEMCOUNT);
  }

  m_itemCount = value;
}

void BTreePage::copyItems(int from, int to, const BTreePage *src, int start) {
  int amount = to - from + 1;
  if(amount == 0) {
    return;
  }
  if(amount < 0 || (start + amount - 1 > src->getItemCount())) {
    throwException(_T("BTreePage::copyItems:Invalid argument(%d,%d,%d,%d)"), from, to, start, src->getItemCount());
  }

  memmove((void*)&getItem(from),&src->getItem(start),amount * getItemSize());
}

void BTreePage::incrItemCount() {
  if(m_itemCount >= MAXITEMCOUNT) {
    throwException(_T("BTreePage::incrItemCount. m_itemCount=%d. MAXITEMCOUNT=%d"), m_itemCount, MAXITEMCOUNT);
  }

  m_itemCount++;
}

void BTreePage::decrItemCount() {
  if(m_itemCount <= 0) {
    throwException(_T("BTreePage::decrItemCount:m_itemCount=%d"), m_itemCount);
  }

  m_itemCount--;
}

void BTreePage::insertItem(int i, const BTreePageItem &t) { 
  if(getItemCount() >= MAXITEMCOUNT) {
    throwException(_T("BTreePage::insertItem:Page is full"));
  }
  if(i < 1 || i > getItemCount()+1) {
    throwException(_T("BTreePage::insertItem(%d):Invalid argument. m_itemCount=%d"), i, getItemCount());
  }

  incrItemCount();

  if(i < getItemCount()) {
    memmove((void*)&getItem(i+1), &getItem(i), getItemSize()*(getItemCount()-i));
  }

  setItem(i,t);
}

void BTreePage::removeItem(int i) {
  if(getItemCount() == 0) {
    throwException(_T("BTreePage::removeItem(%d):Page is empty."),i);
  }
  if(i < 1 || i > getItemCount()) {
    throwException(_T("BTreePage::removeItem(%d):Invalid argument. m_itemCount=%d"),i,getItemCount());
  }

  if(i < getItemCount()) {
    memmove((void*)&getItem(i), &getItem(i+1), getItemSize()*(getItemCount()-i));
  }

  decrItemCount();
}

BTreePage *BTreePage::getChild(int i) const {
  if(i < 0 || i > m_itemCount) {
    throwException(_T("BTreePage::getChild(%d):Invalid argument. m_itemCount=%d"), i, m_itemCount);
  }

  return (i == 0) ? m_child0 : getItem(i).m_child;
}

void BTreePage::setChild(int i, BTreePage *v) {
  if(i < 0 || i > MAXITEMCOUNT) {
    throwException(_T("BTreePage::setChild(%d):Invalid argument. m_itemCount=%d"), i, getItemCount());
  }
  if(i == 0) {
    m_child0 = v;
  } else {
    getItem(i).m_child = v;
  }
}

// ------------------------------------BTreeSetPageImpl---------------------------------------------


const BTreePageItem &BTreeSetPageImpl::getItem(int i) const {
  if(i < 1 || i > getItemCount()) {
    throwException(_T("BTreeSetPageImpl::getItem(%d):Invalid argument. m_itemCount=%d"), i, getItemCount());
  }
  return m_item[i-1];
}

BTreePageItem &BTreeSetPageImpl::getItem(int i) {
  if(i < 1 || i > getItemCount()) {
    throwException(_T("BTreeSetPageImpl::getItem(%d):Invalid argument. m_itemCount=%d"), i, getItemCount());
  }
  return m_item[i-1];
}

void BTreeSetPageImpl::setItem(int i, const BTreePageItem &v) {
  if(i < 1 || i > getItemCount()) {
    throwException(_T("BTreeSetPageImpl::setItem(%d):Invalid argument. m_itemCount=%d"), i, getItemCount());
  }
  m_item[i-1] = v;
}

// ------------------------------------BTreeMapPageImpl---------------------------------------------

const BTreePageItem &BTreeMapPageImpl::getItem(int i) const {
  if(i < 1 || i > getItemCount()) {
    throwException(_T("BTreeMapPageImpl::getItem(%d):Invalid argument. m_itemCount=%d"), i, getItemCount());
  }
  return m_item[i-1];
}

BTreePageItem &BTreeMapPageImpl::getItem(int i) {
  if(i < 1 || i > getItemCount()) {
    throwException(_T("BTreeMapPageImpl::getItem(%d):Invalid argument. m_itemCount=%d"), i, getItemCount());
  }
  return m_item[i-1];
}

void BTreeMapPageImpl::setItem(int i, const BTreePageItem &v) {
  if(i < 1 || i > getItemCount()) {
    throwException(_T("BTreeMapPageImpl::setItem(%d):Invalid argument. m_itemCount=%d"), i, getItemCount());
  }
  m_item[i-1] = (BTreeMapPageItem&)v;
}

// ------------------------------------BTreeSetImpl--------------------------------------------------

BTreeSetImpl::BTreeSetImpl(const AbstractObjectManager &objectManager, const AbstractComparator &comparator) { 
  m_objectManager = objectManager.clone();
  m_comparator    = comparator.clone();
  m_root          = NULL;
  m_size          = 0;
  m_updateCount   = 0;
}

BTreeSetImpl::~BTreeSetImpl() {
  clear();
  delete m_objectManager;
  delete m_comparator;
}

//--------------------------Virtuals of BTreeSetImpl-------------------------------------------------------

BTreePage *BTreeSetImpl::allocatePage() const {
  return new BTreeSetPageImpl;
}

BTreePageItem &BTreeSetImpl::allocatePassupItem() {
  BTreePageItem tmp;
  m_passupItems.add(tmp);
  return m_passupItems.last();
}

void BTreeSetImpl::clearPassupItems() {
  m_passupItems.clear();
}

void BTreeSetImpl::copyItem(BTreePageItem &dst, const BTreePageItem &src) const {
  dst.m_key   = src.m_key;
  dst.m_child = src.m_child;
}

void BTreeSetImpl::deleteItem(BTreePageItem &item) const {
  m_objectManager->deleteObject(item.m_key);
}

//---------------------------------------------------------------------------------

// Returns true if item.key is inserted in tree. 
// Returnvalue of h indicates if middleitem of page a is passed up because of overflow.
bool BTreeSetImpl::pageInsert(BTreePage *a, const BTreePageItem &item, bool &h, BTreePageItem &passup) {
  if(a == NULL) {
    h = true;
    copyItem(passup,item);
    passup.m_child = NULL;
    return true;
  }
  int l = 1;
  int r = a->getItemCount() + 1;
  while(l < r) {
    int m = (l + r) / 2;
    if(m_comparator->cmp(a->getKey(m), item.m_key) <= 0) {
      l = m + 1;
    } else {
      r = m;
    }
  }
  r--;
  if(r > 0 && m_comparator->cmp(a->getItem(r).m_key,item.m_key) == 0) { // got it already 
    return false;
  }

  // item not on this page. try childpage
  BTreePageItem &u = allocatePassupItem();
  if(!pageInsert(a->getChild(r), item, h, u)) {
    return false;
  }

  if(h) { // insert u to the right of a.item[r]
    if(a->getItemCount() < MAXITEMCOUNT) {
      h = false;
      a->insertItem(r+1,u);
    } else { // overflow. split page a into a,b and pass middle item to v
      BTreePage *b = allocatePage();
      b->setItemCount(HALFSIZE);
      if(r <= HALFSIZE) {
        if(r == HALFSIZE) {
          copyItem(passup,u);
        } else { // r < HALFSIZE
          copyItem(passup,a->getItem(HALFSIZE));
          a->copyItems(r+2,HALFSIZE,a,r+1);
          a->setItem(r+1, u);
        }
        b->copyItems(1,HALFSIZE,a,HALFSIZE+1);
      } else { // insert in right page (page b)
        r -= HALFSIZE;
        copyItem(passup,a->getItem(HALFSIZE+1));
        b->copyItems(1,r-1,a,HALFSIZE+2);
        b->setItem(r,u);
        b->copyItems(r+1,HALFSIZE,a,HALFSIZE+r+1);
      }
      a->setItemCount(HALFSIZE);
      b->setChild(0,passup.m_child);
      passup.m_child = b;
    }
  }
  return true;
}

void BTreeSetImpl::pageUnderflow(BTreePage *c, BTreePage *a, int s, bool &h) {
  int mc = c->getItemCount();
  if(s < mc) {
    s++;
    BTreePage *b = c->getChild(s);
    int mb = b->getItemCount();
    int k  = (mb-HALFSIZE+1) / 2;
    if(k > 0) { // move k items from b to a
      a->setItemCount(HALFSIZE-1+k);
      a->setItem(HALFSIZE, c->getItem(s));
      a->setChild(HALFSIZE, b->getChild(0));
      a->copyItems(HALFSIZE+1,HALFSIZE-1+k,b,1);
      c->setItem(s,b->getItem(k));
      c->setChild(s,b);
      b->setChild(0,b->getChild(k));
      mb -= k;
      b->copyItems(1,mb,b,k+1);
      b->setItemCount(mb);
      h = false;
    } else { // k==0. merge a and b into a
      a->setItemCount(MAXITEMCOUNT);
      a->setItem(HALFSIZE, c->getItem(s));
      a->setChild(HALFSIZE, b->getChild(0));
      a->copyItems(HALFSIZE+1,MAXITEMCOUNT,b,1);
      c->removeItem(s);
      h = mc <= HALFSIZE;
      delete b;
    }
  } else { // s >= mc. b := page to the left of a
    BTreePage *b = c->getChild(s-1);
    int mb = b->getItemCount() + 1;
    int k  = (mb-HALFSIZE)/2;
    if(k > 0) { // move k items from b to a
      a->setItemCount(HALFSIZE-1+k);
      a->copyItems(k+1, HALFSIZE-1+k, a, 1);
      a->setItem(k, c->getItem(s));
      a->setChild(k, a->getChild(0));
      mb -= k;
      a->copyItems(1, k-1, b, mb+1);
      a->setChild(0, b->getChild(mb));
      c->setItem(s, b->getItem(mb));
      c->setChild(s, a);
      b->setItemCount(mb-1);
      h = false;
    } else { // k == 0. merge a and b into b
      b->setItemCount(MAXITEMCOUNT);
      b->setItem(mb, c->getItem(s));
      b->setChild(mb, a->getChild(0));
      b->copyItems(mb+1, mb+HALFSIZE-1, a, 1);
      c->setItemCount(mc-1);
      h = mc <= HALFSIZE;
      delete a;
    }
  }
}

void BTreeSetImpl::pageDel(BTreePage *p, BTreePage *a, int r, bool &h) {
  BTreePage *q = p->getChild(p->getItemCount());
  if(q != NULL) {
    pageDel(q,a,r,h);
    if(h)  {
      pageUnderflow(p,q,p->getItemCount(),h);
    }
  } else {
    p->setChild(p->getItemCount(),a->getChild(r));

    deleteItem(a->getItem(r));

    a->setItem(r, p->getLastItem());
    p->decrItemCount();
    h = p->getItemCount() < HALFSIZE;
  }
}

bool BTreeSetImpl::pageRemove(BTreePage *a, const void *key, bool &h) {
  if(a == NULL) { // key not in tree
    h = false;
    return false;
  }

  int l = 1;
  int r = a->getItemCount() + 1; // binary search
  while(l<r) {
    int m = (l + r) / 2;
    if(m_comparator->cmp(a->getKey(m),key) < 0) {
      l = m + 1;
    } else {
      r = m;
    }
  }

  BTreePage *q = a->getChild(r-1);
  if(r <= a->getItemCount() && m_comparator->cmp(a->getKey(r),key) == 0) { // found, now delete
    if(q == NULL) { // a is a terminal page

      deleteItem(a->getItem(r));

      a->removeItem(r);
      h = a->getItemCount() < HALFSIZE;
    } else {
      pageDel(q,a,r,h);
      if(h) {
        pageUnderflow(a,q,r-1,h);
      }
    }
    return true;
  } else {
    bool ret = pageRemove(q,key,h);
    if(h) {
      pageUnderflow(a,q,r-1,h);
    }
    return ret;
  }
}

BTreePageItem *BTreeSetImpl::pageSearch(const void *key) const {
  for(BTreePage *p = m_root; p != NULL;) {
    int l = 1;
    int r = p->getItemCount() + 1;
    while(l < r) {
      int m = (l + r) / 2;
      if(m_comparator->cmp(p->getKey(m),key) <= 0) {
        l = m + 1;
      } else {
        r = m;
      }
    }
    r--;
    if(r > 0 && m_comparator->cmp(p->getKey(r),key) == 0) {
      return &p->getItem(r);
    }

    p = p->getChild(r); // item not on this page. Try child
  }
  return NULL;
}

bool BTreeSetImpl::insertItem(BTreePageItem &item) {
  bool h;
  clearPassupItems();
  BTreePageItem &u = allocatePassupItem();

  if(!pageInsert(m_root,item,h,u)) {
    return false;
  }
  if(h) {
    BTreePage *q = m_root;
    m_root = allocatePage();
    m_root->setItemCount(1);
    m_root->setChild(0,q);
    m_root->setItem(1,u);
  }
  m_size++;
  m_updateCount++;
  return true;
}


bool BTreeSetImpl::add(const void *key) {
  BTreePageItem item;
  item.m_key = m_objectManager->cloneObject(key);

  if(!insertItem(item)) {
    deleteItem(item);
    return false;
  }
  return true;
}

bool BTreeSetImpl::remove(const void *key) {
  bool h;
  if(!pageRemove(m_root,key,h)) {
    return false;
  }
  if(h) { // root page size reduced
    if(m_root->getItemCount() == 0) {
      BTreePage *q = m_root;
      m_root  = q->getChild(0);
      delete q;
    }
  }
  m_size--;
  m_updateCount++;
  return true;
}

BTreePageItem *BTreeSetImpl::findNode(const void *key) {
  return pageSearch(key);
}

const BTreePageItem *BTreeSetImpl::findNode(const void *key) const {
  return pageSearch(key);
}

bool BTreeSetImpl::contains(const void *key) const {
  return findNode(key) != NULL;
}

const void *BTreeSetImpl::select() const {
  if(size() == 0) {
    throwException(_T("BTreeSet::select:Set is empty"));
  }
  return m_root->getItem(1).key();
}

void *BTreeSetImpl::select() {
  if(size() == 0) {
    throwException(_T("BTreeSet::select:Set is empty"));
  }
  return (void*)m_root->getItem(1).key();
}

const BTreePageItem *BTreeSetImpl::getMinNode() const {
  const BTreePage *result = NULL;
  for(const BTreePage *p = m_root; p; result = p, p = p->getChild(0));
  if(result == NULL) {
    throwException(_T("BTreeSet::getMinNode:Set is empty"));
  }
  return &result->getItem(1);
}

const BTreePageItem *BTreeSetImpl::getMaxNode() const {
  const BTreePage *result = NULL;
  for(const BTreePage *p = m_root; p; result = p, p = p->getLastItem().m_child);
  if(result == NULL) {
    throwException(_T("BTreeSet::getMinNode:Set is empty"));
  }
  return &result->getLastItem();
}

const void *BTreeSetImpl::getMin() const {
  return getMinNode()->key();
}

const void *BTreeSetImpl::getMax() const {
  return getMaxNode()->key();
}
  
void BTreeSetImpl::traverse(const BTreePage *p, PageWalker &pw) const {
  if(p) {
    pw.handlePage(*p);
    for(int i = 0; i <= p->getItemCount(); i++) {
      traverse(p->getChild(i),pw);
    }
  }
}

void BTreeSetImpl::traverse(PageWalker &pw) const {
  traverse(m_root,pw);
}

void BTreeSetImpl::deletePage(BTreePage *page) {
  if(page == NULL) {
    return;
  }
  int childCount = page->getItemCount();
  if(page->getChild(0)) {
    for(int i = 0; i <= childCount; i++) {
      deletePage(page->getChild(i));
    }
  }
  for(int i = 1; i <= childCount; i++) {
    deleteItem(page->getItem(i));
  }
  delete page;
}

void BTreeSetImpl::clear() {
  if(m_root != NULL) {
    deletePage(m_root);
    m_root = NULL;
    m_size = 0;
    m_updateCount++;
  }
}

AbstractCollection *BTreeSetImpl::clone(bool cloneData) const {
  BTreeSetImpl *clone = new BTreeSetImpl(*m_objectManager, *m_comparator);
  if(cloneData) {
    AbstractIterator *it = ((BTreeSetImpl*)this)->getIterator();
    while(it->hasNext()) {
      clone->add(it->next());
    }
    delete it;
  }
  return clone;
}

// ------------------------------------- BTreeSetIterator -------------------------------------

class BTreeIteratorStackElement {
public:
  const BTreePage *m_page;
  int              m_index;
  bool             m_childDone;
  BTreeIteratorStackElement(const BTreePage *page, int index, bool childDone) : m_page(page) {
    m_index = index; m_childDone = childDone;
  }
  String toString() const {
    return format(_T("%p:%d:%s"),m_page,m_index,boolToStr(m_childDone));
  }
};

class BTreeSetIterator : public AbstractIterator {
private:
  BTreeSetImpl                    &m_tree;
  Stack<BTreeIteratorStackElement> m_stack;
  const BTreePageItem             *m_next, *m_current;
  unsigned long                    m_updateCount;
  static const TCHAR              *className;
  void push(const BTreePage *page, int index, bool childDone);
  void pop() {
    m_stack.pop();
  }
  BTreeIteratorStackElement *top();
  const BTreePageItem *findFirst();
  const BTreePageItem *findNext();
        BTreePageItem *findPath(void *key);
protected:
  const BTreePageItem *nextNode();
public:
  AbstractIterator *clone();
  BTreeSetIterator(BTreeSetImpl &tree);
  void *next() {
    return nextNode()->m_key;
  }
  bool hasNext() const;
  void remove();
  String toString() {
    return m_stack.toString();
  }
};

const TCHAR *BTreeSetIterator::className = _T("BTreeSetIterator");

AbstractIterator *BTreeSetIterator::clone() {
  return new BTreeSetIterator(*this);
}

void BTreeSetIterator::push(const BTreePage *page, int index, bool childDone) {
  m_stack.push(BTreeIteratorStackElement(page,index,childDone));
}

BTreeIteratorStackElement *BTreeSetIterator::top() {
  return m_stack.isEmpty() ? NULL : &m_stack.top();
}

BTreeSetIterator::BTreeSetIterator(BTreeSetImpl &tree) : m_tree(tree) {
  m_updateCount = m_tree.m_updateCount;
  m_next        = findFirst();
  m_current     = NULL;
}

bool BTreeSetIterator::hasNext() const {
  return m_next != NULL;
}

const BTreePageItem *BTreeSetIterator::nextNode() {
  if(m_next == NULL) {
    noNextElementError(className);
  }
  if(m_updateCount != m_tree.m_updateCount) {
    concurrentModificationError(className);
  }
  m_current = m_next;
  m_next    = findNext();
  return m_current;
}

const BTreePageItem *BTreeSetIterator::findFirst() {
  m_stack.clear();
  if(m_tree.m_root) {
    push(m_tree.m_root, 0, false);
  }
  return findNext();
}

const BTreePageItem *BTreeSetIterator::findNext() {
  for(BTreeIteratorStackElement *sp = top(); sp; sp = top()) {
    const BTreePage *page = sp->m_page;
    if(sp->m_childDone) {
      // page.item[index-1].child.item[0..count].key < page.item[index].key < page.item[index].child[0..count].key
      sp->m_index++;
      if(sp->m_index > page->getItemCount()) {
        pop();
      } else {
        sp->m_childDone = false;
        return &page->getItem(sp->m_index);
      }
    } else {
      sp->m_childDone = true;
      BTreePage *child = page->getChild(sp->m_index);
      if(child != NULL) {
        push(child,0,false);
      }
    }
  }
  return NULL;
}

BTreePageItem *BTreeSetIterator::findPath(void *key) {
  m_stack.clear();
  for(BTreePage *p = m_tree.m_root; p != NULL;) {
    int l = 1;
    int r = p->getItemCount() + 1;
    while(l < r) {
      int m = (l + r) / 2;
      if(m_tree.m_comparator->cmp(p->getKey(m),key) <= 0) {
        l = m + 1;
      } else {
        r = m;
      }
    }
    r--;
    if(r > 0 && m_tree.m_comparator->cmp(p->getKey(r),key) == 0) {
      push(p,r,false);
      return &p->getItem(r);
    }

    push(p,r,true);
    p = p->getChild(r); // item not on this page. Try child
  }
  throwException(_T("%s::findPath:Key not found"), className);
  return NULL;
}

void BTreeSetIterator::remove() {
  if(m_current == NULL) {
    noCurrentElementError(className);
  }
  void *nextKey = NULL;
  if(m_next != NULL) {
    nextKey = m_next->m_key;
  }
  m_tree.remove(m_current->m_key);
  m_current = NULL;

  if(nextKey != NULL) {
    m_next = findPath(nextKey);
  }
  m_updateCount = m_tree.m_updateCount;
}

//-----------------------------------------------------------------

AbstractIterator *BTreeSetImpl::getIterator() {
  return new BTreeSetIterator(*this);
}

int BTreeSetImpl::getHeight() const {
  int h = 0;
  for(BTreePage *p = m_root; p; p = p->getChild(0)) {
    h++;
  }
  return h;
}

void BTreeSetImpl::checkPage(const BTreePage *p, int level, int height) const {
  if(level == height) {
    if(p != NULL) {
      throwException(_T("checkPage::Page at max-level not NULL"));
    }
  } else {
    if(p == NULL) {
      throwException(_T("checkPage::Page at level < %d is NULL"),height);
    }
    if(p->getItemCount() < HALFSIZE) {
      throwException(_T("checkPage::Invariant %d <= itemcount <= %d not satisfied. Itemcount=%d"),HALFSIZE,MAXITEMCOUNT, p->getItemCount());
    }
    for(int i = 0; i < p->getItemCount(); i++) {
      checkPage(p->getChild(i),level+1,height);
    }
  }
}

void BTreeSetImpl::checkTree() {
  int height = getHeight();
  if(height == 0) {
    return;
  }
  for(int i = 0; i <= m_root->getItemCount(); i++) {
    checkPage(m_root->getChild(i),1,height);
  }

  AbstractIterator *it = getIterator();
  Array<void*> list;
  while(it->hasNext()) {
    list.add(it->next());
  }
  delete it;
  for(int i = 1; i < list.size(); i++) {
    if(m_comparator->cmp(list[i-1],list[i]) >= 0) {
      throwException(_T("Order not maintained"));
    }
  }
}

void BTreeSetImpl::showPage(const BTreePage *p, int level, AbstractFormatter &formatter) const {
  if(p) {
    _tprintf(_T("%*.*slevel:%2d itemcount:%d "),  level,level,_T(" "),level,p->getItemCount());
    _tprintf(_T("["));
    for(int i = 1; i <= p->getItemCount(); i++) {
      if(i > 1) _tprintf(_T(", "));
      _tprintf(_T("%s"),formatter.toString(p->getKey(i)).cstr() );
    }
    _tprintf(_T("]\n"));
    for(int i = 0; i <= p->getItemCount();i++)
      showPage(p->getChild(i),level+1,formatter);
  }
}

void BTreeSetImpl::showTree(AbstractFormatter &formatter) const {
  showPage(m_root,0,formatter);
  _tprintf(_T("Size:%d"),m_size);
}

// ------------------------------------BTreeMapImpl----------------------------------------

BTreeMapImpl::BTreeMapImpl(const AbstractObjectManager &keyManager, const AbstractObjectManager &dataManager, const AbstractComparator &comparator) 
: BTreeSetImpl(keyManager, comparator)
{
  m_dataManager = dataManager.clone();
}

BTreeMapImpl::~BTreeMapImpl() {
  clear();
  delete m_dataManager;
}

//--------------------------Virtuals of BTreeMapImpl-------------------------------------------------------
BTreePage *BTreeMapImpl::allocatePage() const {
  return new BTreeMapPageImpl;
}

BTreePageItem &BTreeMapImpl::allocatePassupItem() {
  BTreeMapPageItem tmp;
  m_passupItems.add(tmp);
  return m_passupItems.last();
}

void BTreeMapImpl::clearPassupItems() {
  m_passupItems.clear();
}

void BTreeMapImpl::copyItem(BTreePageItem &dst, const BTreePageItem &src) const {
  BTreeSetImpl::copyItem(dst,src);
  ((BTreeMapPageItem&)dst).m_value = ((BTreeMapPageItem&)src).m_value;
}

void BTreeMapImpl::deleteItem(BTreePageItem &item) const {
  BTreeSetImpl::deleteItem(item);
  m_dataManager->deleteObject(((BTreeMapPageItem&)item).m_value);
}

const AbstractEntry *BTreeMapImpl::getMinEntry() const {
  return (BTreeMapPageItem*)getMinNode();
}

const AbstractEntry *BTreeMapImpl::getMaxEntry() const {
  return (BTreeMapPageItem*)getMaxNode();
}
//--------------------------------------------------------------------------------------------------------

class BTreeMapIterator : public BTreeSetIterator {
public:
  BTreeMapIterator(BTreeSetImpl &set) : BTreeSetIterator(set) {
  }
  AbstractIterator *clone() {
    return new BTreeMapIterator(*this);
  }
  void *next();
};

void *BTreeMapIterator::next() {
  return (AbstractEntry*)((BTreeMapPageItem*)nextNode());
}

AbstractIterator *BTreeMapImpl::getIterator() {
  return new BTreeMapIterator(*this);
}

AbstractMap *BTreeMapImpl::cloneMap(bool cloneData) const {
  BTreeMapImpl *clone = new BTreeMapImpl(*BTreeSetImpl::getObjectManager(),*m_dataManager,*getComparator());
  if(cloneData) {
    AbstractIterator *it = ((BTreeMapImpl*)this)->getIterator();
    while(it->hasNext()) {
      AbstractEntry *n = (AbstractEntry*)it->next();
      clone->put(n->key(),n->value());
    }
    delete it;
  }
  return clone;
}

void *BTreeMapImpl::get(const void *key) {
  BTreePageItem *item = findNode(key);
  if(item == NULL) {
    return NULL;
  } else {
    return ((BTreeMapPageItem*)item)->m_value;
  }
}

const void *BTreeMapImpl::get(const void *key) const {
  const BTreePageItem *item = findNode(key);
  if(item == NULL) {
    return NULL;
  } else {
    return ((BTreeMapPageItem*)item)->m_value;
  }
}

bool BTreeMapImpl::put(const void *key, const void *elem) {
  BTreeMapPageItem item;
  item.m_key  = getObjectManager()->cloneObject(key);
  item.m_value = m_dataManager->cloneObject(elem);
  if(!insertItem(item)) {
    deleteItem(item);
    return false;
  }
  return true;
}

bool BTreeMapImpl::put(const void *key, void *elem) {
  BTreeMapPageItem item;
  item.m_key  = getObjectManager()->cloneObject(key);
  item.m_value = m_dataManager->cloneObject(elem);
  if(!insertItem(item)) {
    deleteItem(item);
    return false;
  }
  return true;
}

bool BTreeMapImpl::remove(const void *key) {
  return BTreeSetImpl::remove(key);
}

AbstractEntry *BTreeMapImpl::selectEntry() const {
  if(size() == 0) {
    throwException(_T("BtreeMap::select:Cannot select element from empty map."));
  }
  return (AbstractEntry*)(BTreeMapPageItem*)findNode(select());
}

void BTreeMapImpl::showPage(const BTreePage *p, int level, AbstractFormatter &keyFormatter, AbstractFormatter &dataFormatter) const {
  if(p) {
    _tprintf(_T("%*.*slevel:%2d itemcount:%d "), level,level,_T(" "), level, p->getItemCount());
    _tprintf(_T("["));
    for(int i = 1; i <= p->getItemCount(); i++) {
      const BTreeMapPageItem &item = (const BTreeMapPageItem&)p->getItem(i);
      if(i > 1) _tprintf(_T(","));
      _tprintf(_T("(%s,%s)"), keyFormatter.toString(item.m_key).cstr(), dataFormatter.toString(item.m_value).cstr());
    }
    _tprintf(_T("]\n"));
    for(int i = 0; i <= p->getItemCount();i++) {
      showPage(p->getChild(i), level+1, keyFormatter, dataFormatter);
    }
  }
}

void BTreeMapImpl::showTree(AbstractFormatter &keyFormatter, AbstractFormatter &dataFormatter) const {
  showPage(getRoot(), 0, keyFormatter, dataFormatter);
  _tprintf(_T("Size:%d"), size());
}

