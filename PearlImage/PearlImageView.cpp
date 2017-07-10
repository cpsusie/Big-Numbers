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
  m_edgeMark.add(EdgeMark(RIGHTMARK      ));
  m_edgeMark.add(EdgeMark(BOTTOMMARK     ));
  m_edgeMark.add(EdgeMark(RIGHTBOTTOMMARK));
}

static CBrush whiteBrush(RGB(255, 255, 255));
static CPen   blackPen(PS_SOLID, 1, RGB(0, 0, 0));

int EdgeMark::getCursorId() const {
  switch(m_type) {
  case RIGHTMARK      : return OCR_SIZEWE;
  case BOTTOMMARK     : return OCR_SIZENS;
  case RIGHTBOTTOMMARK: return OCR_SIZENWSE;
  default             : return OCR_SIZEALL;
  }
}

#define EDGEMARKSIZE 6
#define EDGEWINSIZE 26

CRect EdgeMark::createRect(const CPoint &p, int size) const {
  switch(m_type) {
  case RIGHTMARK      : return CRect(p.x        ,p.y-size/2 , p.x+size  ,p.y+size/2);
  case BOTTOMMARK     : return CRect(p.x-size/2 ,p.y        , p.x+size/2,p.y+size  );
  case RIGHTBOTTOMMARK: return CRect(p.x        ,p.y        , p.x+size  ,p.y+size  );
  default             : return CRect(0,0,0,0);
  }
}

CRect EdgeMark::createActiveRect(const CPoint &p) const {
  return createRect(p,EDGEWINSIZE);
}

CRect EdgeMark::createVisibleRect(const CPoint &p) const {
  return createRect(p,EDGEMARKSIZE);
}

void EdgeMark::setPosition(const CPoint &p) {
  m_activeRect  = createActiveRect(p);
  m_visibleRect = createVisibleRect(p);
  setVisible(true);
}

CPearlImageView::~CPearlImageView() {
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
  if(!GetDocument()->hasImage()) {
    CRect  clRect = getClientRect(this);
    dc.FillSolidRect(&clRect, getBackgroundColor());
  } else {
    const CSize  docSize            = GetDocument()->getSize();
    const CPoint cornerMarkPos      = getViewPoint(docSize);
    const CPoint rightMarkPos       = getViewPoint(CPoint(docSize.cx  , docSize.cy/2));
    const CPoint bottomMarkPos      = getViewPoint(CPoint(docSize.cx/2, docSize.cy  ));
    const CPoint topLeft            = getTopLeft();
    const CRect  clRect             = getClientRect(this);

    int visibleMarkCount = 0;
    for(size_t i = 0; i < m_edgeMark.size(); i++) {
      m_edgeMark[i].setVisible(false);
    }

    if(0 < cornerMarkPos.x && cornerMarkPos.x < clRect.right) {
      dc.FillSolidRect(cornerMarkPos.x, topLeft.y, clRect.right - cornerMarkPos.x, clRect.bottom, getBackgroundColor());
      if(clRect.PtInRect(rightMarkPos)) {
        m_edgeMark[RIGHTMARK].setPosition(rightMarkPos);
        visibleMarkCount++;
      }
    }
    if(0 < cornerMarkPos.y && cornerMarkPos.y < clRect.bottom) {
      dc.FillSolidRect(topLeft.x, cornerMarkPos.y, clRect.right - max(0,clRect.right-cornerMarkPos.x), clRect.bottom - cornerMarkPos.y, getBackgroundColor());
      if(clRect.PtInRect(bottomMarkPos)) {
        m_edgeMark[BOTTOMMARK].setPosition(bottomMarkPos);
        visibleMarkCount++;
      }
    }
    if(visibleMarkCount == 2) { // both right- and bottom-mark are visible => bottomRightMark is visible
      m_edgeMark[RIGHTBOTTOMMARK].setPosition(cornerMarkPos);
      visibleMarkCount++;
    }
    if(visibleMarkCount > 0) {
      CBrush whiteBrush(WHITE);
      CPen blackPen;
      blackPen.CreatePen(PS_SOLID, 1, BLACK);
      CBrush* pOldBrush = dc.SelectObject(&whiteBrush);
      CPen  * pOldPen   = dc.SelectObject(&blackPen  );
      for(size_t i = 0; i < m_edgeMark.size(); i++) {
        const EdgeMark &m = m_edgeMark[i];
        if(m.isVisible()) {
          dc.Rectangle(m.getVisibleRect()-topLeft);
        }
      }
      dc.SelectObject(pOldBrush);
      dc.SelectObject(pOldPen  );
    }
  }
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
}

void CPearlImageView::setCurrentZoomFactor(int factor) {
  m_currentZoomFactor = factor;
  refreshDoc();
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
  CRect clientRect;
  GetClientRect(&clientRect);
  CSize clientSize = clientRect.Size();

  CSize imageSize = GetDocument()->getSize();
  imageSize.cx *= m_currentZoomFactor;
  imageSize.cy *= m_currentZoomFactor;

  SetScrollSizes(MM_TEXT,imageSize,clientSize,CSize(20,20));
}

