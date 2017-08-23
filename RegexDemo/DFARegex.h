#pragma once

#include <Semaphore.h>

class DFATables {
private:
  void init();
  void copy(const DFATables &src);
public:
  size_t      m_stateCount;
  // number of rows in m_transitionMatrix. maybe != m_stateCount
  size_t      m_rowCount;
  // width of each row in m_transisitonMatrix
  size_t      m_columnCount;
  // size = MAX_CHARS
  short      *m_charMap;
  // size = m_stateCount
  short      *m_stateMap;
  // size = m_rowCount * m_columnCount
  short      *m_transitionMatrix;
  // size = m_stateCount
  BYTE       *m_acceptTable;
  DFATables() {
    init();
  }
  DFATables(const DFATables &src);
  DFATables &operator=(const DFATables &src);
  ~DFATables();

  void allocate(size_t stateCount);
  void allocateMatrix(size_t rowCount, size_t columnCount);
  void clear();
  inline bool isEmpty() const {
    return m_stateCount == 0;
  }
  inline short &transition(UINT r, UINT c) {
    return m_transitionMatrix[m_columnCount*r+c];
  }
  inline short &transition(UINT r, UINT c) const {
    return m_transitionMatrix[m_columnCount*r+c];
  }
  inline int nextState(int state, _TUCHAR c) const {
    return transition(m_stateMap[state], m_charMap[c]);
  }
#ifdef _DEBUG
  String toString() const;
#endif
};

#ifdef _DEBUG

class _DFARegexCompilerState {
public:
  const   String  &m_codeText;
  const   String  &m_compilerStack;
  const   String  &m_fastMapStr;
  const   intptr_t m_scannerIndex;

  _DFARegexCompilerState(const String &codeText
                        ,const String &compilerStack
                        ,const String &fastMapStr
                        ,intptr_t      scannerIndex
                        )
   : m_codeText(codeText)
   , m_compilerStack(compilerStack)
   , m_fastMapStr(fastMapStr)
   , m_scannerIndex(scannerIndex)
  {
  }
};

class _DFARegexSearchState {
public:
  const intptr_t    m_startPos;
  const intptr_t    m_charIndex;
  _DFARegexSearchState(intptr_t startPos, intptr_t charIndex) : m_startPos(startPos), m_charIndex(charIndex) {
  }
};

class DFARegex;

class _DFADbgStateInfo {
public:
  const int m_lineNumber;
  BitSet    m_patternIndexSet;
  _DFADbgStateInfo(int lineNumber, const BitSet &patternIndexSet) : m_lineNumber(lineNumber), m_patternIndexSet(patternIndexSet) {
  }
};

typedef Array<_DFADbgStateInfo> _DFADbgInfo;

class _DFARegexMatchState {
private:
  const DFARegex  &m_regex;
  const int        m_state;
  const int        m_lastAcceptState;
  const intptr_t   m_pos;
  const intptr_t   m_charIndex;
  const intptr_t   m_lastAcceptIndex;
public:
  _DFARegexMatchState(const DFARegex *regex, int state, int lastAcceptState, intptr_t pos, intptr_t charIndex, intptr_t lastAcceptIndex)
    : m_regex(*regex)
    , m_state(state)
    , m_lastAcceptState(lastAcceptState)
    , m_pos(pos)
    , m_charIndex(charIndex)
    , m_lastAcceptIndex(lastAcceptIndex)
  {
  }
  UINT          getDBGLineNumber()      const;
  UINT          getDBGLastAcceptLine()  const;
  const BitSet *getDBGPatternIndexSet() const;
  intptr_t      getDBGTextCharIndex()   const;
  intptr_t      getDBGLastAcceptIndex() const;
  intptr_t      getPos() const;
  inline int    getState() const {
    return m_state;
  }
};

class DFARegexStepHandler {
public:
  virtual void handleCompileStep(const _DFARegexCompilerState &state) {
  }
  virtual void handleSearchStep( const _DFARegexSearchState   &state) {
  }
  virtual void handleMatchStep(  const _DFARegexMatchState    &state) {
  }
};

