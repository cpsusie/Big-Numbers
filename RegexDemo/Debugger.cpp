#include "stdafx.h"
#include <Thread.h>
#include <ThreadPool.h>
#include "Debugger.h"

Debugger::Debugger(DebugRegex &regex, const CompileParameters &cp, const BitSet &breakPoints)
: m_command(COMMAND_COMPILE)
, m_regex(regex)
, m_compileParameters(cp)
, m_breakPoints(breakPoints)
{
  initDebugger(true);
}

Debugger::Debugger(RegexCommand command, DebugRegex &regex, const String &text, const BitSet &breakPoints)
: m_command(command)
, m_regex(regex)
, m_text(text)
, m_breakPoints(breakPoints)
{
   if(m_command == COMMAND_COMPILE) {
    m_compileParameters = text;
  }
  initDebugger(m_command == COMMAND_COMPILE);
}

void Debugger::initDebugger(bool singleStep) {
  clearStates();
  setFlag(FL_SINGLESTEP, singleStep);
  m_state       = DEBUGGER_CREATED;
  m_regexPhase  = REGEX_UNDEFINED;
}

void Debugger::clearStates() {
  m_handlerState = nullptr;
}

Debugger::~Debugger() {
  enableHandleStep(false).kill();
}

void Debugger::singleStep(BYTE breakFlags) {
  checkTerminated()
 .clrFlag(FL_ALLBREAKFLAGS)
 .setFlag(breakFlags)
 .enableHandleStep(breakFlags || !m_breakPoints.isEmpty()).resume();
}

void Debugger::kill() {
  setInterrupted();
  waitUntilJobDone();
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_TERMINATED);
}

UINT Debugger::safeRun() {
  SETTHREADDESCRIPTION("Debugger");
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_RUNNING);
  m_foundStart   = -1;
  m_resultLength = 0;
  switch(m_command) {
  case COMMAND_COMPILE:
    m_regex.compilePattern(m_compileParameters);
    m_resultMsg  = _T("Pattern Ok");
    m_regexPhase = REGEX_COMPILEDOK;
    break;
  case COMMAND_SEARCHFORWARD:
  case COMMAND_SEARCHBACKWRD:
    m_foundStart = m_regex.search(m_text, m_command == COMMAND_SEARCHFORWARD);
    if(m_foundStart >= 0) {
      m_resultLength = m_regex.getResultLength();
      m_resultMsg    = format(_T("Found at %zd. length:%zd"), m_foundStart, m_resultLength);
      m_regexPhase   = REGEX_PATTERNFOUND;
    } else {
      m_resultMsg    = _T("Not found");
      m_regexPhase   = REGEX_SEARCHFAILED;
    }
    break;
  case COMMAND_MATCH  :
    if(m_regex.match(m_text)) {
      m_foundStart   = 0;
      m_resultLength = (int)m_text.length();
      m_resultMsg    = _T("Match");
      m_regexPhase   = REGEX_PATTERNFOUND;
    } else {
      m_resultMsg    = _T("No match");
      m_regexPhase   = REGEX_MATCHFAILED;
    }
    break;
  }
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_TERMINATED);
  return 0;
}

Debugger &Debugger::enableHandleStep(bool enabled) {
  m_regex.setHandler(enabled ? this : nullptr);
  return *this;
}

void Debugger::handleCompileStep(const _RegexCompilerState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_COMPILING);
}

void Debugger::handleSearchStep(const _RegexSearchState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_SEARCHING);
}

void Debugger::handleMatchStep(const _RegexMatchState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_MATCHING, state.getDBGLineNumber());
}

void Debugger::handleCompileStep(const _DFARegexCompilerState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_COMPILING);
}

void Debugger::handleSearchStep(const _DFARegexSearchState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_SEARCHING);
}

void Debugger::handleMatchStep(const _DFARegexMatchState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_MATCHING, state.getDBGLineNumber());
}

void Debugger::suspendOnSingleStep(RegexPhaseType phase, int lineNumber) {
  if(isSet(FL_SINGLESTEP) || ((lineNumber>=0) && m_breakPoints.contains(lineNumber))) {
    m_regexPhase  = phase;
    suspend();
  }
  clearStates();
}

