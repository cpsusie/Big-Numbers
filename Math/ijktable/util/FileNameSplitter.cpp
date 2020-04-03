#include "stdafx.h"
#include "FileNameSplitter.h"

DirComponents::DirComponents(const string &dir)
: m_absolutePath(dir.length() && dir[0] == '\\')
{
  for(Tokenizer tok(dir, string("\\")); tok.hasNext();) {
    push_back(tok.next());
  }
}

string DirComponents::toString() const {
  string result = m_absolutePath ? "\\" : EMPTYSTRING;
  for(size_t i = 0; i < size(); i++) {
    result = FileNameSplitter::getChildName(result,(*this)[i]);
  }
  return result;
}

FileNameSplitter::FileNameSplitter(const string &fullPath) {
  _splitpath(fullPath.c_str(), m_drive, m_dir, m_fname, m_ext);
}

FileNameSplitter &FileNameSplitter::setDrive(const char *drive) {
  strncpy(m_drive,drive,ARRAYSIZE(m_drive));
  LASTVALUE(m_drive) = 0;
  return *this;
}

FileNameSplitter &FileNameSplitter::setDrive(const string &drive) {
  return setDrive(drive.c_str());
}

FileNameSplitter &FileNameSplitter::setDir(const char *dir) {
  strncpy(m_dir,dir,ARRAYSIZE(m_dir));
  LASTVALUE(m_dir) = 0;
  return *this;
}

FileNameSplitter &FileNameSplitter::setDir(const string &dir) {
  return setDir(dir.c_str());
}

FileNameSplitter &FileNameSplitter::setFileName(const char *fileName ) {
  strncpy(m_fname,fileName,ARRAYSIZE(m_fname));
  LASTVALUE(m_fname) = 0;
  return *this;
}

FileNameSplitter &FileNameSplitter::setFileName(const string &fileName ) {
  return setFileName(fileName.c_str());
}

FileNameSplitter &FileNameSplitter::setExtension(const char *extension) {
  strncpy(m_ext,extension,ARRAYSIZE(m_ext));
  LASTVALUE(m_ext) = 0;
  return *this;
}

FileNameSplitter &FileNameSplitter::setExtension(const string &extension) {
  return setExtension(extension.c_str());
}

string FileNameSplitter::getFullPath() const {
  char tmp[_MAX_PATH];
  _makepath(tmp, m_drive, m_dir, m_fname, m_ext);
  return tmp;
}

string FileNameSplitter::getAbsolutePath() const {
  char tmp[_MAX_PATH];
  _fullpath(tmp,getFullPath().c_str(),ARRAYSIZE(tmp));
  return tmp;
}

char *FileNameSplitter::getChildName(char *dst, const char *dir, const char *fileName) { // static
  int l = (int)strlen(dir);
  string tmp;
  if(l == 0 || dir[l-1] == '\\') {
    tmp = format("%s%s", dir, fileName);
  } else {
    tmp = format("%s\\%s", dir, fileName);
  }
  return strncpy(dst, tmp.c_str(), _MAX_PATH);
}

string FileNameSplitter::getChildName(const string &dir, const string &fileName) { // static
  char tmp[_MAX_PATH];
  return getChildName(tmp, dir.c_str(), fileName.c_str());
}
