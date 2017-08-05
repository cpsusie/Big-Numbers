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

  void  init();
  void  open(FILE *file, const String &name, const String &absolutName, bool openedByMe);
  void  indent();
  void  flushLine();
public:
  MarginFile(const String &name = EMPTYSTRING);
  MarginFile(const MarginFile &src);             // not defined
  MarginFile &operator=(const MarginFile &src);  // not defined
  virtual ~MarginFile();
  void open(const String &name);
  void close();
  inline bool isOpen() const {
    return m_file != NULL;
  }
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

inline void vfprintf(MarginFile *f, const TCHAR *format, va_list argptr) {
  f->vprintf(format, argptr);
}
void fprintf( MarginFile *f, const TCHAR *format, ...);

inline void fputc(int ch, MarginFile *f) {
  f->putch((TCHAR)ch);
}

extern MarginFile stdoutMarginFile;
extern MarginFile stderrMarginFile;

#define tostdout (&stdoutMarginFile)
#define tostderr (&stderrMarginFile)
