#include "pch.h"
#include "PatternParser.h"
#include "DFA.h"
#include "DFARegex.h"

DFARegex::DFARegex(const StringArray &pattern, BYTE flags)
: m_flags(flags)
{
  compilePattern(pattern);
}

#if defined(_DEBUG)
static String getIndexedPatternString(const StringArray &pattern) {
  String result;
  const size_t n = pattern.size();
  for(size_t i = 0; i < n; i++) {
    result += format(_T("[%2zu]:\"%s\"\n"), i, pattern[i].cstr());
  }
  return result;
}
#endif // _DEBUG

void DFARegex::compilePattern(const StringArray &pattern) {
  NFAStatePool statePool;
  NFA          nfa(statePool);
  try {
    PatternParser parser(pattern, nfa, getIgnoreCase());
    const DFA     dfa(nfa);
#if defined(_DEBUG)
    if(getDumpStates()) {
      dfa.dumpStates();
    }
#endif // _DEBUG
    dfa.getDFATables(m_tables);
    if((!m_tables.isEmpty()) && m_tables.isAcceptState(0)) {
      throwInvalidArgumentException(__TFUNCTION__, _T("Pattern %s will accept empty string"), pattern.toString().cstr());
    }
    nfa.clear();
    statePool.releaseAll();

#if defined(_DEBUG)
    if(getDumpStates()) {
      debugLog(_T("%s\nPattern:\n%s\nTables:\n%s\n"), __TFUNCTION__, getIndexedPatternString(pattern).cstr(), m_tables.toString().cstr());
    }
#endif // _DEBUG
  } catch(...) {
    nfa.clear();
    statePool.releaseAll();
    throw;
  }
}

// --------------------------------------- match --------------------------------------

using namespace std;

template<typename IStreamType, typename CharType> class DFARegexT {
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

// matchedString might be nullptr
int DFARegex::match(istream &in, String *matchedString) const {
  return DFARegexT<istream, char>().match(m_tables, getIgnoreCase(), in, matchedString);
}

// matchedString might be nullptr
int DFARegex::match(wistream &in, String *matchedString) const {
  return DFARegexT<wistream, wchar_t>().match(m_tables, getIgnoreCase(), in, matchedString);
}

#if defined(_DEBUG)
String DFARegex::toString() const {
  return format(_T("IgnoreCase:%s\n:%s"), boolToStr(getIgnoreCase()), m_tables.toString().cstr());
}
#endif // _DEBUG