void CPearlImageView::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  if(m_initialized) {
    repaint();
  } else {
    setScrollRange();
  }
}

CRect CPearlImageView::getDocumentRect() const {
  return CRect(ORIGIN,GetDocument()->getSize());
}

CRect CPearlImageView::getViewRect() const {
  return CRect(ORIGIN,getViewPoint(GetDocument()->getSize()));
}

CPoint CPearlImageView::getViewPoint(const CPoint &docPoint) const {
  const CPoint topLeft = getTopLeft();
  return CPoint(docPoint.x * m_currentZoomFactor - topLeft.x, docPoint.y * m_currentZoomFactor - topLeft.y);
}

CPoint CPearlImageView::getDocPoint(const CPoint &viewPoint) const {
  const CPoint topLeft = getTopLeft();
  return CPoint((viewPoint.x+topLeft.x)/m_currentZoomFactor, (viewPoint.y+topLeft.y)/m_currentZoomFactor);
}

bool CPearlImageView::isMouseOnDocument() const {
  return getDocumentRect().PtInRect(m_lastPoint) ? true : false;
}

void CPearlImageView::OnLButtonDown(UINT nFlags, CPoint point) {
  m_lastPoint = getDocPoint(point);
  __super::OnLButtonDown(nFlags, point);
}

void CPearlImageView::OnLButtonDblClk(UINT nFlags, CPoint point) {
  m_lastPoint = getDocPoint(point);
  __super::OnLButtonDblClk(nFlags, point);
}

void CPearlImageView::OnLButtonUp(UINT nFlags, CPoint point) {
  m_lastPoint = getDocPoint(point);
  __super::OnLButtonUp(nFlags, point);
}

void CPearlImageView::OnMouseMove(UINT nFlags, CPoint point) {
  const CPoint newPoint = getDocPoint(point);
  if(newPoint != m_lastPoint) {
    m_lastPoint = newPoint;
  }
  __super::OnMouseMove(nFlags, point);
}

EdgeMark *CPearlImageView::findEdgeMark(const CPoint &point) {
  for(int i = 0; i < m_edgeMark.size(); i++) {
    EdgeMark &m = m_edgeMark[i];
    if(m.isVisible() && m.getActiveRect().PtInRect(point)) {
      return &m;
    }
  }
  return NULL;
}

BOOL CPearlImageView::PreTranslateMessage(MSG *pMsg) {
  CPoint p = pMsg->pt;
  ScreenToClient(&p);
  EdgeMark *m = findEdgeMark(p);
  switch(pMsg->message) {
  case WM_LBUTTONDOWN:
    if(m != NULL) {
      m_lastDragRect = NULL;
      m_currentEdgeMark = m;
      return TRUE;
    } else if(!getViewRect().PtInRect(p)) {
      return TRUE;
    }
    break;
  case WM_LBUTTONUP  :
    if(m_currentEdgeMark != NULL) {
      resizeDocument(p);
      m_currentEdgeMark = NULL;
      if(m != NULL) {
        setWindowCursor(this,MAKEINTRESOURCE(m->getCursorId()));
      } else {
        setWindowCursor(this,MAKEINTRESOURCE(OCR_NORMAL));
      }
      repaint();
      return TRUE;
    } else if(!getViewRect().PtInRect(p)) {
      return TRUE;
    }
    break;

  case WM_MOUSEMOVE  :
    if(m_currentEdgeMark != NULL) {
      paintResizingFrame(p);
      return TRUE;
    } else if(m != NULL) {
      setWindowCursor(this,MAKEINTRESOURCE(m->getCursorId()));
      return TRUE;
    } else {
      setWindowCursor(this,MAKEINTRESOURCE(OCR_NORMAL));
    }
    break;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CPearlImageView::resizeDocument(const CPoint &p) {
  if(m_lastDragRect != NULL) {
    CSize newSize = m_lastDragRect->Size();
    newSize.cx /= m_currentZoomFactor;
    newSize.cy /= m_currentZoomFactor;
    if(newSize.cx != 0 && newSize.cy != 0) {
      GetDocument()->setSize(newSize);
      getMainFrame()->updateTitle();
    }
  }
}

void CPearlImageView::paintResizingFrame(const CPoint &p) {
  CRect newRect;
  switch(m_currentEdgeMark->getType()) {
  case RIGHTMARK      : newRect = CRect(0,0,p.x                                     , getViewPoint(GetDocument()->getSize()).y); break;
  case BOTTOMMARK     : newRect = CRect(0,0,getViewPoint(GetDocument()->getSize()).x, p.y                                     ); break;
  case RIGHTBOTTOMMARK: newRect = CRect(0,0,p.x                                     , p.y                                     ); break;
  }
  CClientDC dc(this);
  dc.DrawDragRect(&newRect,CSize(1,1),m_lastDragRect,CSize(1,1));
  m_dragRect = newRect;
  m_lastDragRect = &m_dragRect;
}
