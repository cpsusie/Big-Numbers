#include "stdafx.h"
#include <Math/Double64.h>
#include "RegexIStream.h"

static void TestInternetExample() {
  std::stringstream iss("1.0 -NaN inf Inf nan -inf NaN 1.2 inf");
  double u, v, w, x, y, z, a, b, fail_double;
  std::string fail_string;
  iss >> DoubleinManip()
      >> u >> v >> w >> x >> y >> z >> a >> b
      >> DoubleinManip()
      >> fail_double;
   std::cout << u << " " << v << " " << w << " "
             << x << " " << y << " " << z << " " << a << " " << b << std::endl;
   if (iss.fail()) {
     iss.clear();
     iss >> fail_string;
     std::cout << fail_string << std::endl;
     
  } else {
    std::cout << "TEST FAILED" << std::endl;
  }
}

// ----------------------------------------------------------------------

class MyDoubleIstream {
private:
  static const RegexIStream s_undefstream;
  std::istream &m_in;

  void parseOnFail(double &x, bool neg) const {
    m_in.clear();
    const int index = s_undefstream.match(m_in);
    if(index < 0) {
      m_in.setstate(std::ios_base::failbit);
    } else {
      m_in.clear();
      switch(index) {
      case 1:
        neg = true;
        // continue case
      case 0:
        x = std::numeric_limits<double>::infinity();
        break;
      case 3:
        neg = true;
        // continue case
      case 2:
        x = std::numeric_limits<double>::quiet_NaN();
        break;
      }
      if(neg) x = -x;
    }
  }

public:
  MyDoubleIstream(std::istream &in) : m_in(in) {
  }
  MyDoubleIstream &operator>>(double &x) {
    char c;

    if (!m_in.good()) {
      return *this;
    }
    while(iswspace(c = m_in.peek())) {
      m_in.get();
    }
    bool neg = false;
    if(c == '-') {
      neg = true;
    }
    m_in >> x;
    if(m_in.fail()) {
      parseOnFail(x, neg);
    }
    return *this;
  }
};

static StringArray getRegExprLines() {
  return StringArray(Tokenizer(_T("inf\n-inf\nnan\\((ind)\\)?\n-nan\\((ind)\\)?"), _T("\n")));
}

const RegexIStream MyDoubleIstream::s_undefstream(getRegExprLines(), true);

class MyDoubleinManip {
public:
  mutable std::istream  *m_in;
  const MyDoubleinManip &operator>>(double &x) const {
    MyDoubleIstream(*m_in) >> x;
    return *this;
  }
  inline std::istream &operator>>(const MyDoubleinManip &) const {
    return *m_in;
  }
};

inline const MyDoubleinManip &operator>>(std::istream &in, const MyDoubleinManip &dm) {
  dm.m_in = &in;
  return dm;
}

static void TestMyExample() {
  std::stringstream iss("1.0 -NaN inf Inf nan -inf NaN 1.2 inf");
  double u, v, w, x, y, z, a, b, fail_double;
  std::string fail_string;
  iss >> MyDoubleinManip()
      >> u >> v >> w >> x >> y >> z >> a >> b
      >> MyDoubleinManip()
      >> fail_double;
  std::cout << u << " " << v << " " << w << " "
            << x << " " << y << " " << z << " " << a << " " << b << std::endl;
  if(iss.fail()) {
    iss.clear();
    iss >> fail_string;
    std::cout << fail_string << std::endl;
  } else {
    std::cout << "TEST FAILED" << std::endl;
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
