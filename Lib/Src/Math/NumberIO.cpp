#include "pch.h"
#include "NumberIO.h"

namespace NumberIO {

using namespace std;

static const char *numberPatterns[] = {
  "[+/-]?[0-9]*/(/.[0-9]+/)/(e[+/-]?[0-9]+/)?"    // float
 ,"[+/-]?[0-9]+/(/.[0-9]*/)?/(e[+/-]?[0-9]+/)?"   // float
 ,"[+/-]?[0-9]+/(//[0-9]+/)?"                     // rational
 ,nullptr
};

StringArray NumberValueStreamScanner::getRegexLines() { // static
  return StringArray(numberPatterns);
}

}; // NumberIO
