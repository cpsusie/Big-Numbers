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
      throwInvalidArgumentException(__TFUNCTION__, _T("Pattern %s will accept empty string"), pattern.toString().cstr());
    }
    nfa.clear();
    statePool.releaseAll();
  } catch(...) {
    nfa.clear();
    statePool.releaseAll();
    throw;
  }
}

// --------------------------------------- match --------------------------------------

using namespace std;

template<class IStreamType, class CharType> class DFARegexT {
private:
  static CharType nextChar(IStreamType &in) {
    CharType ch;
    if(in.get(ch).eof()) {
      in.clear(ios::eofbit);
      ch = 0;
    }
    return ch;
  }

public:
  int match(const DFATables &tables, bool ignoreCase, IStreamType &in, String *matchedString) const {
    const int oldFlags = in.flags();
    in.unsetf(ios::skipws);

    String  tmpStr, &stringBuffer = matchedString ? *matchedString : tmpStr;
    size_t  lastAcceptCount = stringBuffer.length();
    int     currentState = 0, lastAcceptState = -1;
    for(CharType ch = nextChar(in); ch; ch = nextChar(in)) {
      stringBuffer += ch;
      int nextState;
      const CharType lookahead = TRANSLATE(ch, ignoreCase);
      if(lookahead && ((nextState = tables.nextState(currentState, lookahead))!=-1)) {
        if(tables.isAcceptState(nextState)) { // Is this an accept state
          lastAcceptState = nextState;
          lastAcceptCount = stringBuffer.length();
        }
        currentState = nextState;
      } else {
        break; // found it
      }
    }
    while(stringBuffer.length() > lastAcceptCount) {
      in.unget();
      stringBuffer.removeLast();
    }
    in.flags(oldFlags);
    return (lastAcceptState<0) ? -1 : tables.getAcceptValue(lastAcceptState);
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
  return format(_T("IgnoreCase:%s\n:%s"), boolToStr(m_ignoreCase), m_tables.toString().cstr());
}
