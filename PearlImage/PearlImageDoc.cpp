#include "stdafx.h"
#include "PearlImageDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CPearlImageDoc, CDocument)

BEGIN_MESSAGE_MAP(CPearlImageDoc, CDocument)
END_MESSAGE_MAP()

const CString CPearlImageDoc::defaultName = _T("Untitled");

CPearlImageDoc::CPearlImageDoc() {
  m_pixRect   = NULL;
  m_fileImage = NULL;
  m_size      = CSize(0,0);
  init();
}

void CPearlImageDoc::init() {
  if(m_pixRect != NULL) {
    delete m_pixRect;
    m_pixRect = NULL;
  }
  SetTitle(defaultName);
  setFileImage();
  resetHistory();
}

CPearlImageDoc::~CPearlImageDoc() {
  resetHistory();
  delete m_pixRect;
  delete m_fileImage;
}

BOOL CPearlImageDoc::OnOpenDocument(LPCTSTR name) {
  PixRect *pr = PixRect::load(theApp.m_device, ByteInputFile(name));

  OnNewDocument();
  setPixRect(pr);
  FileNameSplitter info(name);
  SetTitle(info.getFileName().cstr());
  SetPathName(name);
  setFileImage();
  return TRUE;
}

bool CPearlImageDoc::hasDefaultName() const {
  return GetTitle() == defaultName;
}

BOOL CPearlImageDoc::IsModified() {
  if(m_pixRect == NULL) {
    return m_fileImage != NULL;
  } else if(m_fileImage == NULL) {
    return true;
  } else {
    return *m_pixRect != *m_fileImage;
  }
}

void CPearlImageDoc::setFileImage() {
  if(m_fileImage != NULL) {
    delete m_fileImage;
    m_fileImage = NULL;
  }
  if(m_pixRect != NULL) {
    m_fileImage = m_pixRect->clone(true);
  }
}
BOOL CPearlImageDoc::OnSaveDocument(LPCTSTR name) {
  FileNameSplitter info(name);
  if(info.getExtension() == _T(".jpg")) {
    m_pixRect->writeAsJPG(ByteOutputFile(FileNameSplitter(name).setExtension(_T("jpg")).getFullPath()));
  } else if(info.getExtension() == _T(".tiff")) {
    m_pixRect->writeAsTIFF(ByteOutputFile(FileNameSplitter(name).setExtension(_T("tiff")).getFullPath()));
  } else {
    m_pixRect->writeAsBMP(ByteOutputFile(FileNameSplitter(name).setExtension(_T("bmp")).getFullPath()));
  }
  SetTitle(info.getFileName().cstr());
  SetPathName(name);
  setFileImage();
  return TRUE;
}

void CPearlImageDoc::setPixRect(PixRect *pixRect) {
  if(pixRect != m_pixRect) {
    delete m_pixRect;
    m_pixRect = pixRect;
  }
  if (m_pixRect) {
    m_size = m_pixRect->getSize();
  }
}

void CPearlImageDoc::createPixRect() {
  if (getArea(m_size) == 0) {
    m_size = CSize(400,400);
  }
  m_pixRect = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, m_size);
}

void CPearlImageDoc::setSize(const CSize &newSize) {
  if(newSize == m_size) return;
  saveState();
  PixRect *pr = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, newSize);
  pr->rop(0,0,newSize.cx,newSize.cy,SRCCOPY,m_pixRect,0,0);
  setPixRect(pr);
}

void CPearlImageDoc::resetHistory() {
  while(!m_history.isEmpty()) {
    removeLast();
  }
  m_index = 0;
}

String CPearlImageDoc::getInfo() const {
  return format(_T("%s %s"),canUndo()?_T("canUndo"):_T("_______"), canRedo()?_T("canRedo"):_T("_______"));
}

void CPearlImageDoc::saveState() {
  while(canRedo()) {
    removeLast();
  }
  addImage();
  m_index = getHistorySize();
}

void CPearlImageDoc::undo() {
  if(canUndo()) {
    if(!canRedo()) {
      addImage();
      m_index = getHistorySize()-1;
    }
    setPixRect(m_history[--m_index]->clone(true));
  }
}

void CPearlImageDoc::redo() {
  if(canRedo()) {
    setPixRect(m_history[++m_index]->clone(true));
  }
}

bool CPearlImageDoc::canUndo() const {
  return m_index > 0;
}

bool CPearlImageDoc::canRedo() const {
  return m_index < getHistorySize() - 1;
}

void CPearlImageDoc::addImage() {
  if(m_history.isEmpty() || (*m_pixRect != *m_history.last())) {
    m_history.add(m_pixRect->clone(true));
  }
}

void CPearlImageDoc::removeLast() {
  delete m_history.last();
  m_history.removeLast();
}

BOOL CPearlImageDoc::OnNewDocument() {
  if(!CDocument::OnNewDocument()) {
    return FALSE;
  }

  init();
  return TRUE;
}

void CPearlImageDoc::Serialize(CArchive& ar) {
  if (ar.IsStoring()) {
  } else {
  }
}

#ifdef _DEBUG
void CPearlImageDoc::AssertValid() const {
  CDocument::AssertValid();
}

void CPearlImageDoc::Dump(CDumpContext& dc) const {
  CDocument::Dump(dc);
}
#endif //_DEBUG
