#include "stdafx.h"
#include "MandelbrotDlg.h"

MBFrameGenerator::MBFrameGenerator(CMandelbrotDlg *dlg, const String &dirName)
: m_dlg(*dlg)
, m_dirName(dirName)
, m_finalRect(dlg->getTransformation().getFromRectangle())
, m_imageSize(dlg->getImageSize())
, m_frameReady(0)
{
  m_dlg.initScale();
  m_startRect       = m_dlg.getTransformation().getFromRectangle();
  m_totalFrameCount = findTotalFrameCount(m_startRect, m_finalRect);
  m_frameIndex      = 0;
  m_expTransform    = new ExpTransformation(RealInterval(0, m_totalFrameCount), RealInterval(1,m_finalRect.getWidth()/m_startRect.getWidth()));
  m_linearTransform = new RealLinearTransformation(m_expTransform->getToInterval(), RealInterval(1,0));

  HDC screenDC = getScreenDC();
  m_dc = CreateCompatibleDC(screenDC);
  m_bm = CreateCompatibleBitmap(screenDC, m_imageSize.cx, m_imageSize.cy);
  DeleteDC(screenDC);

  m_imageListThread  = new ImageListThread(this);
  m_imageListThread->start();
}

MBFrameGenerator::~MBFrameGenerator() {
  DLOG((_T("enter ~MBFrameGenerator\n")));
  m_frameIndex = m_totalFrameCount + 10;
  m_frameReady.signal();
  while(m_imageListThread->stillActive()) {
    Sleep(50);
  }
  delete m_imageListThread;
  DeleteObject(m_bm);
  DeleteDC(m_dc);
  delete m_expTransform;
  delete m_linearTransform;
  DLOG((_T("leave ~MBFrameGenerator\n")));
}

#define ZOOMSTEP 0.03

ExpTransformation::ExpTransformation(const RealInterval &from, const RealInterval &to) : m_toInterval(to) {
  m_a = root(to.getTo() / to.getFrom(), from.getTo()-from.getFrom());
  m_b = to.getFrom() / pow(m_a, from.getFrom());
}

int MBFrameGenerator::findTotalFrameCount(const RealRectangle &startRect, const RealRectangle &finalRect) { // static
  const Real l0 = startRect.getWidth();
  const Real lf = finalRect.getWidth();
  return getInt((log(lf/l0) / log(Double80::one - ZOOMSTEP))) + 1;
}

RealRectangle MBFrameGenerator::getInterpolatedRectangle() const {
  const Real fw = m_expTransform->transform(m_frameIndex);       // 1 -> finalWidth/startWidth
  const Real t1 = m_linearTransform->forwardTransform(fw);       // 1 -> 0
  const Real t2 = (1 - t1);                                      // 0 -> 1
  const Real x = t1 * m_startRect.x + t2 * m_finalRect.x;
  const Real y = t1 * m_startRect.y + t2 * m_finalRect.y;
  const Real w = fw * m_startRect.getWidth();
  const Real h = fw * m_startRect.getHeight();
  return RealRectangle(x,y,w,h);
}

HBITMAP MBFrameGenerator::nextBitmap() { // should return NULL when no more frames.
  DLOG((_T("enter nextBitmap\n")));
  if(!requestNextFrame()) return NULL;
  DLOG((_T("got frame %d\n"), m_frameIndex));

  HGDIOBJ oldGDI = SelectObject(m_dc, m_bm);
  CClientDC imageDC(m_dlg.getImageWindow());
  BitBlt(m_dc, 0,0,m_imageSize.cx, m_imageSize.cy, imageDC, 0,0,SRCCOPY);
  SelectObject(m_dc, oldGDI);
  DLOG((_T("BitBlt succeeded\n")));
  m_dlg.setScale(getInterpolatedRectangle(), true);
  m_frameIndex++;
  DLOG((_T("zoom succeeded\n")));
  DLOG((_T("leave nextBitmap\n")));
  return m_bm;
}

bool MBFrameGenerator::requestNextFrame() { // return false if done
  if(m_frameIndex > m_totalFrameCount) {
    postMovieDone();
    return false;
  }
  m_dlg.remoteStartCalculation();
  m_frameReady.wait();
  if(m_frameIndex > m_totalFrameCount) {
    postMovieDone();
    return false;
  }
  return true;
}

void MBFrameGenerator::postMovieDone() {
  DLOG((_T("Post MOVIEDONE\n")));
  m_dlg.PostMessage(ID_MSG_MOVIEDONE);
}

