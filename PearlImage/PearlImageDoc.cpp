#include "stdafx.h"
#include "PearlImageDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CPearlImageDoc, CDocument)

BEGIN_MESSAGE_MAP(CPearlImageDoc, CDocument)
END_MESSAGE_MAP()

const CString CPearlImageDoc::s_defaultName = _T("Untitled");

CPearlImageDoc::CPearlImageDoc() {
  m_image     = NULL;
  m_fileImage = NULL;
  init();
}

CPearlImageDoc::~CPearlImageDoc() {
  resetHistory();
  setImage(NULL);
  setFileImage();
}

BOOL CPearlImageDoc::OnOpenDocument(LPCTSTR name) {
  PixRect *image = PixRect::load(theApp.m_device, ByteInputFile(name));

  OnNewDocument();
  setImage(image);
  FileNameSplitter info(name);
  SetTitle(info.getFileName().cstr());
  SetPathName(name);
  setFileImage();
  return TRUE;
}

BOOL CPearlImageDoc::OnNewDocument() {
  if(!__super::OnNewDocument()) {
    return FALSE;
  }
  init();
  return TRUE;
}

void CPearlImageDoc::init() {
  resetHistory();
  if(m_image != NULL) {
    delete m_image  ;
    m_image   = NULL;
  }
  SetTitle(s_defaultName);
  setFileImage();
  CHECKINVARIANT;
}

BOOL CPearlImageDoc::IsModified() {
  return isModified();
}

bool CPearlImageDoc::isModified() const {
  CHECKINVARIANT;
  if(m_image == NULL) {
    return m_fileImage != NULL;
  } else if(m_fileImage == NULL) {
    return true;
  } else {
    return *m_image   != *m_fileImage;
  }
}

bool CPearlImageDoc::hasDefaultName() const {
  return GetTitle() == s_defaultName;
}

BOOL CPearlImageDoc::OnSaveDocument(LPCTSTR name) {
  FileNameSplitter info(name);
  if(info.getExtension() == _T(".jpg")) {
    m_image->writeAsJPG(ByteOutputFile(info.setExtension( _T("jpg" )).getFullPath()));
  } else if(info.getExtension() == _T(".tiff")) {
    m_image->writeAsTIFF(ByteOutputFile(info.setExtension(_T("tiff")).getFullPath()));
  } else if(info.getExtension() == _T(".bmp")) {
    m_image->writeAsBMP(ByteOutputFile(info.setExtension( _T("bmp" )).getFullPath()));
  } else if(info.getExtension() == _T(".png")) {
    m_image->writeAsPNG(ByteOutputFile(info.setExtension( _T("png" )).getFullPath()));
  } else {
    throwException(_T("Unknown fileformat:%s"), info.getExtension().cstr());
  }
  SetTitle(info.getFileName().cstr());
  SetPathName(name);
  setFileImage();
  return TRUE;
}

void CPearlImageDoc::setImage(PixRect *image) {
  if(image != m_image) {
    if(m_image != NULL) {
      delete m_image;
      m_image = NULL;
    }
    m_image = image;
  }
}

PixRect *CPearlImageDoc::getImage() {
  if(m_image == NULL) {
    setImage(theApp.fetchPixRect(CSize(400,400)));
    setFileImage();
  }
  return m_image;
}

void CPearlImageDoc::setFileImage() {
  if(m_fileImage) {
    delete m_fileImage;
    m_fileImage = NULL;
  }
  if(m_image) {
    m_fileImage = m_image->clone(true);
  }
}

void CPearlImageDoc::setSize(const CSize &newSize) {
  if(newSize != getSize()) {
    saveState();
    PixRect *newImage = theApp.fetchPixRect(newSize);
    const CSize oldSize = getSize();
    newImage->rop(0,0,oldSize.cx, oldSize.cy,SRCCOPY,m_image,0,0);
    setImage(newImage);
  }
}

String CPearlImageDoc::getInfo() const {
  return format(_T("%s %s (%2d,%2d)")
               ,canUndo()?_T("canUndo"):_T("_______")
               ,canRedo()?_T("canRedo"):_T("_______")
               ,m_index
               ,getHistorySize()
               );
}

void CPearlImageDoc::resetHistory() {
  while(!m_history.isEmpty()) {
    removeLast();
  }
  m_index = 0;
}

void CPearlImageDoc::removeLast() {
  delete m_history.last();
  m_history.removeLast();
}

void CPearlImageDoc::saveState() {
  CHECKINVARIANT;

  if(canAddImage()) {
    while(canRedo()) {
      removeLast();
    }
    addImage();
    m_index = getHistorySize();
  }
  CHECKINVARIANT;
}

void CPearlImageDoc::addImage() {
  if(canAddImage()) {
    m_history.add(m_image->clone(true));
  }
}

bool CPearlImageDoc::canAddImage() const {
  return m_history.isEmpty() || (*m_image != *m_history.last());
}

bool CPearlImageDoc::undo() {
  CHECKINVARIANT;
  bool done = false;
  if(canUndo()) {
    if(!canRedo()) {
      addImage();
      m_index = max(getHistorySize() - 1, 1);
    }
    setImage(m_history[--m_index]->clone(true));
    done = true;
  }
  CHECKINVARIANT;
  return done;
}

bool CPearlImageDoc::redo() {
  CHECKINVARIANT;
  bool done = false;
  if(canRedo()) {
    setImage(m_history[++m_index]->clone(true));
    done = true;
  }
  CHECKINVARIANT;
  return done;
}

bool CPearlImageDoc::canUndo() const {
  CHECKINVARIANT;
  return (m_index > 0) && (*m_image != *m_history[m_index-1]);
}

bool CPearlImageDoc::canRedo() const {
  CHECKINVARIANT;
  return m_index < getHistorySize() - 1;
}

#ifdef _DEBUG
void CPearlImageDoc::checkInvariant(int line) const {
  if(m_index < 0 || m_index > getHistorySize()) {
    throwException(_T("Broken invariant in %s, line %d:index=%d, historySize=%d"), __TFILE__, line, m_index, m_history.size());
  }
}
#endif

void CPearlImageDoc::Serialize(CArchive& ar) {
  if (ar.IsStoring()) {
  } else {
  }
}

#ifdef _DEBUG
void CPearlImageDoc::AssertValid() const {
  __super::AssertValid();
}

void CPearlImageDoc::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
}
#endif //_DEBUG
