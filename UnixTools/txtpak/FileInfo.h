#pragma once

class FileInfo {
private:
  static const TCHAR *s_skillLine1, *s_skillLine2, *s_skillLine3;
  static const size_t s_skillLength1;
  static const size_t s_skillLength2;
  void getFileTimes();
public:
  String         m_name;
  Timestamp      m_ctime;
  Timestamp      m_atime;
  Timestamp      m_mtime;
  unsigned short m_mode;

  FileInfo() {}
  FileInfo(const String &name);
  void extractFileInfo(const String &line, size_t lineNumber);
  static inline bool isInfoLine(const String &line) {
    return _tcsnccmp(line.cstr(), s_skillLine1, s_skillLength1) == 0;
  }
  bool operator==(const FileInfo &info) const {
    return m_name.equalsIgnoreCase(info.m_name);
  }
  void   setFileTimes() const;
  void   setMode() const;
  String toString() const;
};

inline int fileInfoCmp(const FileInfo &f1, const FileInfo &f2) {
  return alphasort(f1.m_name, f2.m_name);
}
