#include "stdafx.h"
#include "CaptureDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CCaptureDoc, CDocument)

BEGIN_MESSAGE_MAP(CCaptureDoc, CDocument)
END_MESSAGE_MAP()

CCaptureDoc::CCaptureDoc() {
  m_bitmap  = NULL;
  m_size.cx = m_size.cy = 0;
}


CCaptureDoc::CCaptureDoc(HBITMAP bitmap) {
  setImage(bitmap);
}

void CCaptureDoc::resetImage() {
  if(m_bitmap != NULL) {
    DeleteObject(m_bitmap);
    m_bitmap = NULL;
  }
  m_size.cx = m_size.cy = 0;
}

CCaptureDoc::~CCaptureDoc() {
  resetImage();
}

BOOL CCaptureDoc::OnNewDocument() {
  if (!CDocument::OnNewDocument()) {
    return FALSE;
  }
  SetTitle(EMPTYSTRING);
  resetImage();
  return TRUE;
}


void CCaptureDoc::OnOpenDocument(TCHAR *fname) {
  int fisk = 1;
}

void CCaptureDoc::setImage(HBITMAP bitmap) {
  resetImage();
  m_bitmap = bitmap;
  m_size   = getBitmapSize(bitmap);
}

CSize CCaptureDoc::getSizeInMillimeters() const {
  HDC screenDC = getScreenDC();
  CSize result = pixelsToMillimeters(screenDC,getSize());
  DeleteDC(screenDC);
  return result;
}

void CCaptureDoc::save(const String &fileName) {
  String extension = FileNameSplitter(fileName).getExtension();
  if(extension.equalsIgnoreCase(_T(".bmp"))) {
    writeAsBMP(m_bitmap,ByteOutputFile(fileName));
  } else if(extension.equalsIgnoreCase(_T(".jpg"))) {
    writeAsJPG(m_bitmap,ByteOutputFile(fileName));
  } else if(extension.equalsIgnoreCase(_T(".png"))) {
    writeAsPNG(m_bitmap,ByteOutputFile(fileName));
  } else if(extension.equalsIgnoreCase(_T(".tiff"))) {
    writeAsTIFF(m_bitmap,ByteOutputFile(fileName));
  } else {
    throwException(_T("Unsupported fileformat:<%s>"),extension.cstr());
  }
}

CCaptureDoc *CCaptureDoc::getScaledCopy(double factor) {
  if(!hasImage()) {
    return NULL;
  }

  PixRect *p1 = new PixRect(theApp.m_device, m_bitmap);
  PixRect *p2 = PixRect::scaleImage(p1,ScaleParameters(false,Point2D(factor,factor)));

  CCaptureDoc *result = new CCaptureDoc(*p2);
  delete p1;
  delete p2;

  return result ;
}


void CCaptureDoc::Serialize(CArchive& ar) {
  if (ar.IsStoring()) {
  } else {
  }
}

#ifdef _DEBUG
void CCaptureDoc::AssertValid() const {
  CDocument::AssertValid();
}

void CCaptureDoc::Dump(CDumpContext& dc) const {
  CDocument::Dump(dc);
}
#endif //_DEBUG
