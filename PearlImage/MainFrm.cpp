#include "stdafx.h"
#include <MFCUtil/ColorSpace.h>
#include "MainFrm.h"
#include "Degreedlg.h"
#include "ScaleDlg.h"
#include "GridDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_CLOSE()
    ON_COMMAND(ID_APP_EXIT                        , OnAppExit                         )
    ON_COMMAND(ID_FILE_OPEN                       , OnFileOpen                        )
    ON_COMMAND(ID_FILE_GEM                        , OnFileSave                        )
    ON_COMMAND(ID_FILE_GEM_SOM                    , OnFileSaveAs                      )
    ON_COMMAND(ID_FILE_MRU_FILE1                  , OnFileMruFile1                    )
    ON_COMMAND(ID_FILE_MRU_FILE2                  , OnFileMruFile2                    )
    ON_COMMAND(ID_FILE_MRU_FILE3                  , OnFileMruFile3                    )
    ON_COMMAND(ID_FILE_MRU_FILE4                  , OnFileMruFile4                    )
    ON_COMMAND(ID_FILE_MRU_FILE5                  , OnFileMruFile5                    )
    ON_COMMAND(ID_FILE_MRU_FILE6                  , OnFileMruFile6                    )
    ON_COMMAND(ID_FILE_MRU_FILE7                  , OnFileMruFile7                    )
    ON_COMMAND(ID_FILE_MRU_FILE8                  , OnFileMruFile8                    )
    ON_COMMAND(ID_FILE_MRU_FILE9                  , OnFileMruFile9                    )
    ON_COMMAND(ID_FILE_MRU_FILE10                 , OnFileMruFile10                   )
    ON_COMMAND(ID_FILE_MRU_FILE11                 , OnFileMruFile11                   )
    ON_COMMAND(ID_FILE_MRU_FILE12                 , OnFileMruFile12                   )
    ON_COMMAND(ID_FILE_MRU_FILE13                 , OnFileMruFile13                   )
    ON_COMMAND(ID_FILE_MRU_FILE14                 , OnFileMruFile14                   )
    ON_COMMAND(ID_FILE_MRU_FILE15                 , OnFileMruFile15                   )
    ON_COMMAND(ID_FILE_MRU_FILE16                 , OnFileMruFile16                   )
    ON_COMMAND(ID_EDIT_UNDO                       , OnEditUndo                        )
    ON_COMMAND(ID_EDIT_REDO                       , OnEditRedo                        )
    ON_COMMAND(ID_EDIT_COPY                       , OnEditCopy                        )
    ON_COMMAND(ID_OPTIONS_ZOOM_X1                 , OnOptionsZoomX1                   )
    ON_COMMAND(ID_OPTIONS_ZOOM_X2                 , OnOptionsZoomX2                   )
    ON_COMMAND(ID_OPTIONS_ZOOM_X4                 , OnOptionsZoomX4                   )
    ON_COMMAND(ID_OPTIONS_ZOOM_X8                 , OnOptionsZoomX8                   )
    ON_COMMAND(ID_FUNCTION_ROTATE                 , OnFunctionRotate                  )
    ON_COMMAND(ID_FUNCTION_SCALE                  , OnFunctionScale                   )
    ON_COMMAND(ID_FUNCTION_MIRROR_HORIZONTAL      , OnFunctionMirrorHorizontal        )
    ON_COMMAND(ID_FUNCTION_MIRROR_VERTICAL        , OnFunctionMirrorVertical          )
    ON_COMMAND(ID_FUNCTION_MAKEGRAYSCALE          , OnFunctionMakegrayscale           )
    ON_COMMAND(ID_FUNCTION_MAKEPEARLGRID          , OnFunctionMakePearlGrid           )
    ON_COMMAND(ID_SCROLL_LINE_DOWN                , OnScrollLineDown                  )
    ON_COMMAND(ID_SCROLL_LINE_UP                  , OnScrollLineUp                    )
    ON_COMMAND(ID_SCROLL_PAGE_DOWN                , OnScrollPageDown                  )
    ON_COMMAND(ID_SCROLL_PAGE_UP                  , OnScrollPageUp                    )
    ON_COMMAND(ID_SCROLL_LEFT                     , OnScrollLeft                      )
    ON_COMMAND(ID_SCROLL_RIGHT                    , OnScrollRight                     )
    ON_COMMAND(ID_SCROLL_PAGE_LEFT                , OnScrollPageLeft                  )
    ON_COMMAND(ID_SCROLL_PAGE_RIGHT               , OnScrollPageRight                 )
    ON_COMMAND(ID_SCROLL_TO_TOP                   , OnScrollToTop                     )
    ON_COMMAND(ID_SCROLL_TO_BOTTOM                , OnScrollToBottom                  )
    ON_COMMAND(ID_SCROLL_TO_LEFT                  , OnScrollToLeft                    )
    ON_COMMAND(ID_SCROLL_TO_RIGHT                 , OnScrollToRight                   )
    ON_MESSAGE(ID_MSG_CALCULATEIMAGE              , OnMsgCalculateImage               )
