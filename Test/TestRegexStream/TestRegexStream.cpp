#include "stdafx.h"
#include <MathUtil.h>

using namespace std;
static void TestInternetExample() {
  stringstream iss("1.0 -NaN inf Inf nan -inf NaN 1.2 inf");
  double u, v, w, x, y, z, a, b, fail_double;
  string fail_string;
  iss >> CharManip<double>
      >> u >> v >> w >> x >> y >> z >> a >> b
      >> CharManip<double>
      >> fail_double;
   cout << u << " " << v << " " << w << " "
             << x << " " << y << " " << z << " " << a << " " << b << endl;
   if (iss.fail()) {
     iss.clear();
     iss >> fail_string;
     cout << fail_string << endl;

  } else {
    cout << "TEST FAILED" << endl;
  }
}

// ----------------------------------------------------------------------


static void TestMyExample() {
  CompactArray<double> testData;

  const double dpinf  = numeric_limits<double>::infinity();
  const double dpqnan = numeric_limits<double>::quiet_NaN();
  const double dpsnan = numeric_limits<double>::signaling_NaN();
  const double dninf  = -dpinf;
  const double dnqnan = -dpqnan;
  const double dnsnan = -dpsnan;
  const double dvalue = M_PI;

  testData.add(dpinf);
  testData.add(dpqnan);
  testData.add(dpsnan);
  testData.add(dninf);
  testData.add(dnqnan);
  testData.add(dnsnan);
  testData.add(dvalue);

  wstringstream input;
  for(size_t i = 0; i < testData.size(); i++) {
    input << testData[i] << endl;
  }
  wcout << "TestData:" << endl << input.str() << endl;
  wstringstream iss(input.str());
  for(size_t index = 0; !iss.eof(); index++) {
    double x;
    iss >> WcharManip<double> >> x;
    if(index >= testData.size()) {
      cout << "Too many data read. expected only " << testData.size() << " elements" << endl;
      break;
    }
    cout << "Read:" << x << ",    expeced:" << testData[index] << endl;
  }
}

static void usage() {
  _ftprintf(stderr, _T("Usage:TestRegexStream [-i|m]\n"
                       "         -i: Test internet example\n"
                       "         -m: Test my example. (Default)\n"
                     )
           );
  exit(-1);
}

using namespace std;

typedef enum {
  CMD_TESTINTERNETEXAMPLE
 ,CMD_TESTMYEXAMPLE
} Command;

int main(int argc, char **argv) {
  Command cmd = CMD_TESTMYEXAMPLE;
  char *cp;

  for(argv++; *argv && (*(cp = *argv) == '-'); argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'i': cmd = CMD_TESTINTERNETEXAMPLE; continue;
      case 'm': cmd = CMD_TESTMYEXAMPLE      ; continue;
      default : usage();
      }
    }
  }
  cout << UndefNumericStreamScanner::getInstance().toString();
  try {
    switch(cmd) {
    case CMD_TESTINTERNETEXAMPLE :
      TestInternetExample();
      break;
    case CMD_TESTMYEXAMPLE       :
      TestMyExample();
      break;
    default:
      throwException(_T("Unknown cmd:%d"), cmd);
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}
