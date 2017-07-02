#include "stdafx.h"
#include "CharacterMarker.h"

CharacterMarker::CharacterMarker(CDialog *dlg, int ctrlId, int bitmapId, bool above)
: m_ctrlId(ctrlId)
, m_above(above)
, m_markSet(100)
{
  m_dlg = dlg;
  m_markBitmap.LoadBitmap(bitmapId);
  m_bitmapSize        = getBitmapSize(m_markBitmap);
  m_backgroundColor   = ::GetSysColor(COLOR_BTNFACE);
  m_multiMarksAllowed = false;
  CDC dc;
  dc.CreateCompatibleDC(NULL);
  CBrush brush;
  brush.CreateSolidBrush(m_backgroundColor);
  dc.SelectObject(&brush);
  CBitmap *oldBitmap = dc.SelectObject(&m_markBitmap);
  dc.ExtFloodFill(0,0, RGB(255,255,255), FLOODFILLSURFACE);
  dc.ExtFloodFill(m_bitmapSize.cx-1,0, RGB(255,255,255), FLOODFILLSURFACE);
  dc.SelectObject(oldBitmap);
  CFont *font = m_dlg->GetDlgItem(m_ctrlId)->GetFont();
  if(font == NULL) {
    font = dlg->GetFont();
  }
  dc.SelectObject(font);
  saveCtrlRect();;
  m_currentCharMark = -1;
  m_charWidth       = getTextExtent(dc, _T("FFF")).cx - getTextExtent(dc, _T("FF")).cx;
}

void CharacterMarker::setMark(size_t index) {
  unmarkCurrentChar();
  saveCtrlRect();
  saveTextLength();
  m_currentCharMark = (int)index;
  markPosition(CClientDC(m_dlg), index, true);
}

void CharacterMarker::unmarkCurrentChar() {
  if(m_currentCharMark >= 0) {
    markPosition(CClientDC(m_dlg), m_currentCharMark, false);
    m_currentCharMark = -1;
  }
}

void CharacterMarker::setMultiMarksAllowed(bool multiMarksAllowed) {
  if(multiMarksAllowed != m_multiMarksAllowed) {
    if(m_multiMarksAllowed) {
      unmarkAll();
    } else {
      unmarkCurrentChar();
    }
    m_multiMarksAllowed = multiMarksAllowed;
  }
}

void CharacterMarker::unmarkAll() {
  CClientDC dc(m_dlg);
  if(m_multiMarksAllowed) {
    for(Iterator<size_t> it = m_markSet.getIterator(); it.hasNext();) {
      markPosition(dc, it.next(), false);
    }
    m_markSet.clear();
  } else {
    unmarkCurrentChar();
  }
}

void CharacterMarker::setMarks(const BitSet &s) {
  if(m_multiMarksAllowed && (s != m_markSet)) {
    unmarkAll();
    saveCtrlRect();
    saveTextLength();
    m_markSet = s;
    CClientDC dc(m_dlg);
    for(Iterator<size_t> it = m_markSet.getIterator(); it.hasNext();) {
      markPosition(dc, it.next(), true);
    }
  }
}

void CharacterMarker::markPosition(CDC &dc, size_t index, bool mark) {
  if(index > m_textLength) {
    return;
  }
  const CPoint pos = getMarkPosition(index);
  if(mark) {
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
