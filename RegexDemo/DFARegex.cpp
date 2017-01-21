#include "stdafx.h"
#include "PatternParser.h"
#include "DFA.h"
#include "DFARegex.h"

#ifdef _DEBUG
#include "DFAPainter.h"
#endif

DFARegex::DFARegex() : m_fastMap(10) {
  init();
}

DFARegex::DFARegex(const String &pattern, const TCHAR *translateTable) : m_fastMap(MAX_CHARS) {
  init();
  compilePattern(pattern, translateTable);
}

DFARegex::DFARegex(const TCHAR *pattern, const TCHAR *translateTable) : m_fastMap(MAX_CHARS) {
  init();
  compilePattern(pattern, translateTable);
}

#ifdef _DEBUG

#define DBG_setCodeText(s)      setCodeText(s)
#define DBG_setMode(mode, dfa)  { m_currentMode = mode; m_currentDFA = dfa; DFAPainter::stopBlinker(); }
#define DBG_saveMode(tmp)       const DFARegexMode savedMode_##tmp = m_currentMode
#define DBG_restoreMode(tmp)    DBG_setMode(savedMode_##tmp, NULL)

#else

#define DBG_setCodeText(s)
#define DBG_setMode(mode, dfa)
#define DBG_saveMode(tmp)
#define DBG_restoreMode(tmp)

#endif

void DFARegex::init() {
  m_translateTable = NULL;
  m_hasCompiled    = false;
  m_resultLength   = -1;

#ifdef _DEBUG
  m_codeDirty      = false;
  m_cycleCount     = 0;
  m_stepHandler    = NULL;
  DBG_setMode(DFA_IDLE, NULL);
  m_currentDFA     = NULL;
#endif
}


void DFARegex::compilePattern(const TCHAR *pattern, const TCHAR *translateTable) {
  compilePattern(String(pattern), translateTable);
}

void DFARegex::compilePattern(const String &pattern, const TCHAR *translateTable) {
#ifdef _DEBUG
    NFA nfa(m_stepHandler);
#else
    NFA nfa;
#endif

  try {
    NFAState::releaseAll();

    DBG_setCodeText(_T(""));

    m_translateTable = translateTable;
    m_hasCompiled = false;
    DBG_setMode(DFA_PARSING, NULL);
    PatternParser parser(pattern, nfa, translateTable);

    DFA dfa(nfa);
    DBG_setMode(DFA_CONSTRUCTING_UNMIMIMZED_DFA, &dfa);
    dfa.construct();

    DBG_setCodeText(dfa.toString(false, &m_DBGInfo));

    dfa.getDFATables(m_tables);
    m_matchEmpty = (!m_tables.isEmpty()) && (m_tables.m_acceptTable[0] != 0);
    createFastMap();
    m_hasCompiled = true;

#ifdef _DEBUG
    setDBGCharIndexForAcceptStates(pattern.length());
#endif

    _DBG_callCompilerHandler(m_stepHandler, toString(), _T(""), fastMapToString(), -1)

    nfa.clear();
    NFAState::releaseAll();
    DBG_setMode(DFA_IDLE, NULL);
  } catch(...) {
    nfa.clear();
    NFAState::releaseAll();
    DBG_setMode(DFA_IDLE, NULL);
    throw;
  }
}

void DFARegex::createFastMap() {
  m_fastMap.setCapacity(MAX_CHARS);
  m_fastMap.clear();
  if(m_tables.isEmpty()) {
    return;
  }
  for(int ch = 0; ch < MAX_CHARS; ch++) {
    if(m_tables.nextState(0,ch) != FAILURE) {
      m_fastMap.add(ch);
    }
  }
}

static const TCHAR *noRegExpressionMsg  = _T("No regular expression specified");

#ifdef _DEBUG
#define DBG_resetCycleCount()   m_cycleCount = 0
#define DBG_incrCycleCount()    m_cycleCount++
#else
#define DBG_resetCycleCount()
#define DBG_incrCycleCount()
#endif

// --------------------------------------- search -----------------------------------------

intptr_t DFARegex::search(const String &text, bool forward, intptr_t startPos) const { // search for the compiled expression in text
  return search(text.cstr(), text.length(), forward, startPos);
}

intptr_t DFARegex::search(const TCHAR *text, bool forward, intptr_t startPos) const {
  return search(text, _tcsclen(text), forward, startPos);
}

