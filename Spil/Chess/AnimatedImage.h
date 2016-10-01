#pragma once

#include <Thread.h>
#include "Image.h"

#define ANIMATE_INFINITE -1

class AnimatedImage : private Thread {
private:
  ImageArray      m_imageArray;
  PixRect        *m_background;
  double          m_secondsPerCycle, m_pauseAtEnd; // seconds
  double          m_sleepTimeMs, m_sleepAtEnd;     // milliseconds
  CWnd           *m_wnd;
  CPoint          m_point;
  double          m_scale;
  int             m_frameIndex;
  int             m_loopCount;
  bool            m_continue, m_removeWhenDone;
  void saveBackground();
  void restoreBackground();
  void releaseBackground();
  CSize getFrameSize() const;
  int   getFrameCount() const;
public:
  AnimatedImage(int resId, unsigned int imageCount);
  ~AnimatedImage();
  unsigned int run();

  void startAnimation(CWnd *wnd, const CPoint &p, double scale, bool removeWhenDone, int loopCount = 1);
  void stopAnimation();
  void setSecondsPerCycle(double secondsPerCycle, double pauseAtEnd = 0);
  double getSecondsPerCycle() const {
    return m_secondsPerCycle;
  }
  double getPauseAtEnd() const {
    return m_pauseAtEnd;
  }
  void setPauseAtEnd(double pauseAtEnd) {
    setSecondsPerCycle(m_secondsPerCycle, pauseAtEnd);
  }
};

