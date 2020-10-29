#pragma once

template<typename T> class NumberInterval {
private:
  T m_from, m_to;
public:
  NumberInterval() : m_from(0), m_to(0) {
  }

  template<typename S> NumberInterval(const NumberInterval<S> &src) : m_from((T)src.getFrom()), m_to((T)src.getTo()) {
  }

  template<typename T1, typename T2> inline NumberInterval(const T1 &from, const T2 &to) : m_from((T)from), m_to((T)to) {
  }

  template<typename S> NumberInterval<T> &operator=(const NumberInterval<S> &src) {
    return setFrom(src.getFrom()).setTo(src.getTo())
  }

  const T &getFrom() const {
    return m_from;
  }

  const T &getTo()   const {
    return m_to;
  }

  // return this
  template<typename S> NumberInterval<T> &setFrom(const S &v) {
    m_from = (T)v;
    return *this;
  }

  // return this
  template<typename S> NumberInterval<T> &setTo(const S &v) {
    m_to   = (T)v;
    return *this;
  }

  const T &getMin() const {
    return m_from < m_to ? m_from : m_to;
  }

  const T &getMax() const {
    return m_from > m_to ? m_from : m_to;
  }

  NumberInterval &clear() {
    return setFrom(0).setTo(0);
  }

  // Return true if getMin() <= x <= getMax(), else return false
  template<typename S> bool contains(const S &x) const {
    return (getMin() <= (T)x) && ((T)x <= getMax());
  }

  // Return true if makePositive(this).contains(makePositive(i))
  template<typename S> bool contains(const NumberInterval<S> &i) const {
    return (getMin() <= (T)(i.getMin())) && ((T)(i.getMax()) <= getMax());
  }

  // Return true if makePositive(this) overlaps makePositive(i)
  template<typename S> bool overlap(const NumberInterval<S> &i) const {
    return (getMin() <= (T)(i.getMax())) && (getMax() >= (T)(i.getMin()));
  }

  // Return NumberInterval(max(getMin(), i.getMin()), min(getMax(), i.getMax())) if this.overlap(i)
  // else return Interval(0,0)
  NumberInterval<T> interSection(const NumberInterval<T> &i) const {
    if(!overlap(i)) {
      return NumberInterval<T>(T(0), T(0));
    } else {
      return NumberInterval<T>(max(getMin(), i.getMin()), min(getMax(), i.getMax()));
    }
  }

  // union operator
  NumberInterval<T> &operator+=(const NumberInterval<T> &rhs) {
    const T l = rhs.getMin();
    const T r = rhs.getMax();
    if(m_from < m_to) {
      m_from = min(m_from, l);
      m_to   = max(m_to  , r);
    } else {
      m_to   = min(m_to  , l);
      m_from = max(m_from, r);
    }
    return *this;
  }

  inline bool isNegativeLength() const {
    return m_to < m_from;
  }
  inline T getLength() const {
    return m_to - m_from;
  }

  inline NumberInterval<T> &invert() {
    std::swap(m_from, m_to);
    return *this;
  }

  inline NumberInterval<T> &makePositive() {
    return isNegativeLength() ? invert() : *this;
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
