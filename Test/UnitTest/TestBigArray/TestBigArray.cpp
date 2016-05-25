#include "stdafx.h"
#include "CppUnitTest.h"
#include <BigArray.h>
#include <Console.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#ifdef verify
#undef verify
#endif
#define verify(expr) Assert::IsTrue(expr, _T(#expr))

namespace TestBigArray {		

  void OUTPUT(const TCHAR *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    const String msg = vformat(format, argptr);
    va_end(argptr);
    Logger::WriteMessage(msg.cstr());
  }

  class BigElement {
  public:
    int   m_value;
    TCHAR m_str[400];
    BigElement();
    BigElement(int value);
    String toString() const;
    bool operator==(const BigElement &e) const;
    bool operator!=(const BigElement &e) const {
      return !(*this == e);
    }
  };

  BigElement::BigElement() {
    memset(this, 0, sizeof(BigElement));
  }

  BigElement::BigElement(int value) {
    m_value = value;
    const String s = format(_T("Element initieret med %d"), value);
    MEMSET(m_str, _T('0') + (value % 10), ARRAYSIZE(m_str));
    _tcscpy(m_str + ARRAYSIZE(m_str) - s.length() - 1, s.cstr());
  }

  bool BigElement::operator==(const BigElement &e) const {
    return memcmp(this, &e, sizeof(BigElement)) == 0;
  }

  String BigElement::toString() const {
    return format(_T("%8d, %-25s"), m_value, m_str);
  }

  class TestArray {
    BigArray<BigElement> m_bigArray;
    CompactIntArray      m_refArray;
  public:
    void remove(unsigned int index, unsigned int count = 1);
    void add(int value);
    void set(unsigned int index, int value);
    void clear();
    void check(const TCHAR *function) const;
    int size() const {
      return m_bigArray.size();
    }

    unsigned __int64 getPageFileSize() const {
      return m_bigArray.getPageFileSize();
    }

    void list() const;
#ifdef _DEBUG
    void show() const;
#endif
  };

#ifdef _DEBUG
  void TestArray::show() const {
    printf("\n");
    //  list();
    ((BigArray<BigElement>&)m_bigArray).show();
    //  pause();
  }
#endif

  void TestArray::add(int value) {
    m_bigArray.add(BigElement(value));
    m_refArray.add(value);
#ifdef _DEBUG
    check(_T("add"));
    show();
#endif
  }

  void TestArray::set(unsigned int index, int value) {
    m_bigArray[index] = BigElement(value);
    m_refArray[index] = value;
#ifdef _DEBUG
    check(_T("set"));
    show();
#endif
  }

  void TestArray::remove(unsigned int index, unsigned int count) {
    m_bigArray.remove(index, count);
    m_refArray.remove(index, count);
#ifdef _DEBUG
    check(_T("remove"));
    show();
#endif
  }

  void TestArray::clear() {
    m_bigArray.clear();
    m_refArray.clear();
#ifdef _DEBUG
    check(_T("clear"));
    show();
#endif
  }

  void TestArray::check(const TCHAR *function) const {
#ifdef _DEBUG
    Console::setCursorPos(0, 20);
    Console::clearLine(20);
#endif
    if (m_bigArray.size() != m_refArray.size()) {
      throwException(_T("%s:BigArray.size=%d != refArray.size=%d"), function, m_bigArray.size(), m_refArray.size());
    }
    for (unsigned int i = 0; i < m_bigArray.size(); i++) {
      BigElement e(m_refArray[i]);
      if (m_bigArray[i] != e) {
        const String msg = format(_T("Fejl i %s. size=%d\nBigArray[%d]=<%s>,\nRefArray[%d]=<%s>")
                                 , function
                                 , size()
                                 , i, m_bigArray[i].toString().cstr()
                                 , i, e.toString().cstr()
                                 );
        OUTPUT(_T("%s"), msg.cstr());
#ifdef _DEBUG
        //      show();
        pause();
#endif
        throwException(msg);
      }
    }
    OUTPUT(_T("check(%s) arraySize %11s ok. pageFileSize:%11s"), function, format1000(size()).cstr(), format1000(getPageFileSize()).cstr());
  }

  void TestArray::list() const {
    int line = 30;
    for (unsigned int i = 0; i < m_bigArray.size(); i++) {
      Console::printf(110, line++, _T("[%2d]:%s   %d     "), i, m_bigArray[i].toString().cstr(), m_refArray[i]);
    }
    COORD ws = Console::getWindowSize();
    Console::clearRect(110, line, ws.X, ws.Y - 1);
  }

	TEST_CLASS(TestBigArray) {
    public:

      TEST_METHOD(testBigArray) {
        try {
          TestArray ba;

#ifdef _DEBUG
#define COUNT 20
#else
#define COUNT 108000
#endif

          for (int test = 1; test < 5; test++) {
            OUTPUT(_T("Making array. Test %d..."), test);
            for (int i = 0; i < COUNT; i++) {
              if (i % 10000 == 0) {
                OUTPUT(_T("add(%7d). pageFileSize:%11s"), i, format1000(ba.getPageFileSize()).cstr());
              }
              if (((i % 2000) == 0) && (ba.size() < 30000)) {
                ba.check(_T("add"));
              }
              ba.add(test + i);
            }
            ba.check(_T("after add-loop"));
            for (int i = 0; i < COUNT; i++) {
              if (i % 10000 == 0) {
                OUTPUT(_T("set(%7d). pageFileSize:%11s"), i, format1000(ba.getPageFileSize()).cstr());
              }
              const unsigned int index = randInt(ba.size());
              const int          value = randInt(1000000);
              ba.set(index, value);
            }
            ba.check(_T("after set-loop"));

            for (int i = 0; ba.size() > 0; i++) {
              const int index = randInt(ba.size());
              const int count = randInt(ba.size() - index) / 10 + 1;

              if (i % 4 == 0) {
                OUTPUT(_T("ba.size=%7d. remove(%7d,%6d). pageFileSize:%11s"), ba.size(), index, count, format1000(ba.getPageFileSize()).cstr());
              }
              if ((i % 20 == 0) && (ba.size() < 30000)) {
                ba.check(_T("remove"));
              }
              ba.remove(index, count);
            }
            ba.check(_T("after remove-loop"));
          }
          ba.clear();
        } catch (Exception e) {
          OUTPUT(_T("Exception:%s"), e.what());
          verify(false);
        }
      }
  };
}