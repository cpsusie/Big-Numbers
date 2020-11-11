#include "stdafx.h"
#include <Random.h>
#include <MathUtil.h>
#include <InputValue.h>
#include <BitSet.h>
#include <BitMatrix.h>
#include <CompactHashSet.h>
#include <ByteMemoryStream.h>
#include <ByteFile.h>
#include <Thread.h>
#include <TimeMeasure.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestBitSet {

#include <UnitTestTraits.h>

  BitSet &genRandomSet(BitSet &dst, RandomGenerator &rnd, size_t capacity, intptr_t size) {
    dst.clear();
    dst.setCapacity(capacity);
    if(size == -1) {
      for(size_t i = capacity / 2; i--;) {
        dst.add(randSizet(capacity, rnd));
      }
    } else if((size_t)size >= capacity) {
      dst.invert();
    } else if((size_t)size > capacity / 2) {
      dst.invert();
      for(size_t t = capacity - size; t;) {
        const size_t e = randSizet(capacity, rnd);
        if(dst.contains(e)) {
          dst.remove(e);
          t--;
        }
      }
    } else {
      for(size_t t = size; t;) {
        const size_t e = randSizet(capacity, rnd);
        if(!dst.contains(e)) {
          dst.add(e);
          t--;
        }
      }
    }
    return dst;
  }

  BitSet genRandomSet(RandomGenerator &rnd, size_t capacity, intptr_t size = -1) {
    BitSet result(10);
    return genRandomSet(result, rnd, capacity, size);
  }

  MatrixIndex getRandomIndex(const MatrixDimension &dim, RandomGenerator &rnd) {
    return MatrixIndex(randSizet(dim.rowCount, rnd), randSizet(dim.columnCount, rnd));
  }

  BitMatrix &genRandomMatrix(BitMatrix &dst, RandomGenerator &rnd, const MatrixDimension &dim, intptr_t size) {
    dst.clear();
    dst.setDimension(dim);
    const size_t capacity = dim.getElementCount();
    if(size == -1) {
      for(size_t i = capacity / 2; i--;) {
        dst.set(randSizet(dim.rowCount, rnd), randSizet(dim.columnCount, rnd),true);
      }
    } else if((size_t)size >= capacity) {
      dst.invert();
    } else if((size_t)size > capacity / 2) {
      dst.invert();
      for(size_t t = capacity - size; t;) {
        const MatrixIndex i = getRandomIndex(dim, rnd);
        if(dst.get(i)) {
          dst.set(i, false);
          t--;
        }
      }
    } else {
      for(size_t t = size; t;) {
        const MatrixIndex i = getRandomIndex(dim, rnd);
        if(!dst.get(i)) {
          dst.set(i, true);
          t--;
        }
      }
    }
    return dst;
  }

  BitMatrix genRandomMatrix(RandomGenerator &rnd, const MatrixDimension &dim, intptr_t size = -1) {
    BitMatrix result(dim);
    return genRandomMatrix(result, rnd, dim, size);
  }

  class BitSetIteratorTimeTester : public MeasurableFunction {
  private:
    BitSet     m_a;
    JavaRandom m_rnd;
  public:
    BitSetIteratorTimeTester();
    void f() {
      for(auto it = m_a.getIterator(); it.hasNext(); it.next());
    }
  };

  BitSetIteratorTimeTester::BitSetIteratorTimeTester() : m_a(2000) {
    m_rnd.randomize();
    m_a = genRandomSet(m_rnd, 10000, 5000);
  }

