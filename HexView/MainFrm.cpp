#include "stdafx.h"
#include <MFCUtil/Clipboard.h>
#include <MFCUtil/ProgressWindow.h>
#include "EnterAddressDlg.h"
#include "LineSizeDlg.h"
#include "FindDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

static const TCHAR *appName = _T("HexView");

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_CLOSE()
    ON_WM_SIZE()
    ON_WM_KILLFOCUS()
    ON_WM_SETFOCUS()
    ON_WM_SHOWWINDOW()
    ON_COMMAND(ID_FILE_OPEN               , OnFileOpen            )
    ON_COMMAND(ID_FILE_OPENFORUPDATE      , OnFileOpenForUpdate   )
    ON_COMMAND(ID_FILE_CLOSEDOC           , OnFileCloseDoc        )
    ON_COMMAND(ID_FILE_SAVEDOC            , OnFileSaveDoc         )
    ON_COMMAND(ID_FILE_SAVEDOC_AS         , OnFileSaveDocAs       )
    ON_COMMAND(ID_FILE_MRU_FILE1          , OnFileMruFile1        )
    ON_COMMAND(ID_FILE_MRU_FILE2          , OnFileMruFile2        )
    ON_COMMAND(ID_FILE_MRU_FILE3          , OnFileMruFile3        )
    ON_COMMAND(ID_FILE_MRU_FILE4          , OnFileMruFile4        )
    ON_COMMAND(ID_FILE_MRU_FILE5          , OnFileMruFile5        )
    ON_COMMAND(ID_FILE_MRU_FILE6          , OnFileMruFile6        )
    ON_COMMAND(ID_FILE_MRU_FILE7          , OnFileMruFile7        )
    ON_COMMAND(ID_FILE_MRU_FILE8          , OnFileMruFile8        )
    ON_COMMAND(ID_FILE_MRU_FILE9          , OnFileMruFile9        )
    ON_COMMAND(ID_FILE_MRU_FILE10         , OnFileMruFile10       )
    ON_COMMAND(ID_FILE_MRU_FILE11         , OnFileMruFile11       )
    ON_COMMAND(ID_FILE_MRU_FILE12         , OnFileMruFile12       )
    ON_COMMAND(ID_FILE_MRU_FILE13         , OnFileMruFile13       )
    ON_COMMAND(ID_FILE_MRU_FILE14         , OnFileMruFile14       )
    ON_COMMAND(ID_FILE_MRU_FILE15         , OnFileMruFile15       )
    ON_COMMAND(ID_FILE_MRU_FILE16         , OnFileMruFile16       )
    ON_COMMAND(ID_APP_EXIT                , OnAppExit             )
    ON_COMMAND(ID_EDIT_ALLOWEDIT          , OnEditAllowEdit       )
    ON_COMMAND(ID_EDIT_UNDO               , OnEditUndo            )
    ON_COMMAND(ID_EDIT_REDO               , OnEditRedo            )
    ON_COMMAND(ID_EDIT_COPY               , OnEditCopy            )
    ON_COMMAND(ID_EDIT_FIND               , OnEditFind            )
    ON_COMMAND(ID_EDIT_FIND_NEXT          , OnEditFindNext        )
    ON_COMMAND(ID_EDIT_FIND_PREV          , OnEditFindPrev        )
    ON_COMMAND(ID_EDIT_GOTOADDRESS        , OnEditGotoAddress     )
    ON_COMMAND(ID_EDIT_REFRESHFILE        , OnEditRefreshFile     )
    ON_COMMAND(ID_VIEW_DATAOCT            , OnViewDataOct         )
    ON_COMMAND(ID_VIEW_DATADEC            , OnViewDataDec         )
    ON_COMMAND(ID_VIEW_DATAHEX3POS        , OnViewDataHex3Pos     )
    ON_COMMAND(ID_VIEW_DATAHEX2POS        , OnViewDataHex2Pos     )
    ON_COMMAND(ID_VIEW_SHOWADDR           , OnViewShowAddr        )
    ON_COMMAND(ID_VIEW_SHOWASCII          , OnViewShowAscii       )
    ON_COMMAND(ID_VIEW_LINESIZE           , OnViewLineSize        )
    ON_COMMAND(ID_VIEW_ADDROCT            , OnViewAddrOct         )
    ON_COMMAND(ID_VIEW_ADDRDEC            , OnViewAddrDec         )
    ON_COMMAND(ID_VIEW_ADDRHEX            , OnViewAddrHex         )
    ON_COMMAND(ID_ACTIVATE_SETTINGS       , OnActivateSettings    )
    ON_COMMAND(ID_VIEW_COLORS_DATATEXT    , OnViewColorsDataText  )
    ON_COMMAND(ID_VIEW_COLORS_DATABACK    , OnViewColorsDataBack  )
    ON_COMMAND(ID_VIEW_COLORS_ASCIIBACK   , OnViewColorsAsciiBack )
    ON_COMMAND(ID_VIEW_COLORS_ASCIITEXT   , OnViewColorsAsciiText )
    ON_COMMAND(ID_VIEW_COLORS_ADDRTEXT    , OnViewColorsAddrText  )
    ON_COMMAND(ID_VIEW_COLORS_ADDRBACK    , OnViewColorsAddrBack  )
    ON_COMMAND(ID_VIEW_WRAPENDOFLINE      , OnViewWrapEndOfLine   )
    ON_COMMAND(ID_VIEW_DATAHEXUPPERCASE   , OnViewDataHexUppercase)
    ON_COMMAND(ID_VIEW_ADDRHEXUPPERCASE   , OnViewAddrHexUppercase)
    ON_COMMAND(ID_VIEW_ADDRSEPARATORS     , OnViewAddrSeparators  )
    ON_COMMAND(ID_CHARLEFT                , OnCharLeft            )
    ON_COMMAND(ID_CHARRIGHT               , OnCharRight           )
    ON_COMMAND(ID_CTRLCHARLEFT            , OnCtrlCharLeft        )
    ON_COMMAND(ID_CTRLCHARRIGHT           , OnCtrlCharRight       )
    ON_COMMAND(ID_LINEUP                  , OnLineUp              )
    ON_COMMAND(ID_LINEDOWN                , OnLineDown            )
    ON_COMMAND(ID_HOME                    , OnHome                )
    ON_COMMAND(ID_END                     , OnEnd                 )
    ON_COMMAND(ID_CTRLHOME                , OnCtrlHome            )
    ON_COMMAND(ID_CTRLEND                 , OnCtrlEnd             )
    ON_COMMAND(ID_PAGELEFT                , OnPageLeft            )
    ON_COMMAND(ID_PAGERIGHT               , OnPageRight           )
    ON_COMMAND(ID_PAGEUP                  , OnPageUp              )
    ON_COMMAND(ID_PAGEDOWN                , OnPageDown            )
    ON_COMMAND(ID_SHIFT_CHARLEFT          , OnShiftCharLeft       )
    ON_COMMAND(ID_SHIFT_CHARRIGHT         , OnShiftCharRight      )
    ON_COMMAND(ID_SHIFT_CTRLCHARLEFT      , OnShiftCtrlCharLeft   )
    ON_COMMAND(ID_SHIFT_CTRLCHARRIGHT     , OnShiftCtrlCharRight  )
    ON_COMMAND(ID_SHIFT_LINEUP            , OnShiftLineUp         )
    ON_COMMAND(ID_SHIFT_LINEDOWN          , OnShiftLineDown       )
    ON_COMMAND(ID_SHIFT_HOME              , OnShiftHome           )
    ON_COMMAND(ID_SHIFT_END               , OnShiftEnd            )
    ON_COMMAND(ID_SHIFT_CTRLHOME          , OnShiftCtrlHome       )
    ON_COMMAND(ID_SHIFT_CTRLEND           , OnShiftCtrlEnd        )
    ON_COMMAND(ID_SHIFT_PAGELEFT          , OnShiftPageLeft       )
    ON_COMMAND(ID_SHIFT_PAGERIGHT         , OnShiftPageRight      )
    ON_COMMAND(ID_SHIFT_PAGEUP            , OnShiftPageUp         )
    ON_COMMAND(ID_SHIFT_PAGEDOWN          , OnShiftPageDown       )
