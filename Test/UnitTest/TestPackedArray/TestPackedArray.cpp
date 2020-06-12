#include "stdafx.h"
#include <Random.h>
#include <ProcessTools.h>
#include <PackedArray.h>
#include <Console.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestPackedArray {		

#include <UnitTestTraits.h>

  class TestClass {
  public:
    TestClass(int bitsPerItem);
    Array<UINT>         m_a;
    PackedArray         m_pa;
    void set(UINT index, UINT v);
    void or (UINT index, UINT v);
    void and(UINT index, UINT v);
    void xor(UINT index, UINT v);
    void add(UINT v);
    void add(UINT index, UINT v);
    void remove(    UINT index, UINT count = 1);
    void safeRemove(UINT index, UINT count = 1);
    void addZeroes( UINT index, UINT count);
    void addTestElements(int count, bool random);
    size_t size() const;
    void clear();
    bool checkEqual() const;
    bool checkIterators() const;

#if defined(_DEBUG)
    void dump(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) const;
#endif
  };

  TestClass::TestClass(int bitsPerItem) : m_pa(bitsPerItem) {
  }

  void TestClass::set(UINT index, UINT v) {
    m_a[index] = v;
    m_pa.set(index, v);
  }

  //static int orCounter = 0;
  void TestClass:: or (UINT index, UINT v) {
    /*
    int counter = orCounter++;

    const UINT a1  = m_a[index];
    const UINT pa1 = m_pa.get(index);

    UINT expected = a1 | v;
    */
    m_a[index] |= v;
    //  const UINT a2 = m_a[index];

    m_pa. or (index, v);

    /*
    const UINT pa2 = m_pa.get(index);

    if(a2 != expected) {
      OUTPUT(_T("m_a[%d] = %lu (%s) != expected = %lu (%s) = %s | %s")
            ,index
            ,a2, sprintbin(a2).cstr()
            ,expected, sprintbin(expected).cstr()
            ,sprintbin(a1).cstr(), sprintbin(v).cstr()
            );
    }

    if(pa2 != expected) {
      OUTPUT(_T("m_pa.get(%d) = %lu (%s) != expected = %lu (%s) = %s | %s")
            ,index
            ,pa2, sprintbin(pa2).cstr()
            ,expected, sprintbin(expected).cstr()
            ,sprintbin(a1).cstr(), sprintbin(v).cstr()
            );
    }
    */
  }

  void TestClass:: and (UINT index, UINT v) {
    m_a[index] &= v;
    m_pa. and (index, v);
  }

  void TestClass:: xor (UINT index, UINT v) {
    m_a[index] ^= v;
    m_pa. xor (index, v);
  }

  void TestClass::add(UINT v) {
    m_a.add(v);
    m_pa.add(v);
  }

  void TestClass::add(UINT index, UINT v) {
    m_a.add(index, v);
    m_pa.add(index, v);
  }

  void TestClass::remove(UINT index, UINT count) {
    m_a.removeIndex(index, count);
    m_pa.remove(index, count);
  }

  void TestClass::safeRemove(UINT index, UINT count) {
    m_a.removeIndex(index, count);
    const UINT64 n = m_pa.size();
    for(int i = index + count; i < n;) {
      m_pa.set(index++, m_pa.get(i++));
    }
    while(index < n) {
      m_pa.set(index++, 0);
    }
  }

  void TestClass::addZeroes(UINT index, UINT count) {
#if defined(_DEBUG_PACKEDARRAY)
    if(PackedArray::trace) {
      OUTPUT(_T("addZeroes(index=%d,count=%d)"), index, count);
    }
#endif

    for(UINT i = 0; i < count; i++) {
      m_a.add(index, 0);
    }
    m_pa.addZeroes(index, count);
  }

  size_t TestClass::size() const {
    const size_t result = m_a.size();
    if(result != m_pa.size()) {
      checkEqual();
    }
    return result;
  }

  void TestClass::clear() {
    m_a.clear();
    m_pa.clear();
  }

  void TestClass::addTestElements(int count, bool random) {
    const UINT mask = (1 << m_pa.getBitsPerItem()) - 1;
    clear();
    if(random) {
      for(int i = 0; i < count; i++) {
        add(randInt() & mask);
      }
    } else {
      for(int i = 0; i < count; i++) {
        add((~(i + 1)) & mask);
      }
    }
  }

  bool TestClass::checkEqual() const {
    if(m_pa.size() != m_a.size()) {
      OUTPUT(_T("Size differs. Array.size=%zu, PackedArray.size=%llu"), m_a.size(), m_pa.size());
      return false;
    }
    const size_t n = m_a.size();
    for(size_t i = 0; i < n; i++) {
      const UINT ea  = m_a[i];
      const UINT epa = m_pa.get(i);
      if(ea != epa) {
        OUTPUT(_T("Array[%3zu]=%10u, PackedArray.get(%3zu)=%10u <----- Error. bitsPerItem:%d"), i, ea, i, epa, m_pa.getBitsPerItem());
        return false;
      }
    }
    try {
      m_pa.checkInvariant(__TFUNCTION__);
    } catch(Exception e) {
      OUTPUT(_T("%s"), e.what());
      return false;
    }
    if(!checkIterators()) {
      return false;
    }
    return true;
  }

  bool TestClass::checkIterators() const {
    Iterator<UINT> it1 = ((TestClass*)this)->m_a.getIterator();
    Iterator<UINT> it2 = ((TestClass*)this)->m_pa.getIterator();
    bool hnext1 = it1.hasNext();
    bool hnext2 = it2.hasNext();

    while(hnext1 || hnext2) {
      if(!hnext1) {
        OUTPUT(_T("Iterator1 as no more elements"));
        return false;
      }
      if(!hnext2) {
        OUTPUT(_T("Iterator2 as no more elements"));
        return false;
      }
      const UINT v1 = it1.next();
      const UINT v2 = it2.next();
      if(v1 != v2) {
        OUTPUT(_T("Iterators return different values. v1=%u, v2=%u"),v1,v2);
        return false;
      }
      hnext1 = it1.hasNext();
      hnext2 = it2.hasNext();
    }
    return true;
  }

