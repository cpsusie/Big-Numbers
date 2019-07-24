#include "pch.h"
#include "PatternParser.h"
#include "DFA.h"
#include "DFARegex.h"

DFARegex::DFARegex(const StringArray &pattern, bool ignoreCase)
: m_ignoreCase(ignoreCase)
{
  compilePattern(pattern);
}

void DFARegex::compilePattern(const StringArray &pattern) {
  NFAStatePool statePool;
  NFA          nfa(statePool);
  try {
    PatternParser parser(pattern, nfa, m_ignoreCase);
    DFA           dfa(nfa);
    dfa.getDFATables(m_tables);
    if((!m_tables.isEmpty()) && m_tables.isAcceptState(0)) {
      throwInvalidArgumentException(__TFUNCTION__, _T("Pattern %s will accept empty string"), pattern);
    }
    nfa.clear();
    statePool.releaseAll();
  } catch(...) {
    nfa.clear();
    statePool.releaseAll();
    throw;
  }
}

static const TCHAR *noRegExpressionMsg  = _T("No regular expression specified");

// --------------------------------------- match --------------------------------------

using namespace std;

template<class IStreamType, class CharType> class DFARegexT {
private:
  _TUCHAR nextChar(IStreamType &in) const {
    if(in.eof()) {
      return 0;
    }
    CharType ch;
    in >> ch;
    return (_TUCHAR)ch;
  }

public:
  int match(const DFATables &tables, bool ignoreCase, IStreamType &in, String *matchedString) const {
    const int oldFlags = in.flags();
    in.unsetf(ios::skipws);

    String  stringBuffer;
    int     currentState = 0, lastAcceptState = -1; // Most recently seen accept state
    _TUCHAR ch, lookahead;                          // Lookahead character
    while(ch = nextChar(in)) {
      stringBuffer += ch;
      int nextState;
      for(;;) {
        if(lookahead = TRANSLATE(ch, ignoreCase)) {
          nextState = tables.nextState(currentState, lookahead);
          break;
        } else if(lastAcceptState >= 0) {
          nextState = -1;
          break;
        } else {
          goto NoMatch;
        }
      }
      if(nextState != -1) {
        if(tables.isAcceptState(nextState)) { // Is this an accept state
          lastAcceptState = nextState;
        }
        currentState = nextState;
      } else if(lastAcceptState < 0) {
        goto NoMatch;
      } else {
        break; // found it
      }
    }
    if(ch) {
      in.putback((CharType)stringBuffer.last());
      stringBuffer.removeLast();
    }
    if(matchedString != NULL) {
      *matchedString = stringBuffer;
    }
    in.flags(oldFlags);
    return tables.getAcceptValue(lastAcceptState);

  NoMatch:
    while(!stringBuffer.isEmpty()) {
      in.putback((CharType)stringBuffer.last());
      stringBuffer.removeLast();
    }
    in.flags(oldFlags);
    return -1;
  }
};

// matchedString might be NULL
int DFARegex::match(istream &in, String *matchedString) const {
  return DFARegexT<istream, char>().match(m_tables, m_ignoreCase, in, matchedString);
}

// matchedString might be NULL
int DFARegex::match(wistream &in, String *matchedString) const {
  return DFARegexT<wistream, wchar_t>().match(m_tables, m_ignoreCase, in, matchedString);
}

String DFARegex::toString() const {
  return format(_T("IgnoreCase:%s\n:%s")
    , boolToStr(m_ignoreCase)
    , m_tables.toString().cstr()
  );
}
