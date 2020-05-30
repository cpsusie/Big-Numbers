#include "pch.h"
#include "DateTimeStreamScanner.h"

static const char *timePatterns[] = {
  "/([0-1]?[0-9]/|2[0-3]/)[.:/-][0-5]?[0-9]/([.:/-][0-5]?[0-9]/([.:/-][0-9][0-9]?[0-9]?/)?/)?"
 ,NULL
};

StringArray TimeStreamScanner::getRegexLines() { // static
  return StringArray(timePatterns);
}
