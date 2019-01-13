#include "StdAfx.h"
#include "FileInfo.h"

FileInfo::FileInfo(const String &name) {
  m_name = name;
  m_mode = STAT(name).st_mode;
  getFileTimes();
}

const TCHAR *FileInfo::s_skillLine1   = _T("/* ####----####+-+-+-+-+" );
const TCHAR *FileInfo::s_skillLine2   = _T("+-+-+-+-+####-+-+-+-+-+"  );
const TCHAR *FileInfo::s_skillLine3   = _T("+-+-+-+-+####----#### */" );
const size_t FileInfo::s_skillLength1 = _tcslen(FileInfo::s_skillLine1);
const size_t FileInfo::s_skillLength2 = _tcslen(FileInfo::s_skillLine2);

String FileInfo::toString() const {
  return format(_T("%s%s%s<%s><%s><%s><%04X>%s")
               ,s_skillLine1
               ,m_name.cstr()
               ,s_skillLine2
               ,m_ctime.toString(ddMMyyyyhhmmssSSS).cstr()
               ,m_atime.toString(ddMMyyyyhhmmssSSS).cstr()
               ,m_mtime.toString(ddMMyyyyhhmmssSSS).cstr()
               ,m_mode
               ,s_skillLine3);
}

void FileInfo::extractFileInfo(const String &line, size_t lineNumber) {
  String copy(line);

  TCHAR *sp1 = copy.cstr() + s_skillLength1;
  TCHAR *sp2;
  if((sp2 = _tcsstr(sp1, s_skillLine2)) == NULL) {
    libError(line, lineNumber);
  } else {
    const TCHAR ch = *sp2;
    *sp2 = _T('\0');
    m_name = sp1;
    *sp2 = ch;
  }
  sp2 += s_skillLength2;
  Tokenizer tok(sp2, _T("<>"));
  if (!tok.hasNext()) {
    libError(line, lineNumber);
  }
  m_ctime = Timestamp(tok.next());
  if(!tok.hasNext()) {
    libError(line, lineNumber);
  }
  m_atime = Timestamp(tok.next());
  if(!tok.hasNext()) {
    libError(line, lineNumber);
  }
  m_mtime = Timestamp(tok.next());
  if(!tok.hasNext()) {
    libError(line, lineNumber);
  }
  const String modeStr = tok.next();
  if(_stscanf(modeStr.cstr(), _T("%hX"), &m_mode) != 1) {
    libError(line, lineNumber);
  }
}

void FileInfo::setMode() const {
  CHMOD(m_name, m_mode);
}

static FILETIME fromLocal(FILETIME lft) {
  FILETIME ft;
  LocalFileTimeToFileTime(&lft, &ft);
  return ft;
}

static FILETIME toLocal(FILETIME ft) {
  FILETIME lft;
  FileTimeToLocalFileTime(&ft, &lft);
  return lft;
}

void FileInfo::setFileTimes() const {
  HANDLE f = INVALID_HANDLE_VALUE;
  try {
    f = CreateFile(m_name.cstr()
                  ,FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(f == INVALID_HANDLE_VALUE) {
      throwLastErrorOnSysCallNameException(m_name);
    }
    const FILETIME ctime = fromLocal(m_ctime), atime = fromLocal(m_atime), mtime = fromLocal(m_mtime);
    if(!SetFileTime(f, &ctime, &atime, &mtime)) {
      throwLastErrorOnSysCallNameException(m_name);
    }
    CloseHandle(f);
  } catch(...) {
    if(f != INVALID_HANDLE_VALUE) {
      CloseHandle(f);
    }
    throw;
  }
}

void FileInfo::getFileTimes() {
  HANDLE f = INVALID_HANDLE_VALUE;
  try {
    f = CreateFile(m_name.cstr()
                  ,FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(f == INVALID_HANDLE_VALUE) {
      throwLastErrorOnSysCallNameException(m_name);
    }
    FILETIME ctime, atime, mtime;
    if(!GetFileTime(f, &ctime, &atime, &mtime)) {
      throwLastErrorOnSysCallNameException(m_name);
    }
    m_ctime = toLocal(ctime);
    m_atime = toLocal(atime);
    m_mtime = toLocal(mtime);
    CloseHandle(f);
  } catch(...) {
    if(f != INVALID_HANDLE_VALUE) {
      CloseHandle(f);
    }
    throw;
  }
}
