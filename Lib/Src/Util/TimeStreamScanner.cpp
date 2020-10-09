#include "pch.h"
#include "DateTimeIO.h"

namespace DateTimeIO {

static const char *timePatterns[] = {
  "/([0-1]?[0-9]/|2[0-3]/)[.:/-][0-5]?[0-9]/([.:/-][0-5]?[0-9]/([.:/-][0-9][0-9]?[0-9]?/)?/)?"
 ,NULL
};

StringArray TimeStreamScanner::getRegexLines() { // static
  return StringArray(timePatterns);
}

}; // namespace DateTimeIO