END_MESSAGE_MAP()

static UINT indicators[] = {
  ID_SEPARATOR           // status line indicator
 ,ID_INDICATOR_POSITION
 ,ID_INDICATOR_READONLY
#if defined(_DEBUG)
 ,ID_INDICATOR_DEBUG
#endif
 ,ID_INDICATOR_CAPS
 ,ID_INDICATOR_NUM
};

#define INDICATORCOUNT ARRAYSIZE(indicators)

CMainFrame::CMainFrame() {
  m_settingsActivated  = false;
  m_statusPanesVisible = true;
  m_searchMachine      = NULL;
}

CMainFrame::~CMainFrame() {
  destroySearchMachine();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(__super::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }
  if(!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
      | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
  {
    TRACE0("Failed to create toolbar\n");
    return -1;      // fail to create
  }

  if(!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators,INDICATORCOUNT)) {
    TRACE0("Failed to create status bar\n");
    return -1;      // fail to create
  }

  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
  EnableDocking(CBRS_ALIGN_ANY);
  DockControlBar(&m_wndToolBar);

  m_bAutoMenuEnable = false;
  return 0;
}

#if defined(_DEBUG)
void CMainFrame::AssertValid() const {
  __super::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
}

#endif //_DEBUG

void CMainFrame::errorMsg(const Exception &e) {
  showException(e, MB_ICONERROR);
}

