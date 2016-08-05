#include "stdafx.h"
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
  m_pixRect = pixRect;
  if(m_pixelAccessor != NULL) {
    delete m_pixelAccessor;
    m_pixelAccessor = NULL;
  }
  
  if(m_pixRect != NULL) {
    m_pixelAccessor = PixelAccessor::createPixelAccessor(m_pixRect);
  }
}

PixRectOperator::~PixRectOperator() {
  setPixRect(NULL);
}


void PixRectFilter::init() {
  m_result              = NULL;
  m_resultPixelAccessor = NULL;
}

void PixRectFilter::setPixRect(PixRect *pixRect) {
  PixRectOperator::setPixRect(pixRect);
  m_result              = m_pixRect;
  m_resultPixelAccessor = m_pixelAccessor;
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
