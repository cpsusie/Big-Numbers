#pragma once

#include <Regex.h>

class FindParameters {
public:
  bool   m_matchCase;
  bool   m_matchWholeWord;
  bool   m_useRegex;
  bool   m_dirUp;
  String m_findWhat;
  bool   m_skipCurrent;
  bool   m_diffOnly;
  bool   m_nonDiffOnly;

  FindParameters();
  bool isEmpty() const {
    return m_findWhat.length() == 0;
  }
};

class SearchMachine {
private:
  Regex          m_reg;
  FindParameters m_param;
public:
  SearchMachine(const FindParameters &param);
  int findIndex(const String &s, int startpos = -1);
  int getResultLength() const;
  static String regexConvertNothing(const TCHAR *s);
  static String regexConvertAllReg(const TCHAR *s);
};

