#include "stdafx.h"
#include <KMPAutomate.h>
#include <BMAutomate.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestStringSearch {

#include <UnitTestTraits.h>

  class SearchTester {
  private:
    StringSearchAlgorithm<TCHAR> *m_strSearch;
    void callStringSearch(bool ignoreCase, const String &pattern, const String &target, int expected) {
      const String savePattern = pattern;
      const String saveTarget = target;
      int          ret;

      m_strSearch->compilePattern(pattern.cstr(), pattern.length(), true, ignoreCase ? String::upperCaseTranslate : nullptr);
  #if defined(_DEBUG)
      OUTPUT(_T("Pattern:<%s>%s"), pattern.cstr(), ignoreCase ? _T(" Ignore case") : EMPTYSTRING);
  #endif
      StringSearchAlgorithm<TCHAR> *a1 = m_strSearch->clone();
      if ((ret = (int)a1->search(target.cstr(), target.length())) != expected) {
        throwException(_T("error:pattern:<%s> target:<%s> return:%d expected:%d"), pattern.cstr(), target.cstr(), ret, expected);
      }

      if (pattern != savePattern) {
        throwException(_T("pattern has changed: <%s> -> <%s>"), savePattern.cstr(), pattern.cstr());
      }
      if (target != saveTarget) {
        throwException(_T("target has changed: <%s> -> <%s>"), saveTarget.cstr(), target.cstr());
      }
      SAFEDELETE(a1);
  #if defined(_DEBUG)
      OUTPUT(_T("--------------------"));
  #endif
    }
  public:
    SearchTester(StringSearchAlgorithm<TCHAR> &strSearch) : m_strSearch(&strSearch) {
    }
    void doTest();
  };

  void SearchTester::doTest() {
    callStringSearch(false, "j", "rrj", 2);
    callStringSearch(false, "j", "rr", -1);
    callStringSearch(false, "j.", "rrj", -1);
    callStringSearch(false, "j^q", "j", -1); // ^ and $ are normal characters in the middle of the pattern
    callStringSearch(false, "j^", "j", -1);
    callStringSearch(false, "j^", "jj^", 1);
    callStringSearch(false, "$j", "j", -1);
    callStringSearch(false, "$j", "$jj", 0);
    callStringSearch(false, "j$q", "j", -1);
    callStringSearch(false, "j^q", "j^q", 0);
    callStringSearch(false, "j$q", "j$q", 0);
    callStringSearch(false, "je]", "je]", 0);
    callStringSearch(true, "J", "rrJ", 2);
    callStringSearch(true, "j", "rr", -1);
    callStringSearch(false, "jæ", "jjæ", 1); // be careful with characters above 127
  }

	TEST_CLASS(StringSearch)	{
    public:

      TEST_METHOD(stringSearchBM) {
        try {
          SearchTester(BMAutomate()).doTest();
        } catch (Exception e) {
#if defined(_DEBUG)
          OUTPUT(_T("Exception:%s"), e.what());
#endif
          verify(false);
        }
      }

      TEST_METHOD(stringSearchKMP) {
        try {
          SearchTester(KMPAutomate()).doTest();
        } catch (Exception e) {
#if defined(_DEBUG)
          OUTPUT(_T("Exception:%s"), e.what());
#endif
          verify(false);
        }
      }
  };
}
