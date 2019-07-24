#include "pch.h"
#include <FileNameSplitter.h>
#include <MFCUtil/SelectDirDlg.h>

static StringArray getFileNames(const TCHAR *fileNames) {
  StringArray result;
  const TCHAR *dir = fileNames;
  if(*dir == 0) {
    return result;
  }
  UINT addCount = 0;
  for(const TCHAR *cp = dir + _tcslen(dir) + 1; *cp; cp += _tcslen(cp) + 1) {
    result.add(FileNameSplitter::getChildName(dir, cp));
    addCount++;
  }
  if(addCount == 0) {
    result.add(dir);
  }
  return result;
}

StringArray selectMultipleFileNames(const String      &title
                                   ,const TCHAR       *filter
                                   ,UINT               defaultExtensionIndex
                                   ,UINT              *selectedExtensionIndex
                                   ,const String      &initialDir
                                   ,DWORD              flags
                                   )
{
  TCHAR *nameBuffer = NULL;
  try {
    CFileDialog dlg(TRUE);
    dlg.m_ofn.lpstrTitle   = title.cstr();
    dlg.m_ofn.lpstrFilter  = filter;
    dlg.m_ofn.nFilterIndex = defaultExtensionIndex;
    dlg.m_ofn.Flags       |= flags;
  #define NAMEBUFFERLENGTH 500000
    nameBuffer             = new TCHAR[NAMEBUFFERLENGTH]; TRACE_NEW(nameBuffer);
    nameBuffer[0]          = 0;
    dlg.m_ofn.lpstrFile    = nameBuffer;
    dlg.m_ofn.nMaxFile     = NAMEBUFFERLENGTH;
    if(initialDir.length() > 0) {
      dlg.m_ofn.lpstrInitialDir = initialDir.cstr();
    }

    StringArray result;
    if((dlg.DoModal() == IDOK) && (_tcsclen(nameBuffer) != 0)) {
      result = getFileNames(nameBuffer);
      if(selectedExtensionIndex != NULL) {
        *selectedExtensionIndex = dlg.m_ofn.nFilterIndex;
      }
    }
    SAFEDELETEARRAY(nameBuffer);
    return result;
  } catch(...) {
    SAFEDELETEARRAY(nameBuffer);
    throw;
  }
}
