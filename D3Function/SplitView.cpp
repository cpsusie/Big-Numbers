#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include "MainFrm.h"
#include "D3FunctionDoc.h"
#include "SplitView.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CSplitView, CView)

BEGIN_MESSAGE_MAP(CSplitView, CView)
  ON_WM_CREATE()
  ON_WM_DESTROY()
  ON_WM_SIZE()
  ON_WM_SHOWWINDOW()
  ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
  ON_COMMAND(ID_FILE_PRINT_DIRECT, OnFilePrint)
END_MESSAGE_MAP()

static FastSemaphore s_gate;
CRuntimeClass *CSplitView::s_childClass1      = nullptr;
CRuntimeClass *CSplitView::s_childClass2      = nullptr;
SplitDirection CSplitView::s_splitDirection   = SPLIT_NONE;
double         CSplitView::s_relativeSplitPos = 0;

CSplitView *CSplitView::create(CWnd *parent, CRuntimeClass *child1, CRuntimeClass *child2, SplitDirection splitDirection, const CRect &rect, double initialRelativeSplitPos) { // static
  s_gate.wait();
  try {
    s_childClass1      = child1;
    s_childClass2      = child2;
    s_splitDirection   = splitDirection;
    s_relativeSplitPos = initialRelativeSplitPos;

    CSplitView *view = createView(theApp.getMainFrame(), parent, CSplitView, rect);

    s_childClass1      = nullptr;
    s_childClass2      = nullptr;
    s_splitDirection   = SPLIT_NONE;
    s_relativeSplitPos = 0;

    s_gate.notify();
    return view;
  } catch(...) {
    s_childClass1      = nullptr;
    s_childClass2      = nullptr;
    s_splitDirection   = SPLIT_NONE;
    s_relativeSplitPos = 0;

    s_gate.notify();
    throw;
  }
}

CSplitView::CSplitView() : m_splitter(s_splitDirection, s_relativeSplitPos) {
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

#if defined(_DEBUG)
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
  CSize childSize = getClientRect(this).Size();
  switch(m_splitter.getSplitDirection()) {
  case SPLIT_VERTICAL:
    childSize.cx /= 2;
    break;
  case SPLIT_HORIZONTAL:
    childSize.cy /= 2;
    break;
  default:
    return FALSE;
  }
  const MatrixDimension &dim = m_splitter.getDimension();
  const int rows = (int)dim.rowCount, cols = (int)dim.columnCount;
  VERIFY(m_splitter.CreateStatic(this, rows, cols));
  VERIFY(m_splitter.CreateView(0     , 0     , s_childClass1, childSize, nullptr));
  VERIFY(m_splitter.CreateView(rows-1, cols-1, s_childClass2, childSize, nullptr));
  return TRUE;
}

void CSplitView::OnDestroy() {
  __super::OnDestroy();
}

void CSplitView::OnShowWindow(BOOL bShow, UINT nStatus) {
  __super::OnShowWindow(bShow, nStatus);
  if(isChildrenCreated()) {
    m_splitter.setSplit(getClientRect(this).Size());
  }
}

void CSplitView::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  if(isChildrenCreated()) {
    m_splitter.OnSize(cx, cy);
  }
}

void CSplitView::OnDraw(CDC *pDC) {
  if(isChildrenCreated()) {
    m_splitter.RedrawWindow();
  }
}

CWnd *CSplitView::getChild(UINT index) const {
  if(!isChildrenCreated()) {
    return nullptr;
  }
  const MatrixDimension &dim = m_splitter.getDimension();
  if(index >= dim.getElementCount()) {
    return nullptr;
  }
  const UINT r = (UINT)(index / dim.columnCount), c = (UINT)(index % dim.columnCount);
  return m_splitter.GetPane(r, c);
}

BOOL SplitViewSplitter::CreateView(int row, int col, CRuntimeClass* pViewClass,
  SIZE sizeInit, CCreateContext* pContext) {
  const BOOL result = __super::CreateView(row, col, pViewClass, sizeInit, pContext);
  if(result) {
    incrChildCount();
  }
  return result;
}

SplitViewSplitter::SplitViewSplitter(SplitDirection splitDirection, double relativeSplitPos)
: m_splitDirection(     splitDirection  )
, m_dim(createDimension(splitDirection) )
, m_relativeSplitPos(   relativeSplitPos)
, m_flags(              0               )
{
}

