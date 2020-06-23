#include "pch.h"
#include <D3DGraphics/AbstractAnimator.h>

AbstractAnimator::AbstractAnimator(UINT frameCount, const String &name) : m_timer(1, format(_T("Animator for %s"), name.cstr())) {
  init(frameCount);
}

void AbstractAnimator::init(UINT frameCount) {
  m_animationType = ANIMATE_FORWARD;
  m_sleepTime     = 50;
  m_forward       = true;
  m_running       = false;
  setFrameCount(frameCount);
}

AbstractAnimator &AbstractAnimator::setFrameCount(UINT frameCount) {
  if(isRunning()) {
    stopAnimation();
  }
  m_frameCount   = frameCount;
  m_currentIndex = 0;
  return *this;
}

AbstractAnimator::~AbstractAnimator() {
  stopAnimation();
}

AbstractAnimator &AbstractAnimator::startAnimation(AnimationType type) {
  if(isRunning() || (getFrameCount() == 0)) {
    return *this;
  }
  m_animationType = type;
  m_timer.startTimer(getSleepTime(), *this, true);
  m_running = true;
  return *this;
}

void AbstractAnimator::stopAnimation() {
  if(isRunning()) {
    m_timer.stopTimer();
    m_running = false;
  }
}

AbstractAnimator &AbstractAnimator::scaleSpeed(float factor) {
  if((factor == 0) || (factor == 1)) {
    return *this;
  }
  m_sleepTime /= factor;
  if(m_sleepTime < 1) {
    m_sleepTime = 1;
  }
  if(isRunning()) {
    m_timer.setTimeout(getSleepTime(), true);
  }
  return *this;
}

void AbstractAnimator::handleTimeout(Timer &t) {
  nextIndex();
  if(getAnimationType() == ANIMATE_ALTERNATING) {
    m_timer.setTimeout(getSleepTime(), true);
  }
}

int AbstractAnimator::getSleepTime() const {
  switch(getAnimationType()) {
  case ANIMATE_FORWARD    :
  case ANIMATE_BACKWARD   :
    return (int)m_sleepTime;
  case ANIMATE_ALTERNATING:
    { const double m = (double)m_frameCount/2;
      return (int)(m_sleepTime * (1.0 + sqr(fabs((double)m_currentIndex - m) / m*2)));
    }
  }
  return (int)m_sleepTime;
}

//  Invariant:m_currentIndex = [0..m_frameCount-1]
void AbstractAnimator::nextIndex() {
  assert((0 <= m_currentIndex) && (m_currentIndex < m_frameCount));
  switch(getAnimationType()) {
  case ANIMATE_FORWARD    :
    m_currentIndex = (m_currentIndex + 1) % m_frameCount;
    break;
  case ANIMATE_BACKWARD   :
    if(m_currentIndex-- == 0) {
      m_currentIndex = m_frameCount-1;
    }
    break;
  case ANIMATE_ALTERNATING:
    if(m_forward) {
      if(++m_currentIndex == m_frameCount-1) {
        m_forward = false;
      }
    } else if(--m_currentIndex == 0) {
      m_forward = true;
    }
    break;
  }
}
