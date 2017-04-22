#include "stdafx.h"
#include <Console.h>
#include "StringExtractor.h"

StringExtractor::StringExtractor(UINT minLength, bool showNames, bool verbose)
: m_minLength(minLength)
, m_showNames(showNames)
, m_verbose(  verbose  )
{
  m_currentName = EMPTYSTRING;
  if(verbose) {
    m_screenWidth = Console::getWindowSize(STD_ERROR_HANDLE).X - 1;
  }
  m_outputAtTTY = isatty(stdout);
}

void StringExtractor::handleFileName(const TCHAR *name, DirListEntry &info) {
  try {
    m_currentName = name;
    verbose(name);
    extractStrings(name);
    verbose(EMPTYSTRING);
  } catch(Exception e) {
    _ftprintf(stderr, _T("strings:%s\n"), e.what());
  }
}

void StringExtractor::printString(const char *str) {
  if(m_outputAtTTY && m_verbose) {
    verbose(EMPTYSTRING);
  }
  if(m_showNames) {
    _tprintf(_T("%s:"), m_currentName);
  }
  printf("%s\n", str);
}

void StringExtractor::verbose(const TCHAR *str) {
  if(m_verbose) {
    _ftprintf(stderr, _T("%-*.*s\r"), m_screenWidth, m_screenWidth, str);
  }
}
