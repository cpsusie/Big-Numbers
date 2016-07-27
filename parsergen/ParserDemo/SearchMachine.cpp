#include "stdafx.h"
#include "SearchMachine.h"

FindParameter::FindParameter() {
  m_matchCase      = false;
  m_matchWholeWord = false;
  m_useRegExp      = false;
  m_dirUp          = false;
  m_skipCurrent    = false;
}

String regexpConvertAllreg(const char *wc) {
  char tmp[256];
  for(char *dst = tmp; *wc; wc++) {
    switch(*wc) {
    case '\\':
      *(dst++) = '\\';
      *(dst++) = '\\';
      break;
    case '$' :
    case '^' :
    case '.' :
    case '[' :
    case ']' :
    case '+' :
    case '*' :
    case '?' :
      *(dst++) = '\\';
      *(dst++) = *wc;
      break;
    default  :
      *(dst++) = *wc;
      break;
    }
  }
  *dst = '\0';
  return tmp;
}

String regexpConvertNothing(const char *wc) {
  char tmp[256];
  for(char *dst = tmp; *wc;) {
    switch(*wc) {
    case '\\':
      switch(wc[1]) {
      case 'w':
      case 'W':
      case '<':
      case '>':
      case 'b':
      case 'B':
      case '`':
      case '\'':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        *(dst++) = '\\';
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
  *dst = '\0';
  return tmp;
}

SearchMachine::SearchMachine(const FindParameter &param) {
  m_param = param;

  String tmp;
  if(m_param.m_useRegExp) {
    tmp = regexpConvertNothing(param.m_findWhat.cstr());
  } else { // !m_useRegExp
    tmp = regexpConvertAllreg(param.m_findWhat.cstr());
  }

  if(m_param.m_matchWholeWord) {
    tmp = String("\\<") + tmp + "\\>";
  }

//  printf("tmp:<%s>\n", tmp.cstr());
  m_reg.compilePattern(tmp, m_param.m_matchCase ? NULL : String::upperCaseTranslate);
}

int SearchMachine::findIndex(const String &s, int startPos) {
  return m_reg.search(s, !m_param.m_dirUp, startPos);
}

int SearchMachine::resultLen() const {
  return m_reg.getResultLength();
}
