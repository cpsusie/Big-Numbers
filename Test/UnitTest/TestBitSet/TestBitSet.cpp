#include "stdafx.h"
#include "CppUnitTest.h"
#include <Random.h>
#include <BitSet.h>
#include <ByteFile.h>
#include <TimeMeasure.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


#ifdef verify
#undef verify
#endif
#define verify(expr) Assert::IsTrue(expr, _T(#expr))

namespace TestBitSet
{		
  void OUTPUT(const TCHAR *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    const String msg = vformat(format, argptr);
    va_end(argptr);
    Logger::WriteMessage(msg.cstr());
  }

  BitSet &genRandomSet(BitSet &dst, unsigned int capacity, int size) {
    dst.clear();
    dst.setCapacity(capacity);
    if (size == -1) {
      for (unsigned int i = capacity / 2; i--;) {
        dst.add(randInt(capacity));
      }
    }
    else if ((unsigned int)size >= capacity) {
      dst.invert();
    } else if ((unsigned int)size > capacity / 2) {
      dst.invert();
      for (int t = capacity - size; t;) {
        const int e = randInt(capacity);
        if (dst.contains(e)) {
          dst.remove(e);
          t--;
        }
      }
    } else {
      for (int t = size; t;) {
        const int e = randInt(capacity);
        if (!dst.contains(e)) {
          dst.add(e);
          t--;
        }
      }
    }
    return dst;
  }

  BitSet genRandomSet(unsigned int capacity, int size = -1) {
    BitSet result(10);
    return genRandomSet(result, capacity, size);
  }

  class BitSetTimeTester : public MeasurableFunction {
  private:
    BitSet m_a;
  public:
    BitSetTimeTester();
    void f();
  };

  BitSetTimeTester::BitSetTimeTester() : m_a(2000) {
    m_a = genRandomSet(10000, 5000);
  }

  void BitSetTimeTester::f() {
    for (Iterator<unsigned int> it = m_a.getIterator(); it.hasNext(); it.next());
  }

