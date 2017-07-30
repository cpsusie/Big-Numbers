#include "pch.h"
#include <MarginFile.h>

MarginFile::MarginFile(const String &name) {
  init();
  if(name.length() > 0) {
    open(name);
  }
}

void MarginFile::open(const String &name) {
  if(name == _T("stdout")) {
    open(stdout,name,name,false);
  } else if(name == _T("stderr")) {
    open(stderr,name,name,false);
  } else {
    open(MKFOPEN(name,_T("w")),name, FileNameSplitter(name).getAbsolutePath(), true);
  }
}

void MarginFile::open(FILE *file, const String &name, const String &absolutName, bool openedByMe) {
  if (isOpen()) {
    throwException(_T("%s:(%s) - Already open"), __TFUNCTION__, name.cstr());
  }
  m_file             = file;
  m_name             = name;
  m_absolutName      = absolutName;
  m_openedByMe       = openedByMe;
  m_trimRight        = false;
  m_lineNumber       = 1;
  m_formatBufferSize = 0x4000;
  m_formatBuffer     = new TCHAR[m_formatBufferSize];
}

void  MarginFile::init() {
  m_name             = EMPTYSTRING;
  m_absolutName      = EMPTYSTRING;
  m_file             = NULL;
  m_openedByMe       = false;
  m_trimRight        = false;
  m_lineNumber       = 1;
  m_currentLine      = EMPTYSTRING;
  m_leftFiller       = EMPTYSTRING;
  m_formatBuffer     = NULL;
  m_formatBufferSize = 0;
}

MarginFile::~MarginFile() {
  close();
}

void MarginFile::close() {
  if(!isOpen()) {
    return;
  }

  if(m_currentLine.length() > 0) {
    flushLine();
  }
  if(m_file && m_openedByMe) {
    fclose(m_file);
  }
  delete[] m_formatBuffer;
  init();
}

void MarginFile::indent() {
  m_currentLine = m_leftFiller;
}

void MarginFile::flushLine() {
  if(m_trimRight) {
    m_currentLine.trimRight();
  }
  if(m_currentLine.length()) {
    _fputts(m_currentLine.cstr(), m_file);
  }
  _fputtc(_T('\n'), m_file);
  m_lineNumber++;
  m_currentLine = EMPTYSTRING;
}

void MarginFile::putch(TCHAR ch) {
  if(m_file == NULL) return;
  if(m_currentLine.length() == 0) indent();
  switch(ch) {
  case _T('\n'):
  case _T('\r'):
    flushLine();
    break;
  default:
    m_currentLine += ch;
    break;
  }
}

void MarginFile::puts(const TCHAR *s) {
  if(m_file == NULL) return;
  for(;*s; s++) {
    putch(*s);
  }
}

void MarginFile::vprintf(const TCHAR *format, va_list argptr) {
  for(;;) {
    if(_vsntprintf(m_formatBuffer, m_formatBufferSize, format, argptr) >= 0) {
      break;
    }
    delete m_formatBuffer;
    m_formatBufferSize <<= 1;
    m_formatBuffer = new TCHAR[m_formatBufferSize];
  }
  puts(m_formatBuffer);
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