#define INVALIDATE getView()->Invalidate(false)

bool CMainFrame::newFile(const String &fname, bool readOnly) {
  try {
    CHexViewDoc  *doc  = getDoc();
    CHexViewView *view = getView();
    doc->OnOpenDocument((TCHAR*)fname.cstr());

    setWindowText(this, doc->getTitle());

    if(!readOnly) {
      doc->setReadOnly(false);
    }
    checkMenuItem(this, ID_EDIT_ALLOWEDIT, !readOnly);
    view->resetAnchor();
    view->ctrlHome(false);
    INVALIDATE;
    return true;
  } catch(Exception e ) {
    errorMsg(e);
    return false;
  }
}

void CMainFrame::updateTitle() {
  SetWindowText(getDoc()->getTitle().cstr());
}

void CMainFrame::OnFileOpen() {
  selectAndOpenFile(true);
}

void CMainFrame::OnFileOpenForUpdate() {
  selectAndOpenFile(false);
}

static const TCHAR *fileExtensions = _T("EXE-files (*.exe)\0*.exe\0")
                                     _T("DLL-files (*.dll)\0*.dll\0")
                                     _T("DAT-files (*.dat)\0*.dat\0")
                                     _T("Media-files (*.mp3, *.wma, *.aac, *.wmv, *.mp4, *.mpg)\0*.mp3; *.wma; *.aac; *.wmv; *.mp4; *.mpg\0")
                                     _T("Image-files (*.bmp)\0*.bmp\0")
                                     _T("All files (*.*)\0*.*\0")
                                     _T("\0");

void CMainFrame::selectAndOpenFile(bool readOnly) {
  if(!checkSave()) {
    return;
  }
  const TCHAR *defaultExtTag = _T("DefaultExtention");
  CFileDialog dlg(TRUE);
  CHexViewDoc *doc           = getDoc();
  const String docName       = doc->getFileName();
  if(docName.length() > 0) {
    _tcscpy(dlg.m_ofn.lpstrFile, docName.cstr());
  }

  dlg.m_ofn.lpstrTitle   = readOnly ? _T("Open file") : _T("Open file for update");
  dlg.m_ofn.lpstrFilter  = fileExtensions;
  dlg.m_ofn.nFilterIndex = theApp.GetProfileInt(_T("Settings"),defaultExtTag,0);
  if((dlg.DoModal() == IDOK) && _tcslen(dlg.m_ofn.lpstrFile)) {
    theApp.WriteProfileInt( _T("Settings"),defaultExtTag, dlg.m_ofn.nFilterIndex);
    newFile(dlg.m_ofn.lpstrFile, readOnly);
    theApp.addToRecentFileList(dlg.m_ofn.lpstrFile);
  }
}

void CMainFrame::OnFileCloseDoc() {
  if(!checkSave()) {
    return;
  }
  getDoc()->close();
  updateTitle();
  Invalidate(FALSE);
}

void CMainFrame::OnFileSaveDoc() {
  try {
    if(!getDoc()->IsModified()) {
      return;
    }
    getDoc()->save();
  } catch(Exception e) {
    errorMsg(e);
  }
}

void CMainFrame::OnFileSaveDocAs() {
  try {
    CHexViewDoc *doc  = getDoc();
    const String docName = doc->getFileName();
    if(docName.length() == 0) {
      return;
    }
    TCHAR fileName[256];
    _tcscpy(fileName, docName.cstr());
    CFileDialog dlg(FALSE, NULL, fileName);
    dlg.m_ofn.lpstrTitle = _T("Save file");
    if((dlg.DoModal() == IDOK) && _tcslen(dlg.m_ofn.lpstrFile)) {
      const String name = dlg.m_ofn.lpstrFile;
      doc->saveAs(name);
    }
  } catch(Exception e) {
    errorMsg(e);
  }
}

void CMainFrame::OnFileMruFile(int index) {
  if(!checkSave()) {
    return;
  }
  const String fname = theApp.getRecentFile(index);
  if(ACCESS(fname, 4) < 0) {
    const int errorCode = errno;
    showWarning(_T("%s"), getErrnoText().cstr());
    if(errorCode == ENOENT) {
      theApp.removeFromRecentFiles(index);
    }
    return;
  }

  if(fname.length() > 0) {
    newFile(fname, !shiftKeyPressed());
  }
}

