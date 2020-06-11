#include "pch.h"
#include <MyUtil.h>
#include <FileNameSplitter.h>
#include <MarginFile.h>

MarginFile::MarginFile(const String &name) {
  init();
  if(name.length() > 0) {
    open(name);
  }
}

MarginFile::MarginFile(tostream &output, const String &name) {
  init();
  setOutput(output, name, FileNameSplitter(name).getAbsolutePath(), false);
}

void MarginFile::open(const String &name) {
  if(isOpen()) {
    throwException(_T("%s:(%s) - Already open"), __TFUNCTION__, name.cstr());
  }
  if(name == _T("stdout")) {
    setOutput(tcout,name,name,false);
  } else if(name == _T("stderr")) {
    setOutput(tcerr,name,name,false);
  } else {
    FILE *tmp = MKFOPEN(name, _T("w")); // throws exception
    fclose(tmp);
    tofstream *output = new tofstream(name.cstr(), std::ofstream::out); TRACE_NEW(output)
    setOutput(*output,name, FileNameSplitter(name).getAbsolutePath(), true);
  }
}

void MarginFile::setOutput(tostream &output, const String &name, const String &absolutName, bool openedByMe) {
  assert(!isOpen());
  m_output           = &output;
  m_name             = name;
  m_absolutName      = absolutName;
  m_openedByMe       = openedByMe;
  m_trimRight        = false;
  m_lineNumber       = 1;
  m_formatBufferSize = 0x4000;
  m_formatBuffer     = new TCHAR[m_formatBufferSize]; TRACE_NEW(m_formatBuffer);
}

void  MarginFile::init() {
  m_name             = EMPTYSTRING;
  m_absolutName      = EMPTYSTRING;
  m_output           = NULL;
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
  m_output->flush();
  if(m_openedByMe) {
    SAFEDELETE(m_output);
  } else {
    m_output = NULL;
  }
  SAFEDELETEARRAY(m_formatBuffer);
  init();
}

void MarginFile::indent() {
  m_currentLine = m_leftFiller;
}

void MarginFile::flushLine() {
  if(m_trimRight) {
    m_currentLine.trimRight();
  }
  m_output->setf(std::ios::left, std::ios::adjustfield);
  if(m_currentLine.length()) {
    *m_output << m_currentLine.cstr();
  }
  *m_output << _T('\n');
  m_lineNumber++;
  m_currentLine = EMPTYSTRING;
}

void MarginFile::putch(TCHAR ch) {
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
  for(;*s; s++) {
    putch(*s);
  }
}

void MarginFile::vprintf(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  for(;;) {
    if(_vsntprintf(m_formatBuffer, m_formatBufferSize, format, argptr) >= 0) {
      break;
    }
    SAFEDELETEARRAY(m_formatBuffer);
    m_formatBufferSize <<= 1;
    m_formatBuffer = new TCHAR[m_formatBufferSize]; TRACE_NEW(m_formatBuffer);
  }
  puts(m_formatBuffer);
}

void MarginFile::printf(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr,format);
  vprintf(format, argptr);
  va_end(argptr);
}

void fprintf(MarginFile *file, _In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr,format);
  file->vprintf(format,argptr);
  va_end(argptr);
}

MarginFile stdoutMarginFile(_T("stdout"));
MarginFile stderrMarginFile(_T("stderr"));
