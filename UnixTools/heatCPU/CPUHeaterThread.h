#pragma once

#include <Thread.h>

class CPUHeaterThread : public Thread {
private:
  double            m_wantedCPULoad;
  DWORD             m_lastRealTime;
  double            m_lastThreadTime;
  double            m_lastCPULoad;
  CWnd             *m_infoWnd;
  char             *m_buffer;
  void infiniteLoop();
public:
  CPUHeaterThread();
  void setInfoWindow(CWnd *wnd);
  void setCPULoad(double wantedCPULoad); // = [0..1]
  unsigned int run();
};