void Debugger::suspend() {
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_PAUSED);
  __super::suspend();
  m_regexPhase = REGEX_UNDEFINED;
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_RUNNING);
}

void Debugger::getFoundPosition(int &start, int &end) {
  if(getRegexPhase() != REGEX_PATTERNFOUND) {
    start = end = -1;
  } else {
    start = (int)m_foundStart;
    end   = (int)(start + m_resultLength);
  }
}

String Debugger::getResultMsg() const {
  if(getState() == DEBUGGER_RUNNING) {
    return _T("No result yet. Thread not finished");
  }
  return m_resultMsg;
}

String Debugger::registersToString() const {
  if(m_regexPhase != REGEX_PATTERNFOUND) {
    throwException(_T("Cannot get register at this time of the search"));
  }
  return m_regex.registersToString();
}

const _RegexCompilerState &Debugger::getEmacsCompilerState() const {
  validateRegexTypeAndPhase(EMACS_REGEX, REGEX_COMPILING);
  return *(_RegexCompilerState*)m_handlerState;
}

const _RegexSearchState &Debugger::getEmacsSearchState() const {
  validateRegexTypeAndPhase(EMACS_REGEX, REGEX_SEARCHING);
  return *(_RegexSearchState*)m_handlerState;
}

const _RegexMatchState &Debugger::getEmacsMatchState() const {
  validateRegexTypeAndPhase(EMACS_REGEX, REGEX_MATCHING);
  return *(_RegexMatchState*)m_handlerState;
}

const _DFARegexCompilerState &Debugger::getDFACompilerState() const {
  validateRegexTypeAndPhase(DFA_REGEX, REGEX_COMPILING);
  return *(_DFARegexCompilerState*)m_handlerState;
}

const _DFARegexSearchState &Debugger::getDFASearchState() const {
  validateRegexTypeAndPhase(DFA_REGEX, REGEX_SEARCHING);
  return *(_DFARegexSearchState*)m_handlerState;
}

const _DFARegexMatchState &Debugger::getDFAMatchState() const {
  validateRegexTypeAndPhase(DFA_REGEX, REGEX_MATCHING);
  return *(_DFARegexMatchState*)m_handlerState;
}

static const TCHAR *regexTypeName[] = {
  _T("EMACS_REGEX")
 ,_T("DFA_REGEX")
};

String Debugger::getPhaseName(RegexPhaseType phase) { // static
  switch(phase) {
  case REGEX_UNDEFINED     : return _T("Undefined"      );
  case REGEX_COMPILING     : return _T("Compiling"      );
  case REGEX_COMPILEDOK    : return _T("Compiled ok"    );
  case REGEX_COMPILEDFAILED: return _T("Compiled failed");
  case REGEX_SEARCHING     : return _T("Searching"      );
  case REGEX_MATCHING      : return _T("Matching"       );
  case REGEX_PATTERNFOUND  : return _T("Pattern found"  );
  case REGEX_SEARCHFAILED  : return _T("Search failed"  );
  case REGEX_MATCHFAILED   : return _T("Match failed"   );
  default                  : return format(_T("Unknown phase (=%d)"), phase);
  }
}

String Debugger::getStateName(DebuggerState state) { // static
#define CASESTR(s) case DEBUGGER_##s: return _T(#s)
  switch(state) {
  CASESTR(CREATED   );
  CASESTR(RUNNING   );
  CASESTR(PAUSED    );
  CASESTR(TERMINATED);
  default: return format(_T("Unknown debuggerState:%d"), state);
  }
#undef CASESTR
}

void Debugger::validateRegexTypeAndPhase(RegexType expectedType, RegexPhaseType expectedPhase) const {
  if(m_regex.getType() != expectedType) {
    throwException(_T("Expected regextype %s. current is %s")
                  ,regexTypeName[expectedType]
                  ,regexTypeName[m_regex.getType()]);
  }
  if((m_state != DEBUGGER_PAUSED) || (m_regexPhase != expectedPhase)) {
    throwException(_T("Cannot get %s-state at this time (debuggerState:%s)")
                  ,getPhaseName(expectedPhase).cstr()
                  ,getStateName().cstr()
                  );
  }
}
