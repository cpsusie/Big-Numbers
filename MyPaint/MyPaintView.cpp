#include "stdafx.h"
#include "MainFrm.h"
#include "MyPaintDoc.h"
#include "MyPaintView.h"

#ifdef _DEBUG
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
    ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

CMyPaintView::CMyPaintView() {
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
#define BACKGROUNDCOLOR RGB(207,217,232)

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

CMyPaintView::~CMyPaintView() {
}

BOOL CMyPaintView::PreCreateWindow(CREATESTRUCT& cs) {
  return CView::PreCreateWindow(cs);
}

void CMyPaintView::OnDraw(CDC *pDC) {
  CMyPaintDoc *doc        = GetDocument();
  PixRect     *pr         = doc->getImage();
  CSize        docSizePix = doc->getSize();
  HDC          hdc        = pr->getDC();
  CDC         *docDC      = CDC::FromHandle(hdc);

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
  CSize  docSize            = GetDocument()->getSize();
  CPoint cornerMarkPos      = getViewPoint(docSize);
  CPoint rightMarkPos       = getViewPoint(CPoint(docSize.cx  , docSize.cy/2));
  CPoint bottomMarkPos      = getViewPoint(CPoint(docSize.cx/2, docSize.cy  ));
  CPoint topLeft            = getTopLeft();
  CRect clRect;
  GetClientRect(&clRect);

  int visibleMarkCount = 0;
  for(size_t i = 0; i < m_edgeMark.size(); i++) {
    m_edgeMark[i].setVisible(false);
  }

  if(0 < cornerMarkPos.x && cornerMarkPos.x < clRect.right) {
    dc.FillSolidRect(cornerMarkPos.x, topLeft.y, clRect.right - cornerMarkPos.x, clRect.bottom, BACKGROUNDCOLOR);
    if(clRect.PtInRect(rightMarkPos)) {
      m_edgeMark[RIGHTMARK].setPosition(rightMarkPos);
      visibleMarkCount++;
    }
  }
  if(0 < cornerMarkPos.y && cornerMarkPos.y < clRect.bottom) {
    dc.FillSolidRect(topLeft.x, cornerMarkPos.y, clRect.right - max(0,clRect.right-cornerMarkPos.x), clRect.bottom - cornerMarkPos.y, BACKGROUNDCOLOR);
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

int CMyPaintView::getCurrentToolCursor() {
  return getMainFrame()->getCurrentDrawTool()->getCursorId();
}

void CMyPaintView::restoreOldTool() {
  getMainFrame()->PostMessage(WM_COMMAND, MAKELONG(ID_POPTOOL, 0));
}

PixRect *CMyPaintView::getImage() {
  return GetDocument()->getImage();
}

D3DCOLOR CMyPaintView::getColor() {
  return getMainFrame()->getCurrentColor();
}

int CMyPaintView::getApproximateFillTolerance() const {
  return getMainFrame()->getApproximateFillTolerance();
}

void CMyPaintView::repaint() {
  CClientDC dc(this);
  OnPrepareDC(&dc);
  OnDraw(&dc);
}

void CMyPaintView::enableCut(bool enabled) {
  enableMenuItem(getMainFrame(),ID_EDIT_CUT,enabled);
}

void CMyPaintView::saveDocState() {
  getMainFrame()->saveDocState();
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
}

void CMyPaintView::setCurrentZoomFactor(int factor) {
  m_currentZoomFactor = factor;
  refreshDoc();
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

#ifdef _DEBUG
void CMyPaintView::AssertValid() const {
  CView::AssertValid();
}

void CMyPaintView::Dump(CDumpContext& dc) const {
  CView::Dump(dc);
}

CMyPaintDoc* CMyPaintView::GetDocument() {
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyPaintDoc)));
  return (CMyPaintDoc*)m_pDocument;
}
#endif //_DEBUG


void CMyPaintView::OnInitialUpdate() {
  CScrollView::OnInitialUpdate();
  getMainFrame()->setCurrentZoomFactor(ID_OPTIONS_ZOOM_X1);
  setScrollRange();
  m_initialized  = true;
}

void CMyPaintView::setScrollRange() {
  CRect clientRect;
  GetClientRect(&clientRect);
  CSize clientSize = clientRect.Size();

  CSize imageSize = GetDocument()->getSize();
  imageSize.cx *= m_currentZoomFactor;
  imageSize.cy *= m_currentZoomFactor;

  SetScrollSizes(MM_TEXT,imageSize,clientSize,CSize(20,20));
}

void CMyPaintView::OnSize(UINT nType, int cx, int cy) {
  CScrollView::OnSize(nType, cx, cy);
  if(m_initialized) {
    repaint();
  } else {
    setScrollRange();
  }
}

CRect CMyPaintView::getDocumentRect() const {
  return CRect(ORIGIN,GetDocument()->getSize());
}

CRect CMyPaintView::getViewRect() const {
  return CRect(ORIGIN,getViewPoint(GetDocument()->getSize()));
}

CPoint CMyPaintView::getViewPoint(const CPoint &docPoint) const {
  const CPoint topLeft = getTopLeft();
  return CPoint(docPoint.x * m_currentZoomFactor - topLeft.x, docPoint.y * m_currentZoomFactor - topLeft.y);
}

CPoint CMyPaintView::getDocPoint(const CPoint &viewPoint) const {
  const CPoint topLeft = getTopLeft();
  return CPoint((viewPoint.x+topLeft.x)/m_currentZoomFactor, (viewPoint.y+topLeft.y)/m_currentZoomFactor);
}

bool CMyPaintView::isMouseOnDocument() const {
  return getDocumentRect().PtInRect(m_lastPoint) ? true : false;
}

void CMyPaintView::OnLButtonDown(UINT nFlags, CPoint point) {
  m_lastPoint = getDocPoint(point);
  getMainFrame()->getCurrentDrawTool()->OnLButtonDown(nFlags,m_lastPoint);
  CScrollView::OnLButtonDown(nFlags, point);
}

void CMyPaintView::OnLButtonDblClk(UINT nFlags, CPoint point) {
  m_lastPoint = getDocPoint(point);
  getMainFrame()->getCurrentDrawTool()->OnLButtonDblClk(nFlags,m_lastPoint);
  CScrollView::OnLButtonDblClk(nFlags, point);
}

void CMyPaintView::OnLButtonUp(UINT nFlags, CPoint point) {
  m_lastPoint = getDocPoint(point);
  getMainFrame()->getCurrentDrawTool()->OnLButtonUp(nFlags,m_lastPoint);
  CScrollView::OnLButtonUp(nFlags, point);
}

void CMyPaintView::OnMouseMove(UINT nFlags, CPoint point) {
  const CPoint newPoint = getDocPoint(point);
  if(newPoint != m_lastPoint) {
    getMainFrame()->getCurrentDrawTool()->OnMouseMove(nFlags,newPoint);
    m_lastPoint = newPoint;
  }
  CScrollView::OnMouseMove(nFlags, point);
}

EdgeMark *CMyPaintView::findEdgeMark(const CPoint &point) {
  for(int i = 0; i < m_edgeMark.size(); i++) {
    EdgeMark &m = m_edgeMark[i];
    if(m.isVisible() && m.getActiveRect().PtInRect(point)) {
      return &m;
    }
  }
  return NULL;
}

BOOL CMyPaintView::PreTranslateMessage(MSG *pMsg) {
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
      } else if(getViewRect().PtInRect(p)) {
        setWindowCursor(this,getCurrentToolCursor());
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
    } else if(getViewRect().PtInRect(p)) {
      setWindowCursor(this,getCurrentToolCursor());
    } else {
      setWindowCursor(this,MAKEINTRESOURCE(OCR_NORMAL));
    }
    break;
  }

  return CScrollView::PreTranslateMessage(pMsg);
}

void CMyPaintView::resizeDocument(const CPoint &p) {
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

void CMyPaintView::paintResizingFrame(const CPoint &p) {
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
