#include "stdafx.h"
#include "MainFrm.h"
#include "D3FunctionDoc.h"
#include "SplitView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CSplitView, CView)

BEGIN_MESSAGE_MAP(CSplitView, CView)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_SHOWWINDOW()
  ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_DIRECT, OnFilePrint)
END_MESSAGE_MAP()

static FastSemaphore s_gate;
CRuntimeClass *CSplitView::s_childClass1    = NULL;
CRuntimeClass *CSplitView::s_childClass2    = NULL;
SplitDirection CSplitView::s_splitDirection = SPLIT_NONE;

CSplitView *CSplitView::create(CWnd *parent, CRuntimeClass *child1, CRuntimeClass *child2, SplitDirection splitDirection, const CRect &rect) { // static
  s_gate.wait();
  s_childClass1    = child1;
  s_childClass2    = child2;
  s_splitDirection = splitDirection;

  CSplitView *view = (CSplitView*)createView(parent, RUNTIME_CLASS(CSplitView), rect);

  s_childClass1    = NULL;
  s_childClass2    = NULL;
  s_splitDirection = SPLIT_NONE;

  s_gate.notify();

  return view;
}

CSplitView::CSplitView() {
  m_relativeSplitPoint = 0.5;
}

CSplitView::~CSplitView() {
}

BOOL CSplitView::OnPreparePrinting(CPrintInfo *pInfo) {
  return DoPreparePrinting(pInfo);
}

void CSplitView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
}

void CSplitView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
}

#ifdef _DEBUG
void CSplitView::AssertValid() const {
  __super::AssertValid();
}

void CSplitView::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
}

CD3FunctionDoc *CSplitView::GetDocument() { // non-debug version is inline
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CD3FunctionDoc)));
  return (CD3FunctionDoc*)m_pDocument;
}
#endif //_DEBUG

int CSplitView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  CSize childSize  = getClientRect(this).Size();
  int rowCount = 1, colCount = 1;
  switch(s_splitDirection) {
  case SPLIT_VERTICAL:
    childSize.cx /= 2;
    colCount++;
    break;
  case SPLIT_HORIZONTAL:
    childSize.cy /= 2;
    rowCount++;
    break;
  default:
    return FALSE;
  }
  const int r1 = 0           , c1 = 0;
  const int r2 = rowCount - 1, c2 = colCount - 1;
  VERIFY(m_splitter.CreateStatic(this, rowCount, colCount));
  VERIFY(m_splitter.CreateView(r1, c1, s_childClass1, childSize, NULL));
  VERIFY(m_splitter.CreateView(r2, c2, s_childClass2, childSize, NULL));
  return TRUE;
}

void CSplitView::OnShowWindow(BOOL bShow, UINT nStatus) {
  __super::OnShowWindow(bShow, nStatus);
  if(m_splitter.getChildCount() == 2) {
    const CSize size = getClientRect(this).Size();
    if(m_splitter.getPanelCount() == 2) {
      switch(m_splitter.getSplitDirection()) {
      case SPLIT_HORIZONTAL:
        m_splitter.SetRowInfo(0, (int)(m_relativeSplitPoint * size.cy), 10);
        break;
      case SPLIT_VERTICAL:
        m_splitter.SetColumnInfo(0, (int)(m_relativeSplitPoint * size.cx), 10);
        break;
      }
    } else {
      switch(m_splitter.getSplitDirection()) {
      case SPLIT_HORIZONTAL:
        m_splitter.SplitRow(   (int)(m_relativeSplitPoint * size.cy));
        break;
      case SPLIT_VERTICAL:
        m_splitter.SplitColumn((int)(m_relativeSplitPoint * size.cx));
        break;
      }
    }
  }
}

void CSplitView::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  if(m_splitter.getChildCount() == 2) {
    setRelativeSplitPoint(cx,cy);
    m_splitter.RecalcLayout();
  }
}

void CSplitView::OnDraw(CDC *pDC) {
  if(m_splitter.getChildCount() == 2) {
    m_splitter.RedrawWindow();
  }
}

CWnd *CSplitView::getChild(UINT index) const {
  const MatrixDimension dim = getDimension();
  if(index >= dim.getElementCount()) {
    return NULL;
  }
  const UINT r = (UINT)(index / dim.columnCount), c = (UINT)(index % dim.columnCount);
  return m_splitter.GetPane(r, c);
}

void CSplitView::saveRelativeSplitPoint() {
  const CRect r = getClientRect(this);
  switch(m_splitter.getSplitDirection()) {
  case SPLIT_HORIZONTAL:
    { int h1, cyMin;
      m_splitter.GetRowInfo(0, h1, cyMin);
      m_relativeSplitPoint = (double)h1 / r.Height();
    }
    break;
  case SPLIT_VERTICAL:
    { int w1, cxMin;
      m_splitter.GetColumnInfo(0, w1, cxMin);
      m_relativeSplitPoint = (double)w1 / r.Width();
    }
    break;
  }
}

void CSplitView::setRelativeSplitPoint(int cx, int cy) {
  if(m_splitter.getChildCount() != 2) {
    return;
  }
  switch(m_splitter.getSplitDirection()) {
  case SPLIT_HORIZONTAL:
    m_splitter.SetRowInfo(0, (int)(m_relativeSplitPoint * cy), 10);
    break;
  case SPLIT_VERTICAL:
    m_splitter.SetColumnInfo(0, (int)(m_relativeSplitPoint * cx), 10);
    break;
  }
}

BOOL SplitViewSplitter::CreateView(int row, int col, CRuntimeClass* pViewClass,
  SIZE sizeInit, CCreateContext* pContext) {
  const BOOL result = __super::CreateView(row, col, pViewClass, sizeInit, pContext);
  if (result) {
    m_childCount++;
  }
  return result;
}

SplitViewSplitter::SplitViewSplitter()
: m_splitPointMoved(false)
, m_splitDirection(CSplitView::s_splitDirection)
, m_childCount(0)
{
}

void SplitViewSplitter::RecalcLayout() {
  CRect r;
  GetParent()->GetClientRect(&r);
  setWindowSize(this, r.Size());
  GetClientRect(&r);
  __super::RecalcLayout();
  GetClientRect(&r);
  if (m_splitPointMoved) {
    CSplitView *parent = (CSplitView*)GetParent();
    parent->saveRelativeSplitPoint();
    m_splitPointMoved = false;
  }
}

void SplitViewSplitter::OnInvertTracker(const CRect &rect) {
  __super::OnInvertTracker(rect);
  m_splitPointMoved = true;
}

MatrixDimension SplitViewSplitter::getDimension() const {
  return MatrixDimension(GetRowCount(), GetColumnCount());
}
