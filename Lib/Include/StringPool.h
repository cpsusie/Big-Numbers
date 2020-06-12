#pragma once

#include "CompactArray.h"
#include "Iterator.h"
#include "ByteStream.h"

class StringPoolNode {
public:
  size_t          m_offset;
  StringPoolNode *m_next;
};

class StringPoolNodePage {
public:
  UINT                m_count;
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
  size_t               m_size;
  size_t               m_indexCapacity;
  StringPoolNode     **m_index;
  StringPoolNodePage  *m_firstPage;
  CompactArray<TCHAR>  m_textData;

  StringPoolNode *fetchNode();
  static StringPoolNode **allocateIndex( size_t capacity); // if capacity == 0, NULL is returned, else a zero-filled array of pointers
  void init(size_t indexCapacity, size_t textCapacity);
  void copy(const StringPool &src);
  void buildPageList();
  inline const TCHAR *getNodeString(const StringPoolNode *n) const {
    return getString(n->m_offset);
  }
  size_t addTStr(const TCHAR *s);

public:
  StringPool(size_t indexCapacity = 100, size_t textCapacity = 1000);
  StringPool(           const StringPool &src);
  StringPool &operator=(const StringPool &src);
  virtual ~StringPool();

  void setIndexCapacity(size_t capacity);
  void setTextCapacity(size_t capacity);

  inline size_t getIndexCapacity() const {
    return m_indexCapacity;
  }

  inline size_t getTextCapacity() const {
    return m_textData.getCapacity();
  }

  inline size_t getUniqueStringCount() const {
    return m_size;
  }

  inline size_t getTextSize() const {
    return m_textData.size();
  }

  // return index to be used when lookup string using getString
  // If the string already exist in the pool, the index of this will be returned
  // instead of adding the new copy
  size_t addString(const char    *s);
  size_t addString(const wchar_t *s);

  inline const TCHAR *getString(size_t index) const {
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
#if defined(_DEBUG)
  void dump() const;
#endif
};
