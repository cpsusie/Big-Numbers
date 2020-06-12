#include "stdafx.h"
#include <FileNameSplitter.h>
#include "MyPaintDoc.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMyPaintDoc, CDocument)

BEGIN_MESSAGE_MAP(CMyPaintDoc, CDocument)
END_MESSAGE_MAP()

const CString CMyPaintDoc::s_defaultName = _T("Untitled");

CMyPaintDoc::CMyPaintDoc() {
  m_image     = NULL;
  m_fileImage = NULL;
  init();
}

CMyPaintDoc::~CMyPaintDoc() {
  clear();
}

BOOL CMyPaintDoc::OnOpenDocument(LPCTSTR name) {
  PixRect *image = PixRect::load(theApp.m_device, ByteInputFile(name));

  theApp.AddToRecentFileList(name);

  OnNewDocument();
  setImage(image);
  FileNameSplitter info(name);
  SetTitle(info.getFileName().cstr());
  SetPathName(name);
  setFileImage();
  return TRUE;
}

BOOL CMyPaintDoc::OnNewDocument() {
  if(!__super::OnNewDocument()) {
    return FALSE;
  }
  init();
  return TRUE;
}

void CMyPaintDoc::init() {
  resetHistory();
  SetTitle(s_defaultName);
  CHECKINVARIANT;
}

BOOL CMyPaintDoc::IsModified() {
  return isModified();
}

bool CMyPaintDoc::isModified() const {
  CHECKINVARIANT;
  return (m_fileImage == NULL) || (*m_image != *m_fileImage);
}

bool CMyPaintDoc::hasDefaultName() const {
  return GetTitle() == s_defaultName;
}

BOOL CMyPaintDoc::OnSaveDocument(LPCTSTR name) {
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

void CMyPaintDoc::setImage(PixRect *image) {
  SAFEDELETE(m_image);
  m_image = image;
}

PixRect *CMyPaintDoc::getImage() {
  if(m_image == NULL) {
    setImage(theApp.fetchPixRect(CSize(400,400)));
    setFileImage();
  }
  return m_image;
}

void CMyPaintDoc::setFileImage() {
  SAFEDELETE(m_fileImage);
  if(m_image) {
    m_fileImage = m_image->clone(true);
  }
}

void CMyPaintDoc::clear() {
  resetHistory();
  setImage(NULL);
  setFileImage();
}

void CMyPaintDoc::setSize(const CSize &newSize) {
  if(newSize != getSize()) {
    saveState();
    PixRect *newImage = theApp.fetchPixRect(newSize);
    const CSize oldSize = getSize();
    newImage->rop(0,0,oldSize.cx, oldSize.cy,SRCCOPY,m_image,0,0);
    setImage(newImage);
  }
}

String CMyPaintDoc::getInfo() const {
  return format(_T("%s %s"),canUndo()?_T("canUndo"):_T("_______"), canRedo()?_T("canRedo"):_T("_______"));
}

void CMyPaintDoc::resetHistory() {
  while(!m_history.isEmpty()) {
    removeLast();
  }
  m_index = 0;
}

void CMyPaintDoc::removeLast() {
  PixRect *last = m_history.last();
  SAFEDELETE(last);
  m_history.removeLast();
}

void CMyPaintDoc::saveState() {
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

void CMyPaintDoc::addImage() {
  if(canAddImage()) {
    m_history.add(m_image->clone(true));
  }
}

bool CMyPaintDoc::canAddImage() const {
  return m_history.isEmpty() || (*m_image != *m_history.last());
}

bool CMyPaintDoc::undo() {
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

bool CMyPaintDoc::redo() {
  CHECKINVARIANT;
  bool done = false;
  if(canRedo()) {
    setImage(m_history[++m_index]->clone(true));
    done = true;
  }
  CHECKINVARIANT;
  return done;
}

bool CMyPaintDoc::canUndo() const {
  CHECKINVARIANT;
  return (m_index > 0) && (*m_image != *m_history[m_index-1]);
}

bool CMyPaintDoc::canRedo() const {
  CHECKINVARIANT;
  return m_index < getHistorySize() - 1;
}

#if defined(_DEBUG)
void CMyPaintDoc::checkInvariant(int line) const {
  if(m_index < 0 || m_index > getHistorySize()) {
    throwException(_T("Broken invariant in %s, line %d:index=%d, historySize=%d"), __TFILE__, line, m_index, m_history.size());
  }
}
#endif

void CMyPaintDoc::Serialize(CArchive& ar) {
  if (ar.IsStoring()) {
  } else {
  }
}

#if defined(_DEBUG)
void CMyPaintDoc::AssertValid() const {
  __super::AssertValid();
}

void CMyPaintDoc::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
}
#endif //_DEBUG
