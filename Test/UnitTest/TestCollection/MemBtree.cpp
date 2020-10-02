#include "stdafx.h"
#include <LineTracer.h>
#include "MemBtree.h"

// ------------------------------------BTreePage----------------------------------------------------

void BTreePage::throwIndexError(const TCHAR *method, UINT i) const {
  throwInvalidArgumentException(method, _T("Indexerror. i=%u, m_itemCount=%u"), i, getItemCount());
}

void BTreePage::setItemCount(UINT value) {
  if(value > MAXITEMCOUNT) {
    throwInvalidArgumentException(__TFUNCTION__, _T("value=%u. MAXITEMCOUNT=%u"), value, MAXITEMCOUNT);
  }
  m_itemCount = value;
}

void BTreePage::copyItems(UINT from, UINT to, const BTreePage *src, UINT start) {
  const int amount = (int)to - (int)from + 1;
  if(amount == 0) {
    return;
  }
  if((amount < 0) || ((int)start + (int)amount - 1 > (int)src->getItemCount())) {
    throwInvalidArgumentException(__TFUNCTION__,_T("from=%u,to=%u,%start=%u,itemcount=%u)"), from, to, start, src->getItemCount());
  }

  memmove((void*)&getItem(from),&src->getItem(start),amount * getItemSize());
}

void BTreePage::incrItemCount() {
  m_itemCount++;
}

void BTreePage::decrItemCount() {
  if(m_itemCount <= 0) {
    throwException(_T("%s:Page is empty"), __TFUNCTION__, m_itemCount);
  }
  m_itemCount--;
}

void BTreePage::insertItem(UINT i, const BTreePageItem &t) {
  if(getItemCount() >= MAXITEMCOUNT) {
    throwException(_T("%s:Page is full"), __TFUNCTION__);
  }
  if(i < 1 || i > getItemCount()+1) throwIndexError(__TFUNCTION__, i);
  incrItemCount();

  if(i < getItemCount()) {
    memmove((void*)&getItem(i+1), &getItem(i), getItemSize()*(getItemCount()-i));
  }
  setItem(i,t);
}

void BTreePage::removeItem(UINT i) {
  if(getItemCount() == 0) {
    throwException(_T("%s:Page is empty."),__TFUNCTION__);
  }
  if((i < 1) || (i > getItemCount())) throwIndexError(__TFUNCTION__, i);
  if(i < getItemCount()) {
    memmove((void*)&getItem(i), &getItem(i+1), getItemSize()*(getItemCount()-i));
  }
  decrItemCount();
}

BTreePage *BTreePage::getChild(UINT i) const {
  if(i > m_itemCount) throwIndexError(__TFUNCTION__, i);
  return (i == 0) ? m_child0 : getItem(i).m_child;
}

void BTreePage::setChild(UINT i, BTreePage *v) {
  if(i > MAXITEMCOUNT) throwIndexError(__TFUNCTION__, i);
  if(i == 0) {
    m_child0 = v;
  } else {
    getItem(i).m_child = v;
  }
}

// ------------------------------------BTreeSetPageImpl---------------------------------------------


const BTreePageItem &BTreeSetPageImpl::getItem(UINT i) const {
  if((i < 1) || (i > getItemCount())) throwIndexError(__TFUNCTION__, i);
  return m_item[i-1];
}

BTreePageItem &BTreeSetPageImpl::getItem(UINT i) {
  if((i < 1) || (i > getItemCount())) throwIndexError(__TFUNCTION__, i);
  return m_item[i-1];
}

void BTreeSetPageImpl::setItem(UINT i, const BTreePageItem &v) {
  if((i < 1) || (i > getItemCount())) throwIndexError(__TFUNCTION__, i);
  m_item[i-1] = v;
}

// ------------------------------------BTreeMapPageImpl---------------------------------------------

const BTreePageItem &BTreeMapPageImpl::getItem(UINT i) const {
  if((i < 1) || (i > getItemCount())) throwIndexError(__TFUNCTION__, i);
  return m_item[i-1];
}

BTreePageItem &BTreeMapPageImpl::getItem(UINT i) {
  if((i < 1) || (i > getItemCount())) throwIndexError(__TFUNCTION__, i);
  return m_item[i-1];
}

