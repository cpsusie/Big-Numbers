#include "stdafx.h"
#include <BigArray.h>
#include <Console.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestBigArray {		

#include <UnitTestTraits.h>

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
    void remove(size_t index, size_t count = 1);
    void add(int value);
    void set(size_t index, int value);
    void clear();
    void check(const TCHAR *function) const;
    size_t size() const {
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

  void TestArray::set(size_t index, int value) {
    m_bigArray[index] = BigElement(value);
    m_refArray[index] = value;
#ifdef _DEBUG
    check(_T("set"));
    show();
#endif
  }

  void TestArray::remove(size_t index, size_t count) {
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
      throwException(_T("%s:BigArray.size=%s != refArray.size=%s")
                    ,function
                    ,format1000(m_bigArray.size()).cstr()
                    ,format1000(m_refArray.size()));
    }
    for (size_t i = 0; i < m_bigArray.size(); i++) {
      BigElement e(m_refArray[i]);
      if (m_bigArray[i] != e) {
        const String msg = format(_T("Fejl i %s. size=%s\nBigArray[%s]=<%s>,\nRefArray[%s]=<%s>")
                                 , function
                                 , format1000(size()).cstr()
                                 , format1000(i).cstr()
                                 , m_bigArray[i].toString().cstr()
                                 , format1000(i).cstr()
                                 , e.toString().cstr()
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
    for (size_t i = 0; i < m_bigArray.size(); i++) {
      Console::printf(110, line++, _T("[%2s]:%s   %d     ")
                     ,format1000(i).cstr()
                     ,m_bigArray[i].toString().cstr()
                     ,m_refArray[i]);
    }
    COORD ws = Console::getWindowSize();
    Console::clearRect(110, line, ws.X, ws.Y - 1);
  }

	TEST_CLASS(TestBigArray) {
    public:

      TEST_METHOD(BigArrayTest) {
        try {
          TestArray ba;

#ifdef _DEBUG
#define COUNT 20
#else
#define COUNT 108000
#endif

          for (int test = 1; test <= 2; test++) {
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
              const size_t index = randSizet(ba.size());
              const int    value = randInt(1000000);
              ba.set(index, value);
            }
            ba.check(_T("after set-loop"));

            for (size_t i = 0; ba.size() > 0; i++) {
              const size_t index = randSizet(ba.size());
              const size_t count = randSizet(ba.size() - index) / 6 + 1;

//            if (i % 4 == 0) {
                OUTPUT(_T("ba.size=%8s. remove(%7s,%7s). pageFileSize:%11s")
                      ,format1000(ba.size()).cstr()
                      ,format1000(index).cstr()
                      ,format1000(count).cstr()
                      ,format1000(ba.getPageFileSize()).cstr());
  //          }
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