#include "pch.h"
#include <MathUtil.h>

static const char *floatPatterns[] = {
  "[+\\-]?[0-9]*\\(\\.[0-9]+\\)\\(e[+\\-]?[0-9]+\\)?"
 ,"[+\\-]?[0-9]+\\(\\.[0-9]*\\)?\\(e[+\\-]?[0-9]+\\)?"
 ,NULL
};

StringArray FloatValueStreamScanner::getRegexLines() { // static
  return StringArray(floatPatterns);
}
