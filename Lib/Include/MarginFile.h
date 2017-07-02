#pragma once

#include "MyUtil.h"
#include <stdarg.h>

class MarginFile { // print to file, with control of leftmargin
private:
  String     m_name, m_absolutName;
  FILE      *m_file;
  bool       m_openedByMe;
  bool       m_trimRight;
  int        m_lineNumber;
  String     m_currentLine, m_leftFiller;
  TCHAR     *m_formatBuffer;
  int        m_formatBufferSize;

  void  init(FILE *file, const String &name, const String &absolutName, bool openedByMe);
  void  indent();
  void  flushLine();
public:
  MarginFile(const String &name);
  MarginFile(const MarginFile &src);             // not defined
  MarginFile &operator=(const MarginFile &src);  // not defined
 ~MarginFile();
  void close();
  void putch(   TCHAR c);
  void puts(    const TCHAR *s);
  void vprintf( const TCHAR *format, va_list argptr);
  void printf(  const TCHAR *format, ...);

  void setLeftMargin(int m)  {
    m_leftFiller = spaceString(m);
  }
  int getLeftMargin() const {
    return (int)m_leftFiller.length();
  }
  // if trimRight = true, then all trailing spaces will be reoved from the stream
  void setTrimRight(bool trimRight) {
    m_trimRight = trimRight;
  }
  bool getTrimRight() const {
    return m_trimRight;
  }
  int getLineNumber() const {
    return m_lineNumber;
  }
  int getCurrentLineLength() const {
    return (int)m_currentLine.length();
  }
  const String &getName() const {
    return m_name;
  }
  const String &getAbsolutName() const {
    return m_absolutName;
  }
};

void fprintf(MarginFile *f, const TCHAR *format, ...);

extern MarginFile stdoutMarginFile;
extern MarginFile stderrMarginFile;

#define tostdout (&stdoutMarginFile)
#define tostderr (&stderrMarginFile)
