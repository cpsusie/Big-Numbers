#include "pch.h"
#include <D3DGraphics/Profile2D.h>

const TCHAR *Profile2D::s_profileFileExtensions = _T("Profile-files (*.prf)\0*.prf\0All files (*.*)\0*.*\0\0");

bool Profile2D::selectAndLoadProfile() {
  try {
    CFileDialog dlg(TRUE);
    dlg.m_ofn.lpstrFilter = s_profileFileExtensions;
    dlg.m_ofn.lpstrTitle = _T("Open Profile");
    if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
      return false;
    } else {
      load(dlg.m_ofn.lpstrFile);
      return true;
    }
  } catch(Exception e) {
    showException(e);
    return false;
  }
}
