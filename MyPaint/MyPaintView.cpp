#include "stdafx.h"
#include "MainFrm.h"
#include "MyPaintDoc.h"
#include "MyPaintView.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMyPaintView, CScrollView)

BEGIN_MESSAGE_MAP(CMyPaintView, CScrollView)
    ON_WM_SIZE()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
    // Standard printing commands
    ON_COMMAND(ID_FILE_PRINT        , OnFilePrint       )
    ON_COMMAND(ID_FILE_PRINT_DIRECT , OnFilePrint       )
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
END_MESSAGE_MAP()

CMyPaintView::CMyPaintView() {
  m_currentZoomFactor = 1;
  m_printInfo         = NULL;
  m_initialized       = false;
  m_currentEdgeMark   = NULL;
  resetResizingFrame();
}

void CMyPaintView::OnDraw(CDC *pDC) {
  CMyPaintDoc   *doc        = GetDocument();
  const PixRect *pr         = doc->getImage();
  CSize          docSizePix = doc->getSize();
  HDC            hdc        = pr->getDC();
  CDC           *docDC      = CDC::FromHandle(hdc);
  if(!pDC->IsPrinting()) {
    if(m_currentZoomFactor == 1) {
      pDC->BitBlt(0,0,docSizePix.cx,docSizePix.cy
                 ,docDC
                 ,0,0
                 ,SRCCOPY);
    } else {
      pDC->StretchBlt(0,0,docSizePix.cx*m_currentZoomFactor,docSizePix.cy*m_currentZoomFactor
                     ,docDC
                     ,0,0,docSizePix.cx,docSizePix.cy
                     ,SRCCOPY);
    }
    paintBackgroundAndEdge(*pDC);
  } else {
    CSize printSizePix = pr->getSizeInMillimeters() * m_printInfo->m_rectDraw.Size() / getDCSizeInMillimeters(pDC->m_hAttribDC);
    pDC->StretchBlt(0, 0, printSizePix.cx, printSizePix.cy, docDC, 0, 0, docSizePix.cx, docSizePix.cy, SRCCOPY);
  }
  pr->releaseDC(hdc);
  setScrollRange();
}

void CMyPaintView::paintBackgroundAndEdge(CDC &dc) {
  const CRect clRect = getClientRect(this);
  const CPoint cornerMarkPos = getZoomedDocSize();
  const CPoint rightMarkPos( cornerMarkPos.x  ,cornerMarkPos.y/2);
  const CPoint bottomMarkPos(cornerMarkPos.x/2,cornerMarkPos.y  );

  m_edgeMark.setAllInvisible();
  const CPoint vtl  = getViewTopLeft();
  const CSize  bckWH = clRect.Size() - cornerMarkPos + vtl;
  if(bckWH.cx > 0) {   // paint right background
    dc.FillSolidRect(rightMarkPos.x
                    ,vtl.y
                    ,bckWH.cx
                    ,clRect.bottom
                    ,getBackgroundColor());
    m_edgeMark.setPosition(RIGHTMARK,rightMarkPos);
  }
  if(bckWH.cy > 0) { // paint lower background
    dc.FillSolidRect(vtl.x
                    ,bottomMarkPos.y
                    ,clRect.right
                    ,bckWH.cy
                    ,getBackgroundColor());
    m_edgeMark.setPosition(BOTTOMMARK,bottomMarkPos);
  }
  if(m_edgeMark.getVisibleCount() == 2) { // both right- and bottom-mark are visible => bottomRightMark is visible
    m_edgeMark.setPosition(RIGHTBOTTOMMARK,cornerMarkPos);
  }
  if(m_edgeMark.getVisibleCount() > 0) {
    m_edgeMark.paintAll(dc);
  }

#if defined(__NEVER__)
  debugLog(_T("clRect.size:(%3d,%3d) vtl:(%3d,%3d) em:%s\n")
          ,clRect.Size().cx, clRect.Size().cy
          ,vtl.x,vtl.y
          ,m_edgeMark.toString().cstr()
          );
#endif // _DEBUG
}

PixRect *CMyPaintView::getImage() {
  return GetDocument()->getImage();
}

