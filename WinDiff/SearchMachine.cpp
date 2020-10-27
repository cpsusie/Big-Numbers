#include "stdafx.h"
#include "SearchMachine.h"

FindParameters::FindParameters() {
  m_matchCase      = false;
  m_matchWholeWord = false;
  m_useRegex       = false;
  m_dirUp          = false;
  m_skipCurrent    = false;
}

String SearchMachine::regexConvertAllReg(const TCHAR *wc) { // static
  TCHAR tmp[4096], *dst;
  for(dst = tmp; *wc; wc++) {
    switch(*wc) {
    case _T('\\'):
      *(dst++) = _T('\\');
      *(dst++) = _T('\\');
      break;
    case _T('$') :
    case _T('^') :
    case _T('.') :
    case _T('[') :
    case _T(']') :
    case _T('+') :
    case _T('*') :
    case _T('?') :
      *(dst++) = _T('\\');
      *(dst++) = *wc;
      break;
    default  :
      *(dst++) = *wc;
      break;
    }
  }
  *dst = 0;
  return tmp;
}

String SearchMachine::regexConvertNothing(const TCHAR *wc) {  // static
  TCHAR tmp[4096], *dst;
  for(dst = tmp; *wc;) {
    switch(*wc) {
    case _T('\\'):
      switch(wc[1]) {
      case _T('w'):
      case _T('W'):
      case _T('<'):
      case _T('>'):
      case _T('b'):
      case _T('B'):
      case _T('`'):
      case _T('\''):
      case _T('1'):
      case _T('2'):
      case _T('3'):
      case _T('4'):
      case _T('5'):
      case _T('6'):
      case _T('7'):
      case _T('8'):
      case _T('9'):
        *(dst++) = _T('\\');
        *(dst++) = *(wc++);
        *(dst++) = *(wc++);
        break;
      default:
        *(dst++) = *(wc++);
        *(dst++) = *(wc++);
        break;
      }
      break;
    default  :
      *(dst++) = *(wc++);
      break;
    }
  }
  *dst = 0;
  return tmp;
}

SearchMachine::SearchMachine(const FindParameters &param) {
  m_param = param;

  String tmp;
  if(m_param.m_useRegex) {
    tmp = regexConvertNothing(param.m_findWhat.cstr());
  } else { // !m_useRegex
    tmp = regexConvertAllReg(param.m_findWhat.cstr());
  }

  if(m_param.m_matchWholeWord) {
    tmp = String(_T("\\<")) + tmp + _T("\\>");
  }

//  _tprintf(_T("tmp:<%s>\n"),tmp.cstr());
  m_reg.compilePattern(tmp, m_param.m_matchCase ? nullptr : String::upperCaseTranslate);
}

int SearchMachine::findIndex(const String &s, int startPos) {
  return (int)m_reg.search(s.cstr(), !m_param.m_dirUp, startPos);
}

int SearchMachine::getResultLength() const {
  return (int)m_reg.getResultLength();
}
