#include "pch.h"
#include <MathUtil.h>

static const char *undefFloatPatterns[] = {
  "inf"                               // _UNDEFREG_PINF
 ,"-inf"                              // _UNDEFREG_NINF
 ,"-?\\(nan\\((snan)\\)?\\|snan\\)"   // _UNDEFREG_SNAN
 ,"-?\\(nan\\((ind)\\)?\\|qnan\\)"    // _UNDEFREG_QNAN
 ,NULL
};

StringArray UndefFloatValueStreamScanner::getRegexLines() { // static
  return StringArray(undefFloatPatterns);
}