END_MESSAGE_MAP()

static UINT indicators[] = {
  IDS_MOUSEPOSITION
 ,ID_SEPARATOR           // status line indicator
 ,IDS_INFOFIELD
};

// --------------------------- Protected ---------------------------

CMainFrame::CMainFrame() {
  m_currentDegree            = 0;
  m_gridDlg                  = NULL;
  m_gridDlgThread            = NULL;
}

// --------------------------- Public ---------------------------

CMainFrame::~CMainFrame() {
}

BOOL CMainFrame::PreTranslateMessage(MSG *pMsg) {
  BOOL result;
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    result = TRUE;
  } else {
    result = __super::PreTranslateMessage(pMsg);
  }
  CPearlImageView *view = getView();
  if(view->isMouseOnDocument()) {
    CPoint mp = getView()->getCurrentMousePoint();
    m_wndStatusBar.SetPaneText(0,format(_T("%3d,%3d px"),mp.x,mp.y).cstr());
  } else {
    m_wndStatusBar.SetPaneText(0,EMPTYSTRING);
  }
  m_wndStatusBar.SetPaneText(2,getDocument()->getInfo().cstr());
  ajourEnabling();
  return result;
}

bool CMainFrame::loadFile(const String &fileName) {
  try {
    CPearlImageDoc *doc = getDocument();
    doc->OnOpenDocument(fileName.cstr());
    updateTitle();
    getView()->clear();
    if(m_gridDlgThread->isDialogVisible()) {
      m_gridDlg->setImage(doc->getImage());
    } else {
      getView()->refreshDoc();
    }
    return true;
  } catch(Exception e) {
    MessageBox(e.what());
    return false;
  }
}

static const int zoomItems[] = {
  ID_OPTIONS_ZOOM_X1
 ,ID_OPTIONS_ZOOM_X2
 ,ID_OPTIONS_ZOOM_X4
 ,ID_OPTIONS_ZOOM_X8
};

void CMainFrame::setCurrentZoomFactor(int id) {
  for(int i = 0; i < ARRAYSIZE(zoomItems); i++) {
    checkMenuItem(this,zoomItems[i],false);
  }
  checkMenuItem(this,id,true);
  switch(id) {
  case ID_OPTIONS_ZOOM_X1: getView()->setCurrentZoomFactor(1); break;
  case ID_OPTIONS_ZOOM_X2: getView()->setCurrentZoomFactor(2); break;
  case ID_OPTIONS_ZOOM_X4: getView()->setCurrentZoomFactor(4); break;
  case ID_OPTIONS_ZOOM_X8: getView()->setCurrentZoomFactor(8); break;
  default                : getView()->setCurrentZoomFactor(1); break;
  }
}

void CMainFrame::updateTitle() {
  CPearlImageDoc *doc   = getDocument();
  CString         title = doc->GetTitle();
  CSize           size  = doc->getSize();
  setWindowText(this, format(_T("PearlImage %s - %dx%d"), (LPCTSTR)title, size.cx,size.cy));
}

void CMainFrame::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(source == m_gridDlg) {
    switch (id) {
    case PROP_GRIDPARAM:
      const GridParameters *param = (GridParameters*)newValue;
      SendMessage(ID_MSG_CALCULATEIMAGE, (WPARAM)param,0);
      break;
    }
  }
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const {
  __super::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
}

#endif //_DEBUG

// -------------------------- Message handlers ---------------------------

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

  if(!m_wndStatusBar.Create(this) ||!m_wndStatusBar.SetIndicators(indicators,sizeof(indicators)/sizeof(UINT))) {
    TRACE0("Failed to create status bar\n");
    return -1;      // fail to create
  }

  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
  EnableDocking(CBRS_ALIGN_ANY);
  DockControlBar(&m_wndToolBar);

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
  m_bAutoMenuEnable = false;
  theApp.m_device.attach(*this);

  createGridDlg();
  return 0;
}

void CMainFrame::OnDestroy() {
  destroyGridDlg();
  __super::OnDestroy();
}

