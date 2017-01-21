#pragma once

#ifndef _DEBUG
#error "Must compile with _DEBUG"
#endif

#include <Thread.h>
#include <PropertyContainer.h>
#include <Regex.h>
#include "DFARegex.h"

typedef enum {
  SEARCH_RUNNING
 ,SEARCH_REGEXFINISHED
} DebugThreadProperties;

typedef enum {
  REGEX_UNDEFINED
 ,REGEX_COMPILING
 ,REGEX_SEARCHING
 ,REGEX_MATCHING
 ,REGEX_SUCEEDED
} RegexPhaseType;

typedef enum {
  COMMAND_COMPILE
 ,COMMAND_SEARCHFORWARD
 ,COMMAND_SEARCHBACKWRD
 ,COMMAND_MATCH
} ThreadCommand;

typedef enum {
  EMACS_REGEX
 ,DFA_REGEX
} RegexType;

class DebugThread;

class CompileParameters {
public:
  String m_pattern;
  bool   m_ignoreCase;
  CompileParameters(const String &pattern=_T(""), bool ignoreCase = false) : m_pattern(pattern), m_ignoreCase(ignoreCase) {
  }
  inline void reset() {
    m_pattern    = _T("");
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

class DebugRegex {
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
  RegexType getType() const {
    return m_type;
  }
  bool     isCodeDirty() const;
  bool     isCompiled() const;
  BitSet   getPossibleBreakPointLines() const;
  int      getCycleCount() const;
  void     paint(CWnd *wnd, bool animate) const;
  void     setHandler(DebugThread *handler);
  intptr_t getResultLength() const;
  CompileParameters getLastCompiledPattern() const;
};

class DebugThread : public Thread, public RegexStepHandler, public DFARegexStepHandler, public PropertyContainer {
private:
  const ThreadCommand                m_command;
  DebugRegex                        &m_regex;
  RegexPhaseType                     m_regexPhase;
  const void                        *m_handlerState;
  CompileParameters                  m_compileParameters;
  String                             m_text;
  const BitSet                      &m_breakPoints;
  bool                               m_singleStep;
  bool                               m_running;
  bool                               m_finished;
  bool                               m_killed;
  intptr_t                           m_foundStart, m_resultLength;
  String                             m_resultMsg;
  void validateRegexTypeAndPhase(RegexType expectedType, RegexPhaseType expectedPhase) const;
  void setPropRunning(bool value);
  void setPropFinished();
  void suspendOnSingleStep(RegexPhaseType phase, int lineNumber = -1);
  void enableHandleStep(bool enabled);
  void clearStates();
public:
  DebugThread(DebugRegex &regex, const CompileParameters &cp, const BitSet &breakPoints);
  DebugThread(ThreadCommand command, DebugRegex &regex, const String &text, const BitSet &breakPoints);
  ~DebugThread();
  UINT run();
  void handleCompileStep(const _RegexCompilerState    &state);
  void handleSearchStep( const _RegexSearchState      &state);
  void handleMatchStep(  const _RegexMatchState       &state);
  void handleCompileStep(const _DFARegexCompilerState &state);
  void handleSearchStep( const _DFARegexSearchState   &state);
  void handleMatchStep(  const _DFARegexMatchState    &state);
  ThreadCommand getCommand() const {
    return m_command;
  }
  void singleStep();
  void go();
  void kill();
  bool isFinished() const {
    return m_finished;
  }
  bool isRunning() const {
    return m_running;
  }
  RegexPhaseType getRegexPhase() const {
    return m_regexPhase;
  }
  void  getFoundPosition(int &start, int &end);
  const String               getResultMsg()     const;
  String registersToString()                    const;
  const _RegexCompilerState         &getEmacsCompilerState() const;
  const _RegexSearchState           &getEmacsSearchState()   const;
  const _RegexMatchState            &getEmacsMatchState()    const;
  const _DFARegexCompilerState      &getDFACompilerState()   const;
  const _DFARegexSearchState        &getDFASearchState()     const;
  const _DFARegexMatchState         &getDFAMatchState()      const;

};

