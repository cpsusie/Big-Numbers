#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include <MFCUtil/DeleteButton.h>
#include <MFCUtil/resource.h>

void CDeleteButton::Create(CWnd *parent, const CPoint &pos, UINT id, bool tabStop) {
  int flags = WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_OWNERDRAW;
  if(tabStop) flags |= WS_TABSTOP;

  if(!CBitmapButton::Create(EMPTYSTRING
                           ,flags
                           ,CRect(pos.x,pos.y,pos.x+16,pos.y+16)
                           ,parent
                           ,id)) {
    throwException(_T("%s:CBitmapButton::Create failed"), __TFUNCTION__);
  }
  loadBitmap(m_bitmap        ,_IDB_DELETEBUTTON_BITMAP );
  loadBitmap(m_bitmapSel     ,_IDB_DELETEBUTTON_BITMAPD);
  loadBitmap(m_bitmapFocus   ,_IDB_DELETEBUTTON_BITMAP );
  loadBitmap(m_bitmapDisabled,_IDB_DELETEBUTTON_BITMAPI);
  SizeToContent();
  SetPosition(pos.x,pos.y,false);
}

void CDeleteButton::loadBitmap(CBitmap &bm, int id) {
  if(id) {
    if(!bm.LoadBitmap(id)) {
      AfxMessageBox(format(_T("LoadBitmap(%d) failed"), id).cstr(), MB_ICONWARNING);
    }
  }
};

void CDeleteButton::SetPosition(int x, int y, bool redraw) {
  int flags = SWP_NOZORDER | SWP_NOSIZE;
  if(!redraw) flags |= SWP_NOREDRAW;
  CBitmapButton::SetWindowPos(&wndTop,x,y,0,0,flags);
}
