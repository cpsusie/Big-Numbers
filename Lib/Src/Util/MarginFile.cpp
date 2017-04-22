#include "pch.h"
#include <MarginFile.h>

MarginFile::MarginFile(const String &name) {
  if(name == _T("stdout")) {
    init(stdout,name,name,false);
  } else if(name == _T("stderr")) {
    init(stderr,name,name,false);
  } else {
    init(MKFOPEN(name,_T("w")),name, FileNameSplitter(name).getAbsolutePath(), true);
  }
}

void MarginFile::init(FILE *file, const String &name, const String &absolutName, bool openedByMe) {
  m_file        = file;
  m_name        = name;
  m_absolutName = absolutName;
  m_openedByMe  = openedByMe;
  m_lineNumber  = 1;
  m_leftMargin  = 0;
  m_lineStart   = true;
  m_bufSize     = 0x4000;
  m_buffer      = new TCHAR[m_bufSize];
}

MarginFile::~MarginFile() {
  close();
  delete[] m_buffer;
}

void MarginFile::close() {
  if(m_file && m_openedByMe) {
    fclose(m_file);
  }
  m_file = NULL;
}

void MarginFile::indent() {
  if(m_leftMargin > 0) {
    _ftprintf(m_file,_T("%*.*s"),m_leftMargin,m_leftMargin,EMPTYSTRING);
  }
  m_lineStart = false;
  m_currentLineLength = m_leftMargin;
}

void MarginFile::putch(int c) {
  if(m_file == NULL) {
    return;
  }
  if(m_lineStart) {
    indent();
  }
  _fputtc(c, m_file);
  m_currentLineLength++;
  switch(c) {
  case _T('\n'):
  case _T('\r'):
    m_lineStart = true;
    break;
  default:
    m_lineStart = false;
    break;
  }
}

void MarginFile::puts(const TCHAR *s) {
  if(m_file == NULL) {
    return;
  }
  for(;*s; s++) {
    putch(*s);
  }
}

static int countNewLines(const TCHAR *str) {
  int count = 0;
  for(const TCHAR *s = _tcschr(str,_T('\n')); s; s = _tcschr(s+1, _T('\n'))) {
    count++;
  }
  return count;
}

void MarginFile::vprintf(const TCHAR *format, va_list argptr) {
  for(;;) {
    if(_vsntprintf(m_buffer, m_bufSize, format, argptr) >= 0) {
      break;
    }
    delete m_buffer;
    m_bufSize <<= 1;
    m_buffer = new TCHAR[m_bufSize];
  }
  m_lineNumber += countNewLines(m_buffer);
  puts(m_buffer);
}

void MarginFile::printf(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  vprintf(format, argptr);
  va_end(argptr);
}

void fprintf(MarginFile *file, const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  file->vprintf(format,argptr);
  va_end(argptr);
}

MarginFile stdoutMarginFile(_T("stdout"));
MarginFile stderrMarginFile(_T("stderr"));
