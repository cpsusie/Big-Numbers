#include "pch.h"
#include <FileNameSplitter.h>

DirComponents::DirComponents(const String &dir)
  : m_absolutePath(dir.length() && dir[0] == '\\')
  , StringArray(Tokenizer(dir, _T("\\")))
{
}

String DirComponents::toString() const {
  String result = m_absolutePath ? _T("\\") : EMPTYSTRING;
  for(size_t i = 0; i < size(); i++) {
    result = FileNameSplitter::getChildName(result,(*this)[i]);
  }
  return result;
}

FileNameSplitter::FileNameSplitter(const String &fullPath) {
  _tsplitpath(fullPath.cstr(), m_drive, m_dir, m_fname, m_ext);
}

FileNameSplitter &FileNameSplitter::setDrive(const TCHAR *drive) {
  _tcsncpy(m_drive,drive,ARRAYSIZE(m_drive));
  LASTVALUE(m_drive) = 0;
  return *this;
}

FileNameSplitter &FileNameSplitter::setDrive(const String &drive) {
  return setDrive(drive.cstr());
}

FileNameSplitter &FileNameSplitter::setDir(const TCHAR *dir) {
  _tcsncpy(m_dir,dir,ARRAYSIZE(m_dir));
  LASTVALUE(m_dir) = 0;
  return *this;
}

FileNameSplitter &FileNameSplitter::setDir(const String &dir) {
  return setDir(dir.cstr());
}

FileNameSplitter &FileNameSplitter::setFileName(const TCHAR *fileName ) {
  _tcsncpy(m_fname,fileName,ARRAYSIZE(m_fname));
  LASTVALUE(m_fname) = 0;
  return *this;
}

FileNameSplitter &FileNameSplitter::setFileName(const String &fileName ) {
  return setFileName(fileName.cstr());
}

FileNameSplitter &FileNameSplitter::tempFileName(const String &prefix) {
  TCHAR *oldtmp = _tgetenv(_T("TMP"));
  // Unset TMP environment variable, then create a temporary filename in C:\tmp.
  if(oldtmp != NULL) {
    int err = _tputenv_s(_T("TMP"), EMPTYSTRING);
  }
  try {
    FileNameSplitter tmp     = *this;
    String           dir     = tmp.setFileName(EMPTYSTRING).setExtension(EMPTYSTRING).getAbsolutePath();
    TCHAR           *newName = _ttempnam(dir.cstr(), prefix.cstr());
    if(newName == NULL) {
      throwInvalidArgumentException(__TFUNCTION__, _T("_ttempnam failed"));
    }
    *this = FileNameSplitter(newName);
    free(newName);
    if(oldtmp != NULL) {
      int err = _tputenv_s(_T("TMP"), oldtmp);
    }
  } catch(...) {
    if(oldtmp != NULL) {
      int err = _tputenv_s(_T("TMP"), oldtmp);
    }
    throw;
  }
  return *this;
}

FileNameSplitter &FileNameSplitter::setExtension(const TCHAR *extension) {
  _tcsncpy(m_ext,extension,ARRAYSIZE(m_ext));
  LASTVALUE(m_ext) = 0;
  return *this;
}

FileNameSplitter &FileNameSplitter::setExtension(const String &extension) {
  return setExtension(extension.cstr());
}

String FileNameSplitter::getFullPath() const {
  TCHAR tmp[_MAX_PATH];
  _tmakepath(tmp, m_drive, m_dir, m_fname, m_ext);
  return tmp;
}

String FileNameSplitter::getAbsolutePath() const {
  TCHAR tmp[_MAX_PATH];
  _tfullpath(tmp,getFullPath().cstr(),ARRAYSIZE(tmp));
  return tmp;
}

TCHAR *FileNameSplitter::getChildName(TCHAR *dst, const TCHAR *dir, const TCHAR *fileName) { // static
  int l = (int)_tcsclen(dir);
  String tmp;
  if(l == 0 || dir[l-1] == _T('\\')) {
    tmp = format(_T("%s%s"), dir, fileName);
  } else {
    tmp = format(_T("%s\\%s"), dir, fileName);
  }
  return _tcsncpy(dst, tmp.cstr(), _MAX_PATH);
}

String FileNameSplitter::getChildName(const String &dir, const String &fileName) { // static
  TCHAR tmp[_MAX_PATH];
  return getChildName(tmp, dir.cstr(), fileName.cstr());
}
