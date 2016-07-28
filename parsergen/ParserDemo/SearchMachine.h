#pragma once

#include <Regex.h>

class FindParameter {
public:
  bool          m_matchCase;
  bool          m_matchWholeWord;
  bool          m_useRegExp;
  bool          m_dirUp;
  String        m_findWhat;
  bool          m_skipCurrent;
  FindParameter();
  bool isSet() const { return m_findWhat.length() > 0; }
};

class SearchMachine {
private:
  Regex         m_reg;
  FindParameter m_param;
public:
  SearchMachine(const FindParameter &param);
  int findIndex(const String &s, int startPos = -1);
  int resultLen() const;
};

String regexpConvertNothing(   const TCHAR *s);
String regexpConvertAllreg(    const TCHAR *s);
