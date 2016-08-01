#include "stdafx.h"
#include <MFCUtil/ProgressWindow.h>
#include "SaveAsjob.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CHexViewDoc, CDocument)

BEGIN_MESSAGE_MAP(CHexViewDoc, CDocument)
END_MESSAGE_MAP()

CHexViewDoc::CHexViewDoc() {
  init();
  TCHAR **argv = __targv;
  *argv++;
  TCHAR *name = *argv++;
  if(name) {
    try {
      OnOpenDocument(name);
    } catch(Exception e) {
      AfxMessageBox(format(_T("%s:%s"), name, e.what()).cstr());
    }
  }
}

CHexViewDoc::~CHexViewDoc() {
  close();
}

void CHexViewDoc::OnOpenDocument(TCHAR *fname) {
  struct _stati64 st = STAT64(fname);
  FILE           *f  = FOPEN( fname, _T("rb"));

  close();
  m_fileName         = fname;
  m_file             = f;
  m_stat             = st;
  theApp.AddToRecentFileList(fname);
  CWnd *mainWnd = theApp.GetMainWnd();
  if(mainWnd) {
    mainWnd->SetWindowText(getTitle().cstr());
  }
}

void CHexViewDoc::close() {
  if(m_file) {
    fclose(m_file);
    init();
  }
}

void CHexViewDoc::init() {
  m_fileName      = _T("");
  m_file          = NULL;
  m_readOnly      = true;
  resetStat();
  resetContent();
  resetModifications();
  resetHistory();
}

void CHexViewDoc::checkIsOpen() {
  if(!m_file) {
    throwException(_T("setReadOnly:No open file"));
  }
}

void CHexViewDoc::checkCanModify() {
  checkIsOpen();
  if(m_readOnly) {
    throwException(_T("File is readonly"));
  }
}

void CHexViewDoc::setReadOnly(bool readOnly) {
  checkIsOpen();
  if(readOnly != m_readOnly) {
    FILE *newFile = FOPEN(m_fileName, readOnly ? _T("rb") : _T("r+b"));
    fclose(m_file);
    m_file     = newFile;
    m_readOnly = readOnly;
    refresh();
  }
}

String CHexViewDoc::getTitle() const {
  return m_file ? format(_T("HexView - %s%s (%s bytes)"), getFileName().cstr(), (IsModified()?_T(" *"):_T("")), format1000(getSize()).cstr()) : _T("HexView");
}

const TCHAR *CHexViewDoc::getModeString() const {
  return m_file ? m_readOnly ? _T("Read only") : _T("Edit") : _T("");
}

void CHexViewDoc::getBytes(unsigned __int64 start, UINT length, ByteArray &dst) {
  if(getSize() == 0) {
    dst.clear();
    return;
  }
  
  __int64 lastContentIndex = (__int64)m_contentOffset + m_content.size() - 1;
  __int64 lastWantedIndex  = (__int64)start + length - 1;

  if(start < (unsigned __int64)m_contentOffset || lastWantedIndex > lastContentIndex) { // get a new chunk of bytes
    m_contentOffset          = (__int64)start - (int)length * 10;
    m_contentOffset          = max(m_contentOffset, 0);
    UINT bytesToRead = (length + 1) * 20;
    bytesToRead              = max(bytesToRead, 0x4000); // read at least 16k
    BYTE *bytes              = new BYTE[bytesToRead];
    FSEEK(m_file, m_contentOffset);
    const size_t got         = fread(bytes, 1, bytesToRead, m_file);
    m_content.setData(bytes, got);
    delete[] bytes;
  }
  const int offsetInContent = (int)((__int64)start - (__int64)m_contentOffset);
  const int maxLength       = (int)(m_content.size() - offsetInContent);
  length = min(length, (UINT)maxLength);

  if(length == 0) {
    dst.clear();
    return;
  }
  BYTE *data = new BYTE[length];
  memcpy(data, m_content.getData() + offsetInContent, length);
  dst.setData(data, length);
  delete[] data;
  m_modifiedBytes.applyModifications(start, dst);
}

void CHexViewDoc::putBytes(unsigned __int64 start, ByteArray &src) {
  FSEEK(m_file, start);
  FWRITE(src.getData(), 1, src.size(), m_file);
}

