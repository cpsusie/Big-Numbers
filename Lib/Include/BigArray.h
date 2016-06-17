#pragma once

#include "MyUtil.h"
#include "Random.h"
#include "BitSet.h"
#include <io.h>

template <class T> class BigArray {
private:
  class PageMapElement {
  public:
    unsigned int             m_pageIndex : 31; // We can have max 2^31 = 2.147.483.648 pages, each containing a number of elements. Pagesize about 4096 bytes.
    unsigned int             m_loaded    : 1;  // 1 => page is in memory, 0 => swapped out, ie, must load before accessing its elements
    mutable unsigned __int64 m_lastReference;

    PageMapElement() {
      m_pageIndex     = 0;
      m_loaded        = 0;
      m_lastReference = 0;
    }

#ifdef _DEBUG
    String toString() const {
      return format(_T("(page:%lu,%s)"), m_pageIndex, m_loaded?_T("loaded "):_T("swapped"));
    }
#define CHECK_BIGARRAY_INVARIANT() checkInvariant()
#else
#define CHECK_BIGARRAY_INVARIANT()
#endif
  };

#ifdef _DEBUG
#define MAX_PAGESIZE     100
#define MEMORYPAGE_COUNT   4
#else
#define MAX_PAGESIZE     4096
#define MEMORYPAGE_COUNT 1024
#endif

#pragma warning( push )
#pragma warning(disable:4200 4723 4724)

#define _ELEMENTS_PER_PAGE (MAX_PAGESIZE / sizeof(T))


  class ArrayPage {
  public:
    T m_element[_ELEMENTS_PER_PAGE];

#ifdef _DEBUG
    String toString() const {
      String result;
      const TCHAR *del = _T("");
      for(int i = 0; i < _ELEMENTS_PER_PAGE; i++, del = _T(",")) {
        result += format(_T("%s%s"), del, m_element[i].toString().cstr());
      }
      return result;
    }
#endif

  };

  class MemoryPage {
  public:
    ArrayPage *m_page;     // Loaded page. At any time there are at most 1024 pages in memory. The others are swapped out on m_pageFile
    int        m_mapIndex; // -1 indicate, that page is not used. else 
                           // m_pageMap[m_memoryPage[i].m_mapIndex].m_pageIndex = i
                           // and m_pageMap[...].m_loaded = 1

    MemoryPage() {
      m_page     = NULL;
      m_mapIndex = -1;
    }

    void allocate() {
      if(m_page == NULL) {
        m_page = new ArrayPage;
      }
    }

    void deallocate() {
      if(m_page) {
        delete m_page;
        m_page = NULL;
      }
      m_mapIndex = -1;
    }

    ~MemoryPage() {
      deallocate();
    }

    T &operator[](size_t index) {
      return m_page->m_element[index % _ELEMENTS_PER_PAGE];
    }

    const T &operator[](size_t index) const {
      return m_page->m_element[index % _ELEMENTS_PER_PAGE];
    }

#ifdef _DEBUG
    String toString() const {
      return format(_T("(mapIndex:%5d,[%s])"), m_mapIndex, pageToString().cstr());
    }

    String pageToString() const {
      return (m_page==NULL) ? _T("null") : m_page->toString();
    }
#endif

  };

  size_t                               m_size;              // total number of elements in array
  mutable CompactArray<PageMapElement> m_pageMap;           // size = number of pages
  mutable BitSet                       m_freeFilePages;
  mutable unsigned int                 m_pageFileSize;      // In pages. NOT bytes
  mutable unsigned __int64             m_referenceCounter;  // Incremented each time operator[] (non const) is called, and saved to PageMapElement. 
  mutable MemoryPage                   m_memoryPage[MEMORYPAGE_COUNT]; 
                                                            // To find a good page to swap, search for the loaded page with the lowest m_lastReference
  mutable String                       m_pageFileName;      // name of m_pageFile
  mutable FILE                        *m_pageFile;          // An ArrayPage is swapped out to m_pageFile, when not accessed for some time,
                                                            // and there are more than 1024 pages. It's loaded again if any of its elements are needed.

  size_t getPageIndex(size_t index) const {
    return index / _ELEMENTS_PER_PAGE;
  }

  unsigned int getPageOffset(size_t index) const {
    return index % _ELEMENTS_PER_PAGE;
  }

#pragma warning( pop )

  void indexError(size_t index) const {
    throwException(_T("BigArray::Index %s out of range. size=%s, elementSize=%d"), format1000(index).cstr(), format1000(m_size).cstr(), sizeof(T));
  }

  String getPageFileName() const {
    if(m_pageFileName.length() == 0) {
      m_pageFileName = _T("C:\\temp\\cXXXXXX");
      _tmktemp(m_pageFileName.cstr());
    }
    return m_pageFileName;
  }

  void resetPageFileSize() const {
    m_pageFileSize = 0;
    m_freeFilePages.clear();
    m_freeFilePages.setCapacity(256);
  }

  void openPageFile() const {
    m_pageFile = MKFOPEN(getPageFileName(), "w+b");
    resetPageFileSize();
  }

  void closePageFile() const {
    if(m_pageFile) {
      fclose(m_pageFile);
      m_pageFile = NULL;
      UNLINK(m_pageFileName);
      m_pageFileName = EMPTYSTRING;
      resetPageFileSize();
    }
  }

  void addFilePage() const {
    if(m_pageFile == NULL) {
      openPageFile();
    }
    const unsigned int newPageIndex = m_pageFileSize++;
    if(m_pageFileSize >= m_freeFilePages.getCapacity()) {
      m_freeFilePages.setCapacity(2*m_pageFileSize);
    }
    m_freeFilePages.add(newPageIndex);
  }

  size_t findFreeFilePage() const {
    if(m_freeFilePages.isEmpty()) {
      addFilePage();
    }
    return m_freeFilePages.select();
  }

  void readPage(size_t index, ArrayPage *a) const {
    assert(a != NULL);
    assert( index < m_pageFileSize);
    assert((index < m_freeFilePages.getCapacity()) && (!m_freeFilePages.contains(index)));
    assert(m_pageFile != NULL);

    FSEEK(m_pageFile, (unsigned __int64)index * sizeof(ArrayPage));
    FREAD(a, sizeof(ArrayPage), 1, m_pageFile);
    m_freeFilePages.add(index);
  }

  void writePage(size_t index, const ArrayPage *a) const {
    assert(a != NULL);
    if(index == m_pageFileSize) {
      addFilePage();
    }
    assert(index < m_pageFileSize);
    assert(m_freeFilePages.contains(index));
    assert(m_pageFile != NULL);
    FSEEK(m_pageFile, (unsigned __int64)index * sizeof(ArrayPage));
    FWRITE(a, sizeof(ArrayPage), 1, m_pageFile);

    m_freeFilePages.remove(index);
  }

  void allocatePage(unsigned int pageIndex) const {
    assert(pageIndex < ARRAYSIZE(m_memoryPage));
    m_memoryPage[pageIndex].allocate();
  }

  void setLoaded(PageMapElement &pm, int pageIndex) const {
    setUnloaded(pageIndex);

    pm.m_loaded = 1;
    m_memoryPage[pm.m_pageIndex = pageIndex].m_mapIndex = (UINT)(&pm - &m_pageMap[0]); // m_pageMap not empty !!
    allocatePage(pageIndex);
  }

  void setUnloaded(int pageIndex) const {
    assert((pageIndex >= 0) && (pageIndex < ARRAYSIZE(m_memoryPage)));

    int &mapIndex = m_memoryPage[pageIndex].m_mapIndex;
    if(mapIndex >= 0) {
      m_pageMap[mapIndex].m_loaded = 0;
      mapIndex = -1;
    }
  }

// unusedPageIndex only set when return NULL. else unchanged
// If NULL is returned, unusedPageIndex is the index into m_memoryPage
// of an unused page, which can be used without saving it first
  PageMapElement *findPageToSwap(int &unusedPageIndex) const {
    const size_t    lastPageIndex = m_size ? getPageIndex(m_size-1) : 0; // m_size always > 0
    PageMapElement *result        = NULL;

    for(int i = 0; i < ARRAYSIZE(m_memoryPage); i++) {
      const int t = m_memoryPage[i].m_mapIndex;
      if(t < 0) {
        m_memoryPage[i].allocate();
        unusedPageIndex = i;
        return NULL;
      } else if(t != lastPageIndex) {
        PageMapElement &e = m_pageMap[t];
        if((e.m_loaded) && ((result == NULL) || (e.m_lastReference < result->m_lastReference))) {
          result = &e;
        }
      }
    }
    assert(result != NULL);
    return result;
  }

  void loadPage(PageMapElement &pmi) const {
    assert(pmi.m_loaded == 0);

    int unusedPageIndex;
    PageMapElement *pmo = findPageToSwap(unusedPageIndex);

    if(pmo) {
      assert(pmo->m_loaded != 0);
      assert(pmo != &pmi);

      MemoryPage &mp = m_memoryPage[pmo->m_pageIndex];
      ArrayPage  tmpPage;
      const unsigned int filePageIndex = pmi.m_pageIndex;

      readPage( filePageIndex, &tmpPage);
      writePage(filePageIndex, mp.m_page);

      memcpy(mp.m_page, &tmpPage, sizeof(ArrayPage));

      setLoaded(pmi, pmo->m_pageIndex);
      pmo->m_pageIndex = filePageIndex;

      assert(pmo->m_loaded == 0);

    } else {
      assert((unusedPageIndex >= 0) && (unusedPageIndex < ARRAYSIZE(m_memoryPage)));
      MemoryPage &mp = m_memoryPage[unusedPageIndex];
      readPage(pmi.m_pageIndex, mp.m_page);
      setLoaded(pmi, unusedPageIndex);
    }
    assert(pmi.m_loaded == 1);
  }

  void incrementSize() {
    if(getPageOffset(m_size) == 0) { // Need a new page
      const size_t pageIndex = getPageIndex(m_size);
      if(pageIndex >= m_pageMap.size()) {
        m_pageMap.add(PageMapElement());
        PageMapElement &pmi = m_pageMap.last();
        int unusedPageIndex;
        PageMapElement *pmo = findPageToSwap(unusedPageIndex);
        if(pmo) {
          const size_t filePageIndex = findFreeFilePage();
          writePage(filePageIndex, m_memoryPage[pmo->m_pageIndex].m_page);
          setLoaded(pmi, pmo->m_pageIndex);
          pmo->m_pageIndex = filePageIndex;
          assert(pmo->m_loaded == 0);
        } else {
          setLoaded(pmi, unusedPageIndex);
        }
      }
    }
    m_size++;
  }

  void decrementSize(size_t count) {
    assert(m_freeFilePages.getCapacity() > m_pageFileSize);

    if((m_size -= count) == 0) {
      cleanup();
    } else {
      const size_t newPageCount = m_size ? (getPageIndex(m_size-1) + 1) : 0;
      const size_t del          = m_pageMap.size() - newPageCount;
      if(del > 0) {
        for(size_t pi = newPageCount; pi < m_pageMap.size(); pi++) {
          PageMapElement &pm = m_pageMap[pi];
          if(pm.m_loaded) {
            setUnloaded(pm.m_pageIndex);
          } else {
            m_freeFilePages.add(pm.m_pageIndex);
          }
        }
        m_pageMap.remove(newPageCount, del);
      }
    }
  }

  void init() {
    m_size             = 0;
    m_referenceCounter = 0;
    m_pageFile         = NULL;
    m_pageFileSize     = 0;
  }

  void cleanup() {
    closePageFile();
    m_pageMap.clear();
    for(int i = 0; i < ARRAYSIZE(m_memoryPage); i++) {
      m_memoryPage[i].deallocate();
    }
    m_referenceCounter = 0;
    m_size             = 0;
  }

public:
  BigArray() : m_freeFilePages(256) {
    init();
    CHECK_BIGARRAY_INVARIANT();
  }

  virtual ~BigArray() {
    cleanup();
  }

  BigArray(const BigArray<T> &src) : m_freeFilePages(src.m_freeFilePages.getCapacity()){
    init();
    for(size_t i = 0; i < src.m_size; i++) {
      add(src[i]);
    }
  }

  BigArray<T> &operator=(const BigArray<T> &src) {
    if(this == &src) {
      return *this;
    }
    clear();
    for(size_t i = 0; i < src.m_size; i++) {
      add(src[i]);
    }
    return *this;
  }

  T &operator[](size_t i) {
    CHECK_BIGARRAY_INVARIANT();
    if(i >= m_size) {
      indexError(i);
    }
    size_t pageIndex;
    PageMapElement &pm = m_pageMap[pageIndex = getPageIndex(i)];
    if(!pm.m_loaded) {
      loadPage(pm);
    }
    MemoryPage &page = m_memoryPage[pm.m_pageIndex];
    pm.m_lastReference = m_referenceCounter++;
    CHECK_BIGARRAY_INVARIANT();
    return page[i];
  }
  
  const T &operator[](size_t i) const {
    CHECK_BIGARRAY_INVARIANT();
    if(i >= m_size) {
      indexError(i);
    }
    size_t pageIndex;
    PageMapElement &pm = m_pageMap[pageIndex = getPageIndex(i)];
    if(!pm.m_loaded) {
      ((BigArray<T>*)this)->loadPage(pm);
    }
    const MemoryPage &page = m_memoryPage[pm.m_pageIndex];
    pm.m_lastReference     = m_referenceCounter++;
    CHECK_BIGARRAY_INVARIANT();
    return page[i];
  }
  
  void add(size_t i, const T &e) {
    CHECK_BIGARRAY_INVARIANT();
    if(i > m_size) {
      indexError(i);
    }

    incrementSize();
    for(size_t j = m_size-1; j > i; j--) {
      (*this)[j+1] = (*this)[j];
    }
    (*this)[i] = e;
    CHECK_BIGARRAY_INVARIANT();
  }

  void add(const T &e) {
    add(m_size, e);
  }

  void remove(size_t i, size_t count=1) {
    CHECK_BIGARRAY_INVARIANT();
    if(count == 0) {
      return;
    }
    size_t j = i+count;
    if(j > m_size) {
      throwException(_T("BigArray::remove(%s,%%s):Index %s out of range. size=%s, elementSize=%d")
                    ,format1000(i).cstr(), format1000(count).cstr()
                    ,format1000(j).cstr(), format1000(m_size).cstr()
                    ,sizeof(T));
    }
    while(j < m_size) {
      (*this)[i++] = (*this)[j++];
    }
    decrementSize(count);
    CHECK_BIGARRAY_INVARIANT();
  }

  const T &select() const {
    if(m_size == 0) {
      throwException(_T("BigArray::Cannot select element from empty array"));
    }

    return (*this)[randSizet(m_size)];
  }

  size_t size() const {
    return m_size;
  }

  unsigned __int64 getPageFileSize() const { // In bytes
    return (unsigned __int64)m_pageFileSize * sizeof(ArrayPage);
  }

  void clear() {
    CHECK_BIGARRAY_INVARIANT();
    cleanup();
    init();
    CHECK_BIGARRAY_INVARIANT();
  }

  bool isEmpty() const {
    return m_size == 0;
  }

  void save(FILE *f) const {
    FWRITE(&m_size, sizeof(m_size), 1, f);
    for(size_t i = 0; i < m_size;) {
      ArrayPage tmpPage;
      for(int j = 0; j < _ELEMENTS_PER_PAGE && i < m_size; j++, i++) {
        tmpPage.m_element[j] = (*this)[i];
      }
      FWRITE(&tmpPage, sizeof(T), j, f);
    }
  }

  void load(FILE *f) {
    clear();
    size_t size;
    FREAD(&size, sizeof(size), 1, f);
    for(size_t i = 0; i < size;) {
      ArrayPage tmpPage;
      intptr_t rest = min(_ELEMENTS_PER_PAGE, size - i);
      intptr_t got;
      if((got = FREAD(&tmpPage, sizeof(T), rest, f)) != rest) {
        throwException(_T("Expected number of elements in file is %s. Can only read %s elements")
                      ,format1000(size).cstr(), format1000(i + got).cstr());
      }
      for(intptr_t j = 0; j < rest;) {
        add(tmpPage.m_element[j++]);
      }
      i += rest;
    }
  }

  friend bool operator==(const BigArray<T> &a1, const BigArray<T> &a2) {
    const size_t count = a1.size();
    if(count != a2.size()) {
      return false;
    }
    for(size_t i = 0; i < count; i++) {
      if(!(a1[i] == a2[i])) {
        return false;
      }
    }
    return true;
  }

  friend bool operator!=(const BigArray<T> &a1, const BigArray<T> &a2) {
    return !(a1 == a2);
  }

#ifdef _DEBUG
  void checkInvariant() const {
    try {
      if(m_size == 0) {
        for(int i = 0; i < ARRAYSIZE(m_memoryPage); i++) {
          if(m_memoryPage[i].m_mapIndex != -1) {
            throwException(_T("size=0, memoryPage[%d].m_mapIndex=%d (!= -1)"), i, m_memoryPage[i].m_mapIndex);
          }
          if(m_pageMap.size() != 0) {
            throwException(_T("size=0, pageTable.size=%s"), m_pageMap.size());
          }
        }
      } else {
        size_t lastPageIndex = getPageIndex(m_size-1);
        BitSet loadedPages(ARRAYSIZE(m_memoryPage));
        for(size_t i = 0; i <= lastPageIndex; i++) {
          const PageMapElement &pm = m_pageMap[i];
          if(pm.m_loaded) {
            const int pageIndex = pm.m_pageIndex;
            if(pageIndex >= ARRAYSIZE(m_memoryPage)) {
              throwException(_T("pageMap[%s].m_pageIndex=%d (should be < %d)")
                            ,format1000(i).cstr()
                            ,pageIndex
                            ,ARRAYSIZE(m_memoryPage));
            }
            if(loadedPages.contains(pageIndex)) {
              throwException(_T("loadedPages already contains %d"), pageIndex);
            }
            loadedPages.add(pageIndex);
            if(m_memoryPage[pageIndex].m_mapIndex != i) {
              throwException(_T("memoryPage[%d].m_mapIndex = %d (!= %d = pageMap[%s].m_pageIndex)")
                            ,pageIndex
                            ,m_memoryPage[pageIndex].m_mapIndex
                            ,format1000(i).cstr());
            }
          }
        }
        for(int i = 0; i < ARRAYSIZE(m_memoryPage); i++) {
          if(!loadedPages.contains(i)) {
            if(m_memoryPage[i].m_mapIndex != -1) {
              throwException(_T("page %d is not loaded, and memoryPage[%d].m_mapIndex = %d (should be -1)"), i, m_memoryPage[i].m_mapIndex);
            }
          }
        }
      }
      if(m_freeFilePages.getCapacity() <= m_pageFileSize) {
        throwException(_T("freeFilePages.capacity=%d <= pageFileSize = %d"), m_freeFilePages.getCapacity(), m_pageFileSize);
      }
      if(m_pageFileSize == 0) {
        if(!m_freeFilePages.isEmpty()) {
          throwException(_T("m_pageFileSize=0 and !freeFilePages.empty"));
        }
      } else {
        BitSet expectedFreeFilePages(m_freeFilePages.getCapacity());
        expectedFreeFilePages.add(0, m_pageFileSize-1);
        for(size_t i = 0; i < m_pageMap.size(); i++) {
          const PageMapElement &pm = m_pageMap[i];
          if(pm.m_loaded == 0) {
            expectedFreeFilePages.remove(pm.m_pageIndex);
          }
        }
        if(m_freeFilePages != expectedFreeFilePages) {
          throwException(_T("m_freeFilePages != expectedFreeFilePages. freeFilePages=%s, expected=%s")
                        ,m_freeFilePages.toString().cstr(), expectedFreeFilePages.toString().cstr());
        }
      }
    } catch(Exception e) {
      throwException(_T("Broken invariant:%s"), e.what());
    }
  }

  void show() {
    COORD oldCursor = Console::getCursorPos();
    for(int l = 0; l < 20; l++) {
      Console::clearLine(l);
    }
    Console::setCursorPos(0,0);
    size_t lastPageIndex = (m_size == 0) ? 0 : getPageIndex(m_size-1);

    _tprintf(_T("Size=%s. PageCount=%s. PageFileSize=%u\n")
            ,format1000(m_size).cstr()
            ,format1000(m_pageMap.size()).cstr()
            ,m_pageFileSize);
    for(size_t i = 0; i < m_pageMap.size(); i++) {
      _tprintf(_T("pageTable[%s]:[%2s-%2s]%s\n")
              ,format1000(i).cstr()
              ,format1000(i * _ELEMENTS_PER_PAGE).cstr()
              ,format1000((i+1)*_ELEMENTS_PER_PAGE-1).cstr()
              ,m_pageMap[i].toString().cstr());
    }

    for(int i = 0; i < ARRAYSIZE(m_memoryPage); i++) {
      _tprintf(_T("mem-page[%2d]:%s\n"), i, m_memoryPage[i].toString().cstr());
    }
    _tprintf(_T("Free filepages:<%s>\n"), m_freeFilePages.toString().cstr());
    Console::setCursorPos(oldCursor);
  }
#endif

};
