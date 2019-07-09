#include "pch.h"

void BackgroundSaver::saveBackground(HDC hdc, const CRect &r) {
  const CSize rsz = r.Size();
  m_rect = r;
  if(getArea(r) <= 0) {
    return;
  }
  if(m_bm.m_hObject) {
    const CSize bmsz = getBitmapSize(m_bm);
    if((rsz.cx > bmsz.cx) || (rsz.cy > bmsz.cy)) {
      m_bm.DeleteObject();
    }
  }
  if(m_bm.m_hObject == NULL) {
    m_bm.CreateBitmap(rsz.cx, rsz.cy, GetDeviceCaps(hdc, PLANES), GetDeviceCaps(hdc, BITSPIXEL), NULL);
  }
  CDC tmpDC;
  tmpDC.CreateCompatibleDC(NULL);
  HGDIOBJ oldgdi = SelectObject(tmpDC, m_bm);
  BitBlt(tmpDC, 0, 0, rsz.cx, rsz.cy, hdc, r.left, r.top, SRCCOPY);
  SelectObject(tmpDC, oldgdi);
}

void BackgroundSaver::restoreBackground(HDC hdc) {
  if(hasSavedRect()) {
    CDC tmpDC;
    tmpDC.CreateCompatibleDC(NULL);
    HGDIOBJ oldgdi = SelectObject(tmpDC, m_bm);
    BitBlt(hdc, m_rect.left,m_rect.top,m_rect.Width(),m_rect.Height(), tmpDC, 0,0, SRCCOPY);
    SelectObject(tmpDC, oldgdi);
    initRect();
  }
}