void CMainFrame::OnClose() {
  OnAppExit();
}

void CMainFrame::OnAppExit() {
  if(!checkSave()) {
    return;
  }
  PostMessage(WM_QUIT);
}

void CMainFrame::OnFileOpen() {
  if(!checkSave()) {
    return;
  }
  const String fileName = getLoadFileName();
  if(fileName.length() == 0) {
    return;
  }
  loadFile(fileName);
}

void CMainFrame::OnFileSave() {
  onFileSave();
}

void CMainFrame::OnFileSaveAs() {
  CString name = getDocument()->GetPathName();
  FileNameSplitter info((LPCTSTR)name);
  CFileDialog dlg(FALSE,info.getExtension().cstr(),name);
  dlg.m_ofn.lpstrTitle  = _T("Save image");
  dlg.m_ofn.lpstrFilter = s_saveFileDialogExtensions;
  if(dlg.DoModal() == IDOK) {
    name = dlg.m_ofn.lpstrFile;
    save(name);
  }
}

void CMainFrame::OnFileMruFile1()  { onFileMruFile( 0); }
void CMainFrame::OnFileMruFile2()  { onFileMruFile( 1); }
void CMainFrame::OnFileMruFile3()  { onFileMruFile( 2); }
void CMainFrame::OnFileMruFile4()  { onFileMruFile( 3); }
void CMainFrame::OnFileMruFile5()  { onFileMruFile( 4); }
void CMainFrame::OnFileMruFile6()  { onFileMruFile( 5); }
void CMainFrame::OnFileMruFile7()  { onFileMruFile( 6); }
void CMainFrame::OnFileMruFile8()  { onFileMruFile( 7); }
void CMainFrame::OnFileMruFile9()  { onFileMruFile( 8); }
void CMainFrame::OnFileMruFile10() { onFileMruFile( 9); }
void CMainFrame::OnFileMruFile11() { onFileMruFile(10); }
void CMainFrame::OnFileMruFile12() { onFileMruFile(11); }
void CMainFrame::OnFileMruFile13() { onFileMruFile(12); }
void CMainFrame::OnFileMruFile14() { onFileMruFile(13); }
void CMainFrame::OnFileMruFile15() { onFileMruFile(14); }
void CMainFrame::OnFileMruFile16() { onFileMruFile(15); }

void CMainFrame::OnEditUndo() {
  if(getDocument()->undo()) {
    getView()->repaint();
    updateTitle();
  }
}

void CMainFrame::OnEditRedo() {
  if(getDocument()->redo()) {
    getView()->repaint();
    updateTitle();
  }
}

void CMainFrame::OnEditCopy() {
  if(getDocument()->hasImage()) {
    const PixRect *pr = getDocument()->getImage();
    HBITMAP bitmap = *pr;
    putClipboard(*theApp.GetMainWnd(),bitmap);
    DeleteObject(bitmap);
  }
}

void CMainFrame::OnOptionsZoomX1() {
  setCurrentZoomFactor(ID_OPTIONS_ZOOM_X1);
}

void CMainFrame::OnOptionsZoomX2() {
  setCurrentZoomFactor(ID_OPTIONS_ZOOM_X2);
}

void CMainFrame::OnOptionsZoomX4() {
  setCurrentZoomFactor(ID_OPTIONS_ZOOM_X4);
}

void CMainFrame::OnOptionsZoomX8() {
  setCurrentZoomFactor(ID_OPTIONS_ZOOM_X8);
}

void CMainFrame::OnFunctionRotate() {
  CDegreeDlg dlg(m_currentDegree);
  if(dlg.DoModal() != IDOK) {
    return;
  }
  m_currentDegree = dlg.getDegree();

  theApp.BeginWaitCursor();

  CPearlImageDoc *doc = getDocument();
  saveDocState();
  doc->setImage(PixRect::rotateImage(doc->getImage(),m_currentDegree));

  theApp.EndWaitCursor();
  getView()->refreshDoc();
  updateTitle();
}

void CMainFrame::OnFunctionScale() {
  CScaleDlg dlg(m_currentScale);
  if(dlg.DoModal() != IDOK) {
    return;
  }
  m_currentScale = dlg.getScaleParameters();

  theApp.BeginWaitCursor();

  CPearlImageDoc *doc = getDocument();
  saveDocState();
  doc->setImage(PixRect::scaleImage(doc->getImage(),m_currentScale));

  theApp.EndWaitCursor();
  getView()->refreshDoc();
  updateTitle();
}