void CMyPaintView::repaint() {
  CClientDC dc(this);
  OnPrepareDC(&dc);
  OnDraw(&dc);
}

void CMyPaintView::refreshDoc() {
  setScrollRange();
  repaint();
}

void CMyPaintView::clear() {
  CClientDC dc(this);
  OnPrepareDC(&dc);
  CSize docSize = GetDocument()->getSize();
  dc.FillSolidRect(0,0,docSize.cx*m_currentZoomFactor,docSize.cy*m_currentZoomFactor,WHITE);
  resetResizingFrame();
}

void CMyPaintView::setCurrentZoomFactor(int factor) {
  m_currentZoomFactor = factor;
  refreshDoc();
}

void CMyPaintView::setCurrentDocPoint(const CPoint &p) {
  if(p != m_currentDocPoint) {
    m_currentDocPoint = p;
    getMainFrame()->PostMessage(ID_MSG_SHOWDOCPOINT);
  }
}

BOOL CMyPaintView::OnPreparePrinting(CPrintInfo *pInfo) {
  m_printInfo = pInfo;
  return DoPreparePrinting(pInfo);
}

void CMyPaintView::OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo) {
}

void CMyPaintView::OnEndPrinting(CDC *pDC, CPrintInfo *pInfo) {
  m_printInfo = NULL;
}

#if defined(_DEBUG)
void CMyPaintView::AssertValid() const {
  __super::AssertValid();
}

void CMyPaintView::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
}

CMyPaintDoc* CMyPaintView::GetDocument() {
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyPaintDoc)));
  return (CMyPaintDoc*)m_pDocument;
}
#endif //_DEBUG

void CMyPaintView::OnInitialUpdate() {
  __super::OnInitialUpdate();
  getMainFrame()->setCurrentZoomFactor(ID_OPTIONS_ZOOM_X1);
  setScrollRange();
  m_initialized = true;
}

void CMyPaintView::setScrollRange() {
  const CSize clientSize = getClientRect(this).Size();
  CSize       imageSize  = GetDocument()->getSize();
  imageSize.cx *= m_currentZoomFactor;
  imageSize.cy *= m_currentZoomFactor;
  imageSize.cx += EDGEMARKSIZE;
  imageSize.cy += EDGEMARKSIZE;

  SetScrollSizes(MM_TEXT,imageSize,clientSize,CSize(20,20));
  m_maxScroll = CPoint(max(0,imageSize.cx-clientSize.cx),max(0,imageSize.cy-clientSize.cy));
}

void CMyPaintView::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  if(m_initialized) {
    repaint();
  } else {
    setScrollRange();
  }
}

CPoint CMyPaintView::docToView(const CPoint &docPoint) const {
  return CPoint(docPoint.x * m_currentZoomFactor, docPoint.y * m_currentZoomFactor) - getViewTopLeft();
}

CPoint CMyPaintView::viewToDoc(const CPoint &viewPoint) const {
  const CPoint vtl = getViewTopLeft();
  return CPoint((viewPoint.x+vtl.x)/m_currentZoomFactor, (viewPoint.y+vtl.y)/m_currentZoomFactor);
}

void CMyPaintView::OnLButtonDown(UINT nFlags, CPoint point) {
  setCurrentDocPoint(viewToDoc(point));
  getMainFrame()->getCurrentDrawTool()->OnLButtonDown(nFlags,m_currentDocPoint);
  __super::OnLButtonDown(nFlags, point);
}

void CMyPaintView::OnLButtonDblClk(UINT nFlags, CPoint point) {
  setCurrentDocPoint(viewToDoc(point));
  getMainFrame()->getCurrentDrawTool()->OnLButtonDblClk(nFlags,m_currentDocPoint);
  __super::OnLButtonDblClk(nFlags, point);
}

void CMyPaintView::OnLButtonUp(UINT nFlags, CPoint point) {
  setCurrentDocPoint(viewToDoc(point));
  getMainFrame()->getCurrentDrawTool()->OnLButtonUp(nFlags,m_currentDocPoint);
  __super::OnLButtonUp(nFlags, point);
}

