#include "stdafx.h"
#include "CaptureDoc.h"
#include "CaptureView.h"
#include <MFCUtil/Clipboard.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCaptureView, CScrollView)

BEGIN_MESSAGE_MAP(CCaptureView, CScrollView)
    ON_WM_SIZE()
    ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
    ON_COMMAND(ID_FILE_SAVE, OnFileSave)
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
END_MESSAGE_MAP()

CCaptureView::CCaptureView() {
  m_printInfo = NULL;
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
      MessageBox(_T("Nothing to print"),_T("No image"),MB_ICONINFORMATION);
      return;
    }
    CSize printSizePix = doc->getSizeInMillimeters() * m_printInfo->m_rectDraw.Size() / getDCSizeInMillimeters(pDC->m_hAttribDC);
    ::StretchBlt(pDC->m_hDC, 0, 0, printSizePix.cx, printSizePix.cy, docDC, 0, 0, docSizePix.cx, docSizePix.cy, SRCCOPY);
  }
  DeleteDC(docDC);
  ajourMenuItems();
  ASSERT_VALID(doc);
}

void CCaptureView::ajourMenuItems() {
  CWnd *mainWnd = GetParent();
  bool enable = GetDocument()->hasImage();

  enableMenuItem(mainWnd,ID_FILE_SAVE,enable);
  enableMenuItem(mainWnd,ID_FILE_PRINT,enable);
  enableMenuItem(mainWnd,ID_FILE_PRINT_PREVIEW,enable);
  enableMenuItem(mainWnd,ID_EDIT_COPY,enable);
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
  m_printInfo = NULL;
}

#ifdef _DEBUG
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

void CCaptureView::OnFilePrint() {
  CCaptureDoc *doc = GetDocument();
  if(!doc->hasImage()) {
    MessageBox(_T("No image to print"), _T("Error"), MB_ICONINFORMATION);
    return;
  }
  CScrollView::OnFilePrint();
}

void CCaptureView::OnFilePrintPreview() {
  CCaptureDoc *doc = GetDocument();
  if(!doc->hasImage()) {
    MessageBox(_T("No image to show"), _T("Error"), MB_ICONINFORMATION);
    return;
  }
  CScrollView::OnFilePrintPreview();
}

void CCaptureView::OnFileSave() {
  CCaptureDoc *doc = GetDocument();
  if(!doc->hasImage()) {
    MessageBox(_T("No image to save"), _T("Error"), MB_ICONINFORMATION);
    return;
  }

  static const TCHAR *FileDialogExtensions = _T("Bitmap files (*.bmp)\0*.bmp;\0"
                                               "JPEG files (*.jpg)\0*.jpg;\0"
                                               "TIFF files (*.tiff)\0*.tiff;\0"
                                               "PNG files (*.png)\0*.png;\0\0");


  CFileDialog dlg(FALSE);
  dlg.m_ofn.lpstrFilter = FileDialogExtensions;
  dlg.m_ofn.lpstrDefExt = _T(".jpg");
  dlg.m_ofn.lpstrTitle  = _T("Save picture");

  if(dlg.DoModal() != IDOK || _tcslen(dlg.m_ofn.lpstrFile) == 0) {
    return;
  }

  try {
    CCaptureDoc *doc = GetDocument();
    const TCHAR *ext = dlg.m_ofn.lpstrFilter;
    doc->save(dlg.m_ofn.lpstrFile);
  } catch(Exception e) {
    MessageBox(e.what());
  }
}

void CCaptureView::OnEditCopy() {
  CCaptureDoc *doc = GetDocument();
  if(!doc->hasImage()) {
    MessageBox(_T("No image to copy"), _T("Error"), MB_ICONINFORMATION);
    return;
  }

  HBITMAP bitmap = doc->getBitmap();
  try {
    putClipboard(theApp.m_pMainWnd->m_hWnd,bitmap);
  } catch(Exception e) {
    MessageBox(format(_T("putClipboard failed:%s"),e.what()).cstr(),_T("Error"),MB_ICONWARNING);
  }
}
