#pragma once

#include "MyUtil.h"
#include "ByteStream.h"

class StringPoolNode {
public:
  unsigned int    m_offset;
  StringPoolNode *m_next;
};

class StringPoolNodePage {
public:
  unsigned int        m_count;
  StringPoolNodePage *m_next;
  StringPoolNode      m_nodes[20000];

  StringPoolNodePage(StringPoolNodePage *next) {
    m_count = 0;
    m_next  = next;
  }

  inline StringPoolNode *fetchNode() {
    return &m_nodes[m_count++];
  }
  
  inline bool isFull() const {
    return m_count == ARRAYSIZE(m_nodes);
  }

  inline bool isEmpty() const {
    return m_count == 0;
  }
};

class StringPool {
private:
  unsigned int         m_size;
  unsigned int         m_indexCapacity;
  StringPoolNode     **m_index;
  StringPoolNodePage  *m_firstPage;
  CompactArray<TCHAR>  m_textData;

  StringPoolNode *fetchNode();
  static StringPoolNode **allocateIndex( unsigned long capacity); // if capacity == 0, NULL is returned, else a zero-filled array of pointers
  void init(unsigned long indexCapacity, unsigned long textCapacity);
  void copy(const StringPool &src);
  void buildPageList();
  inline const TCHAR *getNodeString(const StringPoolNode *n) const {
    return getString(n->m_offset);
  }

public:
  StringPool(unsigned long indexCapacity = 100, unsigned long textCapacity = 1000);
  StringPool(           const StringPool &src);
  StringPool &operator=(const StringPool &src);
  ~StringPool();

  void setIndexCapacity(unsigned long capacity);
  void setTextCapacity(unsigned long capacity);

  inline unsigned long getIndexCapacity() const {
    return m_indexCapacity;
  }

  inline unsigned long getTextCapacity() const {
    return m_textData.getCapacity();
  }

  inline unsigned long getUniqueStringCount() const {
    return m_size;
  }

  inline unsigned long getTextSize() const {
    return m_textData.size();
  }

  unsigned int addString(const TCHAR *s); // return index to be used when lookup string using getString
                                          // If the string already exist in the pool, the index of this will be returned
                                          // instead of adding the new copy

#ifdef UNICODE
  unsigned int addString(const char *s);
#endif

  inline const TCHAR *getString(unsigned int index) const {
    return (const TCHAR*)m_textData.getBuffer() + index;
  }

  void clear();
  void clearIndex(); // Releases the memory, used by the hashmap. m_size is NOT changed, so if addString is called again,
                     // a rehashing will occur, and all memory will be consumed again. So avoid clearIndex,addString many times
                     // as it will be time consuming. Use addString as long as you have new strings to add, and when
                     // done, then call clearIndex, to release memory used by the hashIndex.

  Iterator<const TCHAR*> getIterator() const;

  inline bool isEmpty() const {
    return m_size == 0;
  }
  bool operator==(const StringPool &sp) const;
  bool operator!=(const StringPool &sp) const;

  void save(ByteOutputStream &s) const;
  void load(ByteInputStream  &s);
};