intptr_t DFARegex::search(const TCHAR  *text
                         ,size_t        size
                         ,bool          forward
                         ,intptr_t      startPos) const {
  if(!m_hasCompiled) {
    throwException(noRegExpressionMsg);
  }
  DBG_resetCycleCount();
  if(startPos < 0) {
    startPos = forward ? 0 : size;
  } else if(startPos > (intptr_t)size) {
    startPos = size;
  }
  if(forward) {
    return search(text, size, startPos, size - startPos);
  } else {
    return search(text, size, startPos, -startPos);
  }
}

intptr_t DFARegex::search(const TCHAR *string
                          ,size_t       size
                          ,intptr_t     startPos
                          ,intptr_t     range
                         ) const {
  DBG_setMode(DFA_SEARCHING, NULL);
  for(;;) {
    // skip quickly over characters that cannot possibly be the start of a match.
    // Note, however, that if the pattern can possibly match
    // the null String, we must test it at each starting point
    // so that we take the first null String we get.

#define GETCP(pos) (string + (pos))

    if((startPos < (intptr_t)size) && !m_matchEmpty) {
      if(range > 0) {
        intptr_t lim    = 0;
        intptr_t irange = range;
        if(startPos + range >= (intptr_t)size) {
          lim = range - (size - startPos);
        }

        const TCHAR *p = GETCP(startPos);

        while((range > lim) && !m_fastMap.contains(TRANSLATE(*p++))) {
          DBG_incrCycleCount();
          DBG_callSearchHandler(startPos, (p-1 - string));
          range--;
        }
        startPos += irange - range;
      } else {
        TCHAR c = *GETCP(startPos);

        if(!m_fastMap.contains(TRANSLATE(c))) {
          DBG_incrCycleCount();
          DBG_callSearchHandler(startPos, startPos);
          goto advance;
        }
      }
    } // end if

    if((range >= 0) && (startPos == size) && !m_matchEmpty) {
      return -1;
    }

    if((m_resultLength = match(string, size, startPos)) >= 0) {
      return startPos;
    }

  advance:
    if(!range) {
      break;
    }
    if(range > 0) {
      range--;
      startPos++;
    } else {
      range++;
      startPos--;
    }
  } // end for
  return -1;
}

// --------------------------------------- match --------------------------------------

bool DFARegex::match(const TCHAR *text) const {
  if(!m_hasCompiled) {
    throwException(noRegExpressionMsg);
  }
  DBG_resetCycleCount();
  if(m_tables.isEmpty()) {
    return false;
  }
  const size_t len = _tcsclen(text);
  return (match(text, len, 0) >= 0) && (m_resultLength == len);
}

bool DFARegex::match(const String &text) const {
  if(!m_hasCompiled) {
    throwException(noRegExpressionMsg);
  }
  DBG_resetCycleCount();
  if(m_tables.isEmpty()) {
    return false;
  }
  return ((match(text.cstr(), text.length(), 0)) >= 0) && (m_resultLength == text.length());
}

intptr_t DFARegex::match(const TCHAR *string, size_t size, intptr_t pos) const { // private
  int            nextState;             // Next state
  int            anchor;                // Anchor mode for most recently seen accepting state
  _TUCHAR        lookahead;             // Lookahead character
  const _TUCHAR *matchEndp       = (const _TUCHAR*)string + pos;
  DBG_saveMode(oldMode);
  DBG_setMode(DFA_MATCHING, NULL);

  m_currentState    = 0;  // Current state
  m_lastAcceptState = m_tables.m_acceptTable[0] ? 0 : -1;  // Most recently seen accept state
  const _TUCHAR *cp, *endString;
  for(cp = matchEndp, endString = (_TUCHAR*)string + size; cp < endString;) {
    // Check end of file. If there's an unprocessed accepting state,
    // lastAcceptState will be nonzero. In this case, ignore EOF for now so
    // that you can do the accepting action; otherwise, try to open another
    // file and return if you can't.

    for(;;) {
      DBG_incrCycleCount();
      DBG_callMatchHandler()
      if(lookahead = TRANSLATE(*cp)) {
        nextState = m_tables.nextState(m_currentState, lookahead);
        break;
      } else if(m_lastAcceptState >= 0) {
        nextState = -1;
        break;
      } else {
        goto NoMatch;
      }
    }
    if(nextState != -1) {
      if(anchor = m_tables.m_acceptTable[nextState]) { // Is this an accept state
        m_lastAcceptState = nextState;
        matchEndp = ++cp;
      } else {
        cp++;
      }
      m_currentState = nextState;
    } else if(m_lastAcceptState < 0) {    // No match
      goto NoMatch;
    } else {
      break; // found it
/*
      if(anchor & ANCHOR_END) {           // If end anchor is active
        pushback(1);                      // Push back the CR or LF
      }
      if(anchor & ANCHOR_START) {         // If start anchor is active
        moveStart();                      // Skip the leading newline
      }
*/
    }
  }
  DBG_callMatchHandler()
  DBG_restoreMode(oldMode);
  return m_resultLength = (m_lastAcceptState < 0) ? -1 : ((matchEndp - ((_TUCHAR*)string + pos)));

NoMatch:
  DBG_restoreMode(oldMode);
  return -1;
}


