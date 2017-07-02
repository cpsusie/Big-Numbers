#include "stdafx.h"
#include "ProfileDlg.h"

const TCHAR *profileFileExtensions = _T("Profile-files (*.prf)\0*.prf\0All files (*.*)\0*.*\0\0");

Profile *selectAndLoadProfile() {
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
      result = new Profile(readTextFile(file), dlg.m_ofn.lpstrFile);
    }
  } catch(Exception e) {
    showException(e);
  }
  if(file != NULL) {
    fclose(file);
  }
  return result;
}
