#include "stdafx.h"
#include "AllTime.h"

String AllTime::toString(TimeFormat tf) const {
  switch(tf) {
  case MYSEC:
    return format(_T("Proc:%7.3lf \xe6sec. Thr:%7.3lf \xe6sec. Real:%7.3lf \xe6sec.")
                 ,m_processTime
                 ,m_threadTime
                 ,m_realTime
                 );
  case MSEC:
    return format(_T("Proc:%7.3lf msec. Thr:%7.3lf msec. Real:%7.3lf msec.")
                 ,m_processTime / 1e3
                 ,m_threadTime  / 1e3
                 ,m_realTime    / 1e3);
  case SEC :
    return format(_T("Proc:%7.3lf sec. Thr:%7.3lf sec. Real:%7.3lf sec.")
                 ,m_processTime / 1e6
                 ,m_threadTime  / 1e6
                 ,m_realTime    / 1e6
                 );
  case MMSS:
    return format(_T("Proc:%s Thr:%s Real:%s")
                 ,formatSeconds(m_processTime).cstr()
                 ,formatSeconds(m_threadTime ).cstr()
                 ,formatSeconds(m_realTime   ).cstr()
                 );
  default:
    return format(_T("Proc:%7.3lf msec. Thr;%7.3lf msec. Real:%7.3lf msec.")
                 ,m_processTime / 1e3
                 ,m_threadTime  / 1e3
                 ,m_realTime    / 1e3
                 );
  }
}

String AllTime::formatSeconds(double time) { // static
  const unsigned int seconds = (unsigned int)(time / 1e6);
  if(seconds < 3600) {
    return format(_T("%02d:%02d min."), seconds / 60, seconds % 60);
  } else {
    return format(_T("%d:%02d:%02d hour."), seconds / 3600, (seconds/60)%60, seconds%60);
  }
}
