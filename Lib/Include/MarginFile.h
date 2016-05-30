#pragma once

#include "MyUtil.h"
#include <stdarg.h>

class MarginFile { // print to file, with control of leftmargin
private:
  String m_name, m_absolutName;
  FILE      *m_file;
  int        m_leftMargin;
  bool       m_openedByMe;
  bool       m_lineStart;
  int        m_lineNumber;
  TCHAR *m_buffer;
  int        m_bufSize;
  int        m_currentLineLength;

  void  init(FILE *file, const String &name, const String &absolutName, bool openedByMe);
  void  indent();
public:
  MarginFile(const String &name);
  MarginFile(const MarginFile &src);             // not defined
  MarginFile &operator=(const MarginFile &src);  // not defined
 ~MarginFile();
  void close();
  void putch(   int c);
  void puts(    const TCHAR *s);
  void vprintf( const TCHAR *format, va_list argptr);
  void printf(  const TCHAR *format, ...);
 
  void setLeftMargin(int m)  {
    m_leftMargin = m;
  }

  int getLeftMargin() const {
    return m_leftMargin;
  }

  int getLineNumber() const {
    return m_lineNumber;
  }

  int getCurrentLineLength() const {
    return m_currentLineLength;
  }

  String getName() const {
    return m_name;
  }

  String getAbsolutName() const {
    return m_absolutName;
  }
};

void fprintf(MarginFile *f, const TCHAR *format, ...);

extern MarginFile stdoutMarginFile;
extern MarginFile stderrMarginFile;

#define tostdout (&stdoutMarginFile)
#define tostderr (&stderrMarginFile)

