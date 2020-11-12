#include "stdafx.h"

Options::Options() : Singleton(__TFUNCTION__) {
  m_implOutputDir        = _T(".");
  m_headerOutputDir      = m_implOutputDir;
  m_tabSize              = defaultTabSize;
  m_verboseLevel         = defaultVerboseLevel;
  m_verbose              = false;
  m_language             = CPP;
  m_lineDirectives       = true;
  m_generateBreaks       = true;
  m_generateActions      = true;
  m_generateLookahead    = false;
  m_generateNonTerminals = false;
  m_skipIfEqual          = false;
  m_callWizard           = false;
  m_useTableCompression  = true;
  m_maxRecursiveCalls    = defaultRecurseLevel;
}

void Options::checkTemplateExist(const String &defaultTemplateName) {
  if(m_templateName.length() == 0) { // template not specified in argv
    m_templateName = searchenv(defaultTemplateName, _T("LIB"));
    if(m_templateName.length() == 0) {
      throwException(_T("Template <%s> not found in environment LIB-path\n"), defaultTemplateName.cstr());
    }
  } else { // -mS options used. Check if templatefile S exist
    if(ACCESS(m_templateName, 0) < 0) {
      throwException(_T("Template <%s> not found"), m_templateName.cstr());
    }
  }
}

void verbose(int level, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  if(level <= Options::getInstance().m_verboseLevel) {
    va_list argptr;
    va_start(argptr, format);
    _vtprintf(format, argptr);
    va_end(argptr);
  }
}
