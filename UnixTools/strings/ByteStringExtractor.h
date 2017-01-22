#pragma once

#include "StringExtractor.h"

class ByteStringExtractor : public StringExtractor {
private:
  int   m_strLength;
  int   m_bufLength;
  char *m_buf;

  void outputString();
  void extendBuffer();
  void appendChar(char ch);
public:
  ByteStringExtractor(UINT len, bool showNames, bool verbose, bool danish);
  ~ByteStringExtractor();
  void extractStrings(const TCHAR *name);
};
