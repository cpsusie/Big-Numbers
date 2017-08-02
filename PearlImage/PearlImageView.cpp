#include "stdafx.h"
#include "MainFrm.h"
#include "PearlImageDoc.h"
#include "PearlImageView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CPearlImageView, CScrollView)

BEGIN_MESSAGE_MAP(CPearlImageView, CScrollView)
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

CPearlImageView::CPearlImageView() {
  m_currentZoomFactor = 1;
  m_printInfo         = NULL;
  m_initialized       = false;
  m_currentEdgeMark   = NULL;
  resetResizingFrame();
}

void CPearlImageView::OnDraw(CDC *pDC) {
  CPearlImageDoc *doc = GetDocument();
  if(!doc->hasImage()) {
    paintBackgroundAndEdge(*pDC);
  } else {
    const PixRect  *pr         = doc->getImage();
    CSize           docSizePix = doc->getSize();
    HDC             hdc        = pr->getDC();
    CDC            *docDC      = CDC::FromHandle(hdc);
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
  }
  setScrollRange();
}

void CPearlImageView::paintBackgroundAndEdge(CDC &dc) {
  const CRect clRect = getClientRect(this);
  if(!GetDocument()->hasImage()) {
    dc.FillSolidRect(&clRect, getBackgroundColor());
  } else {
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

#ifdef __NEVER__
  debugLog(_T("clRect.size:(%3d,%3d) vtl:(%3d,%3d) em:%s\n")
          ,clRect.Size().cx, clRect.Size().cy
          ,vtl.x,vtl.y
          ,m_edgeMark.toString().cstr()
          );
#endif // _DEBUG

  }
/*
  dc.FillSolidRect(0,0,5,5,BLACK);
  dc.FillSolidRect(clRect.right-5,0,5,5,RED);
  dc.FillSolidRect(clRect.right-5,clRect.bottom-5,5,5,GREEN);
  dc.FillSolidRect(0,clRect.bottom-5,5,5,BLUE);
*/
}

const PixRect *CPearlImageView::getImage() {
  return GetDocument()->getImage();
}

void CPearlImageView::repaint() {
  CClientDC dc(this);
  OnPrepareDC(&dc);
  OnDraw(&dc);
}

void CPearlImageView::refreshDoc() {
  setScrollRange();
  repaint();
}

void CPearlImageView::clear() {
  CClientDC dc(this);
  OnPrepareDC(&dc);
  CSize docSize = GetDocument()->getSize();
  dc.FillSolidRect(0,0,docSize.cx*m_currentZoomFactor,docSize.cy*m_currentZoomFactor,WHITE);
  resetResizingFrame();
}

void CPearlImageView::setCurrentZoomFactor(int factor) {
  m_currentZoomFactor = factor;
  refreshDoc();
}

void CPearlImageView::setCurrentDocPoint(const CPoint &p) {
  if(p != m_currentDocPoint) {
    m_currentDocPoint = p;
  }
}

BOOL CPearlImageView::OnPreparePrinting(CPrintInfo *pInfo) {
  m_printInfo = pInfo;
  return DoPreparePrinting(pInfo);
}

void CPearlImageView::OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo) {
}

void CPearlImageView::OnEndPrinting(CDC *pDC, CPrintInfo *pInfo) {
  m_printInfo = NULL;
}

#ifdef _DEBUG
void CPearlImageView::AssertValid() const {
  __super::AssertValid();
}

void CPearlImageView::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
}

CPearlImageDoc* CPearlImageView::GetDocument() {
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPearlImageDoc)));
  return (CPearlImageDoc*)m_pDocument;
}
#endif //_DEBUG

void CPearlImageView::OnInitialUpdate() {
  __super::OnInitialUpdate();
  getMainFrame()->setCurrentZoomFactor(ID_OPTIONS_ZOOM_X1);
  setScrollRange();
  m_initialized = true;
}

void CPearlImageView::setScrollRange() {
  const CSize clientSize = getClientRect(this).Size();
  CSize       imageSize  = GetDocument()->getSize();
  imageSize.cx *= m_currentZoomFactor;
  imageSize.cy *= m_currentZoomFactor;
  imageSize.cx += EDGEMARKSIZE;
  imageSize.cy += EDGEMARKSIZE;

  SetScrollSizes(MM_TEXT,imageSize,clientSize,CSize(20,20));
  m_maxScroll = CPoint(max(0,imageSize.cx-clientSize.cx),max(0,imageSize.cy-clientSize.cy));
}

void CPearlImageView::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  if(m_initialized) {
    repaint();
  } else {
    setScrollRange();
  }
}

CPoint CPearlImageView::docToView(const CPoint &docPoint) const {
  return CPoint(docPoint.x * m_currentZoomFactor, docPoint.y * m_currentZoomFactor) - getViewTopLeft();
}

CPoint CPearlImageView::viewToDoc(const CPoint &viewPoint) const {
  const CPoint vtl = getViewTopLeft();
  return CPoint((viewPoint.x+vtl.x)/m_currentZoomFactor, (viewPoint.y+vtl.y)/m_currentZoomFactor);
}

void CPearlImageView::OnLButtonDown(UINT nFlags, CPoint point) {
  setCurrentDocPoint(viewToDoc(point));
  __super::OnLButtonDown(nFlags, point);
}

void CPearlImageView::OnLButtonDblClk(UINT nFlags, CPoint point) {
  setCurrentDocPoint(viewToDoc(point));
  __super::OnLButtonDblClk(nFlags, point);
}

void CPearlImageView::OnLButtonUp(UINT nFlags, CPoint point) {
  setCurrentDocPoint(viewToDoc(point));
  __super::OnLButtonUp(nFlags, point);
}

void CPearlImageView::OnMouseMove(UINT nFlags, CPoint point) {
  const CPoint newPoint = viewToDoc(point);
  if(newPoint != m_currentDocPoint) {
    setCurrentDocPoint(newPoint);
  }
  __super::OnMouseMove(nFlags, point);
}

BOOL CPearlImageView::PreTranslateMessage(MSG *pMsg) {
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
    } else {
      setCursor(OCR_NORMAL);
    }
    break;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CPearlImageView::resetResizingFrame() {
  m_lastDragRect  = NULL;
  m_dragRect.left = m_dragRect.top = m_dragRect.right = m_dragRect.bottom = 0;
}

void CPearlImageView::paintResizingFrame(const CPoint &docp) {
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
  }
}

void CPearlImageView::resizeDocument() {
  if(m_lastDragRect != NULL) {
    const CPoint vp      = m_lastDragRect->Size();
    const CSize  newSize = viewToDoc(vp);
    if(newSize.cx != 0 && newSize.cy != 0) {
      GetDocument()->setSize(newSize);
      getMainFrame()->updateTitle();
    }
  }
}
