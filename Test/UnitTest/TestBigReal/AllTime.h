#pragma once

typedef enum {
  MYSEC
 ,MSEC
 ,SEC
 ,MMSS
} TimeFormat;

class AllTime {
private:
  double m_processTime, m_threadTime, m_realTime;
  static String formatSeconds(double time);
public:
  AllTime() {
    m_processTime = ::getProcessTime();
    m_threadTime  = ::getThreadTime();
    m_realTime    = ::getSystemTime();
  }
  AllTime(double processTime, double threadTime, double realTime)
    : m_processTime(processTime)
    , m_threadTime( threadTime )
    , m_realTime(   realTime   ) {
  }
  AllTime operator-(const AllTime &t) const {
    return AllTime(m_processTime - t.m_processTime, m_threadTime - t.m_threadTime, m_realTime - t.m_realTime);
  }
  AllTime operator/(double d) {
    return AllTime(m_processTime/d, m_threadTime/d, m_realTime/d);
  }

  double getProcessTime() const {
    return m_processTime;
  }
  double getThreadTime() const {
    return m_threadTime;
  }
  double getRealTime() const {
    return m_realTime;
  }
  String toString(TimeFormat tf) const;
};
