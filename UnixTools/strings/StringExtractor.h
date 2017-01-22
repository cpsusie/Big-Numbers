#pragma once

#include <FileTreeWalker.h>

class StringExtractor : public FileNameHandler {
private:
  const bool   m_showNames;
  int          m_screenWidth;
  bool         m_outputAtTTY;
  const TCHAR *m_currentName;
protected:
  const UINT   m_minLength;
  const bool   m_verbose;

  void printString(const char *str);
  void verbose(const TCHAR *str);
public:
  StringExtractor(UINT minLength, bool showNames, bool verbose);
  StringExtractor(const StringExtractor &src);            // not implemented
  StringExtractor &operator=(const StringExtractor &src); // not implemented
  virtual ~StringExtractor() {}
  void handleFileName(const TCHAR *name, DirListEntry &info);
  virtual void extractStrings(const TCHAR *name) = 0;
};
