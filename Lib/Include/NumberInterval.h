#pragma once

#include "MyUtil.h"

template <class T> class NumberInterval {
private:
  T m_from, m_to;
public:
  inline NumberInterval(const T &from, const T &to) : m_from(from), m_to(to) {
  }

  NumberInterval() {
    m_from = m_to = 0;
  }

  const T &getFrom() const {
    return m_from;
  }

  const T &getTo()   const {
    return m_to;
  }

  NumberInterval<T> &setFrom(const T &v) {
    m_from = v;
    return *this;
  }

  NumberInterval<T> &setTo(const T &v) {
    m_to   = v;
    return *this;
  }

  const T getMin() const {
    return m_from < m_to ? m_from : m_to;
  }

  const T getMax() const {
    return m_from > m_to ? m_from : m_to;
  }

  bool contains(const T &x) const {
    return (getMin() <= x) && (x <= getMax());
  }

  bool contains(const NumberInterval<T> &i) const {
    return (getMin() <= i.getMin()) && (i.getMax() <= getMax());
  }

  bool overlap(const NumberInterval<T> &i) const {
    return (getMin() <= i.getMax()) && (getMax() >= i.getMin());
  }

  NumberInterval<T> interSection(const NumberInterval<T> &i) const {
    if(!overlap(i)) {
      return NumberInterval<T>(T(0), T(0));
    } else {
      return NumberInterval<T>(max(getMin(), i.getMin()), min(getMax(), i.getMax()));
    }
  }

  NumberInterval<T> &operator+=(const NumberInterval<T> &rhs) { // union operator
    const T l = rhs.getMin();
    const T r = rhs.getMax();
    if (m_from < m_to) {
      m_from = min(m_from, l);
      m_to   = max(m_to  , r);
    } else {
      m_to   = min(m_to  , l);
      m_from = max(m_from, r);
    }
    return *this;
  }

  T getLength() const {
    return m_to - m_from;
  }
  bool operator==(const NumberInterval<T> &rhs) const {
    return (getFrom() == rhs.getFrom()) && (getTo() == rhs.getTo());
  }
  bool operator!=(const NumberInterval<T> &rhs) const {
    return !(*this == rhs);
  }
  String toString(int precision = 3) const {
    return format(_T("[%s-%s]"), ::toString(m_from, precision).cstr(), ::toString(m_to, precision).cstr());
  }
};

typedef NumberInterval<float   > FloatInterval;
typedef NumberInterval<double  > DoubleInterval;
typedef NumberInterval<int>      IntInterval;