#if defined(__MEASURETIMES__)
  class BitSetSizeTimeTester : public MeasurableFunction {
  private:
    const BitSet &m_a;
    size_t        m_size;
  public:
    BitSetSizeTimeTester(const BitSet &a) : m_a(a) {
    }
    ~BitSetSizeTimeTester() {
      OUTPUT(_T("Bitset.size():%zd"), m_size);
    }
    void f() {
      m_size = m_a.size();
    }
  };

  class BitSetOldSizeTimeTester : public MeasurableFunction {
  private:
    const BitSet &m_a;
    size_t        m_size;
  public:
    BitSetOldSizeTimeTester(const BitSet &a) : m_a(a) {}
    ~BitSetOldSizeTimeTester() {
      OUTPUT(_T("Bitset.oldSize():%zd"), m_size);
    }
    void f() {
      m_size = m_a.oldSize();
    }
  };

  class BitSetGetIndexTimeTester : public MeasurableFunction {
  private:
    const BitSet     &m_a;
    CompactSizetArray m_values;
    size_t m_result[100];
  public:
    BitSetGetIndexTimeTester(const BitSet &a) : m_a(a) {
      for(auto it = m_a.getReverseIterator(); it.hasNext();) {
         m_values.add(it.next());
         if(m_values.size() >= ARRAYSIZE(m_result)) break;
      }
    }
    void f() {
      for(size_t i = 0; i < ARRAYSIZE(m_result); i++) {
        m_result[i] = m_a.getIndex(m_values[i]);
      }
    }
    const size_t *getResult() const {
      return m_result;
    }
  };

  class BitSetOldGetIndexTimeTester : public MeasurableFunction {
  private:
    const BitSet     &m_a;
    CompactSizetArray m_values;
    size_t m_result[100];
  public:
    BitSetOldGetIndexTimeTester(const BitSet &a) : m_a(a) {
      for(auto it = m_a.getReverseIterator(); it.hasNext();) {
         m_values.add(it.next());
         if(m_values.size() >= ARRAYSIZE(m_result)) break;
      }
    }
    void f() {
      for(size_t i = 0; i < ARRAYSIZE(m_result); i++) {
        m_result[i] = m_a.oldGetIndex(m_values[i]);
      }
    }
    const size_t *getResult() const {
      return m_result;
    }
  };
