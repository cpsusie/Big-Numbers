#include "pch.h"
#include "NFA.h"
#include "PatternParser.h"

static const TCHAR *unexpectedEndMsg    = _T("Unexpected end of regular expression");
static const TCHAR *unmatchedLPMsg      = _T("Unmatched \\(");
static const TCHAR *unmatchedRPMsg      = _T("Unmatched \\)");
static const TCHAR *missingRBMsg        = _T("Missing ] in character class");

PatternParser::PatternParser(const StringArray &pattern, NFA &nfa, bool ignoreCase)
: m_ignoreCase(ignoreCase)
, m_scanner(pattern, ignoreCase)
, m_NFA(nfa)
, m_statePool(nfa.m_statePool)
{
  m_NFA.create(compilePattern());
}

NFAState *PatternParser::compilePattern() {
  /*  pattern           -> exprList0  (line 0)
                           exprList1  (line 1)
                           exprList2  (line 2)
                             .
                             .
                             .
   */

  SubNFA all(m_statePool);
  for(;;) {
    nextToken();
    SubNFA s = exprList();
    if(!s.isEmpty()) {
      s.setAcceptIndex(m_scanner.getCurrentLineIndex());
      s.getStart()->setToStartState();
    }
    if(!match(_EOL)) {
      expected(_T("end of line"));
    }
    all |= s;
    if(!m_scanner.hasNextLine()) {
      break;
    }
    m_scanner.nextLine();
  }
  return all.getStart();
}

/*
#ifdef _WIN32
#pragma message("NB -------------------- In Windows newline include \\r and \\n -------------------")
#else
#pragma message("NB -------------------- Newline include \\n -------------------")
#endif
*/
// In Windows a newline is \r\n, In Unix \n,
// so charerterclasses involving newline depends on the platform

class _WordLetterSet : public BitSet {
public:
  _WordLetterSet();
};

_WordLetterSet::_WordLetterSet() : BitSet(256) {
  add('a','z');
  add('A','Z');
  add('0','9');
}

static const _WordLetterSet wordLetterSet;

// --------------------------------- Helper functions ---------------------------------------------------

static inline bool isWordLetter(_TUCHAR ch) {
  return (ch < 256) && wordLetterSet.contains(ch);
}

SubNFA PatternParser::exprList() {
  //
  // The same translations that were needed in the expr rules are needed here.
  //
  //  exprList                -> exprList factor
  //                           | factor
  //
  //  is translated to:
  //
  //  exprList                -> factor exprList'
  //
  //  exprList'               -> factor exprList'
  //                           | epsilon
  //
  // which will be parsed by this loop:
  //
  //  factor();
  //  while(token in first1(exprList') ) {
  //    factor();
  //  }

  SubNFA s = factor();
  while(firstExprList(m_token)) {
    s += factor();
  }
  return s;
}

bool PatternParser::firstExprList(PatternToken token) const {
  switch(token) {
  case _EOL    :
  case STAR    : // *
  case PLUS    : // +
  case QUEST   : // ?
  case BAR     :
  case RPAR    :
    return false;
  }
  return true;
}

static bool isClosureToken(PatternToken token) {
  switch(token) {
  case STAR    : // *
  case PLUS    : // +
  case QUEST   : // ?
    return true;
  default         :
    return false;
  }
}

SubNFA PatternParser::factor() {
  //
  // factor             -> term
  //                     | term ?
  //                     | term +
  //                     | term *
  SubNFA s = term();
  if(isClosureToken(m_token)) {
    switch(m_token) {
    case QUEST: s = quest(s); break;
    case PLUS : s = plus( s); break;
    case STAR : s = star( s); break;
    default   : unexpectedInput();
    }
    nextToken();
  }
  return s;
}

