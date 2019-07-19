#include "pch.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <limits>
#include <Math/Double64.h>

template<class CharType, class StreamType> class DoubleIstream {
private:
  StreamType &m_in;
 
  void parseOnFail(double &x, bool neg) const {
    const char *exp[] = { "", "inf", "Inf", "NaN", "nan" };
    const char *e     = exp[0];
    int         l     = 0;
    CharType parsed[4], *c = parsed;
    if(neg) *c++ = '-';
    m_in.clear();

    if(!(m_in >> *c).good()) {
      return; // If the stream is broken even before trying to read from it, it's pointless to try.
    }

    switch (*c) { // Switch on the first character to parse, if 'i' or 'I', set up for Infinity, else if 'n' or 'N', set up for NaN
    case 'i': e = exp[l = 1]; break;
    case 'I': e = exp[l = 2]; break;
    case 'N': e = exp[l = 3]; break;
    case 'n': e = exp[l = 4]; break;
    }

    while (*c == *e) {
      if((e - exp[l]) == 2) break;
      ++e;
      if(!(m_in >> *++c).good()) break;
    }

    if(m_in.good() && (*c == *e)) {
      switch(l) {
      case 1:
      case 2:
        x = std::numeric_limits<double>::infinity();
        break;
      case 3:
      case 4:
        x = std::numeric_limits<double>::quiet_NaN();
        break;
      }
      if(neg) x = -x;
      return;
    } else if(!m_in.good()) {
      if(!m_in.fail()) return;
      m_in.clear();
      --c;
    }

    do {
      m_in.putback(*c);
    } while (c-- != parsed);
    m_in.setstate(std::ios_base::failbit);
  }

public:
  DoubleIstream(StreamType &in) : m_in(in) {
  }
  DoubleIstream &operator>>(double &x) {
    CharType c;

    if(!m_in.good()) {
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

const DoubleinManip &DoubleinManip::operator>>(double &x) const {
  DoubleIstream<char, std::istream>(*m_in) >> x;
  return *this;
}

const DoublewinManip &DoublewinManip::operator>>(double &x) const {
  DoubleIstream<wchar_t, std::wistream>(*m_in) >> x;
  return *this;
}
