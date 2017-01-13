#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include <MFCUtil/OBMButton.h>

void OBMButton::Create(CWnd *parent, int res, int resD, int resI, const CPoint &pos, UINT id, bool tabStop) {
  int flags = WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_OWNERDRAW;
  if(tabStop) flags |= WS_TABSTOP;

  if(!CBitmapButton::Create(EMPTYSTRING
                           ,flags
                           ,CRect(pos.x,pos.y,pos.x+16,pos.y+16)
                           ,parent
                           ,id)) {
    throwException(_T("%s:CBitmapButton::Create failed"), __TFUNCTION__);
  }
  loadBitmap(m_bitmap        ,res );
  loadBitmap(m_bitmapSel     ,resD);
  loadBitmap(m_bitmapFocus   ,res );
  loadBitmap(m_bitmapDisabled,resI);
  SizeToContent();
  SetPosition(pos.x,pos.y,false);
}

void OBMButton::loadBitmap(CBitmap &bm, int id) {
  if(id) {
    if(!bm.LoadOEMBitmap(id)) {
      AfxMessageBox(format(_T("%s:LoadOEMBitmap(%d) failed"), __TFUNCTION__, id).cstr(), MB_ICONWARNING);
    }
  }
};

void OBMButton::SetPosition(int x, int y, bool redraw) {
  int flags = SWP_NOZORDER | SWP_NOSIZE;
  if(!redraw) flags |= SWP_NOREDRAW;
  CBitmapButton::SetWindowPos(&wndTop,x,y,0,0,flags);
}
