#include "pch.h"

#if defined(_DEBUG)

#include <CompactStack.h>
#include <HashMap.h>
#include "DFA.h"
#include <MarginFile.h>
#include <DebugLog.h>

void DFA::dumpStates() const {
  const size_t stateCount = m_states.size();
  for(size_t i = 0; i < stateCount; i++) {
    debugLog(_T("%s"), m_states[i].toString().cstr());
  }
}

#define RMARGIN 118

String binToAscii(int c, bool useHex=true) {
  char result[10];
  c &= 0xff;
  if(' ' < c  &&  c < 0x7f  &&  c != '\'' && c != '\\') {
    result[0] = c;
    result[1] = '\0';
  } else {
    result[0] = '\\' ;
    result[2] = '\0' ;

    switch(c) {
    case '\\': result[1] = '\\'; break;
    case '\'': result[1] = '\''; break;
    case '\b': result[1] = 'b' ; break;
    case '\t': result[1] = 't' ; break;
    case '\f': result[1] = 'f' ; break;
    case '\r': result[1] = 'r' ; break;
    case '\n': result[1] = 'n' ; break;
    case ' ' : result[1] = 's' ; break;
    default  : sprintf(&result[1], useHex ? "x%02x" : "%03o", c);
               break;
    }
  }
  return result;
}

void printChar(MarginFile &f, int c) {
  f.printf(_T("%s"), binToAscii(c).cstr());
}

#define FORMATCHAR(ch) binToAscii(ch)

#define _FLUSHRANGE()                                                           \
{ String tmp;                                                                   \
  if(delim) tmp += delim; else delim = _T(",");                                 \
  if(first == last) {                                                           \
    tmp = FORMATCHAR(first);                                                    \
  } else {                                                                      \
    const TCHAR *formatStr = (first + 1 == last) ? _T("%s%s") : _T("%s-%s");    \
    tmp = format(formatStr, FORMATCHAR(first).cstr(), FORMATCHAR(last).cstr()); \
  }                                                                             \
  const UINT tmplen = (UINT)tmp.length();                                       \
  if(charsPrinted + tmplen > RMARGIN) {                                         \
    fprintf(f,_T("\n//              "));                                        \
    charsPrinted = f->getLeftMargin();                                          \
  }                                                                             \
  fprintf(f,_T("%s"), tmp.cstr());                                              \
  charsPrinted += tmplen;                                                       \
}

#define FLUSHRANGE() { if(first <= last) _FLUSHRANGE();   }
#define NEWTRANS()   { first = 1; last = 0; delim = NULL; }

String DFAState::toString() const {
  tostrstream stream;
  MarginFile mf(stream), *f = &mf;

  if(!isAcceptState()) {
    fprintf(f, _T("// DFA State %3d [nonAccepting]"), m_id );
  } else {
    fprintf(f, _T("// DFA State %3d -> index:%d"), m_id, m_acceptIndex);
  }

  int charsPrinted   = f->getLeftMargin();
  int lastTransition = FAILURE;
  unsigned int first,last;
  const TCHAR *delim;

  NEWTRANS();
  for(int ch = 0; ch < MAX_CHARS; ch++) {
    if(m_transition[ch] != FAILURE) {
      if(m_transition[ch] != lastTransition) {
        FLUSHRANGE();
        NEWTRANS();
        fprintf(f,_T("\n//   goto %2d on "), m_transition[ch]);
        charsPrinted = f->getLeftMargin();
      }
      if(first > last) {
        first = last = ch;
      } else if(ch == last+1) {
        last = ch;
      } else {
        FLUSHRANGE();
        first = last = ch;
      }
      lastTransition = m_transition[ch];
    }
  }
  FLUSHRANGE();
  fprintf(f,_T("\n"));
  f->close();
  return stream.str().c_str();
}

#endif