void BTreeMapPageImpl::setItem(UINT i, const BTreePageItem &v) {
  if((i < 1) || (i > getItemCount())) throwIndexError(__TFUNCTION__, i);
  m_item[i-1] = (BTreeMapPageItem&)v;
}

// ------------------------------------BTreeSetImpl--------------------------------------------------

BTreeSetImpl::BTreeSetImpl(const AbstractObjectManager &objectManager, const AbstractComparator &comparator) {
  m_objectManager = objectManager.clone();
  m_comparator    = comparator.clone();
  m_root          = nullptr;
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
  if(a == nullptr) {
    h = true;
    copyItem(passup,item);
    passup.m_child = nullptr;
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
  if(q != nullptr) {
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
  if(a == nullptr) { // key not in tree
    h = false;
    return false;
  }

  UINT l = 1;
  UINT r = a->getItemCount() + 1; // binary search
  while(l<r) {
    const UINT m = (l + r) / 2;
    if(m_comparator->cmp(a->getKey(m),key) < 0) {
      l = m + 1;
    } else {
      r = m;
    }
  }

  BTreePage *q = a->getChild(r-1);
  if(r <= a->getItemCount() && m_comparator->cmp(a->getKey(r),key) == 0) { // found, now delete
    if(q == nullptr) { // a is a terminal page

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
  for(BTreePage *p = m_root; p != nullptr;) {
    UINT l = 1;
    UINT r = p->getItemCount() + 1;
    while(l < r) {
      const UINT m = (l + r) / 2;
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
  return nullptr;
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

BTreePageItem *BTreeSetImpl::findNode(const void *key) const {
  return pageSearch(key);
}

bool BTreeSetImpl::contains(const void *key) const {
  return findNode(key) != nullptr;
}

const void *BTreeSetImpl::select(RandomGenerator &rnd) const {
  if(size() == 0) throwSelectFromEmptyCollectionException(__TFUNCTION__);
  return m_root->getItem(1).key();
}

void *BTreeSetImpl::select(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
  if(size() == 0) throwSelectFromEmptyCollectionException(__TFUNCTION__);
  return (void*)m_root->getItem(1).key();
}

const BTreePageItem *BTreeSetImpl::getMinNode() const {
  const BTreePage *result = nullptr;
  for(const BTreePage *p = m_root; p; result = p, p = p->getChild(0));
  if(result == nullptr) {
    throwException(_T("%s:Set is empty"), __TFUNCTION__);
  }
  return &result->getItem(1);
}

const BTreePageItem *BTreeSetImpl::getMaxNode() const {
  const BTreePage *result = nullptr;
  for(const BTreePage *p = m_root; p; result = p, p = p->getLastItem().m_child);
  if(result == nullptr) {
    throwException(_T("%s:Set is empty"), __TFUNCTION__);
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
    for(UINT i = 0; i <= p->getItemCount(); i++) {
      traverse(p->getChild(i),pw);
    }
  }
}

void BTreeSetImpl::traverse(PageWalker &pw) const {
  traverse(m_root,pw);
}

void BTreeSetImpl::deletePage(BTreePage *page) {
  if(page == nullptr) {
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
  if(m_root != nullptr) {
    deletePage(m_root);
    m_root = nullptr;
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
  DECLARECLASSNAME;
  const BTreePage *m_page;
  int              m_index;
  bool             m_childDone;
  inline BTreeIteratorStackElement(const BTreePage *page, int index, bool childDone) : m_page(page) {
    m_index = index; m_childDone = childDone;
  }
  inline BTreeIteratorStackElement() {
  }
  String toString() const {
    return format(_T("%p:%d:%s"),m_page,m_index,boolToStr(m_childDone));
  }
};

class BTreeSetIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  BTreeSetImpl                           &m_tree;
  CompactStack<BTreeIteratorStackElement> m_stack;
  const BTreePageItem                    *m_next, *m_current;
  size_t                                  m_updateCount;
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
  BTreeSetIterator(const BTreeSetImpl &tree);
  AbstractIterator *clone()       override {
    return new BTreeSetIterator(*this);
  }
  bool hasNext()            const override {
    return m_next != nullptr;
  }
  void *next()                    override {
    return nextNode()->m_key;
  }
  void remove()                   override;
  String toString() {
    return m_stack.toString();
  }
};

DEFINECLASSNAME(BTreeSetIterator);

void BTreeSetIterator::push(const BTreePage *page, int index, bool childDone) {
  m_stack.push(BTreeIteratorStackElement(page,index,childDone));
}

BTreeIteratorStackElement *BTreeSetIterator::top() {
  return m_stack.isEmpty() ? nullptr : &m_stack.top();
}

BTreeSetIterator::BTreeSetIterator(const BTreeSetImpl &tree) : m_tree((BTreeSetImpl&)tree) {
  m_updateCount = m_tree.m_updateCount;
  m_next        = findFirst();
  m_current     = nullptr;
}

const BTreePageItem *BTreeSetIterator::nextNode() {
  if(m_next == nullptr) {
    noNextElementError(s_className);
  }
  if(m_updateCount != m_tree.m_updateCount) {
    concurrentModificationError(s_className);
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
      if(sp->m_index > (int)page->getItemCount()) {
        pop();
      } else {
        sp->m_childDone = false;
        return &page->getItem(sp->m_index);
      }
    } else {
      sp->m_childDone = true;
      BTreePage *child = page->getChild(sp->m_index);
      if(child != nullptr) {
        push(child,0,false);
      }
    }
  }
  return nullptr;
}

BTreePageItem *BTreeSetIterator::findPath(void *key) {
  m_stack.clear();
  for(BTreePage *p = m_tree.m_root; p != nullptr;) {
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
  throwException(_T("%s:Key not found"), __TFUNCTION__);
  return nullptr;
}

void BTreeSetIterator::remove() {
  if(m_current == nullptr) {
    noCurrentElementError(s_className);
  }
  void *nextKey = nullptr;
  if(m_next != nullptr) {
    nextKey = m_next->m_key;
  }
  __assume(m_current != nullptr);
  m_tree.remove(m_current->m_key);
  m_current = nullptr;

  if(nextKey != nullptr) {
    m_next = findPath(nextKey);
  }
  m_updateCount = m_tree.m_updateCount;
}

//-----------------------------------------------------------------

AbstractIterator *BTreeSetImpl::getIterator() const {
  return new BTreeSetIterator(*this);
}

UINT BTreeSetImpl::getHeight() const {
  UINT h = 0;
  for(BTreePage *p = m_root; p; p = p->getChild(0)) {
    h++;
  }
  return h;
}

void BTreeSetImpl::checkPage(const BTreePage *p, int level, int height) const {
  DEFINEMETHODNAME;
  if(level == height) {
    if(p != nullptr) {
      throwException(_T("%s::Page at max-level not nullptr"), method);
    }
  } else {
    if(p == nullptr) {
      throwException(_T("%s::Page at level < %d is nullptr"),method, height);
    }
    if(p->getItemCount() < HALFSIZE) {
      throwException(_T("%s::Invariant %d <= itemcount <= %d not satisfied. Itemcount=%d")
                    ,method, HALFSIZE,MAXITEMCOUNT, p->getItemCount());
    }
    for(UINT i = 0; i < p->getItemCount(); i++) {
      checkPage(p->getChild(i),level+1,height);
    }
  }
}

void BTreeSetImpl::checkTree() {
  UINT height = getHeight();
  if(height == 0) {
    return;
  }
  for(UINT i = 0; i <= m_root->getItemCount(); i++) {
    checkPage(m_root->getChild(i),1,height);
  }

  AbstractIterator *it = getIterator();
  CompactArray<void*> list;
  while(it->hasNext()) {
    list.add(it->next());
  }
  delete it;
  for(size_t i = 1; i < list.size(); i++) {
    if(m_comparator->cmp(list[i-1],list[i]) >= 0) {
      throwException(_T("Order not maintained"));
    }
  }
}

void BTreeSetImpl::showPage(const BTreePage *p, int level, AbstractFormatter &formatter) const {
  if(p) {
    _tprintf(_T("%*.*slevel:%2d itemcount:%d "),  level,level,_T(" "),level,p->getItemCount());
    _tprintf(_T("["));
    for(UINT i = 1; i <= p->getItemCount(); i++) {
      if(i > 1) _tprintf(_T(", "));
      _tprintf(_T("%s"),formatter.toString(p->getKey(i)).cstr() );
    }
    _tprintf(_T("]\n"));
    for(UINT i = 0; i <= p->getItemCount();i++)
      showPage(p->getChild(i),level+1,formatter);
  }
}

void BTreeSetImpl::showTree(AbstractFormatter &formatter) const {
  showPage(m_root,0,formatter);
  _tprintf(_T("Size:%s"), format1000(m_size).cstr());
}

// ------------------------------------BTreeMapImpl----------------------------------------

BTreeMapImpl::BTreeMapImpl(const AbstractObjectManager &keyManager, const AbstractObjectManager &dataManager, const AbstractComparator &comparator)
: BTreeSetImpl(keyManager, comparator)
{
  m_dataManager = dataManager.clone();
}

AbstractMap *BTreeMapImpl::cloneMap(bool cloneData) const {
  BTreeMapImpl     *clone = nullptr;
  AbstractIterator *it    = nullptr;
  clone = new BTreeMapImpl(*BTreeSetImpl::getObjectManager(), *m_dataManager, *getComparator());
  try {
    if(cloneData) {
      it = getIterator(); TRACE_NEW(it);
      while(it->hasNext()) {
        AbstractEntry *n = (AbstractEntry *)it->next();
        clone->put(n->key(), n->value());
      }
      SAFEDELETE(it);
    }
  } catch(...) {
    TRACE_NEW(clone );
    SAFEDELETE(it   );
    SAFEDELETE(clone);
    throw;
  }
  return clone;
}

AbstractCollection *BTreeMapImpl::clone(bool cloneData) const {
  throwUnsupportedOperationException(__TFUNCTION__);
  return nullptr;
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
  BTreeMapIterator(const BTreeSetImpl &set) : BTreeSetIterator((BTreeSetImpl&)set) {
  }
  AbstractIterator *clone() override {
    return new BTreeMapIterator(*this);
  }
  void *next()              override {
    return (AbstractEntry*)((BTreeMapPageItem*)nextNode());
  }
};

AbstractIterator *BTreeMapImpl::getIterator() const {
  return new BTreeMapIterator(*this);
}

void *BTreeMapImpl::get(const void *key) const {
  BTreePageItem *item = findNode(key);
  if(item == nullptr) {
    return nullptr;
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

bool BTreeMapImpl::remove(const void *key) {
  return BTreeSetImpl::remove(key);
}

AbstractEntry *BTreeMapImpl::selectEntry(RandomGenerator &rnd) const {
  if(size() == 0) {
    throwSelectFromEmptyCollectionException(__TFUNCTION__);
  }
  return (AbstractEntry*)(BTreeMapPageItem*)findNode(select(rnd));
}

void BTreeMapImpl::showPage(const BTreePage *p, int level, AbstractFormatter &keyFormatter, AbstractFormatter &dataFormatter) const {
  if(p) {
    _tprintf(_T("%*.*slevel:%2d itemcount:%d "), level,level,_T(" "), level, p->getItemCount());
    _tprintf(_T("["));
    for(UINT i = 1; i <= p->getItemCount(); i++) {
      const BTreeMapPageItem &item = (const BTreeMapPageItem&)p->getItem(i);
      if(i > 1) _tprintf(_T(","));
      _tprintf(_T("(%s,%s)"), keyFormatter.toString(item.m_key).cstr(), dataFormatter.toString(item.m_value).cstr());
    }
    _tprintf(_T("]\n"));
    for(UINT i = 0; i <= p->getItemCount();i++) {
      showPage(p->getChild(i), level+1, keyFormatter, dataFormatter);
    }
  }
}

void BTreeMapImpl::showTree(AbstractFormatter &keyFormatter, AbstractFormatter &dataFormatter) const {
  showPage(getRoot(), 0, keyFormatter, dataFormatter);
  _tprintf(_T("Size:%s"), format1000(size()).cstr());
}

