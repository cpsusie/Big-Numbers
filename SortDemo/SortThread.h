#pragma once

#include <Thread.h>

class SortPanelWnd;

typedef enum {
  STATE_IDLE
 ,STATE_RUNNING
 ,STATE_PAUSED
 ,STATE_KILLED
 ,STATE_ERROR
} SortThreadState;

#define PAUSE_SORT     0x01
#define TERMINATE_SORT 0x02
#define KILL_THREAD    0x04

class SortThread : public Thread {
private:
  SortPanelWnd &m_wnd;
  void doSort();
  void doAnimatedSort();
public:
  SortThread(SortPanelWnd *wnd) : m_wnd(*wnd), Thread(_T("SortThread")) {
    setDeamon(true);
  }
  unsigned int run();
};

