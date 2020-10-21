#include "pch.h"

LayoutAttribute::LayoutAttribute(SimpleLayoutManager *manager, int ctrlId, int flags)
: m_manager(*manager)
, m_ctrlId(  ctrlId )
, m_flags(   flags  )
{
  m_startRect        = m_lastRect = getControlRect();
  m_startFont        = m_font = getControl()->GetFont();
  m_currentFontScale = 1;
}

LayoutAttribute::~LayoutAttribute() {
  setFont(nullptr);
}

CWnd *LayoutAttribute::getControl() {
  return m_manager.getChild(m_ctrlId);
}

const CWnd *LayoutAttribute::getControl() const {
  return m_manager.getChild(m_ctrlId);
}

CRect LayoutAttribute::getControlRect() const {
  return getWindowRect(m_manager.getWindow(), m_ctrlId);
}

bool LayoutAttribute::isControlWindowChanged() const {
  return (m_lastRect != getControlRect()) ? true : false;
}

void LayoutAttribute::resetStartRect() {
  m_startRect = m_lastRect = getControlRect();
}

void LayoutAttribute::setControlRect(const CRect &r) {
  if(r.right <= r.left || r.bottom <= r.top) {
    return;
  }
  CWnd *ctrlWin = getManager().getWindow()->GetDlgItem(m_ctrlId);
  if(ctrlWin->IsWindowVisible()) {
    ::setWindowRect(ctrlWin, r);
  } else {
    ctrlWin->SetWindowPos(nullptr, r.left, r.top, r.Width(), r.Height(), SWP_HIDEWINDOW);
  }
  m_lastRect = r;
}

void LayoutAttribute::setFont(CFont *font) {
  if(m_font && (m_font != m_startFont)) {
    m_font->DeleteObject();
    SAFEDELETE(m_font);
  }
  m_font = font;
}

void LayoutAttribute::setFontScale(double scale) {
  if(scale <= 0) {
    return;
  }
  if(scale == 1) {
    setFont(m_startFont);
  } else {
    setFont(LayoutManager::createScaledFont(*m_startFont, scale));
  }
  m_currentFontScale = scale;
}
