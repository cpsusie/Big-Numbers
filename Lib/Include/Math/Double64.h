#pragma once

#include "PragmaLib.h"

int    getExpo10(         double x);

double mypow(             double x, double y);
double root(              double x, double y);
double cot(               double x);
double acot(              double x);
// calculate both cos and sin. c:inout c, s:out
extern "C" {
  void   sincos(double &c, double &s);
  double exp10(double x);
};

// see http://ideone.com/qIFVo

class DoubleinManip {
public:
  mutable std::istream  *m_in;
  const DoubleinManip &operator>>(double &x) const;

  inline std::istream &operator>>(const DoubleinManip &) const {
    return *m_in;
  }
};

class DoublewinManip {
public:
  mutable std::wistream *m_in;
  const DoublewinManip &operator>>(double &x) const;

  inline std::wistream &operator>>(const DoublewinManip &) const {
    return *m_in;
  }
};

inline const DoubleinManip &operator>>(std::istream &in, const DoubleinManip &dm) {
  dm.m_in = &in;
  return dm;
}

inline const DoublewinManip &operator>>(std::wistream &in, const DoublewinManip &dm) {
  dm.m_in = &in;
  return dm;
}

