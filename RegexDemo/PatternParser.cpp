#include "stdafx.h"
#include "PatternParser.h"

#ifdef _DEBUG

#include "DFARegex.h"

#define TOKENSTR(t) _T(#t)

static const TCHAR *tokenStr[] = {
  TOKENSTR(_EOI         )
 ,TOKENSTR(NORMALCHAR   )
 ,TOKENSTR(NUMBER       )
 ,TOKENSTR(DOT          )
 ,TOKENSTR(QUEST        )
 ,TOKENSTR(PLUS         )
 ,TOKENSTR(STAR         )
 ,TOKENSTR(BAR          )
 ,TOKENSTR(LPAR         )
 ,TOKENSTR(RPAR         )
 ,TOKENSTR(LB           )
 ,TOKENSTR(RB           )
 ,TOKENSTR(LC           )
 ,TOKENSTR(RC           )
 ,TOKENSTR(COMMA        )
 ,TOKENSTR(DASH         )
 ,TOKENSTR(BEGINBUF     )
 ,TOKENSTR(ENDBUF       )
 ,TOKENSTR(BEGINLINE    )
 ,TOKENSTR(ENDLINE      )
 ,TOKENSTR(BEGINWORD    )
 ,TOKENSTR(ENDWORD      )
 ,TOKENSTR(WORDCHAR     )
 ,TOKENSTR(NONWORDCHAR  )
 ,TOKENSTR(WORDBOUND    )
 ,TOKENSTR(NOTWORDBOUND )
 ,TOKENSTR(REGISTER     )
};

#define DBG_callParserHandler() { DBG_callCompilerHandler(buildCodeString(), m_callStack.toString(), _T(""), m_scanner.getIndex()); }

void PatternParser::enter(const TCHAR *function) {
  if(m_NFA.m_stepHandler) {
    const String symStr = (m_token == NORMALCHAR) ? format(_T("'%c'"), m_scanner.getTheCharacter()).cstr() : tokenStr[m_token];
    m_callStack.push(format(_T("%-9s Input=%-12s Rest=\"%-12s\""), function, symStr.cstr(), m_scanner.getRest()));
  }
  DBG_callParserHandler();
}

void PatternParser::leave(const TCHAR *function) {
  if(!m_callStack.isEmpty()) {
    m_callStack.pop();
  }
  DBG_callParserHandler();
}

String PatternParser::buildCodeString() const {
  return NFAState::allAllocatedToString();
}

#define ENTER(f) enter(_T(#f))
#define LEAVE(f) leave(_T(#f))

void PatternParser::nextToken() {
  m_token = m_scanner.nextToken();
  DBG_callParserHandler();
}

#define DBG_setCharIndex(nfa, start, end)  nfa.setCharIndex(start,end)
#define DBG_saveIndex(             name )  intptr_t dbg_##name = m_scanner.getIndex()
#define DBG_setIndex(              name )  dbg_##name     = m_scanner.getIndex()
#define DBG_getIndex(              name )  dbg_##name

#else

#define ENTER(f)
#define LEAVE(f)

#define DBG_callParserHandler()
#define DBG_setCharIndex(nfa, start, end)
#define DBG_saveIndex(             name )
#define DBG_setIndex(              name )

#endif

static const TCHAR *unexpectedEndMsg    = _T("Unexpected end of regular expression");
static const TCHAR *minMaxRepeatMsg     = _T("Max repeatcount must be >= min repeatcount");
static const TCHAR *maxRepeatIsZeroMsg  = _T("Max repeatcount is zero");
static const TCHAR *regnoNotDefined     = _T("Register %d not defined at the specified point of the pattern. %s");
static const TCHAR *expectedNumberMsg   = _T("Expected number");
static const TCHAR *expectedRCurlMsg    = _T("Expected '}'");
static const TCHAR *unmatchedLPMsg      = _T("Unmatched \\(");
static const TCHAR *unmatchedRPMsg      = _T("Unmatched \\)");
static const TCHAR *tooManyCountersMsg  = _T("Too many counters in use. Max=%d");
static const TCHAR *illegalJumpMsg      = _T("Regex::first(%d,%d):Jump to a previous address (=%d) not legal");
static const TCHAR *missingRBMsg        = _T("Missing ] in character class");

