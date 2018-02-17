#pragma once

#include <Registry.h>
#include <ByteArray.h>

#define MAXDATACOUNT 1000
// maximal number of elements in Array

typedef enum {
  FIXED_SEED
 ,SAME_RANDOM_SEED
 ,RANDOM_SEED
} RandomizationMethod;

class InitializeParameters {
private:
  void setDefault();
  static RegistryKey getKey();
public:
  int                       m_initMethod;
  size_t                    m_elementCount;
  int                       m_elementSize;
  UINT                      m_periodCount;
  RandomizationMethod       m_randomizationMethod;
  UINT                      m_seed;
  UINT                      m_randomSeed;
  mutable MersenneTwister32 m_random;
  String                    m_fileName;
  CompactIntArray           m_fileData;

  InitializeParameters();
  void readTextFile(const String &fileName);
  void save();
  void load();
  void nextRandomSeed();
  String getInitString() const;
};

class DataArray : private ByteArray {
private:
  const InitializeParameters &m_param;
  void setElement(BYTE *e, UINT value);
public:
  DataArray(const InitializeParameters &param) : m_param(param) {
  }
  DataArray &operator=(const DataArray &src);
  void init();
  void reset(); // set all elements to 0
  bool isSorted() const;
  inline void *getData() {
    return (void*)ByteArray::getData();
  }
  inline const void *getData() const {
    return (const void*)ByteArray::getData();
  }
  inline int size() const {
    return (int)(ByteArray::size() / getElementSize());
  }
  UINT getMaxValue() const;
  inline int getElementSize() const {
    return m_param.m_elementSize;
  }
  UINT operator[](size_t index) const;
  BitSet &getModified(BitSet &dst, const DataArray &a) const;
};

template <class T> class CountComparator : public AbstractComparator {
private:
  int  m_countBuffer;
  int *m_compareCount;
public:
  CountComparator() {
    m_countBuffer  = 0;
    m_compareCount = &m_countBuffer;
  }
  CountComparator(int &compareCount) {
    m_countBuffer  = -1;
    m_compareCount = &compareCount;
    compareCount   = 0;
  }

  int cmp(const void *e1, const void *e2) {
    (*m_compareCount)++;
    return *(T*)e1 - *(T*)e2;
  }

  int getCompareCount() const {
    return *m_compareCount;
  }

  AbstractComparator *clone() const {
    if(m_compareCount != &m_countBuffer) {
      return new CountComparator<T>(*this);
    } else {
      CountComparator *cc = new CountComparator;
      cc->m_countBuffer = m_countBuffer;
      return cc;
    }
  }
};


void analyzeSortMethod(const SortMethodId *sortMethod, const InitializeParameters &parameters);

void errorMessage(_In_z_ _Printf_format_string_ TCHAR const * const format,...);

void makeMinSizeTimePlot();
