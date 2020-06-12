#include "stdafx.h"
#include "PrShow.h"
#include "ImageDoc.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CImageDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageDoc, CDocument)
END_MESSAGE_MAP()

CImageDoc::CImageDoc() {
  m_pixRect = NULL;
  init();
}

void CImageDoc::init() {
  if(m_pixRect != NULL) {
    delete m_pixRect;
    m_pixRect = NULL;
  }
  m_name    = _T("Untitled");
}

CImageDoc::~CImageDoc() {
  delete m_pixRect;
}

void CImageDoc::setPixRect(PixRect *pixRect) {
  if(pixRect != m_pixRect) {
    delete m_pixRect;
    m_pixRect = pixRect;
  }
}

CSize CImageDoc::getSizeInMillimeters() const {
  HDC screenDC = getScreenDC();
  CSize result = pixelsToMillimeters(screenDC,getSize());
  DeleteDC(screenDC);
  return result;
}

BOOL CImageDoc::OnNewDocument() {
  if(!CDocument::OnNewDocument()) {
    return FALSE;
  }

  init();
  return TRUE;
}

void CImageDoc::Serialize(CArchive& ar) {
  if (ar.IsStoring()) {
  } else {
  }
}

#if defined(_DEBUG)
void CImageDoc::AssertValid() const {
  CDocument::AssertValid();
}

void CImageDoc::Dump(CDumpContext& dc) const {
  CDocument::Dump(dc);
}
#endif //_DEBUG
