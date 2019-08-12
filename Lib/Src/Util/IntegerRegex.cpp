#include "pch.h"
#include <MathUtil.h>

static const char *inegerPatterns[] = {
  "[+\\-]?[0-9]+"
 ,NULL
};

StringArray IntegerValueStreamScanner::getRegexLines() { // static
  return StringArray(inegerPatterns);
}
