#include "stdafx.h"
#include "mandelbrot.h"
#include "ShowColorMapDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CShowColorMapDlg::CShowColorMapDlg(UINT maxIteration, const D3DCOLOR *colorMap, CWnd* pParent /*=NULL*/) 
: CDialog(CShowColorMapDlg::IDD, pParent)
, m_maxIteration(maxIteration)
, m_colorMap(colorMap)
{
}

void CShowColorMapDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CShowColorMapDlg, CDialog)
    ON_WM_SIZE()
    ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CShowColorMapDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATIC_COLORMAPWINDOW, RELATIVE_SIZE    );
  m_layoutManager.addControl(IDC_STATIC_INTERVALWINDOW, RELATIVE_HEIGHT  );
  m_layoutManager.addControl(IDOK                     , RELATIVE_POSITION);
    
  return TRUE;
}

void CShowColorMapDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

void CShowColorMapDlg::OnPaint() {
  CPaintDC dc(    GetDlgItem(IDC_STATIC_COLORMAPWINDOW));
  CPaintDC textDC(GetDlgItem(IDC_STATIC_INTERVALWINDOW));

  textDC.SetBkColor(::GetSysColor(COLOR_BTNFACE));

  CFont font;
  font.CreateFont(10, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                  CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                  DEFAULT_PITCH | FF_MODERN,
                  _T("Courier")
                 );

  const int colorWidth = m_maxIteration < 60000 ? 2 : 1;

  const CSize sz = getClientRect(this, IDC_STATIC_COLORMAPWINDOW).Size();
  const UINT colorsPerRow = sz.cx / colorWidth;
  UINT rows;
  for(rows = 1; colorsPerRow * rows < (m_maxIteration+1);) rows++;
  UINT colorHeight = sz.cy / rows;

  CompactArray<UINT> intervals;
  CPoint p(0,0);
  for(UINT index = 0, rowCount = 0; index <= m_maxIteration; index++) {
    if(rowCount == 0) {
      intervals.add(index);
    }
    const D3DCOLOR c3 = m_colorMap[index];
    dc.FillSolidRect(p.x,p.y,colorWidth,colorHeight, D3DCOLOR2COLORREF(c3));
    p.x += colorWidth;
    rowCount++;
    if(rowCount == colorsPerRow) {
      p.x = 0;
      p.y += colorHeight;
      rowCount = 0;
    }
  }
  if(intervals.last() == m_maxIteration) {
    intervals.last()++;
  } else {
    intervals.add(m_maxIteration+1);
  }
  textDC.SelectObject(&font);
  const int textHeight = getTextExtent(textDC, _T("10-10")).cy;
  int ty = (colorHeight-textHeight) / 2;

  for(size_t i = 1; i < intervals.size(); i++, ty += colorHeight) {
    const UINT from = intervals[i-1];
    const UINT to   = intervals[i]-1;
    const String s = format(_T("%d-%d"), from, to);
    textOut(textDC, 2, ty, s);
  }
}
