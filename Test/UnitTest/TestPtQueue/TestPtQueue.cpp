#include "stdafx.h"
#include "CppUnitTest.h"
#include <PriorityQueue.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#ifdef verify
#undef verify
#endif
#define verify(expr) Assert::IsTrue(expr, _T(#expr))

namespace TestPtQueue {		

  void OUTPUT(const TCHAR *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    const String msg = vformat(format, argptr);
    va_end(argptr);
    Logger::WriteMessage(msg.cstr());
  }

  class TestElement {
  private:
    double m_x;
  public:
    TestElement(double x = 0);
    TestElement(const TestElement &s);
    TestElement &operator=(const TestElement &rhs);
    double getX() const {
      return m_x;
    }
    ~TestElement() {
    }
  };

  TestElement::TestElement(double x) {
    m_x = x;
  }

  TestElement::TestElement(const TestElement &s) {
    m_x = s.m_x;
  }

  TestElement &TestElement::operator=(const TestElement &rhs) {
    m_x = rhs.m_x;
    return *this;
  }

  bool operator<(const TestElement &lts, const TestElement &rhs) {
    return lts.getX()  < rhs.getX();
  }

  bool operator<=(const TestElement &lts, const TestElement &rhs) {
    return lts.getX() <= rhs.getX();
  }

  int testElementCmp(const TestElement **e1, const TestElement **e2) {
    return sign((*e1)->getX() - (*e2)->getX());
  }

  int testElementCmp(const TestElement &e1, const TestElement &e2) {
    return sign(e1.getX() - e2.getX());
  }

  int stringCmp(const String &s1, const String &s2) {
    return _tcscmp(s1.cstr(), s2.cstr());
  }

  int testElementCmpReverse(const TestElement **e1, const TestElement **e2) {
    return -testElementCmp(e1, e2);
  }

  class MyComparator : public Comparator<TestElement> {
    int compare(const TestElement &e1, const TestElement &e2);
    AbstractComparator *clone() const { return new MyComparator(); }
  };

  int MyComparator::compare(const TestElement &e1, const TestElement &e2) {
    return sign(e1.getX() - e2.getX());
  }


	TEST_CLASS(TestPtQueue)	{
    public:

    TEST_METHOD(PtQueuePrimitiveOperations) {
      int i;
      TestElement x;
      PriorityQueue<TestElement> ptq1(testElementCmp, true);
      PriorityQueue<TestElement> ptq2(testElementCmp, false);
      MyComparator comparator;
      PriorityQueue<TestElement> ptq3(comparator, true);
      Array<TestElement>         testArray;
      PriorityQueue<String>      sptq(stringCmp);

#define ANTAL 1000

      //  for(i = 0; i < ANTAL; i++)
      //    sptq.add(String("fisk:")+String(ANTAL-i));
      //  while(!sptq.isEmpty())
      //    printf("%s\n",sptq.remove().cstr());

      for (i = 0; i < ANTAL; i++) {
        TestElement x((double)rand() / 1000.0);
        testArray.add(x);
        ptq1.add(x);
        ptq2.add(x);
        ptq3.add(x);
      }

      testArray.sort(testElementCmpReverse);

      for (i = 0; i < ANTAL; i++) {
        x = ptq1.remove();
        verify(x.getX() == testArray[i].getX());
        x = ptq2.remove();
        verify(x.getX() == testArray[ANTAL - i - 1].getX());
        x = ptq3.remove();
        verify(x.getX() == testArray[i].getX());
      }
    }

  };
}