	TEST_CLASS(TestBitSet)
	{
    public:

    TEST_METHOD(BitSetPrimiteveOperations) {
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
      Iterator<unsigned int> it = a.getIterator();
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

    TEST_METHOD(BitSetTestIterator)
    {
#define ANTALITERATIONER 1000
      int s;
      try {
        for (s = 0; s < ANTALITERATIONER; s++) {
          BitSet a(genRandomSet(200));
          BitSet copy(a);
          copy.clear();
          unsigned int count = 0;
          for (Iterator<unsigned int> it = a.getIterator(); it.hasNext();) {
            if (!copy.isEmpty()) {
              verify(copy.contains(copy.select()));
            }
            else {
              try {
                int x = copy.select();
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
          for (Iterator<unsigned int> it = a.getReverseIterator(); it.hasNext();) {
            copy.add(it.next());
            count++;
          }
          verify(count == copy.size());
          verify(copy == a);
        }
      } catch (Exception e) {
        Assert::Fail(format(_T("Exception:%s. s=%d"), e.what(), s).cstr());
      }
    }

    TEST_METHOD(BitSetMeasureIteratorTime) {
      randomize();
      BitSetTimeTester bstt;

      double msec = measureTime(bstt);

      OUTPUT(_T("BitSetIterator time:%.3le msec"), msec * 1000);
    }

    TEST_METHOD(BitSetTestSize) {
      for (int test = 0; test < 40; test++) {
        BitSet s(genRandomSet(test + 300));
        const int oldSize = s.oldsize();
        const int size = s.size();
        verify(oldSize == size);
      }
    }

    static int getIntIndex(const BitSet &s, unsigned int e) {
      if (!s.contains(e)) {
        return -1;
      }
      int count = 0;
      for (Iterator<unsigned int> it = ((BitSet&)s).getIterator(); it.hasNext(); count++) {
        if (it.next() == e) {
          return count;
        }
      }
      return -1;
    }

    TEST_METHOD(BitSetGetIndex) {
      for (int i = 0; i < 40; i++) {
        BitSet s(genRandomSet(200));
        //    OUTPUT(_T("set:%s"), s.toString().cstr());
        for (Iterator<unsigned int> it = s.getIterator(); it.hasNext();) {
          const unsigned int e = it.next();
          const int index1 = getIntIndex(s, e);
          const int index2 = s.getIndex(e);
          //      OUTPUT(_T("e:%3d. index1:%3d, index2:%3d"), e, index1, index2);
          verify(index1 == index2);
        }
      }
    }

    static int getCount(const BitSet &s, unsigned int from, unsigned int to) {
      int count = 0;
      for (unsigned int i = from; i <= to; i++) {
        if (s.contains(i)) {
          count++;
        }
      }
      return count;
    }

    TEST_METHOD(BitSetGetCount) {
      for (int test = 0; test < 30; test++) {
        const BitSet s(genRandomSet(200));
        //    OUTPUT(_T("set:%s"), s.toString().cstr());
        for (unsigned int from = 0; from < 300; from++) {
          for (unsigned int to = from; to < 300; to++) {
            const unsigned int expected = getCount(s, from, to);
            const unsigned int count = s.getCount(from, to);
            /*
            if(count != expected) {
              OUTPUT(_T("(from,to):(%3d,%3d). expected:%3d, got:%3d"), from, to, expected, count);
              continue;
            }
            */
            verify(count == expected);
          }
        }
      }
    }

    void testAllBitSetIndices(unsigned int capacity) {
      BitSet s(10);
      genRandomSet(s, capacity, capacity/3);

      double startTime = getThreadTime();

      BitSetIndex bi(s);
      double usedTime = (getThreadTime() - startTime) / 1000000;

      OUTPUT(_T("Capacity:%s size:%11s creation-time:%7.3lf"), format1000(capacity).cstr(), format1000(s.size()).cstr(), usedTime);

      startTime = getThreadTime();
      int expectedIndex = 0;
      for (Iterator<unsigned int> it = s.getIterator(); it.hasNext(); expectedIndex++) {
        const int e = it.next();
        int index = bi.getIndex(e);
//        if (expectedIndex % 50000 == 0) {
//          OUTPUT(_T("e:%s. index %s\r"), format1000(e).cstr(), format1000(index).cstr());
//        }
        if (index != expectedIndex) {
          OUTPUT(_T("e:%s. Expected:%d, got %d"), format1000(e).cstr(), expectedIndex, index);
          pause();
          int index = bi.getIndex(e);
        }
      }
      usedTime = (getThreadTime() - startTime) / 1000000;
      OUTPUT(_T("Iteration-time:%7.3lf"), usedTime);
    }

    TEST_METHOD(BitSetTestIndex) {
      /*
      {
      for(double capacity = 10; capacity < 650000000; capacity *= 1.4) {
      BitSet s(10);
      genRandomSet(s, (int)capacity, -1);
      BitSetIndex bi(s);
      }
      return;
      }
      */
      /*
      for(;;) {
      const unsigned int capacity = inputInt(_T("Enter capacity:"));
      testAllBitSetIndices(capacity);
      }
      */
      randomize();
      testAllBitSetIndices(6600000);
    }

    void testAllBitSetFileIndices(unsigned int capacity) {
      BitSet s(10);
      genRandomSet(s, capacity, capacity / 3);

      double startTime = getThreadTime();
      const TCHAR *fileName = _T("c:\\temp\\testBitSet\\bitSetFileIndex.dat");

      BitSetIndex bi(s);
      bi.save(ByteOutputFile(fileName));

      BitSetFileIndex loaded(fileName, 0);

      double usedTime = (getThreadTime() - startTime) / 1000000;

      OUTPUT(_T("Capacity:%s size:%11s creation-time:%7.3lf"), format1000(capacity).cstr(), format1000(s.size()).cstr(), usedTime);

      startTime = getThreadTime();
      int expectedIndex = 0;
      for (Iterator<unsigned int> it = s.getIterator(); it.hasNext(); expectedIndex++) {
        const int e = it.next();
        //    if(expectedIndex % 1000 != 0) {
        //      continue;
        //    }
        int index = loaded.getIndex(e);
//        if (expectedIndex % 50000 == 0) {
//          OUTPUT(_T("e:%s. index %s\r"), format1000(e).cstr(), format1000(index).cstr());
//        }
        if (index != expectedIndex) {
          OUTPUT(_T("e:%s. Expected:%d, got %d"), format1000(e).cstr(), expectedIndex, index);
          pause();
          int index = loaded.getIndex(e);
        }
      }
      usedTime = (getThreadTime() - startTime) / 1000000;
      OUTPUT(_T("Iteration-time:%7.3lf"), usedTime);
    }

    TEST_METHOD(BitSetFileIndexTest) {
      /*
      {
      for(double capacity = 10; capacity < 650000000; capacity *= 1.4) {
      BitSet s(10);
      genRandomSet(s, (int)capacity, -1);
      BitSetIndex bi(s);
      }
      return;
      }
      */
      /*
      for(;;) {
      const unsigned int capacity = inputInt(_T("Enter capacity:"));
      testAllBitSetIndices(capacity);
      }
      */
      randomize();
      testAllBitSetFileIndices(6600000);
    }

    TEST_METHOD(BitSetIndexTimes) {
      const int capacity = 3000000;
      randomize();
      BitSet s(10);
      genRandomSet(s, capacity, capacity / 2);
      BitSetIndex bi(s);
      OUTPUT(_T("  Testing time for BitSetIndex.getIndex()"));
      double startTime = getThreadTime();
      for (int e = 0; e < capacity;) {
        bi.getIndex(e++);
//        if ((e & 0x3ffff) == 0) {
//          OUTPUT(_T("%.2lf%% Time/call:%.4lf msec"), 100.0*e / capacity, (getThreadTime() - startTime) / e);
//        }
      }
      double biTime = getThreadTime() - startTime;
      OUTPUT(_T("BitSetIndexTime:%.3lf sec"), biTime / 1000000);

      OUTPUT(_T("  Testing time for BitSet.getIndex()"));
      startTime = getThreadTime();
      for (int e = 0; e < capacity; e++) {
        const int index = s.getIndex(e);
      }
      double bitSetTime = getThreadTime() - startTime;
      OUTPUT(_T("BitSetTime     :%.3lf sec"), bitSetTime / 1000000);
    }

  };
}
