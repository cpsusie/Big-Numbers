#include "stdafx.h"
#include <Regex.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestRegex {		

#include <UnitTestTraits.h>

  class RegexTester {
  private:
    void callInvalidPattern(const String &pattern);
    void callRegex(bool ignoreCase, const String &pattern, const String &target, int expected, bool expectException = false);
  public:
    void doTest();
  };

  void RegexTester::callRegex(bool ignoreCase, const String &pattern, const String &target, int expected, bool expectException) {
    const String savePattern = pattern;
    const String saveTarget = target;

    Regex regex;
    try {
      regex.compilePattern(pattern, ignoreCase ? String::upperCaseTranslate : nullptr);
    }  catch (Exception e) {
      if (!expectException) {
        throwException(_T("error:pattern:<%s> return Exception:<%s>. No Exception expected"), pattern.cstr(), e.what());
      }
      return;
    }
    if (expectException) {
      throwException(_T("error:pattern:<%s> expected Exception. No Exception returned"), pattern.cstr());
    }
    int ret;


#if defined(_DEBUG)
    OUTPUT(_T("Pattern:<%s>%s"), pattern.cstr(), ignoreCase ? _T(" Ignore case") : EMPTYSTRING);
    OUTPUT(_T("%s\nFastMap:%s"), regex.toString().cstr(), regex.fastMapToString().cstr());
#endif

    Regex regex1;
    regex1 = regex;
    RegexRegisters registers;
    if ((ret = (int)regex1.search(target, true, -1, &registers)) != expected) {
      throwException(_T("error:pattern:<%s> target:<%s> return:%d expected:%d"), pattern.cstr(), target.cstr(), ret, expected);
    }
    if (pattern != savePattern) {
      throwException(_T("pattern has changed: <%s> -> <%s>"), savePattern.cstr(), pattern.cstr());
    }
    if (target != saveTarget) {
      throwException(_T("target has changed: <%s> -> <%s>"), saveTarget.cstr(), target.cstr());
    }
#if defined(_DEBUG)
    OUTPUT(_T("--------------------"));
#endif
  }

  void RegexTester::callInvalidPattern(const String &pattern) {
    try {
      Regex reg(pattern);
      verify(false);
    }  catch (Exception e) {
      // ok
    }
  }

  void RegexTester::doTest() {

    /*
    callRegex(false, "jc?r"                   ,"rrjcr"              , 2);
    callRegex(false, "jc+r"                   ,"rrjccr"             , 2);
    callRegex(false, "jc*r"                   ,"rrjccr"             , 2);
    callRegex(false, "jc{1,1}r"               ,"rrjcr"              , 2);
    callRegex(false, "jc{0,1}r"               ,"rrjcr"              , 2);
    callRegex(false, "jc{2,3}r"               ,"rrjccr"             , 2);
    return;
    */
    callRegex(false, "j", "rrj", 2);
    callRegex(false, "j", "rr", -1);
    callRegex(false, "j.", "rrj", -1);
    callRegex(false, "j.", "rrjq", 2);
    callRegex(false, "j.r", "rrjeq", -1);
    callRegex(false, "j.r", "rrjer", 2);
    callRegex(false, "j.r", "rrjeer", -1);
    callRegex(false, "j.*", "rrj", 2);
    callRegex(false, "j.*r", "rrj", -1);
    callRegex(false, "j.*r", "rrjr", 2);
    callRegex(false, "j.*r", "rrjeer", 2);
    callRegex(false, "j.+", "rrj", -1);
    callRegex(false, "j.+", "rrjr", 2);
    callRegex(false, "j.+r", "rrjr", -1);
    callRegex(false, "j.+r", "rrjer", 2);
    callRegex(false, "j.+r", "rrjeer", 2);
    callRegex(false, "j.?r", "rrje", -1);
    callRegex(false, "j.?r", "rrjr", 2);
    callRegex(false, "j.?r", "rrjer", 2);
    callRegex(false, "j.?r", "rrjeer", -1);
    callRegex(false, "jr?", "rrj", 2);
    callRegex(false, "jr?", "rrjr", 2);
    callRegex(false, "jr?", "rrjrr", 2);
    callRegex(false, "jr?e", "rrje", 2);
    callRegex(false, "jr?e", "rrjre", 2);
    callRegex(false, "jr?e", "rrjrre", -1);
    callRegex(false, "j[rt]", "rrje", -1);
    callRegex(false, "j[rt]", "rrjr", 2);
    callRegex(false, "j[rt]*", "rrj", 2);
    callRegex(false, "j[rt]+", "rrjt", 2);
    callRegex(false, "j[rt]+", "rrj", -1);
    callRegex(false, "j[^t]", "rrjt", -1);
    callRegex(false, "j[^t]", "rrjr", 2);
    callRegex(false, "j[r-t]", "rrjs", 2);
    callRegex(false, "j[^r-t]", "rrjs", -1);
    callRegex(false, "j[^t-a]", "rrj", -1);
    callRegex(false, "j[^t-a]", "rrjt", 2);
    callRegex(false, "j\\(rt\\|re\\)r", "rrjrr", -1);
    callRegex(false, "j\\(rt\\|re\\)r", "rrjrtr", 2);
    callRegex(false, "j\\(rt\\|re\\)r", "rrjrer", 2);
    callRegex(false, "j\\(rt\\|re\\)+r", "rrjrerertr", 2);
    callRegex(false, "j\\(rt\\|re\\)+r", "rrjr", -1);
    callRegex(false, "j\\(rt\\|re\\)*r", "rrjr", 2);
    callRegex(false, "j\\(rt\\|re\\)?r", "rrjr", 2);
    callRegex(false, "j\\(rt\\|re\\){2}r", "rrjrtrer", 2);
    callRegex(false, "je$", "qjer", -1);
    callRegex(false, "je$", "qje", 1);
    callRegex(false, "^je", "qje", -1);
    callRegex(false, "^je", "jeq", 0);
    callRegex(false, "^je$", "jeq", -1);
    callRegex(false, "^je$", "je", 0);
    callRegex(false, "j^q", "j", -1); // ^ and $ are normal characters in the middle of the pattern
    callRegex(false, "j^", "j", -1);
    callRegex(false, "j^", "jj^", 1);
    callRegex(false, "$j", "j", -1);
    callRegex(false, "$j", "$jj", 0);
    callRegex(false, "j$q", "j", -1);
    callRegex(false, "j^q", "j^q", 0);
    callRegex(false, "j$q", "j$q", 0);
    callRegex(false, "j\\w", "rrj ", -1);
    callRegex(false, "j\\w", "rrje", 2);
    callRegex(false, "j\\W", "rrj ", 2);
    callRegex(false, "j\\W", "rrje", -1);
    callRegex(false, "\\<je", "rrje", -1);
    callRegex(false, "\\<je", " je", 1);
    callRegex(false, "\\<je\\>", " je", 1);
    callRegex(false, "\\<je\\>", " je ", 1);
    callRegex(false, "\\<je\\>", " jer", -1);
    callRegex(false, "\\bje\\b", " jer", -1);
    callRegex(false, "\\bje\\b", " je r", 1);
    callRegex(false, "\\Bje\\B", " jer", -1);
    callRegex(false, "\\Bje\\B", " je r", -1);
    callRegex(false, "\\Bje\\B", " rjer", 2);
    callRegex(false, "je\\", "jeq", 0, true);
    callRegex(false, "je\\(", "jeq", 0, true);
    callRegex(false, "je\\)", "jeq", 0, true);
    callRegex(false, "je[", "jeq", 0, true);
    callRegex(false, "je]", "je]", 0);
    callRegex(false, "j\\(r\\(ab\\|cd\\)t\\|re\\)r", "rrjrcdtr", 2);
    callRegex(false, "j\\(r\\(ab\\|cd\\)t\\|re\\)r\\2", "rrjrcdtrcd", 2);
    callRegex(false, "j\\(r\\(ab\\|cd\\)t\\|re\\)r\\2", "rrjrcdtr", -1);
    callRegex(true, "J", "rrj", 2);
    callRegex(true, "j", "rr", -1);
    callRegex(true, "J.", "rrj", -1);
    callRegex(true, "J.", "rrjq", 2);
    callRegex(true, "J.R", "rrjeq", -1);
    callRegex(true, "J.R", "rrjer", 2);
    callRegex(true, "J.R", "rrjeer", -1);
    callRegex(true, "J.*", "rrj", 2);
    callRegex(true, "J.*r", "rrj", -1);
    callRegex(true, "J.*R", "rrjr", 2);
    callRegex(true, "J.*R", "rrjeer", 2);
    callRegex(true, "JR?E", "rrjrre", -1);
    callRegex(true, "J[rt]", "rrje", -1);
    callRegex(true, "J[RT]", "rrjr", 2);
    callRegex(true, "J[RT]*", "rrj", 2);
    callRegex(true, "J[RT]+", "rrjt", 2);
    callRegex(true, "J[RT]+", "rrj", -1);
    callRegex(true, "J[^T]", "rrjt", -1);
    callRegex(true, "J[^T]", "rrjr", 2);
    callRegex(true, "J[R-t]", "rrjs", 2);
    callRegex(true, "J[^R-t]", "rrjs", -1);
    callRegex(true, "J[^T-a]", "rrj", -1);
    callRegex(true, "J[^t-A]", "rrjt", 2);
    callRegex(false, "jæ", "jjæ", 1); // be careful with characters above 127
    callRegex(true, "jæ", "jæ", 0);
    callRegex(false, "[jæ]", "jjæ", 0); // upper/lowercase is not correct. ie translate('æ') != 'Æ'
    callRegex(false, "[j-æ]", "æ", 0);
    callRegex(false, "[j-æ]", "a", -1);
    callRegex(false, "[æ-ø]", "æ", 0);
    callRegex(false, "[æ-ø]", "ø", 0);
    callRegex(true, "[æ-ø]", "æ", 0);
    callRegex(true, "[æ-ø]", "ø", 0);
    callRegex(false, "a\\(cb\\|cd\\|bb\\|c\\|d\\){4,5}b", "acdbbddccbacdcbbb", 10);
    callInvalidPattern("$\\");
  }

	TEST_CLASS(TestRegex)	{
    public:

      TEST_METHOD(testRegex) {
        try {
          RegexTester tester;
          tester.doTest();
        } catch (Exception e) {
#if defined(_DEBUG)
          OUTPUT(_T("Exception:%s"), e.what());
#endif
          verify(false);
        }
      }
  };
}
