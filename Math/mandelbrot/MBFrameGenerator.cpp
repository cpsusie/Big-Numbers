#include "stdafx.h"
#include <ThreadPool.h>
#include "MandelbrotDlg.h"
#include "MBBigRealCalculator.h"

MBFrameGenerator::MBFrameGenerator(CMandelbrotDlg *dlg, const String &dirName)
: m_dlg(*dlg)
, m_dirName(dirName)
, m_finalRect(dlg->getBigRealTransformation().getFromRectangle())
, m_frameSize(dlg->getWindowSize())
, m_frameReady(0)
{
  DigitPool *dp = getDigitPool();
  m_dlg.initScale();
  m_startRect       = m_dlg.getBigRealTransformation().getFromRectangle();
  m_totalFrameCount = findTotalFrameCount(m_startRect, m_finalRect);
  m_frameIndex      = 0;
  BigRealInterval frameInterval(dp), zoomInterval(dp), unitInterval(dp);
  frameInterval     = RealInterval(0,m_totalFrameCount);
  zoomInterval      = RealInterval(1,getReal(m_finalRect.getWidth())/getReal(m_startRect.getWidth()));
  unitInterval      = IntInterval( 1,0);
  m_expTransform    = new ExpTransformation(frameInterval, zoomInterval, dp); TRACE_NEW(m_expTransform);
  m_linearTransform = new BigRealLinearTransformation(m_expTransform->getToInterval(), unitInterval,AUTOPRECISION,dp);
  TRACE_NEW(m_linearTransform);

  HDC screenDC = getScreenDC();
  m_dc = CreateCompatibleDC(screenDC);
  m_bm = CreateCompatibleBitmap(screenDC, m_frameSize.cx, m_frameSize.cy);
  DeleteDC(screenDC);

  m_imageListJob  = new ImageListJob(this); TRACE_NEW(m_imageListJob);
  ThreadPool::executeNoWait(*m_imageListJob);
}

MBFrameGenerator::~MBFrameGenerator() {
  DEBUGLOG(_T("enter ~MBFrameGenerator\n"));
  m_frameIndex = m_totalFrameCount + 10;
  m_frameReady.notify();
  if(!m_imageListJob->isOk()) {
    showError(m_imageListJob->getErrorMsg());
  }
  SAFEDELETE(m_imageListJob);
  DeleteObject(m_bm);
  DeleteDC(m_dc);
  SAFEDELETE(m_expTransform);
  SAFEDELETE(m_linearTransform);
  DEBUGLOG(_T("leave ~MBFrameGenerator\n"));
}

#define ZOOMSTEP 0.0078125

ExpTransformation::ExpTransformation(const BigRealInterval &from, const BigRealInterval &to, DigitPool *digitPool)
: m_a(digitPool?digitPool:from.getDigitPool())
, m_b(digitPool?digitPool:from.getDigitPool())
, m_toInterval(to, digitPool?digitPool:from.getDigitPool())
{
  m_a = rRoot(rQuot(to.getTo(),to.getFrom(),20), from.getTo()-from.getFrom(),20);
  m_b = rQuot(to.getFrom(), rPow(m_a, from.getFrom(),20),20);
}

int MBFrameGenerator::findTotalFrameCount(const BigRealRectangle2D &startRect, const BigRealRectangle2D &finalRect) { // static
  const Double80 l0 = getDouble80(startRect.getWidth());
  const Double80 lf = getDouble80(finalRect.getWidth());
  return getInt((log2(lf/l0) / log2(1.0 - ZOOMSTEP))) + 1;
}

BigRealRectangle2D MBFrameGenerator::getInterpolatedRectangle() const {
  DigitPool *dp = getDigitPool();
  const BigReal fw = m_expTransform->transform(m_frameIndex);       // 1 -> finalWidth/startWidth
  const BigReal t1 = m_linearTransform->forwardTransform(fw);       // 1 -> 0
  const BigReal t2 = (dp->_1() - t1);                             // 0 -> 1
  const BigReal x  = t1 * m_startRect.m_x + t2 * m_finalRect.m_x;
  const BigReal y  = t1 * m_startRect.m_y + t2 * m_finalRect.m_y;
  const BigReal w  = fw * m_startRect.getWidth();
  const BigReal h  = fw * m_startRect.getHeight();
  return BigRealRectangle2D(x,y,w,h);
}

HBITMAP MBFrameGenerator::nextBitmap() { // should return NULL when no more frames.
  DEBUGLOG(_T("enter nextBitmap\n"));
  if(!requestNextFrame()) return NULL;
  DEBUGLOG(_T("got frame %d\n"), m_frameIndex);

  HGDIOBJ oldGDI = SelectObject(m_dc, m_bm);
  CClientDC imageDC(m_dlg.getImageWindow());
  BitBlt(m_dc, 0,0,m_frameSize.cx, m_frameSize.cy, imageDC, 0,0,SRCCOPY);
  SelectObject(m_dc, oldGDI);
  DEBUGLOG(_T("BitBlt succeeded\n"));
  m_dlg.setScale(getInterpolatedRectangle(), true);
  m_frameIndex++;
  DEBUGLOG(_T("zoom succeeded\n"));
  DEBUGLOG(_T("leave nextBitmap\n"));
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
  DEBUGLOG(_T("Post MOVIEDONE\n"));
  m_dlg.PostMessage(ID_MSG_MOVIEDONE);
}
