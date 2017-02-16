#include "stdafx.h"
#include "windiff.h"
#include "Edit2Lines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEdit2Lines::CEdit2Lines() {
  m_diff = NULL;
}

CEdit2Lines::~CEdit2Lines() {
}

BEGIN_MESSAGE_MAP(CEdit2Lines, CEdit)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void CEdit2Lines::substituteControl(CWnd *parent, int id, const StrDiff &diff) {
  DEFINEMETHODNAME;
  m_diff = &diff;
  CEdit *oldCtrl = (CEdit*)parent->GetDlgItem(id);
  if(oldCtrl == NULL) {
    Message(_T("%s:Control with id=%d does not exist"), method, id);
    return;
  }
  const String s = getWindowText(oldCtrl);
  CRect wr = getWindowRect(oldCtrl);
  const DWORD style   = oldCtrl->GetStyle();
  const DWORD exStyle = oldCtrl->GetExStyle();
  CFont      *font    = oldCtrl->GetFont();
  if(font == NULL) {
    font = parent->GetFont();
  }
  const CompactIntArray tabOrder = getTabOrder(parent);

  oldCtrl->DestroyWindow();

  if(!Create(style, wr, parent, id)) {
    Message(_T("%s:Create failed"), method);
    return;
  }
  setTabOrder(parent, tabOrder);

  ModifyStyleEx(0, exStyle);
  SetFont(font);
  setWindowText(this, s);
}

static COLORREF colors[] = {
  GetSysColor(COLOR_BTNFACE)
 ,RGB(255,174,201),
  LIGHTBLUE  ,
  LIGHTGREEN
};

void CEdit2Lines::OnPaint() {

  CPaintDC dc(this);
  CDC tmpDC;
  CBitmap bm;
  CSize sz = getClientRect(this).Size();
  tmpDC.CreateCompatibleDC(NULL);
  bm.CreateBitmap(sz.cx, sz.cy, 1, 32, NULL);
  tmpDC.SelectObject(&bm);
  tmpDC.FillSolidRect(0,0,sz.cx,sz.cy,colors[0]);

  paint(tmpDC);
  dc.BitBlt(0,0,sz.cx,sz.cy,&tmpDC,0,0,SRCCOPY);
}

void CEdit2Lines::paint(CDC &dc) {
  CompactIntArray attr = m_diff->d1;
  attr.add(0); // \r
  attr.add(0); // \n
  attr.addAll(m_diff->d2);
  CFont *oldFont = dc.SelectObject(GetFont());
  DWORD margins = GetMargins();
  const int leftMargin  = margins & 0xffff;
  const int rightMargin = margins >> 16;

  CRect r = getClientRect(this);
  CRgn clipRgn;
  clipRgn.CreateRectRgn(0,0,r.right,r.bottom);
  dc.SelectClipRgn(&clipRgn);
  const String str = getWindowText(this);
  const int length = (int)str.length();
  TCHAR chStr[2];
  chStr[1] = 0;
  for(int i = 0; i < length; i++) {
    chStr[0] = str[i];
    if((chStr[0] == _T('\r')) || (chStr[0] == _T('\n'))) {
      continue;
    }
    const CPoint pos = PosFromChar(i);
    if(r.PtInRect(pos)) {
      dc.SetBkColor(colors[attr[i]]);
      dc.TextOut(pos.x, pos.y, chStr, 1);
    }
  }

  dc.SelectObject(oldFont);
}