#if defined(_DEBUG_PACKEDARRAY)
  void TestClass::dump(_In_z_ _Printf_format_string_ TCHAR const * const formatStr, ...) const {
    va_list argptr;
    va_start(argptr, formatStr);
    const String label = vformat(formatStr, argptr);
    va_end(argptr);
    String tmpStr;
    const int l = m_pa.getBitsPerItem();
    int bitCount = 0;
    for(size_t i = 0; i < m_a.size(); i++) {
      bitCount += l;
      if(bitCount > 32) {
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
    const UINT mask = (1 << bitsPerItem) - 1;
    for(UINT i = 0; i < 100; i++) {
      cl.add(randInt() & mask);
    }

    if(!cl.checkEqual()) {
      return false;
    }

    for(UINT i = 0; i < cl.size(); i += 2) {
      cl.set(i, randInt() & mask);
    }

    if(!cl.checkEqual()) {
      return false;
    }

    for(UINT i = 0; i < cl.size(); i += 2) {
      cl. or (i, randInt() & mask);
    }

    if(!cl.checkEqual()) {
      return false;
    }


    for(UINT i = 0; i < cl.size(); i += 2) {
      cl. and (i, randInt() & mask);
    }

    if(!cl.checkEqual()) {
      return false;
    }

    for(UINT i = 0; i < cl.size(); i += 2) {
      cl. xor (i, randInt() & mask);
    }

    if(!cl.checkEqual()) {
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
#if defined(_DEBUG)
    TestClass clCopy = cl;
#endif
    static int printCount = 0;
    if(++printCount % 5000 == 0) {
      Console::setCursorPos(0, 0);
      _tprintf(_T("bits/item:%d, size:%3d, pos:%3d, count:%3d\r"), bitsPerItem, size, pos, count);
    }

    cl.addZeroes(pos, count);
    if(!cl.checkEqual()) {
#if defined(_DEBUG_PACKEDARRAY)
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
#if defined(_DEBUG)
    TestClass clCopy = cl;
#endif
    static int printCount = 0;
    if(++printCount % 5000 == 0) {
      Console::setCursorPos(0, 0);
      OUTPUT(_T("bits/item:%d, size:%3d, pos:%3d, count:%3d"), bitsPerItem, size, pos, count);
    }

    cl.remove(pos, count);
    if(!cl.checkEqual()) {
#if defined(_DEBUG_PACKEDARRAY)
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

	TEST_CLASS(TestPackedArray) {
    public:

    TEST_METHOD(PackedArrayPrimitiveOperations) {
      const static TCHAR *objectToTest = _T("PackedArray");

      const String verboseStr = format(_T("Testing %s%s"), objectToTest, spaceString(15 - _tcsclen(objectToTest), '.').cstr());

      const int testCount = 300;
      double startTime = getProcessTime();
      bool ok = true;

      for(int i = 0; i < testCount; i++) {
        const int bitsPerItem = randInt(1, 31);
        const int size = randInt() % 20000 + 1;
        int pos = randInt() % (size + 1);
        int count = randInt() % 300 + 1;

        if(i % 30 == 0) {
          INFO(_T("%s %5.0lf%% BitsPerItem=%2d"), verboseStr.cstr(), (double)(i + 1)*100.0 / testCount, bitsPerItem);
        }

        verify(testAddAndSet(bitsPerItem));

        verify(testAddZeroes(bitsPerItem, size, pos, count, true));

        pos = randInt() % size;
        count = randInt() % (size - pos);
        verify(testRemove(bitsPerItem, size, pos, count, true));
      }

#if defined(INTENSIVE_TEST)
      for(int bitsPerItem = 1; bitsPerItem < 32; bitsPerItem++) {

        OUTPUT(_T("Testing PackedArray. bitsPerItem=%d"), bitsPerItem);
        testAddAndSet(bitsPerItem);

        for(int size = 1; size < 100; size++) {
          for(int pos = 0; pos <= size; pos++) {
            for(int count = 1; count < 100; count++) {
              testAddZeroes(bitsPerItem, size, pos, count, false);
            }
          }
        }
        for(size = 1; size < 100; size++) {
          for(int pos = 0; pos <= size; pos++) {
            for(int count = 1; count < size - pos; count++) {
              testRemove(bitsPerItem, size, pos, count, false);
            }
          }
        }

      }
#endif // INTENSIVE_TEST
    }

    TEST_METHOD(TestPackedFileArray) {
      const static TCHAR *objectToTest = _T("PackedFileArray");
      const String fileName = getTestFileName(__TFUNCTION__);
      for(UINT bitsPerItem = 3; bitsPerItem <= 31; bitsPerItem+=5) {
        PackedArray pa(bitsPerItem);
        const UINT   maxV  = pa.getMaxValue();
        const UINT   count = randInt(10000);
        for(size_t i = 0; i < count; i++) {
          pa.add(randInt(maxV));
        }
        pa.save(ByteOutputFile(fileName));
        PackedFileArray pfa(fileName,0);

        verify(pfa.getBitsPerItem() == pa.getBitsPerItem());
        verify(pfa.size() == pa.size());
        for(size_t i = 0; i < count/10; i++) {
          const size_t index = randInt(count);
          const UINT pav = pa.get(index);
          const UINT pfav = pfa.get(index);
          verify(pav == pfav);
        }
        try {
          pfa.get(count);
          verify(false);
        } catch(Exception e) {
          // ignore. Expected
        }
      }
    }
  };
}
