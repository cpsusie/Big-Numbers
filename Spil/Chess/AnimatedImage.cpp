#include "stdafx.h"
#include <ThreadPool.h>
#include <MFCUtil/WinTools.h>
#include "AnimatedImage.h"

AnimatedImage::AnimatedImage(int resId, UINT imageCount) {
  if(imageCount == 0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("imageCount=0"));
  }
  const Image rawImage(resId);
  const int frameWidth  = rawImage.getWidth() / imageCount;
  const int frameHeight = rawImage.getHeight();

  for(UINT i = 0; i < imageCount; i++) {
    Image *frame = new Image(CSize(frameWidth, frameHeight)); TRACE_NEW(frame);
    frame->rop(0,0,frameWidth, frameHeight, SRCCOPY, &rawImage, i*frameWidth, 0);
    m_imageArray.add(frame);
  }
  setSecondsPerCycle(0.25 * imageCount);
  m_wnd        = nullptr;
  m_background = nullptr;
}

AnimatedImage::~AnimatedImage() {
  for(size_t i = 0; i < m_imageArray.size(); i++) {
    SAFEDELETE(m_imageArray[i]);
  }
  releaseBackground();
}

void AnimatedImage::startAnimation(CWnd *wnd, const CPoint &p, double scale, bool removeWhenDone, int loopCount) {
  if(wnd == nullptr) {
    throwInvalidArgumentException(__TFUNCTION__, _T("wnd == nullptr"));
  }
  m_wnd            = wnd;
  m_point          = p;
  m_scale          = scale;
  m_continue       = true;
  m_removeWhenDone = removeWhenDone;
  m_loopCount      = loopCount;
  ThreadPool::executeNoWait(*this);
}

void AnimatedImage::stopAnimation() {
  m_continue = false;
}

void AnimatedImage::saveBackground() {
  if(m_background) {
    return;
  }
  const CSize frameSize = getFrameSize();
  m_background = new Image(frameSize); TRACE_NEW(m_background);
  CClientDC dc(m_wnd);
  PixRect::bitBlt(m_background, ORIGIN, frameSize, SRCCOPY, dc, m_point);
}

void AnimatedImage::restoreBackground() {
  if(!m_background) {
    return;
  }
  CClientDC dc(m_wnd);
  PixRect::bitBlt(dc, m_point, m_background->getSize(), SRCCOPY, m_background, ORIGIN);

  releaseBackground();
}

void AnimatedImage::releaseBackground() {
  SAFEDELETE(m_background);
}

CSize AnimatedImage::getFrameSize() const {
  return m_imageArray[0]->getSize() * m_scale;
}

int AnimatedImage::getFrameCount() const {
  return (int)m_imageArray.size();
}

void AnimatedImage::setSecondsPerCycle(double secondsPerCycle, double pauseAtEnd) {
  if(secondsPerCycle <= 0 || pauseAtEnd < 0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("(%lg,%lg)"), secondsPerCycle, pauseAtEnd);
  }
  m_secondsPerCycle = secondsPerCycle;
  m_pauseAtEnd      = pauseAtEnd;
  m_sleepTimeMs     = m_secondsPerCycle / getFrameCount() * 1000;
  m_sleepAtEnd      = m_sleepTimeMs + m_pauseAtEnd * 1000;
}

UINT AnimatedImage::run() {
  for(;;) {
    saveBackground();
    CClientDC dc(m_wnd);
    m_frameIndex = 0;
    while(m_continue) {
      m_imageArray[m_frameIndex]->paintImage(dc, m_point, m_scale);
      m_frameIndex = (m_frameIndex+1) % getFrameCount();
      if(m_frameIndex == 0) {
        switch(m_loopCount) {
        case ANIMATE_INFINITE:
          break;
        case 0:
        case 1:
          m_continue = false;
          break;
        default:
          m_loopCount--;
          break;
        }
      }
      Sleep((ULONG)(m_frameIndex ? m_sleepTimeMs : m_sleepAtEnd));
    }
    if(m_removeWhenDone) {
      restoreBackground();
    }
  }
}