void CMainFrame::OnFileMruFile1()  { OnFileMruFile( 0); }
void CMainFrame::OnFileMruFile2()  { OnFileMruFile( 1); }
void CMainFrame::OnFileMruFile3()  { OnFileMruFile( 2); }
void CMainFrame::OnFileMruFile4()  { OnFileMruFile( 3); }
void CMainFrame::OnFileMruFile5()  { OnFileMruFile( 4); }
void CMainFrame::OnFileMruFile6()  { OnFileMruFile( 5); }
void CMainFrame::OnFileMruFile7()  { OnFileMruFile( 6); }
void CMainFrame::OnFileMruFile8()  { OnFileMruFile( 7); }
void CMainFrame::OnFileMruFile9()  { OnFileMruFile( 8); }
void CMainFrame::OnFileMruFile10() { OnFileMruFile( 9); }
void CMainFrame::OnFileMruFile11() { OnFileMruFile(10); }
void CMainFrame::OnFileMruFile12() { OnFileMruFile(11); }
void CMainFrame::OnFileMruFile13() { OnFileMruFile(12); }
void CMainFrame::OnFileMruFile14() { OnFileMruFile(13); }
void CMainFrame::OnFileMruFile15() { OnFileMruFile(14); }
void CMainFrame::OnFileMruFile16() { OnFileMruFile(15); }

void CMainFrame::OnAppExit() {
  OnClose();
}

void CMainFrame::OnClose() {
  if(checkSave()) {
    __super::OnClose();
  }
}

bool CMainFrame::checkSave() { // return true if the operation should continue, false if cancelled by user
  try {
    if(!getDoc()->IsModified()) {
      return true;
    }
    switch(showMessageBox(MB_YESNOCANCEL | MB_ICONQUESTION, _T("Save file %s"), getDoc()->getFileName().cstr())) {
    case IDYES   : return saveFile();
    case IDNO    : return true;
    case IDCANCEL: return false;
    }
    return false;
  } catch(Exception e) {
    errorMsg(e);
    return false;
  }
}

bool CMainFrame::saveFile() {
  try {
    getDoc()->save();
    updateTitle();
    return true;
  } catch(Exception e) {
    errorMsg(e);
    return false;
  }
}

bool containsNullByte(const ByteArray &b) {
  size_t size = b.size();
  for(const BYTE *p = b.getData(); size--;) {
    if(*(p++) == 0) {
      return true;
    }
  }
  return false;
}

void CMainFrame::OnEditAllowEdit() {
  CHexViewDoc *doc = getDoc();
  if(!doc->isOpen() || !checkSave()) {
    return;
  }
  try {
    doc->setReadOnly(isMenuItemChecked(this, ID_EDIT_ALLOWEDIT));
    checkMenuItem(this, ID_EDIT_ALLOWEDIT, !doc->isReadOnly());
    Invalidate(FALSE);
  } catch(Exception e) {
    errorMsg(e);
  }
}

void CMainFrame::OnEditUndo() {
  CHexViewDoc *doc = getDoc();
  if(doc->canUndo()) {
    getView()->setCurrentAddr(doc->undo(),true);
  }
}

void CMainFrame::OnEditRedo() {
  CHexViewDoc *doc = getDoc();
  if(doc->canRedo()) {
    getView()->setCurrentAddr(doc->redo()+1,true);
  }
}

void CMainFrame::OnEditCopy() {
  try {
    CHexViewView *view = getView();
    const AddrRange selection = view->getSelection();
    if(selection.isEmpty()) {
      return;
    }
    if(selection.getLength() > 0xfffff) {
      showInformation(_T("Selected number of bytes (=%s) is too big to be copied to the clipboard. Max is %s = 1Mb.")
                     ,format1000(selection.getLength()).cstr()
                     ,format1000(0xfffff).cstr());
      return;
    }

    const UINT length = (UINT)selection.getLength();
    ByteArray selectedBytes;
    getDoc()->getBytes(selection.getFirst(), length, selectedBytes);
    if(containsNullByte(selectedBytes)) {
      if(showMessageBox(MB_YESNO | MB_ICONQUESTION
                       ,_T("The selected bytes contains a 0-byte,  which will terminate the string.\r\n"
                           "Do you still want to copy the first part of the selected bytes to the clipboard")
                       ) == IDNO) {
        return;
      }
    }
    const String s = (char*)selectedBytes.add('\0').getData();

    putClipboard(m_hWnd, s);
  } catch(Exception e) {
    errorMsg(e);
  }
}

