#include "stdafx.h"
#include "DebugThread.h"

void DebugRegex::compilePattern(const CompileParameters &cp) {
  switch(getType()) {
  case EMACS_REGEX:
    m_lastCompiledParameters[m_type].reset();
    m_regex.compilePattern(cp.m_pattern, cp.getTranslateTable());
    break;
  case DFA_REGEX  :
    m_lastCompiledParameters[m_type].reset();
    m_DFARegex.compilePattern(cp.m_pattern, cp.getTranslateTable());
    break;
  default         : throwUnknownTypeException();
  }
  m_lastCompiledParameters[m_type] = cp;
}

bool DebugRegex::match(const String &text) {
  m_text = text;
  switch(getType()) {
  case EMACS_REGEX: return m_regex.match(text, &m_registers);
  case DFA_REGEX  : return m_DFARegex.match(text);
  default         : throwUnknownTypeException();
  }
  return false;
}

intptr_t DebugRegex::search(const String &text, bool forward) {
  m_text = text;
  switch(getType()) {
  case EMACS_REGEX: return m_regex.search(text, forward, -1, &m_registers);
  case DFA_REGEX  : return m_DFARegex.search(text, forward);
  default         : throwUnknownTypeException();
  }
  return -1;
}

String DebugRegex::registersToString() const {
  switch(getType()) {
  case EMACS_REGEX: return m_registers.toString(m_text);
  case DFA_REGEX  : return EMPTYSTRING;
  default         : throwUnknownTypeException();
  }
  return EMPTYSTRING;
}

String DebugRegex::codeToString() const {
  switch(getType()) {
  case EMACS_REGEX: return m_regex.toString();
  case DFA_REGEX  : return m_DFARegex.toString();
  default         : throwUnknownTypeException();
  }
  return EMPTYSTRING;
}

String DebugRegex::fastMapToString() const {
  switch(getType()) {
  case EMACS_REGEX: return m_regex.fastMapToString();
  case DFA_REGEX  : return m_DFARegex.fastMapToString();
  default         : throwUnknownTypeException();
  }
  return EMPTYSTRING;
}

String DebugRegex::getDFATablesToString() const {
  if(!hasDFATables()) {
    return EMPTYSTRING;
  }
  return m_DFARegex.tableToString();
}

bool DebugRegex::hasDFATables() const {
  switch(getType()) {
  case DFA_REGEX  :
    return m_DFARegex.isCompiled();
  default:
    return false;
  }
}

bool DebugRegex::getMatchEmpty() const {
  switch(getType()) {
  case EMACS_REGEX: return m_regex.getMatchEmpty();
  case DFA_REGEX  : return m_DFARegex.getMatchEmpty();
  default         : throwUnknownTypeException();
  }
  return false;
}

void DebugRegex::setType(RegexType type) {
  m_type = type;
}

bool DebugRegex::isCodeDirty() const {
  switch(getType()) {
  case EMACS_REGEX: return m_regex.isCodeDirty();
  case DFA_REGEX  : return m_DFARegex.isCodeDirty();
  default         : throwUnknownTypeException();
  }
  return false;
}

bool DebugRegex::isCompiled() const {
  switch(getType()) {
  case EMACS_REGEX: return m_regex.isCompiled();
  case DFA_REGEX  : return m_DFARegex.isCompiled();
  default         : throwUnknownTypeException();
  }
  return false;
}

BitSet DebugRegex::getPossibleBreakPointLines() const {
  switch(getType()) {
  case EMACS_REGEX: return m_regex.getPossibleBreakPointLines();
  case DFA_REGEX  : return m_DFARegex.getPossibleBreakPointLines();
  default         : throwUnknownTypeException();
  }
  return BitSet(1);
}

int DebugRegex::getPatternFoundCodeLine() const {
  switch(getType()) {
  case EMACS_REGEX: return m_regex.getLastCodeLine();
  case DFA_REGEX  : return m_DFARegex.getCurrentCodeLine();
  default         : throwUnknownTypeException();
  }
  return -1;
}

int DebugRegex::getCycleCount() const {
  switch(getType()) {
  case EMACS_REGEX: return m_regex.getCycleCount();
  case DFA_REGEX  : return m_DFARegex.getCycleCount();
  default         : throwUnknownTypeException();
  }
  return 0;
}

void DebugRegex::paint(CWnd *wnd, CDC &dc, bool animate) const {
  switch(getType()) {
  case EMACS_REGEX:
    break;
  case DFA_REGEX  :
    m_DFARegex.paint(wnd, dc, animate);
    break;
  default         :
    throwUnknownTypeException();
  }
}

void DebugRegex::unmarkAll(CWnd *wnd, CDC &dc) {
  switch(getType()) {
  case EMACS_REGEX:
    break;
  case DFA_REGEX  :
    m_DFARegex.unmarkAll(wnd, dc);
    break;
  default         :
    throwUnknownTypeException();
  }
}

void DebugRegex::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  switch(getType()) {
  case EMACS_REGEX:
    break;
  case DFA_REGEX  :
    { const bool blinkersVisible = *(bool*)newValue;
      m_DFARegex.setBlinkersVisible(blinkersVisible);
    }
    break;
  default         :
    throwUnknownTypeException();
  }
}

void DebugRegex::setHandler(DebugThread *handler) {
  switch(getType()) {
  case EMACS_REGEX:
    m_regex.setHandler(handler);
    break;
  case DFA_REGEX  :
    m_DFARegex.setHandler(handler);
    break;
  default         :
    throwUnknownTypeException();
  }
}

intptr_t DebugRegex::getResultLength() const {
  switch(getType()) {
  case EMACS_REGEX: return m_regex.getResultLength();
  case DFA_REGEX  : return m_DFARegex.getResultLength();
  default         : throwUnknownTypeException();
  }
  return 0;
}

CompileParameters DebugRegex::getLastCompiledPattern() const {
  switch(getType()) {
  case EMACS_REGEX:
  case DFA_REGEX  : return m_lastCompiledParameters[m_type];
  default         : throwUnknownTypeException();
  }
  return EMPTYSTRING;
}

void DebugRegex::throwUnknownTypeException() const {
  throwException(_T("Unknown RegexType:%d"), m_type);
}

