#include "stdafx.h"
#include "PrShow.h"
#include "MainFrm.h"
#include "ImageDoc.h"
#include "ImageView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CImageView, CScrollView)

BEGIN_MESSAGE_MAP(CImageView, CScrollView)
    ON_WM_SIZE()
    // Standard printing commands
    ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

CImageView::CImageView() {
  m_currentZoomFactor = 1;
  m_printInfo = NULL;
}

CImageView::~CImageView() {
}

BOOL CImageView::PreCreateWindow(CREATESTRUCT& cs) {
  return CView::PreCreateWindow(cs);
}

void CImageView::zoom(bool in) {
  if(in) {
    m_currentZoomFactor *= 1.0625;
  } else {
    CClientDC dc(this);
    CSize size = GetDocument()->getPixRect()->getSize();
    dc.FillSolidRect(0,0,(int)(size.cx*m_currentZoomFactor),(int)(size.cy*m_currentZoomFactor),WHITE);
    m_currentZoomFactor /= 1.0625;
  }
  repaint();
}

void CImageView::OnDraw(CDC* pDC) {
  CImageDoc *doc = GetDocument();

  PixRect *pr = doc->getPixRect();
  CSize imageSizePix = pr->getSize();
  HDC hdc = pr->getDC();
  CDC *docDC = CDC::FromHandle(hdc);
  if(!pDC->IsPrinting()) {
    if(m_currentZoomFactor == 1) {
      pDC->BitBlt(0,0,imageSizePix.cx,imageSizePix.cy,docDC,0,0,SRCCOPY);
    } else {
      pDC->StretchBlt(0,0,(int)(imageSizePix.cx*m_currentZoomFactor),(int)(imageSizePix.cy*m_currentZoomFactor)
                     ,docDC
                     ,0,0,imageSizePix.cx,imageSizePix.cy
                     ,SRCCOPY);
    }
  } else {
    CSize printSizePix = doc->getSizeInMillimeters() * m_printInfo->m_rectDraw.Size() / getDCSizeInMillimeters(pDC->m_hAttribDC);
    pDC->StretchBlt(0,0,printSizePix.cx,printSizePix.cy, docDC, 0,0,imageSizePix.cx,imageSizePix.cy, SRCCOPY);
  }
  pr->releaseDC(hdc);
  setScrollRange();
}

PixRect *CImageView::getPixRect() {
  return GetDocument()->getPixRect();
}

void CImageView::repaint() {
  CClientDC dc(this);
  OnPrepareDC(&dc);
  OnDraw(&dc);
}

BOOL CImageView::OnPreparePrinting(CPrintInfo *pInfo) {
  m_printInfo = pInfo;
  return DoPreparePrinting(pInfo);
}

void CImageView::OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo) {
}

void CImageView::OnEndPrinting(CDC *pDC, CPrintInfo *pInfo) {
  m_printInfo = NULL;
}

#ifdef _DEBUG
void CImageView::AssertValid() const {
  CView::AssertValid();
}

void CImageView::Dump(CDumpContext& dc) const {
  CView::Dump(dc);
}

CImageDoc* CImageView::GetDocument() {
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageDoc)));
  return (CImageDoc*)m_pDocument;
}
#endif //_DEBUG


void CImageView::OnInitialUpdate() {
  CScrollView::OnInitialUpdate();
  setScrollRange();
}

void CImageView::setScrollRange() {
  CRect clientRect;
  GetClientRect(&clientRect);
  CSize clientSize = clientRect.Size();

  CSize imageSize = GetDocument()->getSize();
  imageSize.cx = (int)(imageSize.cx * m_currentZoomFactor);
  imageSize.cy = (int)(imageSize.cy * m_currentZoomFactor);

  SetScrollSizes(MM_TEXT,imageSize,clientSize,CSize(20,20));
}

void CImageView::OnSize(UINT nType, int cx, int cy) {
  CScrollView::OnSize(nType, cx, cy);
  setScrollRange();
}

