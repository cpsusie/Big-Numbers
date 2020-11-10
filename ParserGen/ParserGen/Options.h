#pragma once

#include <Singleton.h>

typedef enum {
  CPP
 ,JAVA
} Language;


class Options : public Singleton {
private:
  Options();
public:
  String   m_templateName;
  String   m_implOutputDir;
  String   m_headerOutputDir;
  String   m_wizardName;
  String   m_first1File;
  String   m_nameSpace;
  BYTE     m_tabSize;
  BYTE     m_verboseLevel;
  bool     m_verbose              : 1;
  Language m_language             : 2;
  bool     m_lineDirectives       : 1;
  bool     m_generateBreaks       : 1;
  bool     m_generateActions      : 1;
  bool     m_generateLookahead    : 1;
  bool     m_generateNonTerminals : 1;
  bool     m_skipIfEqual          : 1;
  bool     m_callWizard           : 1;
  bool     m_useTableCompression  : 1;
  UINT     m_maxRecursiveCalls    : 4;
  DEFINESINGLETON(Options)

  static constexpr UINT maxTabSize          = 16;
  static constexpr UINT defaultTabSize      = 4;
  static constexpr UINT maxVerboseLevel     = 2;
  static constexpr UINT defaultVerboseLevel = 1;
  static constexpr UINT maxRecursiveCalls   = 8;
  static constexpr UINT defaultRecurseLevel = 2;
  // 
  void checkTemplateExist(const String &defaultTemplateName);
};

void verbose(int level, _In_z_ _Printf_format_string_ TCHAR const *const format, ...);
