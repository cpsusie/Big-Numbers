#include "stdafx.h"
#include "CppUnitTest.h"
#include <Random.h>
#include <PackedArray.h>
#include <Console.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestPackedArray {		

#include <UnitTestTraits.h>

  class TestClass {
  public:
    TestClass(int bitsPerItem);
    Array<unsigned int> m_a;
    PackedArray         m_pa;
    bool checkEqual() const;
    void set(unsigned int index, unsigned int v);
    void or (unsigned int index, unsigned int v);
    void and(unsigned int index, unsigned int v);
    void xor(unsigned int index, unsigned int v);
    void add(unsigned int v);
    void add(unsigned int index, unsigned int v);
    void remove(unsigned int index, unsigned int count = 1);
    void safeRemove(unsigned int index, unsigned int count = 1);
    void addZeroes(unsigned int index, unsigned int count);
    void addTestElements(int count, bool random);
    size_t size() const;
    void clear();

#ifdef _DEBUG
    void dump(const TCHAR *format, ...) const;
#endif

  };

  TestClass::TestClass(int bitsPerItem) : m_pa(bitsPerItem) {
  }

  void TestClass::set(unsigned int index, unsigned int v) {
    m_a[index] = v;
    m_pa.set(index, v);
  }

  //static int orCounter = 0;
  void TestClass:: or (unsigned int index, unsigned int v) {
    /*
    int counter = orCounter++;

    const unsigned int a1  = m_a[index];
    const unsigned int pa1 = m_pa.get(index);

    unsigned int expected = a1 | v;
    */
    m_a[index] |= v;
    //  const unsigned int a2 = m_a[index];

    m_pa. or (index, v);

    /*
    const unsigned int pa2 = m_pa.get(index);

    if(a2 != expected) {
      OUTPUT(_T("m_a[%d] = %lu (%s) != expected = %lu (%s) = %s | %s")
      , index
      , a2, sprintbin(a2).cstr()
      , expected, sprintbin(expected).cstr()
      , sprintbin(a1).cstr(), sprintbin(v).cstr()
      );
    }

    if(pa2 != expected) {
      OUTPUT(_T("m_pa.get(%d) = %lu (%s) != expected = %lu (%s) = %s | %s")
      , index
      , pa2, sprintbin(pa2).cstr()
      , expected, sprintbin(expected).cstr()
      , sprintbin(a1).cstr(), sprintbin(v).cstr()
      );
      }
    */
  }

  void TestClass:: and (unsigned int index, unsigned int v) {
    m_a[index] &= v;
    m_pa. and (index, v);
  }

  void TestClass:: xor (unsigned int index, unsigned int v) {
    m_a[index] ^= v;
    m_pa. xor (index, v);
  }

  void TestClass::add(unsigned int v) {
    m_a.add(v);
    m_pa.add(v);
  }

  void TestClass::add(unsigned int index, unsigned int v) {
    m_a.add(index, v);
    m_pa.add(index, v);
  }

  void TestClass::remove(unsigned int index, unsigned int count) {
    m_a.removeIndex(index, count);
    m_pa.remove(index, count);
  }

  void TestClass::safeRemove(unsigned int index, unsigned int count) {
    m_a.removeIndex(index, count);
    const unsigned __int64 n = m_pa.size();
    for (int i = index + count; i < n;) {
      m_pa.set(index++, m_pa.get(i++));
    }
    while (index < n) {
      m_pa.set(index++, 0);
    }
  }

  void TestClass::addZeroes(unsigned int index, unsigned int count) {
#ifdef _DEBUG
    if (PackedArray::trace) {
      OUTPUT(_T("addZeroes(index=%d,count=%d)"), index, count);
    }
#endif

    for (unsigned int i = 0; i < count; i++) {
      m_a.add(index, 0);
    }
    m_pa.addZeroes(index, count);
  }

  size_t TestClass::size() const {
    const size_t result = m_a.size();
    if (result != m_pa.size()) {
      checkEqual();
    }
    return result;
  }

  void TestClass::clear() {
    m_a.clear();
    m_pa.clear();
  }

  void TestClass::addTestElements(int count, bool random) {
    const unsigned int mask = (1 << m_pa.getBitsPerItem()) - 1;
    clear();
    if (random) {
      for (int i = 0; i < count; i++) {
        add(randInt() & mask);
      }
    }  else {
      for (int i = 0; i < count; i++) {
        add((~(i + 1)) & mask);
      }
    }
  }

  bool TestClass::checkEqual() const {
    if (m_pa.size() != m_a.size()) {
      OUTPUT(_T("Size differs. Array.size=%d, PackedArray.size=%lld"), m_a.size(), m_pa.size());
      return false;
    }
    const size_t n = m_a.size();
    for (size_t i = 0; i < n; i++) {
      unsigned int ea = m_a[i];
      unsigned int epa = m_pa.get(i);
      if (ea != epa) {
        OUTPUT(_T("Array[%3d]=%10u, PackedArray.get(%3d)=%10u <----- Error. bitsPerItem:%d"), i, ea, i, epa, m_pa.getBitsPerItem());
        return false;
      }
    }

    try {
      m_pa.checkInvariant();
    } catch (Exception e) {
      OUTPUT(_T("%s"), e.what());
      return false;
    }

    return true;
  }