SubNFA PatternParser::term() {
  //
  // term               -> LPAR alternativeList RPAR
  //                     | DOT
  //                     | LB optComplement optCharClass RB
  //                     | NORMALCHAR
  //                     | WORDCHAR
  //                     | NONWORDCHAR
  //
  // optCharClass       -> charClass
  //                     | epsilon
  //
  // optCcomplement     -> ^
  //                     | epsilon
  //
  // The [] is nonstandard. It matches a space ' ', tab '\t', formfeed '\f', or newline '\n',
  // but not a carriage return '\r'.

  SubNFA result(m_statePool);
  switch(m_token) {
  case LPAR:            // \( alternativeList \)
    { const intptr_t lpIndex = m_scanner.getIndex();
      nextToken();
      result = alternativeList();
      if(!match(RPAR)) {
        error(lpIndex, unmatchedLPMsg); // doesn't return
      }
      nextToken();
    }
    break;

  case NORMALCHAR:
    result.create2StateNFA(m_scanner.getTheCharacter());
    nextToken();
    break;
  case DOT            : // DOT
  case LB             : // [ characterClass ]
    { CharacterSet charSet;
      if(match(DOT)) { // . (DOT) matches everything except '\n' (for Windows also '\r')
        charSet.add(0, MAX_CHARS-1);
        if(!m_ignoreCase) charSet = m_scanner.translate(charSet);

        charSet.remove(NEWLINE);
#ifdef _WIN32
        charSet.remove(CR);
#endif
      } else { // [...]
        const intptr_t LBCharIndex = m_scanner.getIndex();
        nextToken();
        bool complementCharClass = false;
        if(match(NORMALCHAR) && (m_scanner.getRawCharacter() == _T('^'))) {            // Complement character class
          complementCharClass = true;
          nextToken();
        }
        if(match(RB)) {                     // [] or [^]
          charSet.add(_T(' ')).add(_T('\t')).add(_T('\f')).add(NEWLINE);
        } else {
          charClass(charSet);
        }
        if(complementCharClass) {
          charSet.add(NEWLINE); // This will exclude '\n' if not specified in input
#ifdef _WIN32
          charSet.add(CR);      // and \r
#endif
          charSet.invert();
        }
        if(!match(RB)) {
          error(LBCharIndex, missingRBMsg);
        }
      }
      nextToken();
      result.create2StateNFA(charSet);
    }
    break;
  case WORDCHAR    :            // \w
  case NONWORDCHAR :            // \W
    { CharacterSet charSet;
      charSet += !m_ignoreCase ? m_scanner.translate(wordLetterSet) : wordLetterSet;
      if(m_token == NONWORDCHAR) charSet.invert();

      nextToken();
      result.create2StateNFA(charSet);
    }
    break;
  }
  return result;
}

void PatternParser::charClass(CharacterSet &set) {
  //
  // charClass          -> ccl charClass'
  //
  // charClass'         -> ccl charClass'
  //                     | epsilon
  //
  // ccl                 | character
  //                     | character DASH character
  // character          -> any, except ]-/ which should be specified as /] /- and // respectively. / followed by any char is just '/'
  // characters inside charclass are treated different than outside charclass. characters like . ? + * are just normal characters inside charclass
  while(!match(_EOL) && !match(RB)) {
    switch(m_token) {
    case NORMALCHAR:
      { const _TUCHAR c1 = m_scanner.getTheCharacter();
        nextToken();
        if(!match(DASH)) {
          set.add(c1);
        } else {
          nextToken();
          _TUCHAR to;
          switch(m_token) {
          case DASH       : to = _T('-'); break;
          case RB         : to = _T(']'); break;
          case NORMALCHAR : to = m_scanner.getTheCharacter(); break;
          default         : unexpectedEndOfPattern();
          }
          set.add(c1, to);
          nextToken();
        }
      }
      break;
    case _EOL: unexpectedEndOfPattern();
    default  : unexpectedInput();
    }
  }
}

SubNFA PatternParser::alternativeList() {
  //  alternativeList   -> alternativeList BAR exprList
  //                     | exprList
  //
  // Should be parsed as
  //
  //  alternativeList   -> exprList alternativeList'
  //
  //  alternativeList'  -> BAR exprList alternativeList'
  //                     | epsilon
  //
  // which will be done by this loop:
  //
  //  exprList()
  //  while(match(BAR)) {
  //    nextToken();
  //    exprList();
  //  }

  SubNFA s = exprList();
  while(match(BAR)) {
    nextToken();
    s |= exprList();
  }
  return s;
}

//void PatternParser::escapeCommand(SubNFA &s) {
  //  EscapeCommand      -> \\SpecialCharacter
  //
  //  SpecialChararacter -> `                              - Back quote.   Match only if at start of buffer
  //                      | '                              - Single quote. Match only if at end of buffer
  //                      | <                              - Match only if at begining of af word
  //                      | >                              - Match only if at end of af word
  //                      | b                              - Match only if at word bound
  //                      | B                              - Match only if not at word bound
  //                      | w                              - Match any wordcharacter. {0-9,a-z,A-Z}
  //                      | W                              - Match any non-wordcharacter
  //                      | 1..9                           - Match if string equals the corresponding register. See below.
//}


void PatternParser::unexpectedEndOfPattern() {
  error(unexpectedEndMsg);
}

void PatternParser::unexpectedInput() {
  error(_T("Unexpected input:%s"), m_scanner.getTokenStr().cstr());
}

void PatternParser::expected(const TCHAR *input) {
  error(_T("Expected %s"), input);
}

void PatternParser::error(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr,format);
  verror(m_scanner.getIndex(), format, argptr);
  va_end(argptr);
}

void PatternParser::error(intptr_t index, _In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr,format);
  verror(index, format, argptr);
  va_end(argptr);
}

void PatternParser::verror(intptr_t index, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  const String errMsg = vformat(format, argptr);
  throwException(_T("(%d):%s"), (int)index, errMsg.cstr());
}
