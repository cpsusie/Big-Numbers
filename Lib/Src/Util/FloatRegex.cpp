#include "pch.h"
#include <MathUtil.h>

static const char *decFloatPatterns[] = {
  "[+/-]?[0-9]*/(/.[0-9]+/)/(e[+/-]?[0-9]+/)?"
 ,"[+/-]?[0-9]+/(/.[0-9]*/)?/(e[+/-]?[0-9]+/)?"
 ,NULL
};

StringArray DecFloatValueStreamScanner::getRegexLines() { // static
  return StringArray(decFloatPatterns);
}
