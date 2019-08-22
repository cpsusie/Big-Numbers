#include "stdafx.h"
#include <Math/Double80.h>
#include "TestToString.h"
#include "StreamParametersIterator.h"

using namespace std;

String D80ToDbgString(const Double80 &d80) {
  const int expo10 = Double80::getExpo10(d80);
  if(expo10 < -4 || (expo10 >= 18)) {
    TCHAR str[50];
    return d80tot(str, d80);
  } else {
    return toString(d80, 19-expo10,21,ios::fixed);
  }
}

static void usage() {
  fprintf(stderr
         ,"testToString [-l] [-b] [-f]\n"
          "   -l: Skip testing of long double (Double80)\n"
          "   -b: Skip testing of BigReal\n"
          "   -f: Dump all combinations of formats-flags, that will be testet\n"
          "Both options cannot be specified in the same test.\n"
          "Default: Test of formats for both Double80 and BigReal will be done.\n"
         );
  exit(-1);
}

int main(int argc, char **argv) {
  UINT flags           = TTS_ALL;
  bool dumpFormatFlags = false;
  try {
    char *cp;
    for(argv++; *argv && (*(cp = *argv) == '-'); argv++) {
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'l': flags &= ~TTS_D80; continue;
        case 'b': flags &= ~TTS_BR ; continue;
        case 'f': dumpFormatFlags = true; continue;
        default : usage();
        }
      }
    }
    if(flags == 0) {
      usage();
    }
    if(dumpFormatFlags) {
      redirectDebugLog();
      StreamParametersIterator::dumpAllformats();
    }
    const UINT64   ui64 = -1;
    const Double80 x    = ui64;
    const String   s1   = D80ToDbgString(x);
    testToString(flags);
  } catch(Exception e) {
    tcout << _T("Exception:") << e.what() << endl;
    return -1;
  }
  return 0;
}