MatrixDimension SplitViewSplitter::createDimension(SplitDirection splitDirection) { // static
  switch(splitDirection) {
  case SPLIT_VERTICAL   : return MatrixDimension(1, 2);
  case SPLIT_HORIZONTAL : return MatrixDimension(2, 1);
  default               : return MatrixDimension(0, 0);
  }
}

void SplitViewSplitter::OnSize(int cx, int cy) {
  setRelativeSplitPos(cx, cy);
  RecalcLayout();
}

void SplitViewSplitter::OnInvertTracker(const CRect &rect) {
  __super::OnInvertTracker(rect);
  setFlag(SPLFMASK_SPLITPOSMOVED);
}

void SplitViewSplitter::RecalcLayout() {
  if(isSet(SPLFMASK_RECALCACTIVE)) { // to prevent recursion
    return;
  }
  setFlag(SPLFMASK_RECALCACTIVE);
  try {
    const CSize sz = getClientRect(GetParent()).Size();
    setWindowSize(this, sz);
    __super::RecalcLayout();
    if(isSet(SPLFMASK_SPLITPOSMOVED)) {
      saveRelativeSplitPos(sz);
      clrFlag(SPLFMASK_SPLITPOSMOVED);
    }
    clrFlag(SPLFMASK_RECALCACTIVE);
  } catch(...) {
    clrFlag(SPLFMASK_RECALCACTIVE);
    throw;
  }
}

void SplitViewSplitter::setSplit(const CSize &size) {
  if(getPanelCount() == 2) {
    switch(getSplitDirection()) {
    case SPLIT_HORIZONTAL:
      SetRowInfo(0, (int)(m_relativeSplitPos * size.cy), 10);
      break;
    case SPLIT_VERTICAL:
      SetColumnInfo(0, (int)(m_relativeSplitPos * size.cx), 10);
      break;
    }
  } else {
    switch(getSplitDirection()) {
    case SPLIT_HORIZONTAL:
      SplitRow(   (int)(m_relativeSplitPos * size.cy));
      break;
    case SPLIT_VERTICAL:
      SplitColumn((int)(m_relativeSplitPos * size.cx));
      break;
    }
  }
}

void SplitViewSplitter::setRelativeSplitPos(int cx, int cy) {
  if(getChildCount() != 2) {
    return;
  }
  switch(getSplitDirection()) {
  case SPLIT_HORIZONTAL:
    SetRowInfo(0, (int)(m_relativeSplitPos * cy), 10);
    break;
  case SPLIT_VERTICAL:
    SetColumnInfo(0, (int)(m_relativeSplitPos * cx), 10);
    break;
  }
}

void SplitViewSplitter::saveRelativeSplitPos(const CSize &size) {
  switch(getSplitDirection()) {
  case SPLIT_HORIZONTAL:
    { int h1, cyMin;
      GetRowInfo(0, h1, cyMin);
      m_relativeSplitPos = (double)h1 / size.cy;
    }
    break;
  case SPLIT_VERTICAL:
    { int w1, cxMin;
      GetColumnInfo(0, w1, cxMin);
      m_relativeSplitPos = (double)w1 / size.cx;
    }
    break;
  }
}

CWnd *SplitViewSplitter::findNeighbor(CWnd *wnd) const {
  const int rows = GetRowCount(), cols = GetColumnCount();
  bool wndFound = false;
  CWnd *result = nullptr;
  for(int r = 0; r < rows; r++) {
    for(int c = 0; c < cols; c++) {
      CWnd *w = GetPane(r, c);
      if(w == wnd) {
        wndFound = true;
      } else {
        if(result != nullptr) {
          return nullptr; // Error....this may only contain 2 panes
        }
        result = w;
      }
    }
  }
  if(!wndFound) {
    result = nullptr;
  }
  return result;
}

MatrixIndex findPosition(const CSplitterWnd *splitter, const CWnd *wnd) {
  const int rows = splitter->GetRowCount(), cols = splitter->GetColumnCount();
  for(int r = 0; r < rows; r++) {
    for(int c = 0; c < cols; c++) {
      CWnd *w = splitter->GetPane(r, c);
      if(w == wnd) {
        return MatrixIndex(r, c);
      }
    }
  }
  return MatrixIndex(-1, -1);
}
