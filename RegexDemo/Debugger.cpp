#include "stdafx.h"
#include <Thread.h>
#include "Debugger.h"

Debugger::Debugger(DebugRegex &regex, const CompileParameters &cp, const BitSet &breakPoints)
: m_command(COMMAND_COMPILE)
, m_regex(regex)
, m_compileParameters(cp)
, m_breakPoints(breakPoints)
, m_go(0)
{
  initDebugger(true);
}

Debugger::Debugger(RegexCommand command, DebugRegex &regex, const String &text, const BitSet &breakPoints)
: m_command(command)
, m_regex(regex)
, m_text(text)
, m_breakPoints(breakPoints)
, m_go(0)
{
  initDebugger(m_command == COMMAND_COMPILE);
  if(m_command == COMMAND_COMPILE) {
    m_compileParameters = text;
  }
}

void Debugger::initDebugger(bool singleStep) {
  clearStates();
  m_state       = DEBUGGER_CREATED;
  m_killRequest = false;
  m_singleStep  = singleStep;
  m_regexPhase  = REGEX_UNDEFINED;
}

void Debugger::clearStates() {
  m_handlerState = NULL;
}

Debugger::~Debugger() {
  enableHandleStep(false);
  kill();
  m_terminated.wait();
}

void Debugger::singleStep() {
  if(isTerminated()) {
    throwException(_T("%s:Debugger has exited"),__TFUNCTION__);
  }
  m_singleStep = true;
  enableHandleStep(true);
  resume();
}

void Debugger::go() {
  if(isTerminated()) {
    throwException(_T("%s:Debugger has exited"),__TFUNCTION__);
  }
  m_singleStep = false;
  enableHandleStep(!m_breakPoints.isEmpty());
  resume();
}

void Debugger::kill() {
  if(!isTerminated()) {
    m_killRequest = true;
    if(!isRunning()) {
      resume();
    }
  }
}

UINT Debugger::run() {
  m_terminated.wait();
  setThreadDescription("Debugger");
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_RUNNING);
  m_foundStart   = -1;
  m_resultLength = 0;
  try {
    suspend();
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
  } catch(Exception e) {
    m_resultMsg  = e.what();
    m_regexPhase = REGEX_UNDEFINED;
  } catch(...) {
    m_resultMsg  = _T("Unknown exception");
    m_regexPhase = REGEX_UNDEFINED;
  }
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_TERMINATED);
  m_terminated.notify();
  return 0;
}

void Debugger::enableHandleStep(bool enabled) {
  m_regex.setHandler(enabled ? this : NULL);
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
  if(m_singleStep || ((lineNumber>=0) && m_breakPoints.contains(lineNumber))) {
    m_regexPhase  = phase;
    suspend();
  }
  clearStates();
  if(m_killRequest) {
    throwException(_T("Killed"));
  }
}

void Debugger::suspend() {
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_PAUSED);
  m_go.wait();
  m_regexPhase = REGEX_UNDEFINED;
  setProperty(DEBUGGER_STATE, m_state, DEBUGGER_RUNNING);
}

void Debugger::resume() {
  m_go.notify();
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
  if(isRunning()) {
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

String Debugger::getDebuggerStateName(DebuggerState state) { // static
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
                  ,getDebuggerStateName().cstr()
                  );
  }
}
