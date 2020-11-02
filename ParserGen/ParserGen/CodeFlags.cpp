#include "stdafx.h"
#include "CodeFlags.h"

CodeFlags::CodeFlags() {
  m_lineDirectives       = true;
  m_generateBreaks       = true;
  m_generateActions      = true;
  m_generateLookahead    = false;
  m_generateNonTerminals = false;
  m_useTableCompression  = true;
  m_dumpStates           = false;
  m_DFAVerbose           = false;
  m_skipIfEqual          = false;
  m_callWizard           = false;
  m_verbose              = false;
}
