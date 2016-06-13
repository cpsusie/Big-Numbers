#include "stdafx.h"
#include <MyUtil.h>
#include <MFCUtil/WinTools.h>
#include "TestOBMBitmapsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTestOBMBitmapsDlg::CTestOBMBitmapsDlg(CWnd* pParent /*=NULL*/) : CDialog(CTestOBMBitmapsDlg::IDD, pParent) {
    //{{AFX_DATA_INIT(CTestOBMBitmapsDlg)
    //}}AFX_DATA_INIT
}

void CTestOBMBitmapsDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CTestOBMBitmapsDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestOBMBitmapsDlg, CDialog)
    //{{AFX_MSG_MAP(CTestOBMBitmapsDlg)
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CTestOBMBitmapsDlg::OnPaint()  {
  CDialog::OnPaint();
  paintOBMBitmaps();
}

#define OBM(id) id, _T(#id)

static const OBMResource resourceTable[] = {
  OBM(OBM_BTNCORNERS)
 ,OBM(OBM_BTSIZE)
 ,OBM(OBM_CHECK)
 ,OBM(OBM_CHECKBOXES)
 ,OBM(OBM_CLOSE)
 ,OBM(OBM_COMBO)
 ,OBM(OBM_DNARROW)
 ,OBM(OBM_DNARROWD)
 ,OBM(OBM_DNARROWI)
 ,OBM(OBM_LFARROW)
 ,OBM(OBM_LFARROWD)
 ,OBM(OBM_LFARROWI)
 ,OBM(OBM_MNARROW)
 ,OBM(OBM_OLD_CLOSE)
 ,OBM(OBM_OLD_DNARROW)
 ,OBM(OBM_OLD_LFARROW)
 ,OBM(OBM_OLD_REDUCE)
 ,OBM(OBM_OLD_RESTORE)
 ,OBM(OBM_OLD_RGARROW)
 ,OBM(OBM_OLD_UPARROW)
 ,OBM(OBM_OLD_ZOOM)
 ,OBM(OBM_REDUCE)
 ,OBM(OBM_REDUCED)
 ,OBM(OBM_RESTORE)
 ,OBM(OBM_RESTORED)
 ,OBM(OBM_RGARROW)
 ,OBM(OBM_RGARROWD)
 ,OBM(OBM_RGARROWI)
 ,OBM(OBM_SIZE)
 ,OBM(OBM_UPARROW)
 ,OBM(OBM_UPARROWD)
 ,OBM(OBM_UPARROWI)
 ,OBM(OBM_ZOOM)
 ,OBM(OBM_ZOOMD)
};


void CTestOBMBitmapsDlg::paintOBMBitmaps() {
  const CSize cs = getClientRect(this).Size();
  CPoint p(10,0);
  int lineHeight = 0;
  for(int i = 0; i < ARRAYSIZE(resourceTable); i++) {
    const CSize sz = paintOBMBitmap(resourceTable[i], p);
    p.x += sz.cx + 20;
    lineHeight = max(lineHeight, sz.cy);
    if(p.x + sz.cx > cs.cx - 50) {
      p.y += lineHeight + 20;
      lineHeight = 0;
      p.x = 10;
    }
  }
}

const CSize CTestOBMBitmapsDlg::paintOBMBitmap(const OBMResource &res, const CPoint &p) {
  CClientDC dc(this);
  CBitmap bm;
  if(bm.LoadOEMBitmap(res.m_id) == 0) {
    String s = format(_T("%s -"), res.m_name);
    CSize tsize = getTextExtent(dc, s);
    dc.TextOut(p.x, p.y + 3, s.cstr(), (int)s.length());
    tsize.cy += 3;
    return tsize;
  } else {
    CDC bmDC;
    bmDC.CreateCompatibleDC(NULL);
    bmDC.SelectObject(&bm);
    CSize size = getBitmapSize(bm);
    dc.BitBlt(p.x,p.y,size.cx, size.cy,  &bmDC, 0,0, SRCCOPY);
    BITMAP info;
    bm.GetBitmap(&info);
    String s = format(_T("%s:(%2dx%2d)"), res.m_name, info.bmWidth,info.bmHeight);
    const CSize tsize = getTextExtent(dc, s);
    dc.TextOut(p.x, p.y + size.cy + 3, s.cstr(), (int)s.length());
    size.cx = max(size.cx, tsize.cx);
    size.cy += tsize.cy + 3;
    return size;
  }
}
