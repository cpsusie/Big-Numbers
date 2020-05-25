#include "pch.h"
#include "DateTimeStreamScanner.h"

static const char *datePatterns[] = {
  "/(0?[1-9]/|[12][0-9]/|3[01]/)/(/./|///|-/)/(0?[1-9]/|1[0-2]/)/(/./|///|-/)[0-9][0-9]?[0-9]?[0-9]?"
 ,NULL
};

StringArray DateStreamScanner::getRegexLines() { // static
  return StringArray(datePatterns);
}
