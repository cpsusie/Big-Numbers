#include "stdafx.h"
#include "CppUnitTest.h"
#include <Random.h>
#include <TinyBitSet.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

extern void myVerify(bool b, const TCHAR *s);

#ifdef verify
#undef verify
#endif
//#define verify(expr) Assert::IsTrue(expr, _T(#expr))
#define verify(expr) myVerify(expr, _T(#expr))

namespace TestTinyBitSet {
  void OUTPUT(const TCHAR *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    const String msg = vformat(format, argptr);
    va_end(argptr);
    Logger::WriteMessage(msg.cstr());
  }

  template<class BS> BS &genRandomSet(BS &dst, int size) {
    const UINT capacity = dst.getCapacity();
    dst.clear();
    if (size == -1) {
      for (size_t i = capacity / 2; i--;) {
        dst.add(randInt(capacity));
      }
    } else if ((size_t)size >= capacity) {
      dst.invert();
    } else if ((size_t)size > capacity / 2) {
      dst.invert();
      for (int t = capacity - size; t > 0;) {
        const int e = randInt(capacity);
        if (dst.contains(e)) {
          dst.remove(e);
          t--;
        }
      }
    } else {
      for (int t = size; t > 0;) {
        const int e = randInt(capacity);
        if (!dst.contains(e)) {
          dst.add(e);
          t--;
        }
      }
    }
    return dst;
  }

  template<class BS> BS genRandomSet(int size = -1) {
    BS result;
    return genRandomSet(result, size);
  }

	TEST_CLASS(TestTinyBitSet) {
    public:

    template<class BS> void testPrimitiveOperations() {
      BS a;
      const int capacity = a.getCapacity();
      verify(a.size() == 0);
      verify(a.isEmpty());
      a.invert();

      verify(!a.isEmpty());
      verify(a.size() == capacity);
      a.remove(capacity-1);
      verify(a.size() == capacity-1);
      a.add(capacity-1);
      verify(a.size() == capacity);
      a.remove(1, 4);
      verify(a.size() == capacity-4);
      a.remove(capacity-2,capacity-1);
      verify(a.size() == capacity-6);
      a.add(capacity-2,capacity-1);
      verify(a.size() == capacity-4);
      a.add(1,4);
      verify(a.size() == capacity);
      BS c(a);
      verify(c == a);
      BS d;
      d = a;
      verify(d == a);
      BS b;
      b.add(1, 5);
      a -= b;
      verify(a.size() == capacity-b.size());
      a.remove(0);
      b.invert();
      a ^= b;
      verify(a.size() == 1);
      a ^= b;
      c = b - a;
      verify(c.size() == 1);
      a.clear();
      a.add(1);
      a.add(3);
      a.add(capacity-2);
      a.add(capacity-1);
      verify(a.size() == 4);

      BS e;
      e.invert();
      e *= a;
      verify(e.size() == 4);
      Iterator<UINT> it = a.getIterator();
      verify(it.next() == 1);
      verify(it.next() == 3);
      verify(it.next() == capacity-2);
      verify(it.next() == capacity-1);
      verify(!it.hasNext());
      verify(a.contains(1));
      verify(!a.contains(2));
      verify(a.contains(capacity-2));
      verify(a.contains(capacity-1));
      verify(!a.contains(capacity));

      const String str = a.toString();
      const String expected = format(_T("(1,3,%d,%d)"), capacity-2, capacity-1);
      verify(str == expected);
    }

    TEST_METHOD(TestTinyBitSetPrimitiveOperations) {
      testPrimitiveOperations<BitSet8 >();
      testPrimitiveOperations<BitSet16>();
      testPrimitiveOperations<BitSet32>();
      testPrimitiveOperations<BitSet64>();
    }

    template<class BS> void testTinyBitSetIterator() {
#define ANTALITERATIONER 20
      int s;
      try {
        for (s = 0; s < ANTALITERATIONER; s++) {
          BS a(genRandomSet<BS>());
          BS copy(a);
          UINT cap = a.getCapacity();
          copy.clear();
          UINT count = 0;
          for (Iterator<UINT> it = a.getIterator(); it.hasNext();) {
            if (!copy.isEmpty()) {
              verify(copy.contains(copy.select()));
            } else {
              try {
                UINT x = copy.select();
                Assert::Fail(_T("TinyBitSet.select should throw exception when called on empty set"));
              } catch (Exception e) {
                //ok
              }
            }
            copy.add(it.next());
            count++;
          } // for(Iterator...
          verify(count == copy.size());
          verify(copy == a);
          copy.clear();
          count = 0;
          for (Iterator<UINT> it = a.getReverseIterator(); it.hasNext();) {
            copy.add(it.next());
            count++;
          }
          verify(count == copy.size());
          verify(copy == a);
          for (UINT start = 0; start < cap+2; start++) {
            for (UINT end = 0; end < cap; end++) {
              BS am(a), tmp;
              am.remove(start, end);
              for(Iterator<UINT> it = a.getIterator(start, end); it.hasNext();) {
                tmp.add(it.next());
              }
              verify(a - am == tmp);

              tmp.clear();
              for(Iterator<UINT> it = a.getReverseIterator(end, start); it.hasNext();) {
                tmp.add(it.next());
              }
              verify(a - am == tmp);
            }
          } // for(UINT start
        } // for(s = 0
      } catch(Exception e) {
        Assert::Fail(format(_T("Exception:%s. s=%d"), e.what(), s).cstr());
      }
    } // testTinyBitSetIterator

    TEST_METHOD(TestTinyBitSetIterator) {
      testTinyBitSetIterator<BitSet8>();
      testTinyBitSetIterator<BitSet16>();
      testTinyBitSetIterator<BitSet32>();
      testTinyBitSetIterator<BitSet64>();
    }

  };
}
