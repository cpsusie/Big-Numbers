#include "pch.h"
#include <StringPool.h>

StringPool::StringPool(size_t indexCapacity, size_t textCapacity) {
  init(indexCapacity, textCapacity);
}

StringPool::StringPool(const StringPool &src) {
  copy(src);
}

StringPool &StringPool::operator=(const StringPool &src) {
  if(this == &src) {
    return *this;
  }
  clear();
  copy(src);
  return *this;
}

StringPool::~StringPool() {
  clear();
}

void StringPool::init(size_t indexCapacity, size_t textCapacity) {
  m_size          = 0;
  m_indexCapacity = indexCapacity;
  m_index         = allocateIndex(indexCapacity);
  m_firstPage     = NULL;
  setTextCapacity(textCapacity);
}

StringPoolNode *StringPool::fetchNode() {
  if(m_firstPage == NULL || m_firstPage->isFull()) {
    m_firstPage = new StringPoolNodePage(m_firstPage);
  }
  return m_firstPage->fetchNode();
}

StringPoolNode **StringPool::allocateIndex(size_t capacity) { // static
  StringPoolNode **result = capacity ? new StringPoolNode*[capacity] : NULL;
  if(result) {
    memset(result, 0, sizeof(result[0])*capacity);
  }
  return result;
}

void StringPool::copy(const StringPool &src) {
  init(0, src.getTextCapacity());
  m_textData = src.m_textData;
  m_size     = src.m_size;
  StringPoolNodePage **last = &m_firstPage;
  for(StringPoolNodePage *page = src.m_firstPage; page; page = page->m_next, last = &(*last)->m_next) {
    StringPoolNodePage *newPage = new StringPoolNodePage(*page);
    *last = newPage;
  }
  *last = NULL;
  setIndexCapacity(src.getIndexCapacity());
}

size_t StringPool::addString(const TCHAR *s) {
  unsigned long hashCode;
  if(*s == 0) {
    return 0; // offset 0 is a 0-byte. see setTextCapacity
  }
  for(;;) {
    if(m_indexCapacity) {
      hashCode = strHash(s) % m_indexCapacity;
      for(const StringPoolNode *p = m_index[hashCode]; p; p = p->m_next) {
        if(_tcscmp(s, getNodeString(p)) == 0) {
          return p->m_offset;
        }
      }
    }
    if(m_size+1 <= m_indexCapacity*2) {
      break;
    }
    setIndexCapacity(m_size*3+5);
  }
  const size_t stringOffset = m_textData.size();
  m_textData.append(s, _tcsclen(s) + 1);

  StringPoolNode *n = fetchNode();
  n->m_offset       = stringOffset;
  n->m_next         = m_index[hashCode];
  m_index[hashCode] = n;
  m_size++;
  return stringOffset;
}

void StringPool::clear() {
  clearIndex();
  m_textData.clear();
  setTextCapacity(0);
  m_size = 0;
}

void StringPool::clearIndex() {
  for(StringPoolNodePage *p = m_firstPage, *q = NULL; p; p = q) {
    q = p->m_next;
    delete p;
  }
  m_firstPage = NULL;
  const size_t tmp = m_size;
  m_size = 0;
  setIndexCapacity(0);
  m_size = tmp;
}

class StringPoolIterator : public AbstractIterator {
  size_t                    m_currentOffset;
  const CompactArray<TCHAR> &m_textData;
  const TCHAR               *m_result;
private:
public:
  StringPoolIterator(const CompactArray<TCHAR> &textData);
  AbstractIterator *clone();
  bool hasNext() const;
  void *next();
  void remove();
};

StringPoolIterator::StringPoolIterator(const CompactArray<TCHAR> &textData) : m_textData(textData) {
  m_currentOffset = 1;
}

AbstractIterator *StringPoolIterator::clone() {
  return new StringPoolIterator(*this);
}

bool StringPoolIterator::hasNext() const {
  return m_currentOffset < m_textData.size();
}

void *StringPoolIterator::next() {
  m_result = (TCHAR*)m_textData.getBuffer() + m_currentOffset;
  m_currentOffset += _tcsclen(m_result) + 1;
  return &m_result;
}

void StringPoolIterator::remove() {
  unsupportedOperationError(__TFUNCTION__);
}


Iterator<const TCHAR*> StringPool::getIterator() const {
  return Iterator<const TCHAR*>(new StringPoolIterator(m_textData));
}

void StringPool::setTextCapacity(size_t capacity) {
  m_textData.setCapacity(capacity);
  if(m_textData.size() == 0) {
    m_textData.add(0); // first byte of m_textData will always contain the empty string
  }
}

void StringPool::setIndexCapacity(size_t capacity) {
  if(capacity < m_size) {
    capacity = m_size;
  }
  if(capacity == m_indexCapacity) {
    return;
  }
  if(m_index) {
    delete[] m_index;
    m_index = NULL;
  }

  if((m_size > 0) && (m_firstPage == NULL)) {
    buildPageList();
  }
    
  m_indexCapacity = capacity;
  m_index         = allocateIndex(capacity);

  for(StringPoolNodePage *page = m_firstPage; page; page = page->m_next) {
    for(UINT i = 0; i < page->m_count; i++) {
      StringPoolNode *n = page->m_nodes+i;
      const unsigned long hashCode = strHash(getNodeString(n)) % m_indexCapacity;
      n->m_next = m_index[hashCode];
      m_index[hashCode] = n;
    }
  }
}

void StringPool::buildPageList() {
  const TCHAR *base = (const TCHAR*)m_textData.getBuffer();
  const TCHAR *eob  = base + m_textData.size();
  for(const TCHAR *cp = base + 1; cp < eob; cp += _tcsclen(cp)+1) { // skip every stringterminator following each string in buffer
    fetchNode()->m_offset = cp - base;
  }
}

bool StringPool::operator==(const StringPool &sp) const {
  if(this == &sp) {
    return true;
  }
  return (m_size == sp.m_size) && (m_textData == sp.m_textData);
}

bool StringPool::operator!=(const StringPool &sp) const {
  return !(*this == sp);
}

void StringPool::save(ByteOutputStream &s) const {
  const size_t size = m_size;
  s.putBytes((const BYTE*)&size, sizeof(size));
  m_textData.save(s);
}

void StringPool::load(ByteInputStream &s) {
  clear();
  size_t size;
  s.getBytesForced((BYTE*)&size, sizeof(size));
  m_size = size;
  m_textData.load(s);
}