PatternParser::PatternParser(const String &pattern, NFA &nfa, const TCHAR *translateTable) 
: m_scanner(pattern.cstr(), pattern.length(), translateTable)
, m_NFA(nfa)
{
  thompsonConstruction();
}

void PatternParser::thompsonConstruction() {
  nextToken();
  m_NFA.create(compilePattern());
}

NFAState *PatternParser::compilePattern() {
  //  pattern           -> anchor_bol exprList anchor_eol
  //
  //  anchor_bol        -> ^
  //                     | epsilon
  //
  //  anchor_eol        -> $
  //                     | epsilon
  //

  SubNFA s;
  BYTE   anchor = ANCHOR_NONE;

  ENTER(compile);
  if(match(BEGINLINE)) { // Anchor bol (beginning of line)
    nextToken();
    s.create2StateNFA(NEWLINE);
    s += exprList();
    anchor |= ANCHOR_START ;
  } else {
    s = exprList();
  }

  if(match(ENDLINE)) {   // Anchor eol pattern followed by \r or \n (use a character class).
    nextToken();

    SubNFA end;
#ifdef _WIN32
    CharacterSet charSet;
    charSet.add(NEWLINE).add(CR);
    end.create2StateNFA(charSet);
#else
    end.create2StateNFA(NEWLINE)
#endif
    s += end;
    anchor |= ANCHOR_END  ;
  }

  if(!s.isEmpty()) {
    s.setAccepting(anchor);
    DBG_setCharIndex(s, 0, m_scanner.getIndex());
    s.getStart()->setToStartState();
  }
  if(!match(_EOI)) {
    expected(_T("end of pattern"));
  }
  LEAVE(compile);
  return s.getStart();
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

  ENTER(exprList);
  SubNFA s = factor();
  while(firstExprList(m_token)) {
    s += factor();
  }
  LEAVE(exprList);
  return s;
}

bool PatternParser::firstExprList(PatternToken token) const {
  switch(token) {
  case _EOI    :
  case STAR    : // *
  case PLUS    : // +
  case QUEST   : // ?
  case LC      : // {
  case BAR     :
  case RPAR    :
  case ENDLINE :
    return false;
  }
  return true;
}