#ifdef _DEBUG
  void TestClass::dump(const TCHAR *formatStr, ...) const {
    va_list argptr;
    va_start(argptr, formatStr);
    const String label = vformat(formatStr, argptr);
    va_end(argptr);
    String tmpStr;
    const int l = m_pa.getBitsPerItem();
    int bitCount = 0;
    for (size_t i = 0; i < m_a.size(); i++) {
      bitCount += l;
      if (bitCount > 32) {
        tmpStr += _T(":");
        bitCount %= 32;
      }
      tmpStr += format(_T("%*d"), l, m_a[i]);
    }
    OUTPUT(_T("%sArray: Size:%d:[%s]"), label.cstr(), m_a.size(), tmpStr.cstr());
    m_pa.dump();
    OUTPUT(_T("---------------------------------------------------------------------------------"));
  }
#endif

  static bool testAddAndSet(int bitsPerItem) {
    TestClass cl(bitsPerItem);
    const unsigned int mask = (1 << bitsPerItem) - 1;
    for (unsigned int i = 0; i < 100; i++) {
      cl.add(randInt() & mask);
    }

    if (!cl.checkEqual()) {
      return false;
    }

    for (unsigned int i = 0; i < cl.size(); i += 2) {
      cl.set(i, randInt() & mask);
    }

    if (!cl.checkEqual()) {
      return false;
    }

    for (unsigned int i = 0; i < cl.size(); i += 2) {
      cl. or (i, randInt() & mask);
    }

    if (!cl.checkEqual()) {
      return false;
    }


    for (unsigned int i = 0; i < cl.size(); i += 2) {
      cl. and (i, randInt() & mask);
    }

    if (!cl.checkEqual()) {
      return false;
    }

    for (unsigned int i = 0; i < cl.size(); i += 2) {
      cl. xor (i, randInt() & mask);
    }

    if (!cl.checkEqual()) {
      return false;
    }

    return true;
  }


  static bool testAddZeroes(int bitsPerItem, int size, int pos, int count, bool random) {
    //  if((bitsPerItem * count) % 8 != 0) {
    //    return true;
    //  }
    TestClass cl(bitsPerItem);
    cl.addTestElements(size, random);
#ifdef _DEBUG
    TestClass clCopy = cl;
#endif
    static int printCount = 0;
    if (++printCount % 5000 == 0) {
      Console::setCursorPos(0, 0);
      _tprintf(_T("bits/item:%d, size:%3d, pos:%3d, count:%3d\r"), bitsPerItem, size, pos, count);
    }

    cl.addZeroes(pos, count);
    if (!cl.checkEqual()) {
#ifdef _DEBUG
      cl = clCopy;
      cl.dump(_T("Before insert: "));
      cl.addZeroes(pos, count);
      cl.dump(_T("After insert(%d,%d): "), pos, count);

      cl = clCopy;
      PackedArray::trace = true;
      cl.addZeroes(pos, count);
      PackedArray::trace = false;
#endif
      pause();
      return false;
    }
    return true;
  }

  static bool testRemove(int bitsPerItem, int size, int pos, int count, bool random) {
    //  if((bitsPerItem * count) % 8 == 0) {
    //    return true;
    //  }
    TestClass cl(bitsPerItem);
    cl.addTestElements(size, random);
#ifdef _DEBUG
    TestClass clCopy = cl;
#endif
    static int printCount = 0;
    if (++printCount % 5000 == 0) {
      Console::setCursorPos(0, 0);
      OUTPUT(_T("bits/item:%d, size:%3d, pos:%3d, count:%3d"), bitsPerItem, size, pos, count);
    }

    cl.remove(pos, count);
    if (!cl.checkEqual()) {
#ifdef _DEBUG
      cl = clCopy;
      cl.dump(_T("Before remove: "));
      cl.remove(pos, count);
      cl.dump(_T("After remove(%d,%d): "), pos, count);
      cl = clCopy;
      PackedArray::trace = true;
      cl.remove(pos, count);
      PackedArray::trace = false;
#endif
      pause();
      return false;
    }
    return true;
  }

	TEST_CLASS(TestPackedArray)
	{
    public:

    TEST_METHOD(PackedArrayPrimitiveOperations) {
      const static TCHAR *objectToTest = _T("PackedArray");

      const String verboseStr = format(_T("Testing %s%s"), objectToTest, spaceString(15 - _tcsclen(objectToTest), '.').cstr());

      const int testCount = 300;
      double startTime = getProcessTime();
      bool ok = true;

      for (int i = 0; i < testCount; i++) {
        const int bitsPerItem = randInt(1, 31);
        const int size = randInt() % 20000 + 1;
        int pos = randInt() % (size + 1);
        int count = randInt() % 300 + 1;

        if (i % 30 == 0) {
          OUTPUT(_T("%s %5.0lf%% BitsPerItem=%2d"), verboseStr.cstr(), (double)(i + 1)*100.0 / testCount, bitsPerItem);
        }

        verify(testAddAndSet(bitsPerItem));

        verify(testAddZeroes(bitsPerItem, size, pos, count, true));

        pos = randInt() % size;
        count = randInt() % (size - pos);
        verify(testRemove(bitsPerItem, size, pos, count, true));
      }

#ifdef INTENSIVE_TEST
      for (int bitsPerItem = 1; bitsPerItem < 32; bitsPerItem++) {

        OUTPUT(_T("Testing PackedArray. bitsPerItem=%d"), bitsPerItem);
        testAddAndSet(bitsPerItem);

        for (int size = 1; size < 100; size++) {
          for (int pos = 0; pos <= size; pos++) {
            for (int count = 1; count < 100; count++) {
              testAddZeroes(bitsPerItem, size, pos, count, false);
            }
          }
        }
        for (size = 1; size < 100; size++) {
          for (int pos = 0; pos <= size; pos++) {
            for (int count = 1; count < size - pos; count++) {
              testRemove(bitsPerItem, size, pos, count, false);
            }
          }
        }

      }
#endif
    }
  };
}
