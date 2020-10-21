#include "pch.h"
#include <MathUtil.h>

static const char *hexFloatPatterns[] = {
  "[+/-]?/(0x/)?[0-9a-f]*/(/.[0-9a-f]+/)/(p[+/-]?[0-9]+/)?"
 ,"[+/-]?/(0x/)?[0-9a-f]+/(/.[0-9a-f]*/)?/(p[+/-]?[0-9]+/)?"
 ,nullptr
};

StringArray HexFloatValueStreamScanner::getRegexLines() { // static
  return StringArray(hexFloatPatterns);
}