#ifdef _DEBUG

DFARegexStepHandler *DFARegex::setHandler(DFARegexStepHandler *handler) {
  DFARegexStepHandler *oldHandler = m_stepHandler;
  m_stepHandler = handler;
  return oldHandler;
}

CharacterFormater *DFARegex::setCharacterFormater(CharacterFormater *formater) {
  CharacterFormater *oldFormater = NFAState::getFormater();
  NFAState::setFormater(formater);
  return oldFormater;
}

void DFARegex::setCodeText(const String codeText) {
  if(codeText != m_codeText) {
    m_codeText = codeText;
    m_codeDirty = true;
  }
}

void DFARegex::setDBGCharIndexForAcceptStates(size_t patternLength) {
  for(size_t s = 0; s < m_tables.m_stateCount; s++) {
    if(m_tables.m_acceptTable[s]) {
      BitSet &set = m_DBGInfo[s].m_patternIndexSet;
      if(set.getCapacity() <= patternLength+1) {
        set.setCapacity(patternLength+1);
      }
      set.add(patternLength);
    }
  }
}

const String &DFARegex::toString() const {
  m_codeDirty = false;
  return m_codeText;
}

String DFARegex::fastMapToString() const {
  return charBitSetToString(m_fastMap, NFAState::getFormater());
}

String DFARegex::tableToString() const {
  return m_tables.toString();
}

const _DFADbgStateInfo *DFARegex::getDBGStateInfo(size_t state) const {
  if(state >= m_DBGInfo.size()) {
    return NULL;
  }
  return &m_DBGInfo[state];
}

BitSet DFARegex::getPossibleBreakPointLines() const {
  if(!isCompiled()) {
    return BitSet(10);
  }
  int maxLineNumber = 0;
  for(size_t s = 0; s < m_DBGInfo.size(); s++) {
    const _DFADbgStateInfo *info = getDBGStateInfo(s);
    if(info->m_lineNumber > maxLineNumber) {
      maxLineNumber= info->m_lineNumber;
    }
  }
  BitSet result(maxLineNumber+1);
  for(size_t s = 0; s < m_DBGInfo.size(); s++) {
    result.add(getDBGStateInfo(s)->m_lineNumber);
  }
  return result;
}

void DFARegex::paint(CWnd *wnd, bool animate) const {
  switch(m_currentMode) {
  case DFA_PARSING:
    if(animate) {
      NFAAnimateBuildStep(wnd);
    } else {
      NFAPaint(wnd);
    }
    break;
  case DFA_CONSTRUCTING_UNMIMIMZED_DFA:
  case DFA_CONSTRUCTING_DFA           :
    m_currentDFA->paint(wnd);
    break;
  default:
    { NFA dummy;
      DFA dfa(m_tables, dummy);
      int state, lastAcceptState;
      if(m_currentMode != DFA_MATCHING) {
        state = lastAcceptState = -1;
      } else {
        state           = m_currentState;
        lastAcceptState = m_lastAcceptState;
      }
      dfa.paint(wnd, state, lastAcceptState);
    }
    break;
  }
}

int DFARegex::getAllocatedNFAStates() { // static 
  return NFAState::getAllocated();
}

UINT _DFARegexMatchState::getDBGLineNumber() const {
  const _DFADbgStateInfo *info = m_regex.getDBGStateInfo(m_state);
  return info ? info->m_lineNumber : -1;
}

UINT _DFARegexMatchState::getDBGLastAcceptLine() const {
  const _DFADbgStateInfo *info = m_regex.getDBGStateInfo(m_lastAcceptState);
  return info ? info->m_lineNumber : -1;
}

const BitSet *_DFARegexMatchState::getDBGPatternIndexSet() const {
  const _DFADbgStateInfo *info = m_regex.getDBGStateInfo(m_state);
  return info ? &info->m_patternIndexSet : NULL;
}

intptr_t _DFARegexMatchState::getDBGTextCharIndex() const {
  return m_charIndex;
}

intptr_t _DFARegexMatchState::getDBGLastAcceptIndex() const {
  return m_lastAcceptIndex;
}

intptr_t _DFARegexMatchState::getPos() const {
  return m_pos;
}

#endif

