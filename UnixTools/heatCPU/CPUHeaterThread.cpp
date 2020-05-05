#include "stdafx.h"
#include <DebugLog.h>
#include "CPUHeaterThread.h"

#define BUFFERSIZE (1024*1024)

CPUHeaterThread::CPUHeaterThread() : Thread("HeaterThread") {
  setDemon(true);
  m_wantedCPULoad = 0;
  m_lastCPULoad   = 1;
  m_infoWnd       = NULL;
  m_buffer        = new char[BUFFERSIZE];
}

void CPUHeaterThread::setInfoWindow(CWnd *wnd) {
  m_infoWnd = wnd;
}

void CPUHeaterThread::setCPULoad(double wantedCPULoad) {
  if(wantedCPULoad == 0) {
    m_wantedCPULoad = 0;
  } else {
    bool doResume = (m_wantedCPULoad == 0);

    m_wantedCPULoad  = wantedCPULoad;
    m_lastThreadTime = getThreadTime();
    m_lastRealTime   = GetTickCount();
    m_lastCPULoad    = 1;

    if(doResume) {
      resume();
    }
  }
}
#define MOMENTUM 0.9

unsigned int CPUHeaterThread::run() {
  try {
    infiniteLoop();
  } catch(Exception e) {
    debugLog(_T("Exception:%s\n"), e.what());
  }
  return 0;
}

void CPUHeaterThread::infiniteLoop() {
  double sleepTime = 1;
  setPriority(PRIORITY_IDLE);
  setPriorityBoost(true);
/*
  unsigned long loopCounts = 20000000;
  double lasttt = -1;
*/
  for(;;) {
    for(int i = 0; i < 1000; i++) {
      memset(m_buffer,0x55,BUFFERSIZE);
      memset(m_buffer,0xaa,BUFFERSIZE);
    }

    DWORD        rt  = GetTickCount();
    const double tt  = getThreadTime();
    const double dtt = (tt - m_lastThreadTime) / 1000; // in milli-seconds
    const double drt = rt - m_lastRealTime;
    m_lastRealTime   = rt;
    m_lastThreadTime = tt;

    double measuredCPULoad = 0;

    if(drt != 0.0) {
      measuredCPULoad = (dtt/drt + MOMENTUM*m_lastCPULoad) / (1.0+MOMENTUM);
      m_lastCPULoad = measuredCPULoad;

      if(m_wantedCPULoad == 1) {
        sleepTime = 0;
      } else {
        sleepTime += (measuredCPULoad - m_wantedCPULoad) * drt;
        if(sleepTime <= 0) {
          sleepTime = 0;
        }
      }
      if(sleepTime) {
        Sleep((DWORD)sleepTime);
      }
    }
    if(m_wantedCPULoad == 0) {
      if(m_infoWnd) {
        m_infoWnd->SetWindowText(EMPTYSTRING);
      }
      suspend();
    } else {
      if(m_infoWnd) {
        m_infoWnd->SetWindowText(format(_T("%5.1lf%%/%5.1lf%%.  Sleep %.0lf msec."), measuredCPULoad*100, m_wantedCPULoad*100, sleepTime).cstr());
      }
    }
  }
}
