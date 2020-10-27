#pragma once

#include <ByteFile.h>

#define BUF_SIZE  0x40000

inline void writeLine(FILE *f, const String &s) {
  _fputts(s.cstr(), f); _fputtc(_T('\n'), f);
}

class HelpFile {
private:
  static TCHAR  s_drive;
  FILE         *m_f;
  String        m_name;
  UINT64        m_lineCount;
  void makeName();
public:
  inline String *readLine(String &s) {
    return ::readLine(m_f, s) ? &s : nullptr;
  }
  inline void writeLine(const String &s) {
    ::writeLine(m_f, s); m_lineCount++;
  }

  inline HelpFile() : m_f(nullptr), m_lineCount(0) {
  }

  inline ~HelpFile() {
    close();
  }
  void destroy();
  void open(FileMode mode);
  void close();
  inline bool isOpen() const {
    return m_f != nullptr;
  }
  inline const TCHAR *getName() const {
    return m_name.cstr();
  }
  inline UINT64 getLineCount() const {
    return m_lineCount;
  }
  static void setDrive(TCHAR d) {
    s_drive = d;
  }
};