#endif

  class TesterSet {
  private:
    BitSet               m_bs;
    CompactUIntHashSet<> m_hs;
    bool                 m_memberCheckEnabled;

    void verifyMembers() const {
      for(auto it = m_hs.getIterator(); it.hasNext();) {
        const UINT k = it.next();
        verify(m_bs.contains(k));
      }
    }
  public:
    TesterSet(UINT capacity)
      : m_bs(capacity)
      , m_memberCheckEnabled(true)
    {
    }
    inline void setMemberCheckEnabled(bool enable) {
      m_memberCheckEnabled = enable;
    }
    void verifyConsistent() const {
      verify(m_bs.size() == m_hs.size());
      if(m_memberCheckEnabled) {
        verifyMembers();
      }
    }
    inline TesterSet &clear() {
      m_bs.clear();
      m_hs.clear();
      verifyConsistent();
      return *this;
    }

    inline TesterSet &add(UINT v) {
      m_bs.add(v);
      m_hs.add(v);
      verifyConsistent();
      return *this;
    }
    inline TesterSet &add(UINT a, UINT b) {
      m_bs.add(a, b);
      for(UINT i = a; i <= b; i++) m_hs.add(i);
      verifyConsistent();
      return *this;
    }
    inline TesterSet &remove(UINT a, UINT b) {
      m_bs.remove(a, b);
      for(UINT i = a; i <= b; i++) m_hs.remove(i);
      verifyConsistent();
      return *this;
    }
    inline TesterSet &remove(UINT v) {
      m_bs.remove(v);
      m_hs.remove(v);
      verifyConsistent();
      return *this;
    }
    inline TesterSet &operator+=(const TesterSet &s) {
      m_bs += s.m_bs;
      m_hs.addAll(s.m_hs);
      verifyConsistent();
      return *this;
    }
    TesterSet &operator-=(const TesterSet &s) {
      m_bs -= s.m_bs;
      m_hs.removeAll(s.m_hs);
      verifyConsistent();
      return *this;
    }
    TesterSet &operator&=(const TesterSet &s) {
      m_bs &= s.m_bs;
      m_hs.retainAll(s.m_hs);
      verifyConsistent();
      return *this;
    }
    TesterSet &operator^=(const TesterSet &s) {
      m_bs ^= s.m_bs;
      m_hs = m_hs ^ s.m_hs;
      verifyConsistent();
      return *this;
    }
    TesterSet &invert() {
      m_bs.invert();
      const size_t k = m_bs.getCapacity();
      const CompactUIntHashSet<> tmp(m_hs);
      for(UINT i = 0; i < k; i++) m_hs.add(i);
      m_hs.removeAll(tmp);
      verifyConsistent();
      return *this;
    }
    void checkCompare(const TesterSet &s) const {
      verify((m_bs == s.m_bs) == (m_hs == s.m_hs));
      verify((m_bs != s.m_bs) == (m_hs != s.m_hs));
      verify((m_bs <= s.m_bs) == (m_hs <= s.m_hs));
      verify((m_bs <  s.m_bs) == (m_hs <  s.m_hs));
      verify((m_bs >= s.m_bs) == (m_hs >= s.m_hs));
      verify((m_bs >  s.m_bs) == (m_hs >  s.m_hs));
    }
  };

	TEST_CLASS(TestBitSet) {
    public:

    TEST_METHOD(BitSetTestPrimitiveOperations) {
      BitSet a(33);
      verify(a.size() == 0);
      verify(a.isEmpty());
      a.invert();

      verify(!a.isEmpty());
      verify(a.size() == 33);
      a.remove(31);
      verify(a.size() == 32);
      a.add(31);
      verify(a.size() == 33);
      a.remove(10, 15);
      verify(a.size() == 27);
      a.remove(28, 32);
      verify(a.size() == 22);
      a.add(28, 31);
      verify(a.size() == 26);
      a.add(10, 32);
      verify(a.size() == 33);
      BitSet c(a);
      verify(c == a);
      BitSet d(1);
      d = a;
      verify(d == a);
      BitSet b(10);
      b.add(1, 5);
      a -= b;
      verify(a.size() == 28);
      a.remove(0);
      b.invert();
      a ^= b;
      verify(a.size() == 24);
      a ^= b;
      b += a;
      verify(b.size() == 28);
      a.clear();
      a.add(1);
      a.add(10);
      a.add(31);
      a.add(32);

      BitSet e(200);
      e.invert();
      e |= b;
      e *= a;
      verify(e.size() == 4);
      Iterator<size_t> it = a.getIterator();
      verify(it.next() == 1);
      verify(it.next() == 10);
      verify(it.next() == 31);
      verify(it.next() == 32);
      verify(!it.hasNext());
      verify(a.contains(1));
      verify(!a.contains(2));
      verify(a.contains(31));
      verify(a.contains(32));
      verify(!a.contains(33));

      verify(a.toString() == _T("(1,10,31,32)"));
    }

    void addEvenNumbers(TesterSet &s, int max) {
      s.setMemberCheckEnabled(false);
      max &= ~1;
      for(int i = 0; i < max; i += 2) {
        s.add(i);
      }
      s.setMemberCheckEnabled(true);
      s.verifyConsistent();
    }
    void addOddNumbers(TesterSet &s, int max) {
      s.setMemberCheckEnabled(false);
      for(int i = 1; i < max; i += 2) {
        s.add(i);
      }
      s.setMemberCheckEnabled(true);
      s.verifyConsistent();
    }


    TEST_METHOD(BitSetTestCompare) {
      for(int aCap = 5; aCap < 60; aCap++) {
        for(int bCap = 5; bCap < 60; bCap++) {
          TesterSet a(aCap), b(bCap);
          addEvenNumbers(a, aCap); addEvenNumbers(b, bCap);
          a.checkCompare(b);
          a.add(1);
          a.checkCompare(b);
          a.remove(1);
          a.checkCompare(b);
          a.clear(); b.clear();
          a.checkCompare(b);

          addOddNumbers(a, aCap); addOddNumbers(b, bCap);
          a.checkCompare(b);
          a.add(2);
          a.checkCompare(b);
          a.remove(2);
          a.checkCompare(b);
          a.clear(); b.clear();
          a.checkCompare(b);

          addOddNumbers(a, aCap); addOddNumbers(b, bCap);
          a ^= b;
          a += b;
          a.invert();
          a += b;
          a -= b;
          a += b;
          a ^= b;
        }
      }
    }

    TEST_METHOD(BitSetTestIterator)
    {
#define ANTALITERATIONER 20
      JavaRandom rnd;
      rnd.randomize();
      int s;
      try {
        for(size_t cap = 20; cap < 80; cap++) {
          for(s = 0; s < ANTALITERATIONER; s++) {
            BitSet a(genRandomSet(rnd, cap));
            BitSet copy(a);
            copy.clear();
            unsigned int count = 0;
            for(auto it = a.getIterator(); it.hasNext();) {
              if(!copy.isEmpty()) {
                verify(copy.contains(copy.select()));
              } else {
                try {
                  size_t x = copy.select();
                  Assert::Fail(_T("BitSet.select should throw exception when called on empty set"));
                } catch (Exception e) {
                  //ok
                }
              }
              copy.add(it.next());
              count++;
            }
            verify(count == copy.size());
            verify(copy == a);
            copy.clear();
            count = 0;
            for(auto it = a.getReverseIterator(); it.hasNext();) {
              copy.add(it.next());
              count++;
            }
            verify(count == copy.size());
            verify(copy == a);
            for(size_t start = 0; start < cap+2; start++) {
              for(size_t end = 0; end < cap; end++) {
                BitSet am(a), tmp(a.getCapacity());
                am.remove(start, end);
                for(auto it = a.getIterator(start, end); it.hasNext();) {
                  tmp.add(it.next());
                }
                verify(a - am == tmp);

                tmp.clear();
                for(auto it = a.getReverseIterator(end, start); it.hasNext();) {
                  tmp.add(it.next());
                }
                verify(a - am == tmp);
              }
            }
          }
        }
      } catch (Exception e) {
        Assert::Fail(format(_T("Exception:%s. s=%d"), e.what(), s).cstr());
      }
    }

    TEST_METHOD(BitSetMeasureIteratorTime) {
      randomize();
      BitSetIteratorTimeTester bstt;

      double msec = measureTime(bstt);

      INFO(_T("BitSetIterator time:%.3le msec"), msec * 1000);
    }
#if defined(__NEVER__)
    TEST_METHOD(BitSetTestSize) {
      for(int test = 0; test < 40; test++) {
        const BitSet s(genRandomSet(test + 300));
        const size_t oldSize = s.oldSize();
        const size_t size    = s.size();
        verify(oldSize == size);
      }
    }
#endif

#if defined(__MEASURETIMES__)
    TEST_METHOD(BitSetMeasureSize) {
      randomize();
      for(int i = 0; i < 10; i++) {
        const size_t capacity = 10000000*(i+1);
        const BitSet m_a = genRandomSet(capacity, 3*capacity/4);
        BitSetSizeTimeTester bstt1(m_a);
        double msec = measureTime(bstt1);
        INFO(_T("BitSetSize time(cap=%zd):%.3le msec"), capacity, msec * 1000);
        BitSetOldSizeTimeTester bstt2(m_a);
        msec = measureTime(bstt2);
        INFO(_T("BitSet.oldSize time(cap=%zd):%.3le msec"), capacity, msec * 1000);
      }
    }
    TEST_METHOD(BitSetMeasureGetIndex) {
      randomize();
      for(int i = 0; i < 10; i++) {
        const size_t capacity = 10000000*(i+1);
        const BitSet m_a = genRandomSet(capacity, 3*capacity/4);
        BitSetGetIndexTimeTester bstt1(m_a);
        double msec = measureTime(bstt1);
        INFO(_T("BitSet.getIndex() time(cap=%zd %.3le msec"), capacity, msec * 1000);
        BitSetOldGetIndexTimeTester bstt2(m_a);
        msec = measureTime(bstt2);
        INFO(_T("BitSet.oldGetIndex() time(cap=%zd %.3le msec"), capacity, msec * 1000);
      }
    }
#endif
    static intptr_t getIntIndex(const BitSet &s, size_t e) {
      if(!s.contains(e)) {
        return -1;
      }
      size_t count = 0;
      for(auto it = s.getIterator(); it.hasNext(); count++) {
        if(it.next() == e) {
          return count;
        }
      }
      return -1;
    }

    TEST_METHOD(BitSetGetIndex) {
      JavaRandom rnd;
      rnd.randomize();
      for(int i = 0; i < 40; i++) {
        BitSet s(genRandomSet(rnd, 200));
        INFO(_T("set:%s"), s.toString().cstr());
        for(auto it = s.getIterator(); it.hasNext();) {
          const size_t e = it.next();
          const intptr_t index1 = getIntIndex(s, e);
          const intptr_t index2 = s.getIndex(e);
          INFO(_T("e:%3d. index1:%3d, index2:%3d"), e, index1, index2);
          verify(index1 == index2);
        }
      }
    }

    static size_t getCount1(const BitSet &s, size_t from, size_t to) {
      size_t count = 0;
      for(size_t i = from; i <= to; i++) {
        if(s.contains(i)) {
          count++;
        }
      }
      return count;
    }

    static size_t getCount2(const BitSet &s, size_t from, size_t to) {
      size_t count = 0;
      for(auto it = s.getIterator(from, to); it.hasNext(); it.next()) {
        count++;
      }
      return count;
    }

    TEST_METHOD(BitSetGetCount) {
      JavaRandom rnd;
      rnd.randomize();
      for(int test = 0; test < 30; test++) {
        const BitSet s(genRandomSet(rnd, 200));
        INFO(_T("set:%s"), s.toString().cstr());
        for(size_t from = 0; from < 300; from++) {
          for(size_t to = from; to < 300; to++) {
            const size_t expected1 = getCount1(s, from, to);
            const size_t expected2 = getCount2(s, from, to);
            const size_t count     = s.getCount(from, to);
            /*
            if(count != expected) {
              OUTPUT(_T("(from,to):(%s,%s). expected:%s, got:%s")
                    , format1000(from).cstr()
                    , format1000(to).cstr()
                    , format1000(expected).cstr()
                    , format1000(count).cstr());
              continue;
            }
            */
            verify(count == expected1);
            verify(count == expected2);
          }
        }
      }
    }

    TEST_METHOD(TestFileBitSet) {
      JavaRandom rnd;
      rnd.randomize();
      const size_t capacity = 20000;
      const BitSet s(genRandomSet(rnd, capacity));
      const String fileName = getTestFileName(__TFUNCTION__);
      s.save(ByteOutputFile(fileName));
      FileBitSet fs(fileName, 0);
      for(size_t i = 0; i < capacity; i++) {
        verify(s.contains(i) == fs.contains(i));
      }
    }

    void testAllBitSetIndices(size_t capacity, RandomGenerator &rnd) {
      BitSet s(10);
      genRandomSet(s, rnd, capacity, capacity/3);

      double startTime = getThreadTime();

      BitSetIndex bi(s);
      double usedTime = (getThreadTime() - startTime) / 1000000;

      INFO(_T("Capacity:%s size:%11s creation-time:%7.3lf"), format1000(capacity).cstr(), format1000(s.size()).cstr(), usedTime);

      startTime = getThreadTime();
      intptr_t expectedIndex = 0;
      for(auto it = s.getIterator(); it.hasNext(); expectedIndex++) {
        const size_t e     = it.next();
        intptr_t     index = bi.getIndex(e);
//        if(expectedIndex % 50000 == 0) {
//          OUTPUT(_T("e:%s. index %s\r"), format1000(e).cstr(), format1000(index).cstr());
//        }
        verify(index == expectedIndex);
/*
        if(index != expectedIndex) {
          OUTPUT(_T("e:%s. Expected:%s, got %s")
                ,format1000(e).cstr()
                ,format1000(expectedIndex).cstr()
                ,format1000(index).cstr());
          pause();
          intptr_t index = bi.getIndex(e);
        }
*/
      }
      usedTime = (getThreadTime() - startTime) / 1000000;
      INFO(_T("Iteration-time:%7.3lf"), usedTime);
    }

    TEST_METHOD(TestBitSetIndex) {
      JavaRandom rnd;
      rnd.randomize();
      testAllBitSetIndices(6600000, rnd);
    }

    void testAllFileBitSetIndices(size_t capacity, RandomGenerator &rnd) {
      BitSet s(10);
      genRandomSet(s, rnd, capacity, capacity / 3);

      double startTime = getThreadTime();
      const String fileName = getTestFileName(__TFUNCTION__);

      const size_t ssize = s.size();

      BitSetIndex bi(s);
      verify(bi.getCapacity() == capacity);
      verify(bi.size()        == ssize   );

      bi.save(ByteOutputFile(fileName));

      FileBitSetIndex loaded(fileName, 0);
      verify(loaded.getCapacity() == capacity);
      verify(loaded.size()        == ssize   );

      double usedTime = (getThreadTime() - startTime) / 1000000;

      INFO(_T("Capacity:%s size:%11s creation-time:%7.3lf"), format1000(capacity).cstr(), format1000(s.size()).cstr(), usedTime);

      startTime = getThreadTime();
      intptr_t expectedIndex = 0;
      for(auto it = s.getIterator(); it.hasNext(); expectedIndex++) {
        const size_t e = it.next();
        //    if(expectedIndex % 1000 != 0) {
        //      continue;
        //    }
        const intptr_t index = loaded.getIndex(e);
//        if(expectedIndex % 50000 == 0) {
//          OUTPUT(_T("e:%s. index %s\r"), format1000(e).cstr(), format1000(index).cstr());
//        }
        verify(index == expectedIndex);
/*
        if(index != expectedIndex) {
          OUTPUT(_T("e:%s. Expected:%s, got %s")
                , format1000(e).cstr()
                , format1000(expectedIndex).cstr()
                , format1000(index).cstr());
          intptr_t index = loaded.getIndex(e);
        }
*/
      }
      usedTime = (getThreadTime() - startTime) / 1000000;
      INFO(_T("Iteration-time:%7.3lf"), usedTime);
    }

    TEST_METHOD(FileBitSetIndexTest) {
      JavaRandom rnd;
      rnd.randomize();

      {
        for(double capacity = 10; capacity < 650000; capacity *= 1.4) {
          INFO(_T("testAllFileBitSetIndices(%.0lf"), capacity);
          testAllFileBitSetIndices((UINT)capacity, rnd);
        }
        return;
      }
      /*
      for(;;) {
      const UINT capacity = inputValue<UINT>(_T("Enter capacity:"));
      testAllBitSetIndices(capacity, rnd);
      }
      */
      testAllFileBitSetIndices(6600000, rnd);
    }

    TEST_METHOD(BitSetIndexTimes) {
      JavaRandom rnd;
      rnd.randomize();
      const size_t capacity = 3000000;
      BitSet s(10);
      genRandomSet(s, rnd, capacity, capacity / 2);
      BitSetIndex bi(s);
      INFO(_T("  Testing time for BitSetIndex.getIndex()"));
      double startTime = getThreadTime();
      for(size_t e = 0; e < capacity;) {
        bi.getIndex(e++);
#if defined(_DEBUG)
        if((e & 0x3ffff) == 0) {
          OUTPUT(_T("%.2lf%% Time/call:%.4lf msec"), PERCENT(e,capacity), (getThreadTime() - startTime) / e);
        }
#endif
      }
      double biTime = getThreadTime() - startTime;
      INFO(_T("BitSetIndexTime:%.3lf sec"), biTime / 1000000);

      INFO(_T("  Testing time for BitSet.getIndex()"));
      startTime = getThreadTime();
      for(size_t e = 0; e < capacity; e++) {
        const intptr_t index = s.getIndex(e);
#if defined(_DEBUG)
        if((e & 0x3ffff) == 0) {
          OUTPUT(_T("%.2lf%% Time/call:%.4lf msec"), PERCENT(e,capacity), (getThreadTime() - startTime) / e);
        }
#endif
      }
      double bitSetTime = getThreadTime() - startTime;
      INFO(_T("BitSetTime     :%.3lf sec"), bitSetTime / 1000000);
    }

    static void sendReceive(Packer &dst, const Packer &src) {
      ByteArray a;
      src.write(ByteMemoryOutputStream(a));
      dst.read( ByteMemoryInputStream(a));
    }

    TEST_METHOD(BitSetPacker) {
      JavaRandom rnd;
      rnd.randomize();
      const size_t capacity = 3000000;
      BitSet set1(10);
      genRandomSet(set1, rnd, capacity, capacity / 2);
      Packer s,d;
      d << set1;
      sendReceive(d,s);
      BitSet set2(10);
      d >> set2;
      verify(set1 == set2);
    }

    TEST_METHOD(BitSetMatrix) {
      JavaRandom rnd;
      rnd.randomize();

      MatrixDimension dim;
      for(dim.rowCount = 1; dim.rowCount < 35; dim.rowCount++) {
        for(dim.columnCount = 1; dim.columnCount < 35; dim.columnCount++) {
          const BitMatrix m = genRandomMatrix(rnd, dim, -1);

          verify(m.getDimension() == dim);

          BitMatrix copy(m);
          verify(copy == m);

          copy.clear();
          verify(copy.isEmpty());

          copy = m;
          verify(copy == m);

          MatrixDimension dim1(dim.rowCount+1, dim.columnCount+1);
          copy.setDimension(dim1);
          verify(copy.getDimension() == dim1);
          verify(copy.size() == m.size());
          for(auto it = m.getIterator(); it.hasNext();) {
            const MatrixIndex i = it.next();
            verify(copy.get(i));
          }

          size_t totalCount = 0;
          for(size_t r = 0; r < copy.getRowCount(); r++) {
            BitSet rb = copy.getRow(r);
            int rowCount = 0;
            for(Iterator<MatrixIndex> it = copy.getRowIterator(r); it.hasNext();) {
              const MatrixIndex mi = it.next();
              rowCount++;
              verify(copy.get(mi));
              verify(rb.contains(mi.c));
            }
            verify(rowCount == rb.size());
            totalCount += rowCount;
          }
          verify(totalCount == copy.size());

          totalCount = 0;
          for(size_t c = 0; c < copy.getColumnCount(); c++) {
            BitSet cb = copy.getColumn(c);
            int colCount = 0;
            for(Iterator<MatrixIndex> it = copy.getColumnIterator(c); it.hasNext();) {
              const MatrixIndex mi = it.next();
              colCount++;
              verify(copy.get(mi));
              verify(cb.contains(mi.r));
            }
            verify(colCount == cb.size());
            totalCount += colCount;
          }
          verify(totalCount == copy.size());
        }
      }
    }
  };
}
