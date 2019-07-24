#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestString {		

#include <UnitTestTraits.h>

	TEST_CLASS(TestString) {
    public:

    TEST_METHOD(testString) {
      String s1(" abcABCÊ¯Â∆ÿ≈ ");
      String s2;

      s2 = s1;
      verify(s1 == s2);

      s2 = s1 + s2;
      verify(s2 == _T(" abcABCÊ¯Â∆ÿ≈  abcABCÊ¯Â∆ÿ≈ "));

      s2 = s1;
      s2 += s1;
      verify(s2 == _T(" abcABCÊ¯Â∆ÿ≈  abcABCÊ¯Â∆ÿ≈ "));

      s2 = s1;
      s2 += s2;
      verify(s2 == _T(" abcABCÊ¯Â∆ÿ≈  abcABCÊ¯Â∆ÿ≈ "));

      s2 = s1;
      s2 += s2.cstr() + 1;
      verify(s2 == _T(" abcABCÊ¯Â∆ÿ≈ abcABCÊ¯Â∆ÿ≈ "));

      String s3(s1);
      s3 += s3.cstr() + 1;
      verify(s3 == _T(" abcABCÊ¯Â∆ÿ≈ abcABCÊ¯Â∆ÿ≈ "));

      s2 = s1;
      s2.trimRight();
      verify(s2 == _T(" abcABCÊ¯Â∆ÿ≈"));

      s2 = s1;
      s2.trimLeft();
      verify(s2 == _T("abcABCÊ¯Â∆ÿ≈ "));

      s2 = _T("    ");
      s2.trimLeft();
      verify(s2 == EMPTYSTRING);

      s2 = _T("    ");
      s2.trimRight();
      verify(s2 == EMPTYSTRING);
      s2.trimLeft();
      verify(s2 == EMPTYSTRING);
      s2.trimRight();
      verify(s2 == EMPTYSTRING);

      s2 = s1;
      s2.trim();
      verify(s2 == _T("abcABCÊ¯Â∆ÿ≈"));

      s2 = _T("∆ÿ≈");
      s2.trim();
      verify(s2 == _T("∆ÿ≈"));
      s2 = _T("Ê¯Â");
      s2.trim();
      verify(s2 == _T("Ê¯Â"));

      s2 = s1;
      s2.trim();

      verify(s2[0] == _T('a'));
      verify(s2[1] == _T('b'));
      verify(s2.length() == 12);
      verify(substr(s2, 1, 2) == _T("bc"));
      verify(substr(s2, -1, 10) == EMPTYSTRING);
      verify(substr(s2, 1, 11) == _T("bcABCÊ¯Â∆ÿ≈"));
      verify(substr(s2, 1, 1000) == _T("bcABCÊ¯Â∆ÿ≈"));

      s2 = _T("abc");
      s2 = rev(s2);
      verify(s2 == _T("cba"));

      s2 = toLowerCase(s1);
      verify(s2 == _T(" abcabcÊ¯Â∆ÿ≈ "));

      s2 = toUpperCase(s2);
      verify(s2 == _T(" ABCABCÊ¯Â∆ÿ≈ "));

      s2 = s1;
      s2.remove(10, 10);
      verify(s2 == _T(" abcABCÊ¯Â"));

      s2 = s1;
      verify(s2.last() == _T(' '));
      s2.removeLast();
      verify(s2 == _T(" abcABCÊ¯Â∆ÿ≈"));
      verify(s2.last() == _T('≈'));

      verify(!s2.isEmpty());
      s2 = EMPTYSTRING;
      verify(s2.isEmpty());
      s2.removeLast();
      verify(s2.isEmpty());
      verify(s2.last() == 0);

      s2 = s1;
      s2.remove(10);
      verify(s2 == _T(" abcABCÊ¯Âÿ≈ "));

      verify(s2.find(_T("AB")) == 4);
      verify(s2.find(_T("AB"), 4) == 4);
      verify(s2.find(_T("AB"), 5) == -1);

      verify(s2.find(_T(' '), 13) == -1);

      s1 = _T("ABC");
      verify(s2.find(s1) == 4);

      s1 = _T("≈ ");
      verify(s2.find(s1, 12) == -1);

      verify(s2.find(_T(" "), 14) == -1);

      verify(s2.find(_T('A')) == 4);
      verify(s2.find(_T('A'), 4) == 4);
      verify(s2.find(_T('A'), 5) == -1);

      verify(s2.rfind(_T('A')) == 4);

      String insertedChars;
      String s2Start = s2;
      for (int i = 0; i < 100; i++) {
        const TCHAR ch = _T('X');
        s2.insert(0, ch);
        insertedChars += ch;

        verify(s2 == insertedChars + s2Start);
      }

      insertedChars = "";
      s2 = _T("fisk");
      s2Start = s2;
      for (int i = 0; i < 10; i++) {
        const String ins = format(_T("%d"), i);
        const size_t pos = insertedChars.length();
        s2.insert(pos, ins);
        insertedChars += ins;
        verify(s2 == insertedChars + s2Start);
      }

      s2 = _T("a--b--c");
      s2.replace(_T("--"), _T('x'));
      verify(s2 == _T("axbxc"));

      s2 = _T("a--b--c-");
      String fromStr = EMPTYSTRING;
      s2.replace(fromStr, _T('x'));
      verify(s2 == _T("a--b--c-"));

      fromStr = _T("-");
      s2.replace(fromStr, _T('x'));
      verify(s2 == _T("axxbxxcx"));


      s2 = _T("a b c");
      s2.replace(_T(' '), _T("++"));
      verify(s2 == _T("a++b++c"));
      s2.replace(_T('+'), _T("-+"));
      verify(s2 == _T("a-+-+b-+-+c"));

      s2.replace(_T('+'), _T("-"));
      verify(s2 == _T("a----b----c"));

      s2.replace(_T('x'), _T("MM"));
      verify(s2 == _T("a----b----c"));

      s2.replace(_T('-'), EMPTYSTRING);
      verify(s2 == _T("abc"));

      verify(left(s2, 2) == _T("ab"));
      verify(left(s2, 3) == _T("abc"));
      verify(left(s2, 4) == _T("abc"));
      verify(left(s2, 0) == EMPTYSTRING);
      verify(left(s2, -1) == EMPTYSTRING);

      verify(right(s2, -1) == EMPTYSTRING);
      verify(right(s2, 0) == EMPTYSTRING);
      verify(right(s2, 2) == _T("bc"));
      verify(right(s2, 3) == _T("abc"));
      verify(right(s2, 4) == _T("abc"));

      s2 = _T("a--b--c");
      s2.replace(_T("--"), EMPTYSTRING);
      verify(s2 == _T("abc"));

      s2 = _T("fisk1fisk2fisk3");
      s2.replace(_T("fisk"), _T("fusk"));
      verify(s2 == _T("fusk1fusk2fusk3"));

      s2.replace(EMPTYSTRING, _T("bbb"));
      verify(s2 == _T("fusk1fusk2fusk3"));

      s2.replace(_T("f"), _T("bbb"));
      verify(s2 == _T("bbbusk1bbbusk2bbbusk3"));

      s2.replace(_T("bb"), _T("b"));
      verify(s2 == _T("bbusk1bbusk2bbusk3"));

      s2.replace(_T("xx"), _T("yy"));
      verify(s2 == _T("bbusk1bbusk2bbusk3"));

      //  printf(_T("s1:<%s> length:%d\n"),s1.cstr(),s1.length());
      //  printf(_T("s2:<%s> length:%d\n"),s2.cstr(),s2.length());
      //  pause();

      String res = format(_T("%-*s"), 100000, _T("jesper"));
      verify(res.length() == 100000);
      res = s2;
      verify(res == s2);
      TCHAR *nullStr = NULL;

      res = nullStr;
      verify(res == _T("null"));

      s2 = "";
      res += s2;
      verify(res == _T("null"));

      TCHAR *emptyString = EMPTYSTRING;
      res += emptyString;
      verify(res == _T("null"));

      res += nullStr;
      verify(res == _T("nullnull"));

      res = spaceString(100000, _T(' '));
      verify(res.length() == 100000);

      res = spaceString(20, _T('x'));
      res = spaceString(10, _T('a'));
      res += _T("bbbb");
      verify(res == _T("aaaaaaaaaabbbb"));

      res += _T('c');
      verify(res == _T("aaaaaaaaaabbbbc"));

      res = spaceString(-4, _T('r'));
      verify(res.length() == 0);

      res = _T("  \tfisk\t \t\n");
      res = trim(res);
      verify(res == _T("fisk"));

      res = _T(" \n \tfisk\t \t\n");
      res = trimLeft(res);
      verify(res == _T("fisk\t \t\n"));

      res = _T(" \n \tfisk\t \t\n");
      res = trimRight(res);
      verify(res == _T(" \n \tfisk"));

      size_t reslen = res.length();
      for (size_t i = 0; i <= reslen + 4; i++) {
        try {
          TCHAR ch = res[i];
          verify(i < reslen);
        } catch (Exception e) {
          verify(i >= reslen);
        }
      }

      const String constRes = res;
      reslen = constRes.length();;
      for (size_t i = 0; i <= reslen + 4; i++) {
        try {
          TCHAR ch = constRes[i];
          verify(i < reslen);
        } catch (Exception e) {
          verify(i >= reslen);
        }
      }

      s1 = _T("abcEFISK");
      s2 = _T("abcefisk");
      verify(s1.equalsIgnoreCase(s2));

      verify(s1 != s2);

      verify(s1 != s2.cstr());

      verify(s2 > s1);
      verify(s2 >= s1);
      verify(s2 >= s2);

      verify(s1 < s2);
      verify(s1 <= s2);
      verify(s1 <= s1);

      s1.printf(_T("%s %d"), _T("fisk"), 1);
      verify(s1 == _T("fisk 1"));

      const ULONG hash1 = s1.hashCode();
      s2 = s1 + _T("fusk");
      const ULONG hash2 = s2.hashCode();

      verify(hash1 != hash2);

    }

    TEST_METHOD(testStringCompare) {
      String s1(_T("JOHNSTON"));
      String s2(_T("johnston"));
      String s3(_T("JOHN_HENRY"));
      String s4(_T("john_henry"));
      String s5(_T("JOHNSTON "));

      verify(s1.compare(s2) < 0);
      verify(s1 < s2);
      verify(s1.compareIgnoreCase(s2) == 0);

      verify(s1.compare(s3) < 0);
      verify(s1 < s3);
      verify(s1.compareIgnoreCase(s3) > 0);

      verify(s3.compare(s4) < 0);
      verify(s3 < s4);
      verify(s3.compareIgnoreCase(s4) == 0);

      verify(s1.compare(s5) < 0);
      verify(s1 < s5);

      verify(s2.compare(s5) > 0);
      verify(s2 > s5);
      verify(s2.compareIgnoreCase(s5) < 0);
    }
  };
}
