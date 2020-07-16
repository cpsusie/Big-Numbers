#include "pch.h"
#include <D3DGraphics/Profile2D.h>

const TCHAR *profileFileExtensions = _T("Profile-files (*.prf)\0*.prf\0All files (*.*)\0*.*\0\0");

Profile2D *selectAndLoadProfile() {
#if !defined(__TODO__)
  showWarning(_T("%s:Function not implemented"), __TFUNCTION__);
  return NULL;
#else
  FILE *file = NULL;
  Profile *result = NULL;
  try {
    CFileDialog dlg(TRUE);
    dlg.m_ofn.lpstrFilter = profileFileExtensions;
    dlg.m_ofn.lpstrTitle = _T("Open Profile");
    if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
      return NULL;
    } else {
      file = FOPEN(dlg.m_ofn.lpstrFile,_T("r"));
      result = new Profile(readTextFile(file), dlg.m_ofn.lpstrFile); TRACE_NEW(result);
    }
  } catch(Exception e) {
    showException(e);
  }
  if(file != NULL) {
    fclose(file);
  }
  return result;
#endif
}
