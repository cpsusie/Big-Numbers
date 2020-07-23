#include "stdafx.h"
#include <MFCUtil/ColorSpace.h>
#include "SelectGlyphDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSelectGlyphDlg::CSelectGlyphDlg(const LOGFONT &logFont, CWnd *pParent) : CDialog(IDD, pParent), m_logFont(logFont) {
  m_prFont = new PixRectFont(theApp.m_prDevice, m_logFont); TRACE_NEW(m_prFont);
}

CSelectGlyphDlg::~CSelectGlyphDlg() {
  SAFEDELETE(m_prFont);
}

void CSelectGlyphDlg::DoDataExchange(CDataExchange* pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSelectGlyphDlg, CDialog)
  ON_WM_PAINT()
  ON_COMMAND(ID_GODOWN,  OnGoDown )
  ON_COMMAND(ID_GOLEFT,  OnGoLeft )
  ON_COMMAND(ID_GORIGHT, OnGoRight)
  ON_COMMAND(ID_GOUP   , OnGoUp   )
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

BOOL CSelectGlyphDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDD_SELECTGLYPH));
  m_currentSelection = -1;

  m_selectedBrush.CreateSolidBrush(RED);
  m_unselectedBrush.CreateSolidBrush(GREY);
  return TRUE;
}

class GlyphPainter : public CurveOperator {
private:
  CDC    &m_dc;
  CPen    m_pen;
  Point2D m_pos;
public:
  void line(const Point2D &from, const Point2D &to);
  GlyphPainter(CDC &dc, const Point2D &pos);
};

GlyphPainter::GlyphPainter(CDC &dc, const Point2D &pos) : m_dc(dc) {
  m_pen.CreatePen(BS_SOLID,1,BLACK);
  m_dc.SelectObject(&m_pen);
  m_pos = pos;
}

void GlyphPainter::line(const Point2D &from, const Point2D &to) {
  m_dc.MoveTo(m_pos%from);
  m_dc.LineTo(m_pos%to);
}

CRect CSelectGlyphDlg::getClientRect(int id) {
  CRect rect = getWindowRect(id);
  ScreenToClient(&rect);
  return rect;
}

CRect CSelectGlyphDlg::getWindowRect(int id) { 
  CWnd *control = GetDlgItem(id);
  CRect rect;
  control->GetWindowRect(&rect);
  return rect;
}

void CSelectGlyphDlg::OnPaint() {
  CPaintDC    dc(this);
  CRect       clientRect = getClientRect(IDC_STATICGLYPHFRAME);
  dc.SelectObject(&m_prFont->getFont());
  const int   chWidth  = m_prFont->getTextMetrics().tmMaxCharWidth + 4;
  const int   chHeight = m_prFont->getTextMetrics().tmHeight + 8;
  const CSize chSize(chWidth,chHeight);

  int leftEdge  = clientRect.left  + chWidth;
  int rightEdge = clientRect.right - chWidth;

  CPoint p(leftEdge,clientRect.top + chHeight * 2);
  int row = 0, col = 0;
  m_rectArray.clear();
  for(int ch = 0; ch < 256; ch++) {
    CRect     rect(p, chSize);
    GlyphRect gr(rect, row, col);
    CPoint    glyphPos = gr.CenterPoint();
//    glyphPos.y += 2;
    glyphPos.y         = gr.top + 2;
    const String chStr = format(_T("%c"), ch);
    textOutTransparentBackground(dc,glyphPos, chStr, m_prFont->getFont(), BLACK);
//    applyToGlyph(m_prFont->getGlyphData(ch)->m_glyphCurveData,GlyphPainter(dc,glyphPos));
    dc.FrameRect(gr,&m_unselectedBrush);

    m_rectArray.add(gr);

    p.x += chWidth;
    col++;
    if(p.x >= rightEdge) {
      p.x =  leftEdge;
      p.y += chHeight;
      col = 0;
      row++;
    }
  }
  markSelection(m_currentSelection);
}

void CSelectGlyphDlg::OnOK() {
  if(m_currentSelection < 0) {
    showWarning(_T("%s"), _T("Please select af letter from the font"));
    return;
  }
  m_selectedGlyphCurveData = m_prFont->getGlyphData(m_currentSelection)->m_glyphCurveData;
  __super::OnOK();
}

int CSelectGlyphDlg::findSelection(CPoint &p) {
  for(int i = 0; i < m_rectArray.size(); i++) {
    const CRect &r = m_rectArray[i];
    if(r.PtInRect(p))
      return i;
  }
  return -1;
}

void CSelectGlyphDlg::markSelection(int index) {
  if(index >= 0) {
    CClientDC dc(this);
    dc.FrameRect(m_rectArray[index],&m_selectedBrush);
  }
}

void CSelectGlyphDlg::unmarkSelection(int index) {
  if(index >= 0) {
    CClientDC dc(this);
    dc.FrameRect(m_rectArray[index],&m_unselectedBrush);
  }
}

void CSelectGlyphDlg::setCurrentSelection(int index) {
  unmarkSelection(m_currentSelection);
  m_currentSelection = index;
  markSelection(m_currentSelection);
}

int CSelectGlyphDlg::getSelectedRow() {
  if(m_currentSelection < 0) return 0;
  return m_rectArray[m_currentSelection].m_row;
}

int CSelectGlyphDlg::getSelectedCol() {
  if(m_currentSelection < 0) return 0;
  return m_rectArray[m_currentSelection].m_col;
}

void CSelectGlyphDlg::selectPosition(int row, int col) {
  for(int i = 0; i < m_rectArray.size(); i++) {
    if(m_rectArray[i].m_row == row && m_rectArray[i].m_col == col) {
      setCurrentSelection(i);
      return;
    }
  }
  setCurrentSelection(-1);
}

void CSelectGlyphDlg::selectPosition(const CPoint &p) {
  for(int i = 0; i < m_rectArray.size(); i++) {
    if(m_rectArray[i].PtInRect(p)) {
      setCurrentSelection(i);
      return;
    }
  }
  setCurrentSelection(-1);
}

BOOL CSelectGlyphDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CSelectGlyphDlg::OnGoDown() {
  selectPosition(getSelectedRow()+1,getSelectedCol());
}

void CSelectGlyphDlg::OnGoUp() {
  selectPosition(getSelectedRow()-1,getSelectedCol());
}

void CSelectGlyphDlg::OnGoLeft() {
  selectPosition(getSelectedRow(),getSelectedCol()-1);
}

void CSelectGlyphDlg::OnGoRight() {
  selectPosition(getSelectedRow(),getSelectedCol()+1);
}

void CSelectGlyphDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  selectPosition(point);
  __super::OnLButtonDown(nFlags, point);
}

void CSelectGlyphDlg::OnLButtonDblClk(UINT nFlags, CPoint point) {
  selectPosition(point);
  if(m_currentSelection >= 0) {
    OnOK();
  }
  __super::OnLButtonDblClk(nFlags, point);
}