SearchMachine *CMainFrame::createSearchMachine() {
  destroySearchMachine();
  m_searchMachine = new SearchMachine(); TRACE_NEW(m_searchMachine);
  m_searchMachine->prepareSearch(m_searchParam, getDoc());
  return m_searchMachine;
}
void CMainFrame::destroySearchMachine() {
  SAFEDELETE(m_searchMachine);
}

void CMainFrame::OnEditFind() {
  CHexViewDoc *doc = getDoc();
  if(doc->getSize() == 0) {
    return;
  }
  CHexViewView *view = getView();
  const AddrRange selection = view->getSelection();
  if(!selection.isEmpty()) {
    const int length = min((int)selection.getLength(), 10);
    ByteArray selectedBytes;
    doc->getBytes(selection.getFirst(), length, selectedBytes);
    m_searchParam.m_findWhat = getSettings().unEscape(selectedBytes);
  }
  m_searchParam.m_startPosition = view->getCurrentAddr();
  CFindDlg dlg(this);
  if(dlg.DoModal() == IDOK) {
    const AddrRange &searchResult = dlg.getSearchResult();
    view->dropAnchor(searchResult.getFirst());
    view->setCurrentAddr(searchResult.getLast()+1, true);
  }
}

bool CMainFrame::canFindNext() const {
  return hasSearchMachine() && m_searchMachine->isSet() && (((CMainFrame*)this)->getDoc()->getSize() != 0);
}
bool CMainFrame::canFindPrev() const {
  return canFindNext();
}

void CMainFrame::OnEditFindNext() {
  if(!canFindNext()) return;

  m_searchParam   = m_searchMachine->getSearchParameters();
  CHexViewDoc     *doc           = getDoc();
  CHexViewView    *view          = getView();
  INT64            startPosition = view->getCurrentAddr();
  if(view->hasAnchor()) {
    const AddrRange selection = view->getSelection();
    if((selection.getLength() == m_searchMachine->getPatternLength()) && (startPosition == selection.getLast())) {
      startPosition = selection.getFirst() + 1;
    }
  }
  m_searchParam.m_startPosition = startPosition;
  m_searchParam.m_forwardSearch = true;
  searchText();
}

void CMainFrame::OnEditFindPrev() {
  if(!canFindPrev()) return;

  m_searchParam   = m_searchMachine->getSearchParameters();
  CHexViewDoc     *doc           = getDoc();
  CHexViewView    *view          = getView();
  INT64            startPosition = view->getCurrentAddr()-1;
  if(view->hasAnchor()) {
    const AddrRange selection = view->getSelection();
    if((selection.getLength() == m_searchMachine->getPatternLength()) && (startPosition == selection.getLast()-1)) {
      startPosition = selection.getFirst();
    }
  }
  m_searchParam.m_startPosition = startPosition;
  m_searchParam.m_forwardSearch = false;
  searchText();
}

void CMainFrame::searchText() {
  ProgressWindow prgWnd(this, *createSearchMachine(), 500);
  AddrRange result = m_searchMachine->getResult();
  if(result.isEmpty()) {
    if(!m_searchMachine->isInterrupted()) {
      showInformation(m_searchMachine->getErrorMsg());
    }
  } else {
    CHexViewView *view = getView();
    view->dropAnchor(result.getFirst());
    view->setCurrentAddr(result.getLast()+1,true);
  }
}

void CMainFrame::OnEditGotoAddress() {
  CHexViewDoc *doc     = getDoc();
  const UINT64 docSize = doc->getSize();
  if(docSize == 0) {
    return;
  }
  CEnterAddressDlg dlg(docSize);
  if(dlg.DoModal() == IDOK) {
    CHexViewView *view = getView();
    view->resetAnchor();
    view->setCurrentAddr(dlg.getAddress(),true);
  }
}

void CMainFrame::OnEditRefreshFile() {
  CHexViewDoc *doc = getDoc();
  if(doc->IsModified()) {
    if(MessageBox(_T("The file has been modified. Refreshing the file will discard all changes.\r\nDo you want to continue")
                 ,appName
                 ,MB_YESNO | MB_ICONQUESTION) == IDNO) {
      return;
    }
  }
  doc->refresh();
  updateTitle();
  Invalidate(FALSE);
}

void CMainFrame::enableToolbarButtonAndMenuItem(int id, bool enable) {
  CToolBarCtrl &ctrlBar = m_wndToolBar.GetToolBarCtrl();
  if(enable == (ctrlBar.IsButtonIndeterminate(id)?true:false)) {
    if(enable) {
      ctrlBar.SetState(id, TBSTATE_ENABLED);
      ctrlBar.EnableButton(id, TRUE);
    } else {
      ctrlBar.SetState(id, TBSTATE_INDETERMINATE);
      ctrlBar.EnableButton(id, FALSE);
    }
  }
  enableMenuItem(this, id, enable);
}