void CMyPaintView::OnMouseMove(UINT nFlags, CPoint point) {
  const CPoint newPoint = viewToDoc(point);
  if(newPoint != m_currentDocPoint) {
    getMainFrame()->getCurrentDrawTool()->OnMouseMove(nFlags,newPoint);
    setCurrentDocPoint(newPoint);
  }
  __super::OnMouseMove(nFlags, point);
}

BOOL CMyPaintView::PreTranslateMessage(MSG *pMsg) {
  CPoint p = pMsg->pt;
  ScreenToClient(&p);
  const EdgeMark *mm       = findEdgeMark(p+getViewTopLeft());
  const CPoint    docPoint = viewToDoc(p);
  switch(pMsg->message) {
  case WM_LBUTTONDOWN:
    if(mm != NULL) {
      resetResizingFrame();
      m_currentEdgeMark = mm;
      return TRUE;
    } else if(!getViewRect().PtInRect(p)) {
      return TRUE;
    }
    break;
  case WM_LBUTTONUP  :
    if(m_currentEdgeMark != NULL) {
      resizeDocument();
      resetResizingFrame();
      m_currentEdgeMark = NULL;
      if(mm != NULL) {
        setCursor(mm->getCursorId());
      } else if(getViewRect().PtInRect(p)) {
        setCursor(getCurrentToolCursor());
      } else {
        setCursor(OCR_NORMAL);
      }
      repaint();
      return TRUE;
    } else if(!getViewRect().PtInRect(p)) {
      return TRUE;
    }
    break;

  case WM_MOUSEMOVE  :
    if(m_currentEdgeMark != NULL) {
      paintResizingFrame(docPoint);
      return TRUE;
    } else if(mm != NULL) {
      setCursor(mm->getCursorId());
      return TRUE;
    } else if(getViewRect().PtInRect(p)) {
      setCursor(getCurrentToolCursor());
    } else {
      setCursor(OCR_NORMAL);
    }
    break;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CMyPaintView::resetResizingFrame() {
  m_lastDragRect  = NULL;
  m_dragRect.left = m_dragRect.top = m_dragRect.right = m_dragRect.bottom = 0;
}

void CMyPaintView::paintResizingFrame(const CPoint &docp) {
  const CPoint lrCorner = docToView(getDocSize());
  const CPoint vp       = docToView(docp);
  CRect        newRect;
  switch(m_currentEdgeMark->getType()) {
  case RIGHTMARK      : newRect = CRect(0,0,vp.x      , lrCorner.y); break;
  case BOTTOMMARK     : newRect = CRect(0,0,lrCorner.x, vp.y      ); break;
  case RIGHTBOTTOMMARK: newRect = CRect(0,0,vp.x      , vp.y      ); break;
  }
  if(newRect != m_dragRect) {
    CClientDC dc(this);
    OnPrepareDC(&dc);
    dc.DrawDragRect(&newRect,CSize(1,1),m_lastDragRect,CSize(1,1));
    m_dragRect     = newRect;
    m_lastDragRect = &m_dragRect;
    const CPoint docSize = viewToDoc(newRect.Size());
    getMainFrame()->PostMessage(ID_MSG_SHOWRESIZESIZE, docSize.x,docSize.y);
  }
}

void CMyPaintView::resizeDocument() {
  if(m_lastDragRect != NULL) {
    const CPoint vp      = m_lastDragRect->Size();
    const CSize  newSize = viewToDoc(vp);
    if(newSize.cx != 0 && newSize.cy != 0) {
      GetDocument()->setSize(newSize);
      getMainFrame()->updateTitle();
    }
  }
}

int CMyPaintView::getCurrentToolCursor() {
  return getMainFrame()->getCurrentDrawTool()->getCursorId();
}

void CMyPaintView::restoreOldTool() {
  getMainFrame()->PostMessage(ID_MSG_POPTOOL);
}

D3DCOLOR CMyPaintView::getColor() {
  return getMainFrame()->getCurrentColor();
}

int CMyPaintView::getApproximateFillTolerance() const {
  return getMainFrame()->getApproximateFillTolerance();
}

void CMyPaintView::enableCut(bool enabled) {
  enableMenuItem(getMainFrame(),ID_EDIT_CUT,enabled);
}

void CMyPaintView::saveDocState() {
  getMainFrame()->saveDocState();
}
