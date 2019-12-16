#pragma once

#include <Runnable.h>

class SortPanelWnd;

typedef enum {
  STATE_CREATED  // inital state
 ,STATE_IDLE
 ,STATE_RUNNING
 ,STATE_PAUSED
 ,STATE_ERROR
 ,STATE_KILLED
} SortJobState;

#define PAUSE_SORT     0x01
#define TERMINATE_SORT 0x02
#define KILL_THREAD    0x04

class SortJob : public Runnable {
private:
  Thread       *m_thread;
  SortPanelWnd &m_wnd;
  FastSemaphore m_terminated;
  void doSort();
  void doAnimatedSort();
public:
  SortJob(SortPanelWnd *wnd);
  ~SortJob();
  UINT run();
  double getTimeUsage() const;
};
