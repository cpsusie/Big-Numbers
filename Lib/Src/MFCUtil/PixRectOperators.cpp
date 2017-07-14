#include "pch.h"
#include <MFCUtil/PixRect.h>

PixRectOperator::PixRectOperator(PixRect *pr) {
  init();
  setPixRect(pr);
}

void PixRectOperator::init() {
  m_pixRect       = NULL;
  m_pixelAccessor = NULL;
}

void PixRectOperator::setPixRect(PixRect *pixRect) {
  releasePixelAccessor();
  m_pixRect = pixRect;
  if(m_pixRect != NULL) {
    m_pixelAccessor = m_pixRect->getPixelAccessor();
  }
}

void PixRectOperator::releasePixelAccessor() {
  if(m_pixelAccessor) {
    m_pixRect->releasePixelAccessor();
    m_pixelAccessor = NULL;
  }
}

PixRectOperator::~PixRectOperator() {
  setPixRect(NULL);
}

void PixRectFilter::init() {
  m_result              = NULL;
  m_resultPixelAccessor = NULL;
}

void PixRectFilter::setPixRect(PixRect *pr) {
  if(pr == NULL) {
    releasePixelAccessor();
    if(m_result && (m_result != m_pixRect)) {
      m_pixRect->rop(m_pixRect->getRect(),SRCCOPY,m_result,ORIGIN);
      SAFEDELETE(m_result);
    }
  }
  __super::setPixRect(pr);
  m_result              = m_pixRect;
  m_resultPixelAccessor = m_pixelAccessor;
}

void PixRectFilter::releasePixelAccessor() {
  if(m_resultPixelAccessor && (m_resultPixelAccessor != m_pixelAccessor)) {
    m_result->releasePixelAccessor();
    m_resultPixelAccessor = NULL;
  }
  __super::releasePixelAccessor();
}

CRect PixRectFilter::getRect() const {
  return (m_pixRect == NULL) ? CRect(0,0,0,0) : CRect(0,0,m_pixRect->getWidth(),m_pixRect->getHeight());
}

void SetColor::apply(const CPoint &p) {
  if(m_pixRect->contains(p)) {
    m_pixelAccessor->setPixel(p, m_color);
  }
}

void SetAlpha::apply(const CPoint &p) {
  if(m_pixRect->contains(p)) {
    const D3DCOLOR color  = ARGB_TORGB(m_pixelAccessor->getPixel(p)) | m_alphaMask;
    m_pixelAccessor->setPixel(p, color);
  }
}

void SubstituteColor::apply(const CPoint &p) {
  if(m_pixelAccessor->getPixel(p) == m_from) {
    m_pixelAccessor->setPixel(p,m_to);
  }
}

void InvertColor::apply(const CPoint &p) {
  if(m_pixRect->contains(p)) {
    m_pixelAccessor->setPixel(p,~m_pixelAccessor->getPixel(p));
  }
}
