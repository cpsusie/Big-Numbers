#include "pch.h"
#include <MathUtil.h>

static const char *decIntPatterns[] = {
  "[+\\-]?[0-9]+"
 ,NULL
};

StringArray DecIntValueStreamScanner::getRegexLines() { // static
  return StringArray(decIntPatterns);
}

static const char *hexIntPatterns[] = {
  "[+\\-]?\\(0x\\)?[0-9a-f]+"
 ,NULL
};

StringArray HexIntValueStreamScanner::getRegexLines() { // static
  return StringArray(hexIntPatterns);
}

static const char *octIntPatterns[] = {
  "[+\\-]?0?[0-7]+"
 ,NULL
};

StringArray OctIntValueStreamScanner::getRegexLines() { // static
  return StringArray(octIntPatterns);
}

// radix must be 8,10, 16
const RegexIStream &getIntegerStreamScanner(int radix) {
  switch (radix) {
  case 8 : return OctIntValueStreamScanner::getInstance();
  case 10: return DecIntValueStreamScanner::getInstance();
  case 16: return HexIntValueStreamScanner::getInstance();
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("radix=%d"), radix);
    return DecIntValueStreamScanner::getInstance();
  }
}