void CMainFrame::OnFunctionMirrorHorizontal()    { applyMirror(false);                 }
void CMainFrame::OnFunctionMirrorVertical()      { applyMirror(true);                  }
void CMainFrame::OnFunctionMakegrayscale()       { applyFilter(GrayScaleFilter());     }

void CMainFrame::OnFunctionMakePearlGrid() {
  CPearlImageDoc *doc = getDocument();
  if(!doc->hasImage()) return;

  saveDocState();
  m_gridDlgThread->setCurrentDialogProperty(&doc->getGridParameters());
  m_gridDlg->setImage(doc->getImage());
  if(!m_gridDlgThread->isDialogVisible()) {
    m_gridDlgThread->setDialogVisible(true);
  }
}

void CMainFrame::OnScrollLineDown() {
  scroll(0,20);
}

void CMainFrame::OnScrollLineUp() {
  scroll(0,-20);
}

void CMainFrame::OnScrollPageDown() {
  scroll(0,getClientRect(getView()).Height());
}

void CMainFrame::OnScrollPageUp() {
  scroll(0,-getClientRect(getView()).Height());
}

void CMainFrame::OnScrollLeft() {
  scroll(-20,0);
}

void CMainFrame::OnScrollRight() {
  scroll(20,0);
}

void CMainFrame::OnScrollPageLeft() {
  scroll(-getClientRect(getView()).Width(),0);
}

void CMainFrame::OnScrollPageRight() {
  scroll(getClientRect(getView()).Width(),0);
}

void CMainFrame::OnScrollToTop() {
  const CPoint topLeft = getView()->GetScrollPosition();
  getView()->ScrollToPosition(CPoint(topLeft.x,0));
}

void CMainFrame::OnScrollToBottom() {
  const CPoint topLeft = getView()->GetScrollPosition();
  getView()->ScrollToPosition(CPoint(topLeft.x,getMaxScroll().y));
}

void CMainFrame::OnScrollToLeft() {
  const CPoint topLeft = getView()->GetScrollPosition();
  getView()->ScrollToPosition(CPoint(0,topLeft.y));
}

void CMainFrame::OnScrollToRight() {
  const CPoint topLeft = getView()->GetScrollPosition();
  getView()->ScrollToPosition(CPoint(getMaxScroll().x,topLeft.y));
}

LRESULT CMainFrame::OnMsgCalculateImage(WPARAM wp, LPARAM lp) {
  CPearlImageDoc       *doc   = getDocument();
  const GridParameters *param = (GridParameters*)wp;
  doc->setImage(param->calculateImage(m_gridDlg->getImage()));
  Invalidate();
  return 0;
}

// -------------------------- Private ----------------------------------

const TCHAR *CMainFrame::s_saveFileDialogExtensions = _T("Bitmap files (*.bmp)\0*.bmp;\0"
                                                         "JPEG files (*.jpg)\0*.jpg;\0"
                                                         "PNG-files (*.png)\0*.png;\0"
                                                         "TIFF files (*.tiff)\0*.tiff;\0"
                                                         "All files (*.*)\0*.*\0\0");

const TCHAR *CMainFrame::s_loadFileDialogExtensions = _T("Picture files\0*.bmp;*.jpg;*.png;*.tiff;*.gif;*.ico;*.cur;*.dib;\0"
                                                         "BMP-Files (*.bmp)\0*.bmp;\0"
                                                         "JPG-files (*.jpg)\0*.jpg;\0"
                                                         "PNG-files (*.png)\0*.png;\0"
                                                         "TIFF-files (*.tiff)\0*.tiff;\0"
                                                         "GIF-files (*.gif)\0*.gif;\0"
                                                         "ICO-files (*.ico)\0*.ico;\0"
                                                         "CUR-files (*.cur)\0*.cur;\0"
                                                         "DIB-files (*.dib)\0*.dib;\0"
                                                         "All files (*.*)\0*.*\0\0");


String CMainFrame::getLoadFileName() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = s_loadFileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open file");
  dlg.m_ofn.Flags |= OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLESIZING;

  if(dlg.DoModal() != IDOK) {
    return EMPTYSTRING;
  } else {
    return dlg.m_ofn.lpstrFile;
  }
}

void CMainFrame::onFileMruFile(int index) {
  if(!checkSave()) {
    return;
  }
  if(!loadFile(theApp.getRecentFile(index))) {
    theApp.removeRecentFile(index);
  }
}

