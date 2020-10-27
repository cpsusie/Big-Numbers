#include "stdafx.h"
#include "CaptureDoc.h"
#include "CaptureView.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCaptureView, CScrollView)

BEGIN_MESSAGE_MAP(CCaptureView, CScrollView)
    ON_WM_SIZE()
END_MESSAGE_MAP()

CCaptureView::CCaptureView() {
  m_printInfo = nullptr;
}

CCaptureView::~CCaptureView() {
}

BOOL CCaptureView::PreCreateWindow(CREATESTRUCT& cs){
  return CScrollView::PreCreateWindow(cs);
}

void CCaptureView::repaint() {
  OnDraw(GetDC());
}

void CCaptureView::OnDraw(CDC* pDC) {
  CCaptureDoc *doc  = GetDocument();
  HBITMAP docBitmap = doc->getBitmap();
  HDC     docDC     = CreateCompatibleDC(pDC->m_hDC);
  SelectObject(docDC,docBitmap);

  CSize docSizePix = doc->getSize();

  if(!pDC->IsPrinting()) {
    ::BitBlt(pDC->m_hDC,0,0,docSizePix.cx,docSizePix.cy,docDC,0,0,SRCCOPY);
    setScrollRange();
  } else { // print or printPreview
    if(!doc->hasImage()) {
      showInformation(_T("Nothing to print"));
      return;
    }
    CSize printSizePix = doc->getSizeInMillimeters() * m_printInfo->m_rectDraw.Size() / getDCSizeInMillimeters(pDC->m_hAttribDC);
    ::StretchBlt(pDC->m_hDC, 0, 0, printSizePix.cx, printSizePix.cy, docDC, 0, 0, docSizePix.cx, docSizePix.cy, SRCCOPY);
  }
  DeleteDC(docDC);
  ASSERT_VALID(doc);
}

void CCaptureView::setScrollRange() {
  CRect clientRect;
  GetClientRect(&clientRect);
  CSize clientSize = clientRect.Size();

  CCaptureDoc *doc = GetDocument();
  CSize imageSize = doc->getSize();
  if(imageSize.cx == 0 || imageSize.cy == 0) {
    imageSize = clientSize;
  }

  SetScrollSizes(MM_TEXT,imageSize,clientSize,CSize(20,20));
}

void CCaptureView::OnInitialUpdate() {
  setScrollRange();
  CScrollView::OnInitialUpdate();
}

BOOL CCaptureView::OnPreparePrinting(CPrintInfo *pInfo) {
  m_printInfo = pInfo;
  return DoPreparePrinting(pInfo);
}

void CCaptureView::OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo) {
}

void CCaptureView::OnEndPrinting(CDC *pDC, CPrintInfo *pInfo) {
  m_printInfo = nullptr;
}

#if defined(_DEBUG)
void CCaptureView::AssertValid() const {
  CScrollView::AssertValid();
}

void CCaptureView::Dump(CDumpContext& dc) const {
  CScrollView::Dump(dc);
}

CCaptureDoc* CCaptureView::GetDocument() { // non-debug version is inline
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCaptureDoc)));
  return (CCaptureDoc*)m_pDocument;
}
#endif //_DEBUG


void CCaptureView::OnSize(UINT nType, int cx, int cy) {
  CScrollView::OnSize(nType, cx, cy);
  setScrollRange();
}

