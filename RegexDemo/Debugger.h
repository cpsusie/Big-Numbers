#pragma once

#ifndef _DEBUG
#error "Must compile with _DEBUG"
#endif

#include <InterruptableRunnable.h>
#include <PropertyContainer.h>
#include <Regex.h>
#include "DFARegex.h"

#define FL_SINGLESTEP 0x01
#define FL_ALLBREAKFLAGS (FL_SINGLESTEP)

typedef enum {
  DEBUGGER_STATE
} DebuggerProperties;

typedef enum {
  DEBUGGER_CREATED
 ,DEBUGGER_RUNNING
 ,DEBUGGER_PAUSED
 ,DEBUGGER_TERMINATED
} DebuggerState;

typedef enum {
  REGEX_UNDEFINED
 ,REGEX_COMPILING
 ,REGEX_COMPILEDOK
 ,REGEX_COMPILEDFAILED
 ,REGEX_SEARCHING
 ,REGEX_MATCHING
 ,REGEX_PATTERNFOUND
 ,REGEX_SEARCHFAILED
 ,REGEX_MATCHFAILED
} RegexPhaseType;

typedef enum {
  COMMAND_COMPILE
 ,COMMAND_SEARCHFORWARD
 ,COMMAND_SEARCHBACKWRD
 ,COMMAND_MATCH
} RegexCommand;

typedef enum {
  EMACS_REGEX
 ,DFA_REGEX
} RegexType;

class Debugger;

class CompileParameters {
public:
  String m_pattern;
  bool   m_ignoreCase;
  CompileParameters(const String &pattern=EMPTYSTRING, bool ignoreCase = false) : m_pattern(pattern), m_ignoreCase(ignoreCase) {
  }
  inline void reset() {
    m_pattern    = EMPTYSTRING;
    m_ignoreCase = false;
  }

  inline const TCHAR *getTranslateTable() const {
    return m_ignoreCase ? String::upperCaseTranslate : NULL;
  }

  inline bool operator==(const CompileParameters &cp) const {
    return (m_ignoreCase == cp.m_ignoreCase) && (m_pattern == cp.m_pattern);
  }

  inline bool operator!=(const CompileParameters &cp) const {
    return !(*this == cp);
  }
};

class DebugRegex : public PropertyChangeListener {
private:
  RegexType         m_type;
  Regex             m_regex;
  DFARegex          m_DFARegex;
  RegexRegisters    m_registers;
  String            m_text;
  CompileParameters m_lastCompiledParameters[2];
  void throwUnknownTypeException() const;
public:
  DebugRegex() {
    m_type = EMACS_REGEX;
  }
  void      compilePattern(const CompileParameters &cp);
  bool      match(const String &text);
  intptr_t  search(const String &text, bool forward);
  String    registersToString() const;
  String    codeToString() const;
  String    fastMapToString() const;
  String    getDFATablesToString() const;
  bool      hasDFATables() const;
  bool      getMatchEmpty() const;
  void      setType(RegexType type);
  inline RegexType getType() const {
    return m_type;
  }
  bool     isCodeDirty() const;
  bool     isCompiled() const;
  BitSet   getPossibleBreakPointLines() const;
  int      getPatternFoundCodeLine() const;
  int      getCycleCount() const;
  void     paint(CWnd *wnd, CDC &dc, bool animate) const;
  void     handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  void     unmarkAll(CWnd *wnd, CDC &dc);
  void     setHandler(Debugger *handler);
  intptr_t getResultLength() const;
  CompileParameters getLastCompiledPattern() const;
};

class Debugger : public InterruptableRunnable, public RegexStepHandler, public DFARegexStepHandler, public PropertyContainer {
private:
  FLAGTRAITS(Debugger, BYTE, m_flags);
  DebuggerState                      m_state;
  const BitSet                      &m_breakPoints;
  const RegexCommand                 m_command;
  DebugRegex                        &m_regex;
  RegexPhaseType                     m_regexPhase;
  const void                        *m_handlerState;
  CompileParameters                  m_compileParameters;
  String                             m_text;
  intptr_t                           m_foundStart, m_resultLength;
  String                             m_resultMsg;
  void validateRegexTypeAndPhase(RegexType expectedType, RegexPhaseType expectedPhase) const;
  void suspendOnSingleStep(RegexPhaseType phase, int lineNumber = -1);
  Debugger &enableHandleStep(bool enabled);
  void initDebugger(bool singleStep);
  void clearStates();
  inline Debugger &checkTerminated() {
    if(getState() == DEBUGGER_TERMINATED) throwException(_T("Debugger is terminated"));
    return *this;
  }
  void suspend();
public:
  Debugger(DebugRegex &regex, const CompileParameters &cp, const BitSet &breakPoints);
  Debugger(RegexCommand command, DebugRegex &regex, const String &text, const BitSet &breakPoints);
  ~Debugger();
  void singleStep(BYTE breakFlags);
  inline void go() {
    singleStep(0);
  }
  void kill();
  UINT safeRun();
  inline DebuggerState getState() const {
    return m_state;
  }
  inline String getStateName() const {
    return getStateName(getState());
  }
  static String getStateName(DebuggerState state);

  inline RegexCommand getCommand() const {
    return m_command;
  }
  inline RegexPhaseType getRegexPhase() const {
    return m_regexPhase;
  }
  static String getPhaseName(RegexPhaseType phase);
  inline String getPhaseName() const {
    return getPhaseName(getRegexPhase());
  }

  void   getFoundPosition(int &start, int &end);
  String getResultMsg()      const;
  String registersToString() const;
  void handleCompileStep(const _RegexCompilerState    &state);
  void handleSearchStep( const _RegexSearchState      &state);
  void handleMatchStep(  const _RegexMatchState       &state);
  void handleCompileStep(const _DFARegexCompilerState &state);
  void handleSearchStep( const _DFARegexSearchState   &state);
  void handleMatchStep(  const _DFARegexMatchState    &state);
  const _RegexCompilerState         &getEmacsCompilerState() const;
  const _RegexSearchState           &getEmacsSearchState()   const;
  const _RegexMatchState            &getEmacsMatchState()    const;
  const _DFARegexCompilerState      &getDFACompilerState()   const;
  const _DFARegexSearchState        &getDFASearchState()     const;
  const _DFARegexMatchState         &getDFAMatchState()      const;
};
