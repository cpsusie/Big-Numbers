#include "stdafx.h"
#include <MyUtil.h>
#include <Random.h>
#include <TinyBitSet.h>
#include "TestBitSet.h"

template<class T> class TinyBitSetTester {
private:
  int m_capacity;

public:
  TinyBitSetTester() {
    TinyBitSet<T> tmp;
    m_capacity = tmp.getCapacity();
  }

  TinyBitSet<T> genRandomSet(int size) const {
    TinyBitSet<T> result;
    if(size == -1) {
      for(int i = m_capacity/2; i--;) {
        result.add(randInt(m_capacity));
      }
    } else {
      if(size >= m_capacity) {
        result.invert();
      } else if(size > m_capacity/2) {
        result.invert();
        for(int t = m_capacity-size; t;) {
          const int e = randInt(m_capacity);
          if(result.contains(e)) {
            result.remove(e);
            t--;
          }
        }
      } else {
        for(int t = size; t;) {
          const int e = randInt(m_capacity);
          if(!result.contains(e)) {
            result.add(e);
            t--;
          }
        }
      }
    }
    return result;
  }

  void testIterator() {
    _tprintf(_T("  Testing iterator..."));
#define ANTALITERATIONER 1000
    int s;
    try {
      for(s = 0; s < ANTALITERATIONER; s++) {
        TinyBitSet<T> a(genRandomSet(m_capacity*2/3));
        TinyBitSet<T> copy(a);
        copy.clear();
        int count = 0;
        for(Iterator<unsigned int> it = a.getIterator(); it.hasNext();) {
          if(!copy.isEmpty()) {
            verify(copy.contains(copy.select()));
          } else {
            try {
              int x = copy.select();
              verify(false);
            } catch(Exception e) {
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
        for(Iterator<unsigned int> it = a.getReverseIterator(); it.hasNext();) {
          copy.add(it.next());
          count++;
        }
        verify(count == copy.size());
        verify(copy == a);
      }
    } catch(Exception e) {
      _tprintf(_T("Exception:%s. s=%d\n"),e.what(), s);
      verify(false);
    }
    _tprintf(_T("Iterator ok!\n"));
  }

  void testTinyBitSet() {
    const String objectToTest = format(_T("TinyBitSet(capacity=%d)"), m_capacity);

    _tprintf(_T("Testing %s%s\n"), objectToTest.cstr(),spaceString(15-objectToTest.length(),_T('.')).cstr());

    TinyBitSet<T> a;
    verify(a.size() == 0);
    verify(a.isEmpty());
    a.invert();

    verify(!a.isEmpty());
    verify(a.size() == m_capacity);
    a.remove(m_capacity-1);
    verify(a.size() == m_capacity-1);
    a.add(m_capacity-1);
    verify(a.size() == m_capacity);

    a.remove(2, m_capacity-3); // a == {0,1,c-2,c-1] => size = 4
    verify(a.contains(1));
    verify(!a.contains(2));
    verify(!a.contains(m_capacity-3));
    verify(a.contains(m_capacity-2));
    verify(a.size() == 4);

    a.add(3, m_capacity-4); // a == {0,1,3,..,c-4,c-2,c-1} => size == c-3

    verify(!a.contains(2));
    verify(a.contains(3));
    verify(a.contains(m_capacity-4));
    verify(!a.contains(m_capacity-3));
    verify(a.size() == m_capacity-2);

    TinyBitSet<T> c(a);
    verify(c == a);
    TinyBitSet<T> d;
    d = a;
    verify(d == a);
    TinyBitSet<T> b;
    b.add(3, 4);
    verify(b.size() == 2);
    a -= b;
    verify(a.size() == m_capacity-4);
    a.remove(0);
    verify(a.size() == m_capacity-5);
    b.invert();
    a ^= b;
    verify(a.size() == 3);
    a.invert();
    verify(a.size() == m_capacity-3);
    b.clear();
    b.invert();
    b.remove(1);
    a &= b;
    verify(a.size() == m_capacity-4);

    a.clear();
    a.add( 1);
    a.add( 4);
    a.add( 5);
//    String stra = a.toString();

    b.clear();
    b.add(0,1);
    b.add(m_capacity-1);
//    String strb = b.toString();

    TinyBitSet<T> e;
    e = a & b;
//    String stre = e.toString();
    verify(e.size() == 1);

    e = a | b;
    verify(e.size() == 5);

    e = a - b;
    verify(e.size() == 2);

    e = a ^ b;
    verify(e.size() == 4);

    Iterator<unsigned int> it = b.getIterator();
    verify(it.next() == 0);
    verify(it.next() == 1);
    verify(it.next() == (unsigned int)m_capacity-1);
    verify(!it.hasNext());
  
    testIterator();
  
    verify(a.toString() == _T("(1,4,5)"));
    verify(b.toString() == format(_T("(0,1,%d)"), m_capacity-1));

    _tprintf(_T("%s ok!\n"),objectToTest.cstr());
  }
};

void testTinyBitSet() {
  TinyBitSetTester<unsigned char> tester8;
  tester8.testTinyBitSet();
  TinyBitSetTester<unsigned short> tester16;
  tester16.testTinyBitSet();
  TinyBitSetTester<unsigned long> tester32;
  tester32.testTinyBitSet();
  TinyBitSetTester<unsigned __int64> tester64;
  tester64.testTinyBitSet();
}

