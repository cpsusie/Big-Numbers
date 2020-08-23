#include "pch.h"
#include <D3DGraphics/Profile2D.h>

const TCHAR *Profile2D::s_profileFileExtensions = _T("Profile-files (*.prf)\0*.prf\0All files (*.*)\0*.*\0\0");

String selectAndValidateProfileFile() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = Profile2D::s_profileFileExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open file");
  dlg.m_ofn.Flags      |= OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLESIZING;

  if(dlg.DoModal() == IDOK) {
    const String fileName = dlg.m_ofn.lpstrFile;
    try {
      Profile2D profile;
      profile.load(fileName);
      return fileName;
    } catch(Exception e) {
      showException(e);
    }
  }
  return EMPTYSTRING;
}
