#include "stdafx.h"
#include <Math/Double64.h>
#include "RegexIStream.h"

using namespace std;
static void TestInternetExample() {
  stringstream iss("1.0 -NaN inf Inf nan -inf NaN 1.2 inf");
  double u, v, w, x, y, z, a, b, fail_double;
  string fail_string;
  iss >> DoubleinManip()
      >> u >> v >> w >> x >> y >> z >> a >> b
      >> DoubleinManip()
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

#define POSITIVE_INFINITY  0
#define NEGATIVE_INFINITY  1
#define POSITIVE_NAN       2
#define NEGATIVE_SIGNALNAN 3
#define NEGATIVE_QUUET_NAN 4

class UndefFloatingValueStreamScanner : public RegexIStream {
private:
  static StringArray getRegExprLines() {
    return StringArray(Tokenizer(_T("inf\n-inf\nnan\n-nan\n-nan(ind)?"), _T("\n")));
  }
  UndefFloatingValueStreamScanner() : RegexIStream(getRegExprLines(), true) {
  }
public:
  static const UndefFloatingValueStreamScanner &getInstance() {
    static UndefFloatingValueStreamScanner s_instance;
    return s_instance;
  }
};

class MyDoubleIstream {
private:
  istream &m_in;
  void parseOnFail(double &x) const {
    m_in.clear();
    const int index = UndefFloatingValueStreamScanner::getInstance().match(m_in);
    if(index < 0) {
      m_in.setstate(ios_base::failbit);
    } else {
      m_in.clear();
      switch(index) {
      case POSITIVE_INFINITY  : x =  numeric_limits<double>::infinity();      break;
      case NEGATIVE_INFINITY  : x = -numeric_limits<double>::infinity();      break;
      case POSITIVE_NAN       : x =  numeric_limits<double>::quiet_NaN();     break;
      case NEGATIVE_SIGNALNAN : x = -numeric_limits<double>::signaling_NaN(); break;
      case NEGATIVE_QUUET_NAN : x = -numeric_limits<double>::quiet_NaN();     break;
      }
    }
  }

public:
  MyDoubleIstream(istream &in) : m_in(in) {
  }
  MyDoubleIstream &operator>>(double &x) {
    char c = 0;
    if(!m_in.good()) {
      return *this;
    }
    while(iswspace(c = m_in.peek())) {
      m_in.get();
    }
    m_in >> x;
    if(m_in.fail()) {
      m_in.clear();
      if(c == '-') m_in.putback(c);
      parseOnFail(x);
    }
    return *this;
  }
};


class MyDoubleinManip {
public:
  mutable istream  *m_in;
  const MyDoubleinManip &operator>>(double &x) const {
    MyDoubleIstream(*m_in) >> x;
    return *this;
  }
  inline istream &operator>>(const MyDoubleinManip &) const {
    return *m_in;
  }
};

inline const MyDoubleinManip &operator>>(istream &in, const MyDoubleinManip &dm) {
  dm.m_in = &in;
  return dm;
}

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

  stringstream input;
  for(size_t i = 0; i < testData.size(); i++) {
    input << testData[i] << endl;
  }
  cout << "TestData:" << endl << input.str() << endl;
  stringstream iss(input.str());
  for(size_t index = 0; !iss.eof(); index++) {
    double x;
    iss >> MyDoubleinManip() >> x;
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
