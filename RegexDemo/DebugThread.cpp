#include "stdafx.h"
#include "DebugThread.h"

DebugThread::DebugThread(DebugRegex &regex, const CompileParameters &cp, const BitSet &breakPoints)
: m_command(COMMAND_COMPILE)
, m_regex(regex)
, m_compileParameters(cp)
, m_breakPoints(breakPoints)
{
  clearStates();
  m_running     = m_finished = m_killed = false;
  m_singleStep  = true;
  m_regexPhase  = REGEX_UNDEFINED;
}

DebugThread::DebugThread(ThreadCommand command, DebugRegex &regex, const String &text, const BitSet &breakPoints)
: m_command(command)
, m_regex(regex)
, m_text(text)
, m_breakPoints(breakPoints)
{
  clearStates();
  m_running     = m_finished = m_killed = false;
  m_singleStep  = m_command == COMMAND_COMPILE;
  if(m_command == COMMAND_COMPILE) {
    m_compileParameters = text;
  }
  m_regexPhase  = REGEX_UNDEFINED;
}

void DebugThread::clearStates() {
  m_handlerState = NULL;
}

DebugThread::~DebugThread() {
  enableHandleStep(false);
  kill();
  for(int i = 0; i < 10; i++) {
    if(!stillActive()) {
      return;
    }
    Sleep(50);
  }
  if(stillActive()) {
    AfxMessageBox(_T("DebugThread still active in destructor"));
  }
}

void DebugThread::singleStep() {
  if(isFinished()) {
    throwException(_T("singleStep:Thread has exited"));
  }

  m_singleStep = true;
  enableHandleStep(true);
  resume();
}

void DebugThread::go() {
  if(isFinished()) {
    throwException(_T("singleStep:Thread has exited"));
  }
  m_singleStep = false;
  enableHandleStep(!m_breakPoints.isEmpty());
  resume();
}

void DebugThread::kill() {
  if(!isFinished()) {
    m_killed = true;
    if(!isRunning()) {
      resume();
    }
  }
}

UINT DebugThread::run() {
  setPropRunning(true);
  m_foundStart   = -1;
  m_resultLength = 0;
  try {
    switch(m_command) {
    case COMMAND_COMPILE:
      m_regex.compilePattern(m_compileParameters);
      m_resultMsg  = _T("Pattern Ok");
      m_regexPhase = REGEX_SUCEEDED;
      break;
    case COMMAND_SEARCHFORWARD:
    case COMMAND_SEARCHBACKWRD:
      m_foundStart = m_regex.search(m_text, m_command == COMMAND_SEARCHFORWARD);
      if(m_foundStart >= 0) {
        m_resultLength = m_regex.getResultLength();
        m_resultMsg  = format(_T("Found at %d. length:%d"), m_foundStart, m_resultLength);
        m_regexPhase = REGEX_SUCEEDED;
      } else {
        m_resultMsg  = _T("Not found");
      }
      break;
    case COMMAND_MATCH  :
      if(m_regex.match(m_text)) {
        m_foundStart   = 0;
        m_resultLength = (int)m_text.length();
        m_resultMsg    = _T("Match");
        m_regexPhase   = REGEX_SUCEEDED;
      } else {
        m_resultMsg    = _T("No match");
      }
      break;
    }
    setPropFinished();
  } catch(Exception e) {
    m_resultMsg  = e.what();
    m_regexPhase = REGEX_UNDEFINED;
  } catch(...) {
    m_resultMsg  = _T("Unknown exception");
    m_regexPhase = REGEX_UNDEFINED;
  }

  setPropRunning(false);
  return 0;
}

void DebugThread::enableHandleStep(bool enabled) {
  m_regex.setHandler(enabled ? this : NULL);
}

void DebugThread::handleCompileStep(const _RegexCompilerState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_COMPILING);
}

void DebugThread::handleSearchStep(const _RegexSearchState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_SEARCHING);
}

void DebugThread::handleMatchStep(const _RegexMatchState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_MATCHING, state.getDBGLineNumber());
}

void DebugThread::handleCompileStep(const _DFARegexCompilerState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_COMPILING);
}

void DebugThread::handleSearchStep(const _DFARegexSearchState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_SEARCHING);
}