void CHexViewDoc::save() {
  checkCanModify();
  if(IsModified()) {
    m_modifiedBytes.applyModifications(*this);
    resetModifications();
    resetContent();
  }
}


void CHexViewDoc::saveAs(const String &newName) {
  SaveAsJob job(newName, *this);
  ProgressWindow(NULL, job, 1000);
  if(!job.isOk()) {
    throwException(job.getErrorMessage());
  } else {
    const bool readOnly = isReadOnly();
    OnOpenDocument((TCHAR*)newName.cstr());
    setReadOnly(readOnly);
  }
}

bool CHexViewDoc::setByte(unsigned __int64 addr, BYTE byte) {
  checkCanModify();
  if(addr < 0 || addr >= getSize()) {
    return false;
  }
  BYTE old;
  if(m_modifiedBytes.addModification(*this, addr, byte, old)) {
    if(m_updateHistory) {
      addToHistory(addr, old, byte);
    }
    return true;
  }
  return false;
}

BYTE CHexViewDoc::getByte(unsigned __int64 addr) {
  ByteArray a;
  getBytes(addr, 1, a);
  if(a.size() != 1) {
    throwException( _T("getByte addr=%s failed. fileSize=%s"), format1000(addr).cstr(), format1000(getSize()).cstr());
  }
  return a[0];
}

void CHexViewDoc::refresh() {
  resetContent();
  resetModifications();
  resetHistory();
  if(m_file) {
    m_stat = STAT64(m_fileName);
  } else {
    resetStat();
  }
}

void CHexViewDoc::resetStat() {
  memset(&m_stat,0, sizeof(m_stat));
}

void CHexViewDoc::resetContent() {
  m_content.clear();
  m_contentOffset = 0;
}

void CHexViewDoc::resetModifications() {
  m_modifiedBytes.clear();
}

void CHexViewDoc::resetHistory() {
  m_editHistory.clear();
  m_historyIndex  = 0;
  m_updateHistory = true;
}

void CHexViewDoc::addToHistory(unsigned __int64 addr, BYTE from, BYTE to) {
  CHECKINVARIANT;

  while(canRedo()) {
    removeLast();
  }
  m_editHistory.add(ByteModification(addr, from, to));
  m_historyIndex = getHistorySize();
  CHECKINVARIANT;
}

void CHexViewDoc::removeLast() {
  m_editHistory.removeLast();
}

__int64 CHexViewDoc::undo() {
  CHECKINVARIANT;
  __int64 addr = -1;
  if(canUndo()) {
    const ByteModification &bm = m_editHistory[--m_historyIndex];
    try {
      m_updateHistory = false;
      setByte(addr = bm.getAddr(), bm.getFrom());
      m_updateHistory = true;
    } catch(...) {
      m_updateHistory = true;
      throw;
    }
  }
  CHECKINVARIANT;
  return addr;
}

__int64 CHexViewDoc::redo() {
  CHECKINVARIANT;
  __int64 addr = -1;
  if(canRedo()) {
    const ByteModification &bm = m_editHistory[m_historyIndex++];
    try {
      m_updateHistory = false;
      setByte(addr = bm.getAddr(), bm.getTo());
      m_updateHistory = true;
    } catch(...) {
      m_updateHistory = true;
      throw;
    }
  }
  CHECKINVARIANT;
  return addr;
}

bool CHexViewDoc::canUndo() const {
  CHECKINVARIANT;
  return !isReadOnly() && (m_historyIndex > 0);
}

bool CHexViewDoc::canRedo() const {
  CHECKINVARIANT;
  return !isReadOnly() && (m_historyIndex < getHistorySize());
}

#ifdef _DEBUG
void CHexViewDoc::checkInvariant(int line) const {
  if(m_historyIndex < 0 || m_historyIndex > getHistorySize()) {
    throwException(_T("Broken invariant in %s, line %d:index=%d, historySize=%d"), __FILE__, line, m_historyIndex, getHistorySize());
  }
}
#endif

void CHexViewDoc::Serialize(CArchive& ar) {
  if (ar.IsStoring()) {
  } else {
  }
}

#ifdef _DEBUG
void CHexViewDoc::AssertValid() const {
  CDocument::AssertValid();
}

void CHexViewDoc::Dump(CDumpContext& dc) const {
  CDocument::Dump(dc);
}
#endif //_DEBUG

