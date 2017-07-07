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
  init();
}

CPearlImageDoc::~CPearlImageDoc() {
  CHECKINVARIANT;
  resetHistory();
  setImage(NULL);
}

BOOL CPearlImageDoc::OnOpenDocument(LPCTSTR name) {
  CHECKINVARIANT;
  if(!__super::OnOpenDocument(name)) {
    return FALSE;
  }
  PixRect *image = PixRect::load(theApp.m_device, ByteInputFile(name));

  resetHistory();
  m_gridParam.m_cellCount = m_gridParam.reset().findCellCount(image->getSize());
  setImage(image);
  FileNameSplitter info(name);
  SetTitle(info.getFileName().cstr());
  SetPathName(name);
  setSaveTime();
  CHECKINVARIANT;
  return TRUE;
}

void CPearlImageDoc::init() {
  resetHistory();
  setImage(NULL);
  SetTitle(s_defaultName);
  setSaveTime();
  CHECKINVARIANT;
}

BOOL CPearlImageDoc::IsModified() {
  return isModified();
}

bool CPearlImageDoc::isModified() const {
  CHECKINVARIANT;
  return hasImage() && (m_image.m_ts != m_lastImageSave);
}

bool CPearlImageDoc::hasDefaultName() const {
  return GetTitle() == s_defaultName;
}

BOOL CPearlImageDoc::OnSaveDocument(LPCTSTR name) {
  CHECKINVARIANT;
  if(!hasImage()) return FALSE;
  FileNameSplitter info(name);
  const PixRect *pr = m_image.m_pr;
  if(info.getExtension() == _T(".jpg")) {
    pr->writeAsJPG(ByteOutputFile(info.setExtension( _T("jpg" )).getFullPath()));
  } else if(info.getExtension() == _T(".tiff")) {
    pr->writeAsTIFF(ByteOutputFile(info.setExtension(_T("tiff")).getFullPath()));
  } else if(info.getExtension() == _T(".bmp")) {
    pr->writeAsBMP(ByteOutputFile(info.setExtension( _T("bmp" )).getFullPath()));
  } else if(info.getExtension() == _T(".png")) {
    pr->writeAsPNG(ByteOutputFile(info.setExtension( _T("png" )).getFullPath()));
  } else {
    throwException(_T("Unknown fileformat:%s"), info.getExtension().cstr());
  }
  SetTitle(info.getFileName().cstr());
  SetPathName(name);
  setSaveTime();
  return TRUE;
}

void CPearlImageDoc::setImage(PixRect *image) {
  if(image == m_image.m_pr) return;
  if(image == NULL) {
    m_image.clear();
  } else if (!m_image.hasImage()) {
    m_image.set(image);
  } else if(*image != *m_image.m_pr) {  // both are != NULL
    m_image.set(image);
  }
}

void CPearlImageDoc::setSaveTime() {
  m_lastImageSave = m_image.m_ts;
}

void CPearlImageDoc::setSize(const CSize &newSize) {
  if(!hasImage()) return;
  if(newSize != getSize()) {
    saveState();
    PixRect *newImage = theApp.fetchPixRect(newSize);
    const CSize oldSize = getSize();
    newImage->rop(0,0,oldSize.cx, oldSize.cy,SRCCOPY,m_image.m_pr,0,0);
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
  m_history.last().clear();
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
    m_history.add(PixRectWithTimestamp(m_image.m_pr->clone(true), m_image.m_ts));
  }
}

bool CPearlImageDoc::canAddImage() const {
  return hasImage() && (m_history.isEmpty() || (m_image != m_history.last()));
}

bool CPearlImageDoc::undo() {
  CHECKINVARIANT;
  bool done = false;
  if(canUndo()) {
    if(!canRedo()) {
      addImage();
      m_index = max(getHistorySize() - 1, 1);
    }
    m_image.set(m_history[--m_index]);
    done = true;
  }
  CHECKINVARIANT;
  return done;
}

bool CPearlImageDoc::redo() {
  CHECKINVARIANT;
  bool done = false;
  if(canRedo()) {
    m_image.set(m_history[++m_index]);
    done = true;
  }
  CHECKINVARIANT;
  return done;
}

bool CPearlImageDoc::canUndo() const {
  CHECKINVARIANT;
  return (m_index > 0) && (m_image != m_history[m_index-1]);
}

bool CPearlImageDoc::canRedo() const {
  CHECKINVARIANT;
  return m_index < getHistorySize() - 1;
}

#ifdef _DEBUG
void CPearlImageDoc::checkInvariant(int line) const {
  if(hasImage() && (m_image.m_ts < m_lastImageSave)) {
    throwException(_T("Broken in %s, line %d:lastImageChange:%s, lastImageSave:%s")
                  ,__TFILE__, line
                  ,m_image.m_ts.toString().cstr()
                  ,m_lastImageSave.toString().cstr()
                  );
  }
  if(m_index < 0 || m_index > getHistorySize()) {
    throwException(_T("Broken history invariant in %s, line %d:index=%d, historySize=%d"), __TFILE__, line, m_index, m_history.size());
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