static bool isClosureToken(PatternToken token) {
  switch(token) {
  case STAR    : // *
  case PLUS    : // +
  case QUEST   : // ?
  case LC      : // {
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
  //                     | term { range }
  //
  // range              -> number , number
  //                     | number ,

  ENTER(factor);
  SubNFA s = term();
  if(isClosureToken(m_token)) {
    switch(m_token) {
    case QUEST: s = quest(s); break;
    case PLUS : s = plus( s); break;
    case STAR : s = star( s); break;
    case LC   : s = range(s); break;
    default   : unexpectedInput();
    }
    nextToken();
  }
  LEAVE(factor);
  return s;
}

SubNFA PatternParser::range(const SubNFA &s) {
  ENTER(range);

  nextToken();
  if(!match(NUMBER)) {
    expected(_T("number"));
  }
  const int minRepeat = m_scanner.getTheNumber();
  nextToken();
  if(!match(COMMA)) {
    expected(_T(","));
  }

  int maxRepeat = 0;
  nextToken();
  if(match(NUMBER)) {
    maxRepeat = m_scanner.getTheNumber();
    nextToken();
    if(maxRepeat == 0) {
      error(maxRepeatIsZeroMsg);
    }
    if(maxRepeat < minRepeat) {
      error(minMaxRepeatMsg);
    }
  }
  if(!match(RC)) {
    expected(_T("}"));
  }

  SubNFA result;
  if((maxRepeat == 0) && (minRepeat <= 1)) {      // Like '+' or '*' closure
    if(minRepeat == 1) {                          // Like '+' closure
      result = plus(s);
    } else {
      result = star(s);
    }
  } else if(maxRepeat == 1) {
    if(minRepeat == 1) {                          // minRepeat == maxRepeat == 1 => Just match once. => do nothing
      result = s;
    } else {                                      // minRepeat == 0 and maxRepeat == 1 =>  Like '?' closure
      result = quest(s);
    }
  } else {
    CompactArray<SubNFA> nfaArray;
    if(minRepeat == 0) {
      const SubNFA q = quest(s);
      DBG_callParserHandler();
      nfaArray.add(q);
      for(int i = 1; i < maxRepeat; i++) {        // Create maxRepeat-1 copies of s? so result will be s?s?s?... maxRepeat times
        nfaArray.add(q.clone());
        DBG_callParserHandler();
      }
    } else {
      nfaArray.add(s);
      for(int i = 1; i < minRepeat; i++) {        // First create minRepeat copies of s
        nfaArray.add(s.clone());
        DBG_callParserHandler();
      }
      const int optionalCount = maxRepeat - minRepeat;
      if(optionalCount > 0) {                     // Then create optinalCount copies of s?
        const SubNFA q = quest(s.clone());
        DBG_callParserHandler();
        nfaArray.add(q);                          // First add q which is s?
        for(int i = 1; i < optionalCount; i++) {      // Then add optionalCount-1 copies of q
          nfaArray.add(q.clone());
          DBG_callParserHandler();
        }
      }
    }
    for(size_t i = 0; i < nfaArray.size(); i++) {    // Result will be sssssqqqqq... with minRepeat occurrences of s
      result += nfaArray[i];                      // and maxRepeat-minRepeat occurrences of q (q = s?)
      DBG_callParserHandler();
    }
    nfaArray.clear();
  }
  LEAVE(range);

  return result;
}

SubNFA PatternParser::term() {
  //
  // term               -> LPAR alternativeList RPAR
  //                     | DOT
  //                     | LB optComplement optCharClass RB
  //                     | NORMALCHAR
  //                     | WORDCHAR    :
  //                     | NONWORDCHAR :
  // 
  // optCharClass       -> charClass
  //                     | epsilon
  //
  // optCcomplement     -> ^
  //                     | epsilon
  //
  // The [] is nonstandard. It matches a space ' ', tab '\t', formfeed '\f', or newline '\n',
  // but not a carriage return '\r'.

  ENTER(term);

  SubNFA result;
  DBG_saveIndex(startIndex);
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
    DBG_setCharIndex(result, DBG_getIndex(startIndex), m_scanner.getIndex());
    break;
  case DOT            : // DOT
  case LB             : // [ characterClass ]
    { CharacterSet charSet;
      if(match(DOT)) { // . (DOT) matches everything except '\n' (for Windows also '\r')
        charSet.add(0, MAX_CHARS-1);
        if(m_scanner.hasTranslateTable()) charSet = m_scanner.translate(charSet);

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

      DBG_setCharIndex(result, DBG_getIndex(startIndex), m_scanner.getIndex());
    }
    break;
  case WORDCHAR    :            // \w
  case NONWORDCHAR :            // \W
    { CharacterSet charSet;
      charSet += m_scanner.hasTranslateTable() ? m_scanner.translate(wordLetterSet) : wordLetterSet;
      if(m_token == NONWORDCHAR) charSet.invert();

      nextToken();
      result.create2StateNFA(charSet);

      DBG_setCharIndex(result, DBG_getIndex(startIndex), m_scanner.getIndex());
    }
    break;
  }

  LEAVE(term);
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

  ENTER(charClass);
  while(!match(_EOI) && !match(RB)) {
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
    case _EOI: unexpectedEndOfPattern();
    default  : unexpectedInput();
    }
  }
  LEAVE(charClass);
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

  ENTER(altList);
  SubNFA s = exprList();
  while(match(BAR)) {
    nextToken();
    s |= exprList();
  }
  LEAVE(altList);
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

void PatternParser::error(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  verror(m_scanner.getIndex(), format, argptr);
  va_end(argptr);
}

void PatternParser::error(intptr_t index, const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  verror(index, format, argptr);
  va_end(argptr);
}

void PatternParser::verror(intptr_t index, const TCHAR *format, va_list argptr) {
  const String errMsg = vformat(format, argptr);
  throwException(_T("(%d):%s"), (int)index, errMsg.cstr());
}