bool CMainFrame::onFileSave() { // retuns true if succeeded
  CPearlImageDoc *doc = getDocument();
  CString name = doc->GetPathName();
  if(doc->hasDefaultName()) {
    CFileDialog dlg(FALSE,_T("bmp"),name);
    dlg.m_ofn.lpstrTitle  = _T("Save image");
    dlg.m_ofn.lpstrFilter = s_saveFileDialogExtensions;
    if(dlg.DoModal() == IDOK) {
      name = dlg.m_ofn.lpstrFile;
    } else {
      return false;
    }
  }
  return save(name);
}

bool CMainFrame::save(const CString &name) { // returns true if succeeded
  try {
    getDocument()->OnSaveDocument(name);
    updateTitle();
    return true;
  } catch(Exception e) {
    MessageBox(e.what());
    return false;
  }
}

bool CMainFrame::checkSave() { // return true to continue user action
  if(!getDocument()->IsModified()) {
    return true;
  }
  switch(MessageBox(_T("Save changes?"),_T("Save"),MB_YESNOCANCEL | MB_ICONEXCLAMATION)) {
  case IDYES   :
    return onFileSave();
  case IDNO    :
    return true;
  case IDCANCEL:
    return false;
  }
  return false;
}

void CMainFrame::saveDocState() {
  getDocument()->saveState();
}

void CMainFrame::ajourEnabling() {
  CPearlImageDoc *doc            = getDocument();
  const bool      hasImage       = doc->hasImage();
  const bool      gridDlgVisible = m_gridDlgThread->isDialogVisible();

  enableMenuItem(this, ID_FILE_GEM          , doc->IsModified()?true:false);
  enableMenuItem(this, ID_FILE_GEM_SOM      , hasImage);
  enableMenuItem(this, ID_EDIT_COPY         , hasImage);
  enableMenuItem(this, ID_FILE_PRINT        , hasImage);
  enableMenuItem(this, ID_FILE_PRINT_DIRECT , hasImage);
  enableMenuItem(this, ID_FILE_PRINT_PREVIEW, hasImage);

  enableMenuItem(this, ID_EDIT_UNDO         , !gridDlgVisible && doc->canUndo());
  enableMenuItem(this, ID_EDIT_REDO         , !gridDlgVisible && doc->canRedo());

  enableSubMenuContainingId(this, ID_OPTIONS_ZOOM_X1, hasImage);
  enableSubMenuContainingId(this, ID_FUNCTION_ROTATE, hasImage && !gridDlgVisible);
}

void CMainFrame::scroll(int dx, int dy) {
  const CPoint topLeft   = getView()->GetScrollPosition();
  const CPoint maxScroll = getMaxScroll();
  const int    newX      = minMax(topLeft.x+dx,0,maxScroll.x);
  const int    newY      = minMax(topLeft.y+dy,0,maxScroll.y);
  getView()->ScrollToPosition(CPoint(newX,newY));
}

CPoint CMainFrame::getMaxScroll() {
  const CSize rectSize = getClientRect(getView()).Size();
  const CSize docSize  = getDocument()->getSize();
  const int   zoom     = getView()->getCurrentZoomFactor();
  return CPoint(max(0,docSize.cx*zoom-rectSize.cx),max(0,docSize.cy*zoom-rectSize.cy));
}

void CMainFrame::applyFilter(PixRectFilter &filter) {
  CPearlImageDoc *doc = getDocument();
  if(!doc->hasImage()) return;
  theApp.BeginWaitCursor();
  saveDocState();
  PixRect *pr = doc->getImage()->clone(true);
  doc->setImage(&(pr->apply(filter)));
  Invalidate();
  theApp.EndWaitCursor();
}

void CMainFrame::applyMirror(bool vertical) {
  theApp.BeginWaitCursor();
  saveDocState();
  CPearlImageDoc *doc = getDocument();
  doc->setImage(PixRect::mirror(doc->getImage(),vertical));
  Invalidate();
  theApp.EndWaitCursor();
}

void CMainFrame::createGridDlg() {
  if(m_gridDlg == NULL) {
    m_gridDlg = new CGridDlg();
    m_gridDlg->addPropertyChangeListener(this);
  }
  if(m_gridDlgThread == NULL) {
    m_gridDlgThread = CPropertyDlgThread::startThread(m_gridDlg);
  }
}

void CMainFrame::destroyGridDlg() {
  if (m_gridDlg) {
    m_gridDlg->removePropertyChangeListener(this);
  }
  if(m_gridDlgThread) {
    m_gridDlgThread->kill();
    m_gridDlgThread = NULL;
  }
}

bool CMainFrame::hasGridDlg() const {
  return m_gridDlgThread != NULL;
}
