#pragma once

class CodeFlags {
public:
  bool m_lineDirectives       : 1;
  bool m_generateBreaks       : 1;
  bool m_generateActions      : 1;
  bool m_generateLookahead    : 1;
  bool m_generateNonTerminals : 1;
  bool m_useTableCompression  : 1;
  bool m_dumpStates           : 1;
  bool m_DFAVerbose           : 1;
  bool m_skipIfEqual          : 1;
  bool m_callWizard           : 1;
  bool m_verbose              : 1;
  CodeFlags();
};