void CMainFrame::showStatusBarPanes(bool show) {
  if(show != m_statusPanesVisible) {
    if(show) {
      for(size_t i = 0; i < m_paneInfo.size(); i++) {
        const StatusBarPaneInfo &info = m_paneInfo[i];
        m_wndStatusBar.SetPaneInfo(info.m_index, info.m_id, info.m_style, info.m_width);
      }
    } else {
      if(m_paneInfo.isEmpty()) {
        for(int i = 1; i < INDICATORCOUNT; i++) {
          StatusBarPaneInfo info;
          m_wndStatusBar.GetPaneInfo(i, info.m_id, info.m_style, info.m_width);
          info.m_index = i;
          m_paneInfo.add(info);
        }
      }
      for(int i = 1; i < INDICATORCOUNT; i++) {
        m_wndStatusBar.SetPaneInfo(i, indicators[i], SBPS_NOBORDERS | SBPS_DISABLED ,0);
      }
    }
    m_statusPanesVisible = show;
  }
}

void CMainFrame::ajourMenuItems() {
  const CHexViewDoc *doc      = getDoc();
  const bool         hasBytes = doc->getSize() > 0;

  enableMenuItem(          this, ID_FILE_SAVEDOC    , doc->IsModified()                     );
  enableMenuItem(          this, ID_FILE_SAVEDOC_AS , doc->isOpen()                         );
  enableMenuItem(          this, ID_FILE_CLOSEDOC   , doc->isOpen()                         );
  enableMenuItem(          this, ID_EDIT_ALLOWEDIT  , hasBytes                              );
  enableMenuItem(          this, ID_EDIT_UNDO       , hasBytes && doc->canUndo()            );
  enableMenuItem(          this, ID_EDIT_REDO       , hasBytes && doc->canRedo()            );
  enableToolbarButtonAndMenuItem(ID_EDIT_COPY       , !getView()->getSelection().isEmpty()  );
  enableToolbarButtonAndMenuItem(ID_EDIT_FIND       , hasBytes                              );
  enableToolbarButtonAndMenuItem(ID_EDIT_FIND_NEXT  , canFindNext()                         );
  enableToolbarButtonAndMenuItem(ID_EDIT_FIND_PREV  , canFindNext()                         );
  enableMenuItem(          this, ID_EDIT_GOTOADDRESS, hasBytes                              );
  enableMenuItem(          this, ID_EDIT_REFRESHFILE, doc->isOpen()                         );

  showStatusBarPanes(doc->isOpen());
}

bool CMainFrame::setDataRadix(int radix, bool hex3Pos) {
  checkMenuItem(this, ID_VIEW_DATAOCT    , false);
  checkMenuItem(this, ID_VIEW_DATADEC    , false);
  checkMenuItem(this, ID_VIEW_DATAHEX3POS, false);
  checkMenuItem(this, ID_VIEW_DATAHEX2POS, false);

  int menuItemToCheck;
  switch(radix) {
  case 8 : menuItemToCheck = ID_VIEW_DATAOCT; break;
  case 10: menuItemToCheck = ID_VIEW_DATADEC; break;
  case 16: menuItemToCheck = hex3Pos ? ID_VIEW_DATAHEX3POS : ID_VIEW_DATAHEX2POS; break;
  }
  checkMenuItem(this, menuItemToCheck, true);

  Settings &settings = getSettings();
  bool repaint = settings.setDataRadix(radix);
  if(radix == 16) {
    repaint |= settings.setHex3Pos(hex3Pos);
  }
  return repaint;
}

bool CMainFrame::setAddrRadix(int radix) {
  checkMenuItem(this, ID_VIEW_ADDROCT , false);
  checkMenuItem(this, ID_VIEW_ADDRDEC , false);
  checkMenuItem(this, ID_VIEW_ADDRHEX , false);

  int menuItemToCheck;
  switch(radix) {
  case 8 : menuItemToCheck = ID_VIEW_ADDROCT; break;
  case 10: menuItemToCheck = ID_VIEW_ADDRDEC; break;
  case 16: menuItemToCheck = ID_VIEW_ADDRHEX; break;
  }
  checkMenuItem(this, menuItemToCheck, true);

  Settings &settings = getSettings();
  const bool repaint = settings.setAddrRadix(radix);

  if(m_settingsActivated && !settings.getShowAddr()) {
    OnViewShowAddr();
    return false;
  }
  return repaint;
}

void CMainFrame::OnViewDataOct() {
  if(setDataRadix(8)) {
    getView()->keepSelection().resetDigitIndex().Invalidate();
  }
}

void CMainFrame::OnViewDataDec() {
  if(setDataRadix(10)) {
    getView()->keepSelection().resetDigitIndex().Invalidate();
  }
}