typedef enum {
  DFA_IDLE
 ,DFA_PARSING
 ,DFA_CONSTRUCTING_UNMIMIMZED_DFA
 ,DFA_CONSTRUCTING_DFA
 ,DFA_SEARCHING
 ,DFA_MATCHING
} DFARegexMode;

class DFA;

#endif // _DEBUG

class DFARegex {
private:
  DFATables             m_tables;
  BitSet                m_fastMap;

  // Translate table to apply to all characters before comparing.
  // or NULL for no translation. Translation is applied to each
  // character in a pattern when compiled and to characters in the
  // text string it is matched/matched
  const TCHAR          *m_translateTable;
  bool                  m_hasCompiled;
  mutable bool          m_patternFound;
  bool                  m_matchEmpty;
  // initialized to -1
  mutable intptr_t      m_resultLength;
  mutable int           m_currentState;
  mutable int           m_lastAcceptState;
  void init();
  void createFastMap();
  // Return resultlength, or -1 if no match
  intptr_t  match( const TCHAR     *string
                  ,size_t           size
                  ,intptr_t         pos) const;
  // Return resultlength, or -1 if no match
  intptr_t  search(const TCHAR     *string
                  ,size_t           size
                  ,intptr_t         startPos
                  ,intptr_t         range) const;

#ifdef _DEBUG
  String                          m_codeText;
  mutable bool                    m_codeDirty;
  mutable UINT                    m_cycleCount;
  // Built by compilePattern/DFA::toString(). Indexed by stateNumber
  mutable _DFADbgInfo             m_DBGInfo;
  DFARegexStepHandler            *m_stepHandler;
  mutable DFARegexMode            m_currentMode;
  mutable const DFA              *m_currentDFA;
  mutable Semaphore               m_gate;
  void setCodeText(const String codeText);
  void setDBGCharIndexForAcceptStates(size_t patternLength);
#endif // _DEBUG

public:
  DFARegex();
  DFARegex(           const String &pattern, const TCHAR *translateTable = NULL);
  DFARegex(           const TCHAR  *pattern, const TCHAR *translateTable = NULL);
  void compilePattern(const String &pattern, const TCHAR *translateTable = NULL);
  void compilePattern(const TCHAR  *pattern, const TCHAR *translateTable = NULL);
  // Search for the compiled expression in text
  intptr_t  search(   const String &text, bool forward = true, intptr_t startPos = -1) const;
  // Search for the compiled expression in text
  intptr_t  search(   const TCHAR  *text, bool forward = true, intptr_t startPos = -1) const;
  // Search for the compiled expression in text
  intptr_t  search(   const TCHAR  *text, size_t size, bool forward = true, intptr_t startPos = -1) const;
  // Check for exact match
  bool match(         const String &text) const;
  // Check for exact match
  bool match(         const TCHAR  *text) const;

  inline intptr_t getResultLength() const {
    return m_resultLength;
  }
  inline bool getMatchEmpty() const {
    return m_matchEmpty;
  }
  inline bool isCompiled() const {
    return m_hasCompiled;
  }

#ifdef _DEBUG
  DFARegexStepHandler *setHandler(DFARegexStepHandler *handler);
  CharacterFormater   *setCharacterFormater(CharacterFormater *formater);
  const String &toString() const;
  String fastMapToString() const;
  String tableToString()   const;
  bool isCodeDirty() const {
    return m_codeDirty;
  }
  const _DFADbgStateInfo *getDBGStateInfo(size_t state) const;
  BitSet                  getPossibleBreakPointLines() const;
  int                     getCurrentCodeLine() const;
  UINT                    getCycleCount() const {
    return m_cycleCount;
  }
  void paint(CWnd *wnd, CDC &dc, bool animate) const;
  void unmarkAll(CWnd *wnd, CDC &dc);
  void setBlinkersVisible(bool visible);
  static int   getAllocatedNFAStates();
#endif // _DEBUG
};
