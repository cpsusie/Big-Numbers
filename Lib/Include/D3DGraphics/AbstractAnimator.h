#pragma once

#include <Timer.h>

typedef enum {
  ANIMATE_FORWARD
 ,ANIMATE_BACKWARD
 ,ANIMATE_ALTERNATING
} AnimationType;

class AbstractAnimator : public TimeoutHandler {
private:
  Timer         m_timer;
  UINT          m_frameCount;
  float         m_sleepTime;
  AnimationType m_animationType;
  bool          m_forward;
  bool          m_running;
  UINT          m_currentIndex;
  void nextIndex();
  int  getSleepTime() const;
  void init(UINT frameCount);
protected:
  AbstractAnimator(UINT frameCount = 0, const String &name = EMPTYSTRING);
  AbstractAnimator &setFrameCount(UINT n);
public:
  ~AbstractAnimator();
  AbstractAnimator &startAnimation(AnimationType type = ANIMATE_FORWARD);
  void stopAnimation();
  virtual void handleTimeout(Timer &timer);
  inline bool isRunning() const {
    return m_running;
  }
  inline AnimationType getAnimationType() const {
    return m_animationType;
  }
  // sleepTime /= factor
  AbstractAnimator &scaleSpeed(float factor);
  inline float getFramesPerSec() const {
    return m_running ? 1000.0f / m_sleepTime : 0;
  }
  inline UINT getFrameCount() const {
    return m_frameCount;
  }
  inline UINT getCurrentIndex() const {
    return m_currentIndex;
  }
};
