#pragma once

#include <ByteArray.h>
#include "StringExtractor.h"

class CStringExtractor : public StringExtractor {
private:
  ByteArray m_buffer;

  bool nextString(FILE *input);
  inline void appendChar(char ch) {
    m_buffer += (BYTE)ch;
  }
  void outputString();
public:
  CStringExtractor(UINT len, bool showNames, bool verbose);
  void extractStrings(const TCHAR *name);
};
