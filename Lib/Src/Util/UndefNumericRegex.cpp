#include "pch.h"
#include <MathUtil.h>

static const char *undefNumericPatterns[] = {
  "inf"                          // _UNDEFREG_PINF
 ,"-inf"                         // _UNDEFREG_NINF
 ,"-?/(nan/((snan)/)?/|snan/)"   // _UNDEFREG_SNAN
 ,"-?/(nan/((ind)/)?/|qnan/)"    // _UNDEFREG_QNAN
 ,NULL
};

StringArray UndefNumericStreamScanner::getRegexLines() { // static
  return StringArray(undefNumericPatterns);
}