void CMainFrame::OnViewDataHex3Pos() {
  if(setDataRadix(16, true)) {
    getView()->keepSelection().resetDigitIndex().Invalidate();
  }
}

void CMainFrame::OnViewDataHex2Pos() {
  if(setDataRadix(16, false)) {
    getView()->keepSelection().resetDigitIndex().Invalidate();
  }
}

void CMainFrame::OnViewShowAddr() {
  if(getSettings().setShowAddr(toggleMenuItem(this, ID_VIEW_SHOWADDR))) {;
    getView()->keepSelection().Invalidate();
  }
}

void CMainFrame::OnViewShowAscii() {
  if(getSettings().setShowAscii(toggleMenuItem(this, ID_VIEW_SHOWASCII))) {
    getView()->keepSelection().resetDigitIndex().Invalidate();
  }
}

void CMainFrame::OnViewLineSize() {
  CLineSizeDlg dlg;
  if(dlg.DoModal()) {
    CHexViewView *view = getView();
    const INT64 addr = view->getCurrentAddr();
    view->updateSettings();
    view->setCurrentAddr(addr, true);
  }
}

void CMainFrame::OnViewAddrOct() {
  if(setAddrRadix(8)) {
    getView()->keepSelection().Invalidate();
  }
}

void CMainFrame::OnViewAddrDec() {
  if(setAddrRadix(10)) {
    getView()->keepSelection().Invalidate();
  }
}

void CMainFrame::OnViewAddrHex() {
  if(setAddrRadix(16)) {
    getView()->keepSelection().Invalidate();
  }
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  if(m_settingsActivated) {
    getSettings().setWindowSize(getWindowSize(this));
  }
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) {
  __super::OnShowWindow(bShow, nStatus);
  if(bShow) {
    if(!m_settingsActivated) {
      PostMessage(WM_COMMAND, MAKELONG(ID_ACTIVATE_SETTINGS, 0), 0);
    }
  }
}

void CMainFrame::OnActivateSettings() {
  const Settings settings = getSettings();
  setDataRadix(settings.getDataRadix(), settings.getHex3Pos());
  setAddrRadix(settings.getAddrRadix());
  checkMenuItem(this, ID_VIEW_SHOWADDR        , settings.getShowAddr()        );
  checkMenuItem(this, ID_VIEW_SHOWASCII       , settings.getShowAscii()       );
  checkMenuItem(this, ID_VIEW_WRAPENDOFLINE   , settings.getWrapEndOfLine()   );
  checkMenuItem(this, ID_VIEW_DATAHEXUPPERCASE, settings.getDataHexUppercase());
  checkMenuItem(this, ID_VIEW_ADDRHEXUPPERCASE, settings.getAddrHexUppercase());
  checkMenuItem(this, ID_VIEW_ADDRSEPARATORS  , settings.getAddrSeparators()  );
  setWindowSize(this, getSettings().getWindowSize());
  m_settingsActivated = true;
  centerWindow(this);
  updateTitle();
}

void CMainFrame::OnViewColorsDataText() {
  Settings &settings = getSettings();
  COLORREF color = settings.getDataTextColor();
  if(editColor(color) && settings.setDataTextColor(color)) {
    getView()->keepSelection().Invalidate();
  }
}

void CMainFrame::OnViewColorsDataBack() {
  Settings &settings = getSettings();
  COLORREF color = settings.getDataBackColor();
  if(editColor(color) && settings.setDataBackColor(color)) {
    getView()->keepSelection().Invalidate();
  }
}

void CMainFrame::OnViewColorsAsciiText() {
  Settings &settings = getSettings();
  COLORREF color = settings.getAsciiTextColor();
  if(editColor(color) && settings.setAsciiTextColor(color)) {
    getView()->keepSelection().Invalidate();
  }
}

void CMainFrame::OnViewColorsAsciiBack() {
  Settings &settings = getSettings();
  COLORREF color = settings.getAsciiBackColor();
  if(editColor(color) && settings.setAsciiBackColor(color)) {
    getView()->keepSelection().Invalidate();
  }
}

void CMainFrame::OnViewColorsAddrText() {
  Settings &settings = getSettings();
  COLORREF color = settings.getAddrTextColor();
  if(editColor(color) && settings.setAddrTextColor(color)) {
    getView()->keepSelection().Invalidate();
  }
}

void CMainFrame::OnViewColorsAddrBack() {
  Settings &settings = getSettings();
  COLORREF color = settings.getAddrBackColor();
  if(editColor(color) && settings.setAddrBackColor(color)) {
    getView()->keepSelection().Invalidate();
  }
}

