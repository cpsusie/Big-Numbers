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
  m_expTransform    = new ExpTransformation(MBInterval(0, m_totalFrameCount), MBInterval(1,m_finalRect.getWidth()/m_startRect.getWidth())); TRACE_NEW(m_expTransform   );
  m_linearTransform = new MBLinearTransformation(m_expTransform->getToInterval(), MBInterval(1,0));                                         TRACE_NEW(m_linearTransform);

  HDC screenDC = getScreenDC();
  m_dc = CreateCompatibleDC(screenDC);
  m_bm = CreateCompatibleBitmap(screenDC, m_imageSize.cx, m_imageSize.cy);
  DeleteDC(screenDC);

  m_imageListThread  = new ImageListThread(this); TRACE_NEW(m_imageListThread);
  m_imageListThread->start();
}

MBFrameGenerator::~MBFrameGenerator() {
  DLOG(_T("enter ~MBFrameGenerator\n"));
  m_frameIndex = m_totalFrameCount + 10;
  m_frameReady.signal();
  while(m_imageListThread->stillActive()) {
    Sleep(50);
  }
  SAFEDELETE(m_imageListThread);
  DeleteObject(m_bm);
  DeleteDC(m_dc);
  SAFEDELETE(m_expTransform);
  SAFEDELETE(m_linearTransform);
  DLOG(_T("leave ~MBFrameGenerator\n"));
}

#define ZOOMSTEP 0.0078125

ExpTransformation::ExpTransformation(const MBInterval &from, const MBInterval &to) : m_toInterval(to) {
  m_a = root(to.getTo() / to.getFrom(), from.getTo()-from.getFrom());
  m_b = to.getFrom() / pow(m_a, from.getFrom());
}

int MBFrameGenerator::findTotalFrameCount(const MBRectangle2D &startRect, const MBRectangle2D &finalRect) { // static
  const MBReal l0 = startRect.getWidth();
  const MBReal lf = finalRect.getWidth();
  return getInt((log2(lf/l0) / log2(1.0 - ZOOMSTEP))) + 1;
}

MBRectangle2D MBFrameGenerator::getInterpolatedRectangle() const {
  const MBReal fw = m_expTransform->transform(m_frameIndex);       // 1 -> finalWidth/startWidth
  const MBReal t1 = m_linearTransform->forwardTransform(fw);       // 1 -> 0
  const MBReal t2 = (1 - t1);                                      // 0 -> 1
  const MBReal x  = t1 * m_startRect.m_x + t2 * m_finalRect.m_x;
  const MBReal y  = t1 * m_startRect.m_y + t2 * m_finalRect.m_y;
  const MBReal w  = fw * m_startRect.getWidth();
  const MBReal h  = fw * m_startRect.getHeight();
  return MBRectangle2D(x,y,w,h);
}

HBITMAP MBFrameGenerator::nextBitmap() { // should return NULL when no more frames.
  DLOG(_T("enter nextBitmap\n"));
  if(!requestNextFrame()) return NULL;
  DLOG(_T("got frame %d\n"), m_frameIndex);

  HGDIOBJ oldGDI = SelectObject(m_dc, m_bm);
  CClientDC imageDC(m_dlg.getImageWindow());
  BitBlt(m_dc, 0,0,m_imageSize.cx, m_imageSize.cy, imageDC, 0,0,SRCCOPY);
  SelectObject(m_dc, oldGDI);
  DLOG(_T("BitBlt succeeded\n"));
  m_dlg.setScale(getInterpolatedRectangle(), true);
  m_frameIndex++;
  DLOG(_T("zoom succeeded\n"));
  DLOG(_T("leave nextBitmap\n"));
  return m_bm;
}

bool MBFrameGenerator::requestNextFrame() {
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
  DLOG(_T("Post MOVIEDONE\n"));
  m_dlg.PostMessage(ID_MSG_MOVIEDONE);
}
