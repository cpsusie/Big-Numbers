#include "pch.h"
#include <D3DGraphics/D3AbstractTextureFactory.h>

static const TCHAR *s_loadFileDialogExtensions = _T("Picture files\0*.bmp;*.jpg;*.png;*.tiff;*.gif;*.ico;*.cur;*.dib;\0"
                                                    "BMP-Files (*.bmp)\0*.bmp;\0"
                                                    "JPG-files (*.jpg)\0*.jpg;\0"
                                                    "PNG-files (*.png)\0*.png;\0"
                                                    "TIFF-files (*.tiff)\0*.tiff;\0"
                                                    "GIF-files (*.gif)\0*.gif;\0"
                                                    "ICO-files (*.ico)\0*.ico;\0"
                                                    "CUR-files (*.cur)\0*.cur;\0"
                                                    "DIB-files (*.dib)\0*.dib;\0"
                                                    "All files (*.*)\0*.*\0\0");

String selectAndValidateTextureFile(AbstractTextureFactory &atf) {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = s_loadFileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open file");
  dlg.m_ofn.Flags      |= OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLESIZING;

  if(dlg.DoModal() == IDOK) {
    const String fileName = dlg.m_ofn.lpstrFile;
    if(!atf.validateTextureFile(fileName)) {
      return EMPTYSTRING;
    }
    return fileName;
  }
  return EMPTYSTRING;
}
