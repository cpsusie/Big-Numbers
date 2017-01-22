#pragma once

#define BUF_SIZE  0x40000
#define LINE_SIZE 0x10000

typedef TCHAR Line[LINE_SIZE];

class HelpFile {
private:
  typedef TCHAR FileName[256];
  static TCHAR  s_drive;
  FILE         *m_f;
  FileName      m_name;
  void makeName();
public:
  void writeLine(Line s);
  TCHAR *readLine(Line s);
  HelpFile();
  ~HelpFile();
  void destroy();
  void open(FileMode mode);
  void close();
  bool isOpen() const           { return m_f != NULL; }
  TCHAR *getName()              { return m_name;      }
  static void setDrive(TCHAR d) { s_drive = d;        }
};
