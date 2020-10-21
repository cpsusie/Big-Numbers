#pragma once

#include <Unicode.h>
#include <MyString.h>

class MarginFile { // print to tostream, with control of leftmargin
private:
  String    m_name, m_absolutName;
  tostream *m_output;
  bool      m_openedByMe;
  bool      m_trimRight;
  int       m_lineNumber;
  String    m_currentLine, m_leftFiller;
  TCHAR    *m_formatBuffer;
  int       m_formatBufferSize;

  MarginFile(const MarginFile &src);             // Not defined. Class not cloneable
  MarginFile &operator=(const MarginFile &src);  // Not defined. Class not cloneable

  void  init();
  void  indent();
  void  flushLine();
  void  setOutput(tostream &output, const String &name, const String &absolutName, bool openedByMe);

public:
  MarginFile(const String &name = EMPTYSTRING);
  // assume output has been opened
  MarginFile(tostream &output, const String &name = EMPTYSTRING);
  virtual ~MarginFile();
  void open(const String &name);
  void close();
  inline bool isOpen() const {
    return m_output != nullptr;
  }

  void putch(   TCHAR c);
  void puts(    const TCHAR *s);
  void vprintf( _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  void printf(  _In_z_ _Printf_format_string_ TCHAR const * const format, ...);

  // return old leftMargin
  inline int setLeftMargin(int m)  {
    const int old = getLeftMargin(); m_leftFiller = spaceString(m); return old;
  }
  inline int getLeftMargin() const {
    return (int)m_leftFiller.length();
  }
  // if trimRight = true, then all trailing spaces will be reoved from the stream
  inline void setTrimRight(bool trimRight) {
    m_trimRight = trimRight;
  }
  inline bool getTrimRight() const {
    return m_trimRight;
  }
  inline int getLineNumber() const {
    return m_lineNumber;
  }
  inline int getCurrentLineLength() const {
    return (int)m_currentLine.length();
  }
  inline const String &getName() const {
    return m_name;
  }
  inline const String &getAbsolutName() const {
    return m_absolutName;
  }
};

inline void vfprintf(MarginFile *f, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  f->vprintf(format, argptr);
}
void fprintf( MarginFile *f, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);

inline void fputc(int ch, MarginFile *f) {
  f->putch((TCHAR)ch);
}

extern MarginFile stdoutMarginFile;
extern MarginFile stderrMarginFile;

#define tostdout (&stdoutMarginFile)
#define tostderr (&stderrMarginFile)
