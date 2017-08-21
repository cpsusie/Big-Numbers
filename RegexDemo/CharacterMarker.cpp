#include "stdafx.h"
#include "CharacterMarker.h"

CharacterMarker::CharacterMarker(CDialog *dlg, int ctrlId, int bitmapId, bool above)
  : m_ctrlId(ctrlId)
  , m_above(above)
  , m_multiMarksAllowed(false)
  , m_marksVisible(true)
  , m_markSet(64)
  , m_blinkTimer(NULL)
{
  m_dlg = dlg;
  m_markBitmap.LoadBitmap(bitmapId);
  m_bitmapSize = getBitmapSize(m_markBitmap);
  m_backgroundColor = ::GetSysColor(COLOR_BTNFACE);
  CDC dc;
  dc.CreateCompatibleDC(NULL);
  CBrush brush;
  brush.CreateSolidBrush(m_backgroundColor);
  dc.SelectObject(&brush);
  CBitmap *oldBitmap = dc.SelectObject(&m_markBitmap);
  dc.ExtFloodFill(0, 0, RGB(255, 255, 255), FLOODFILLSURFACE);
  dc.ExtFloodFill(m_bitmapSize.cx - 1, 0, RGB(255, 255, 255), FLOODFILLSURFACE);
  dc.SelectObject(oldBitmap);
  CFont *font = m_dlg->GetDlgItem(m_ctrlId)->GetFont();
  if (font == NULL) {
    font = dlg->GetFont();
  }
  dc.SelectObject(font);
  saveCtrlRect();
  m_charWidth = getTextExtent(dc, _T("FFF")).cx - getTextExtent(dc, _T("FF")).cx;
}

CharacterMarker::~CharacterMarker() {
  SAFEDELETE(m_blinkTimer);
}

void CharacterMarker::setMark(size_t index) {
  if(index >= m_markSet.getCapacity()) {
    m_gate.wait();
    m_markSet.setCapacity(index + 10);
    m_gate.signal();
  }
  if(!m_markSet.contains(index)) {
    if(!m_multiMarksAllowed) {
      unmarkAll();
    }
    m_gate.wait();
    m_markSet.add(index);
    m_gate.signal();

    saveCtrlRect();
    saveTextLength();
    paintMarkPosition(CClientDC(m_dlg), index, true);
  }
}

void CharacterMarker::setMultiMarksAllowed(bool multiMarksAllowed) {
  if(multiMarksAllowed != m_multiMarksAllowed) {
    if(m_multiMarksAllowed) {
      unmarkAll();
    }
    m_multiMarksAllowed = multiMarksAllowed;
  }
}

void CharacterMarker::setBlinking(bool on, int msec) {
  if(on) {
    if(m_blinkTimer == NULL) {
      m_blinkTimer = new Timer(1); TRACE_NEW(m_blinkTimer);
    }
    m_blinkTimer->startTimer(msec, *this, true);
  } else if(isBlinking()) {
    m_blinkTimer->stopTimer();
    setMarksVisible(true);
  }
}

void CharacterMarker::handleTimeout(Timer &timer) {
  setMarksVisible(!m_marksVisible);
}

void CharacterMarker::setMarksVisible(bool visible) {
  m_gate.wait();
  m_marksVisible = visible;
  if(!m_markSet.isEmpty()) {
    paintAllMarkPositions(m_marksVisible);
  }
  m_gate.signal();
}

void CharacterMarker::unmarkAll() {
  if(m_markSet.isEmpty()) {
    return;
  }
  m_gate.wait();
  paintAllMarkPositions(false);
  m_markSet.clear();
  m_gate.signal();
}

void CharacterMarker::setMarks(const BitSet &s) {
  if(m_multiMarksAllowed && (s != m_markSet)) {
    unmarkAll();
    saveCtrlRect();
    saveTextLength();
    m_gate.wait();
    m_markSet = s;
    paintAllMarkPositions(true);
    m_gate.signal();
  }
}

void CharacterMarker::paintAllMarkPositions(bool mark) {
  CClientDC dc(m_dlg);
  for(Iterator<size_t> it = m_markSet.getIterator(); it.hasNext();) {
    paintMarkPosition(dc, it.next(), mark);
  }
}

void CharacterMarker::paintMarkPosition(CDC &dc, size_t index, bool mark) {
  if(index > m_textLength) {
    return;
  }
  const CPoint pos = getMarkPosition(index);
  if(mark && m_marksVisible) {
    CDC srcDC;
    srcDC.CreateCompatibleDC(NULL);
    srcDC.SelectObject(&m_markBitmap);
    dc.BitBlt(pos.x, pos.y, m_bitmapSize.cx, m_bitmapSize.cy, &srcDC, 0,0, SRCCOPY);
  } else {
    dc.FillSolidRect(pos.x, pos.y, m_bitmapSize.cx, m_bitmapSize.cy, m_backgroundColor);
  }
}

#define LEFTMARGIN 4

CPoint CharacterMarker::getMarkPosition(size_t index) const {
  const int charLeft   = (int)index * m_charWidth + LEFTMARGIN;
  const int charMiddle = charLeft + m_charWidth/2;
  return CPoint(m_ctrlRect.left + charMiddle-m_bitmapSize.cx/2, m_above ? (m_ctrlRect.top - m_bitmapSize.cy) : m_ctrlRect.bottom);
}

void CharacterMarker::saveCtrlRect() {
  CComboBox *combo = (CComboBox*)m_dlg->GetDlgItem(m_ctrlId);
  m_ctrlRect = getWindowRect(combo);
}

void CharacterMarker::saveTextLength() {
  m_textLength = getWindowText(m_dlg, m_ctrlId).length();
}