void DebugThread::handleMatchStep(const _DFARegexMatchState &state) {
  m_handlerState = &state;
  suspendOnSingleStep(REGEX_MATCHING, state.getDBGLineNumber());
}

void DebugThread::suspendOnSingleStep(RegexPhaseType phase, int lineNumber) {
  if(m_singleStep || ((lineNumber>=0) && m_breakPoints.contains(lineNumber))) {
    m_regexPhase  = phase;
    setPropRunning(false);
    suspend();
    m_regexPhase  = REGEX_UNDEFINED;
    setPropRunning(true);
  }
  clearStates();
  if(m_killed) {
    throwException(_T("Killed"));
  }
}

void DebugThread::getFoundPosition(int &start, int &end) {
  if(getRegexPhase() != REGEX_SUCEEDED) {
    start = end = -1;
  } else {
    start = (int)m_foundStart;
    end   = (int)(start + m_resultLength);
  }
}

const String DebugThread::getResultMsg() const {
  if(isRunning()) {
    return _T("No result yet. Thread not finished");
  }
  return m_resultMsg;
}

String DebugThread::registersToString() const {
  if(m_regexPhase != REGEX_SUCEEDED) {
    throwException(_T("Cannot get register at this time of the search"));
  }
  return m_regex.registersToString();
}

const _RegexCompilerState &DebugThread::getEmacsCompilerState() const {
  validateRegexTypeAndPhase(EMACS_REGEX, REGEX_COMPILING);
  return *(_RegexCompilerState*)m_handlerState;
}

const _RegexSearchState &DebugThread::getEmacsSearchState() const {
  validateRegexTypeAndPhase(EMACS_REGEX, REGEX_SEARCHING);
  return *(_RegexSearchState*)m_handlerState;
}

const _RegexMatchState &DebugThread::getEmacsMatchState() const {
  validateRegexTypeAndPhase(EMACS_REGEX, REGEX_MATCHING);
  return *(_RegexMatchState*)m_handlerState;
}

const _DFARegexCompilerState &DebugThread::getDFACompilerState() const {
  validateRegexTypeAndPhase(DFA_REGEX, REGEX_COMPILING);
  return *(_DFARegexCompilerState*)m_handlerState;
}

const _DFARegexSearchState &DebugThread::getDFASearchState() const {
  validateRegexTypeAndPhase(DFA_REGEX, REGEX_SEARCHING);
  return *(_DFARegexSearchState*)m_handlerState;
}

const _DFARegexMatchState &DebugThread::getDFAMatchState() const {
  validateRegexTypeAndPhase(DFA_REGEX, REGEX_MATCHING);
  return *(_DFARegexMatchState*)m_handlerState;
}

void DebugThread::setPropRunning(bool value) {
  const bool oldValue = m_running;
  m_running = value;
  if(m_running != oldValue) {
    notifyPropertyChanged(SEARCH_RUNNING, &oldValue, &m_running);
  }
}

void DebugThread::setPropFinished() {
  const bool oldValue = m_finished;
  m_finished = true;
  if(m_finished != oldValue) {
    notifyPropertyChanged(SEARCH_REGEXFINISHED, &oldValue, &m_finished);
  }
}

static const TCHAR *regexTypeName[] = {
  _T("EMACS_REGEX")
 ,_T("DFA_REGEX")
};

static const TCHAR *getPhaseName(RegexPhaseType phase) {
  switch(phase) {
  case REGEX_COMPILING: return _T("compiler");
  case REGEX_SEARCHING: return _T("search");
  case REGEX_MATCHING : return _T("match");
  default: return _T("unknown state");
  }
}

void DebugThread::validateRegexTypeAndPhase(RegexType expectedType, RegexPhaseType expectedPhase) const {
  if(m_regex.getType() != expectedType) {
    throwException(_T("Expected regextype %s. current is %s")
                  ,regexTypeName[expectedType]
                  ,regexTypeName[m_regex.getType()]);
  }
  if(m_running || m_finished || (m_regexPhase != expectedPhase)) {
    throwException(_T("Cannot get %s-state at this time"), getPhaseName(expectedPhase));
  }
}