bool CMainFrame::editColor(COLORREF &color) {
  CColorDialog dlg;
  dlg.m_cc.Flags |= CC_RGBINIT | CC_FULLOPEN;
  dlg.m_cc.rgbResult = color;
  if(dlg.DoModal() == IDOK) {
    color = dlg.m_cc.rgbResult;
    return true;
  }
  return false;
}

void CMainFrame::OnViewWrapEndOfLine() {
  getSettings().setWrapEndOfLine(toggleMenuItem(this, ID_VIEW_WRAPENDOFLINE));
}

void CMainFrame::OnViewDataHexUppercase() {
  if(getSettings().setDataHexUppercase(toggleMenuItem(this, ID_VIEW_DATAHEXUPPERCASE))) {
    getView()->keepSelection().Invalidate();
  }
}

void CMainFrame::OnViewAddrHexUppercase() {
  if(getSettings().setAddrHexUppercase(toggleMenuItem(this, ID_VIEW_ADDRHEXUPPERCASE))) {
    getView()->keepSelection().Invalidate();
  }
}

void CMainFrame::OnViewAddrSeparators() {
  if(getSettings().setAddrSeparators(toggleMenuItem(this, ID_VIEW_ADDRSEPARATORS))) {
    getView()->keepSelection().Invalidate();
  }
}

void CMainFrame::OnCharLeft() {
  getView()->charLeft(false);
}

void CMainFrame::OnCharRight() {
  getView()->charRight(false);
}

void CMainFrame::OnCtrlCharLeft() {
  getView()->ctrlCharLeft(false);
}

void CMainFrame::OnCtrlCharRight() {
  getView()->ctrlCharRight(false);
}

void CMainFrame::OnLineUp() {
  getView()->lineUp(false);
}

void CMainFrame::OnLineDown() {
  getView()->lineDown(false);
}

void CMainFrame::OnHome() {
  getView()->home(false);
}

void CMainFrame::OnEnd() {
  getView()->end(false);
}

void CMainFrame::OnCtrlHome() {
  getView()->ctrlHome(false);
}

void CMainFrame::OnCtrlEnd() {
  getView()->ctrlEnd(false);
}

void CMainFrame::OnPageLeft() {
  getView()->pageLeft(false);
}

void CMainFrame::OnPageRight() {
  getView()->pageRight(false);
}

void CMainFrame::OnPageUp() {
  getView()->pageUp(false);
}

void CMainFrame::OnPageDown() {
  getView()->pageDown(false);
}

void CMainFrame::OnShiftCharLeft() {
  getView()->charLeft(true);
}

void CMainFrame::OnShiftCharRight() {
  getView()->charRight(true);
}

void CMainFrame::OnShiftCtrlCharLeft() {
  getView()->ctrlCharLeft(true);
}

void CMainFrame::OnShiftCtrlCharRight() {
  getView()->ctrlCharRight(true);
}

void CMainFrame::OnShiftLineUp() {
  getView()->lineUp(true);
}

void CMainFrame::OnShiftLineDown() {
  getView()->lineDown(true);
}

void CMainFrame::OnShiftHome() {
  getView()->home(true);
}

void CMainFrame::OnShiftEnd() {
  getView()->end(true);
}

void CMainFrame::OnShiftCtrlHome() {
  getView()->ctrlHome(true);
}

void CMainFrame::OnShiftCtrlEnd() {
  getView()->ctrlEnd(true);
}

void CMainFrame::OnShiftPageLeft() {
  getView()->pageLeft(true);
}

void CMainFrame::OnShiftPageRight() {
  getView()->pageRight(true);
}

void CMainFrame::OnShiftPageUp() {
  getView()->pageUp(true);
}

void CMainFrame::OnShiftPageDown() {
  getView()->pageDown(true);
}

BOOL CMainFrame::PreTranslateMessage(MSG *pMsg) {
  BOOL ret = __super::PreTranslateMessage(pMsg);
  m_wndStatusBar.SetPaneText(1, format(_T("%s:%s"), getSettings().getAddrRadixShortName(), getView()->getCurrentAddrAsString().cstr()).cstr());
  m_wndStatusBar.SetPaneText(2, getDoc()->getModeString());
#if defined(_DEBUG)
//  m_wndStatusBar.SetPaneText(3, getView()->getDebugString().cstr());
#endif
  return ret;
}

void CMainFrame::OnKillFocus(CWnd *pNewWnd) {
  __super::OnKillFocus(pNewWnd);
  getView()->hideCaret();
}

void CMainFrame::OnSetFocus(CWnd *pOldWnd) {
  __super::OnSetFocus(pOldWnd);
  getView()->keepSelection().showCaret();
}
