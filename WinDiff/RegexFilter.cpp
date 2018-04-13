#include "stdafx.h"
#include "SearchMachine.h"
#include "RegexFilter.h"

RegexFilter::RegexFilter() {
  clear();
}

void RegexFilter::clear() {
  m_matchCase      = false;
  m_matchWholeWord = false;
  m_regex          = EMPTYSTRING;
}

bool RegexFilter::operator==(const RegexFilter &ref) const {
  return m_matchCase      == ref.m_matchCase
      && m_matchWholeWord == ref.m_matchWholeWord
      && m_regex          == ref.m_regex;
}

void RegexFilter::compile() {
  String tmp = SearchMachine::regexConvertNothing(m_regex.cstr());

  if(m_matchWholeWord) {
    tmp = _T("\\<") + tmp + _T("\\>");
  }

//  _tprintf(_T("tmp:<%s>\n"),tmp.cstr());
  m_reg.compilePattern(tmp, m_matchCase ? NULL : String::upperCaseTranslate);
  intptr_t index = m_reg.search(EMPTYSTRING);
  if(index == 0 && m_reg.getResultLength() == 0) {
    throwException(_T("regular expression <%s> matches every String"),m_regex.cstr());
  }
}

String RegexFilter::filter(const String &s) const {
  String result;
  intptr_t pos = 0;
  for(const TCHAR *cp = s.cstr(); *cp; ) {
    const intptr_t index = m_reg.search(cp,true,pos);
    if(index < 0 || m_reg.getResultLength() == 0) {
      break;
    }
    if(index > pos) {
      result += substr(s,pos,index-pos);
    }
    pos = index + m_reg.getResultLength();
  }
  if((intptr_t)s.length() > pos) {
    result += substr(s,pos,s.length());
  }
  return result;
}

static const TCHAR *CASESENSITIVE  = _T("CaseSensitive");
static const TCHAR *WHOLEWORDSONLY = _T("WholeWordsOnly");
static const TCHAR *REGEX          = _T("RegExp");

void RegexFilter::saveData(RegistryKey &key, const RegexFilter &rf) { // static
  key.setValue(CASESENSITIVE  ,rf.m_matchCase     );
  key.setValue(WHOLEWORDSONLY ,rf.m_matchWholeWord);
  key.setValue(REGEX          ,rf.m_regex         );
}

void RegexFilter::loadData(RegistryKey &key, RegexFilter &rf) { // static
  rf.m_matchCase      = key.getBool(  CASESENSITIVE  , false );
  rf.m_matchWholeWord = key.getBool(  WHOLEWORDSONLY , false );
  rf.m_regex          = key.getString(REGEX          , EMPTYSTRING);
  if(!rf.isEmpty()) {
    rf.compile();
  }
}
