#include "stdafx.h"
#include "PixRectArray.h"
#include "SettingsDlg.h"
#include "MainFrm.h"

#ifdef _DEBUG
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_SHOWWINDOW()
    ON_COMMAND(ID_FILE_OPEN                 , OnFileOpen                 )
    ON_COMMAND(ID_FILE_CLOSE_GIFFILE        , OnFileCloseGiffile         )
    ON_COMMAND(ID_FILE_MRU_FILE1            , OnFileMruFile1             )
    ON_COMMAND(ID_FILE_MRU_FILE2            , OnFileMruFile2             )
    ON_COMMAND(ID_FILE_MRU_FILE3            , OnFileMruFile3             )
    ON_COMMAND(ID_FILE_MRU_FILE4            , OnFileMruFile4             )
    ON_COMMAND(ID_FILE_MRU_FILE5            , OnFileMruFile5             )
    ON_COMMAND(ID_FILE_MRU_FILE6            , OnFileMruFile6             )
    ON_COMMAND(ID_FILE_MRU_FILE7            , OnFileMruFile7             )
    ON_COMMAND(ID_FILE_MRU_FILE8            , OnFileMruFile8             )
    ON_COMMAND(ID_FILE_MRU_FILE9            , OnFileMruFile9             )
    ON_COMMAND(ID_FILE_MRU_FILE10           , OnFileMruFile10            )
    ON_COMMAND(ID_FILE_GEM                  , OnFileSave                 )
    ON_COMMAND(ID_FILE_GEM_SOM              , OnFileSaveAs               )
    ON_COMMAND(ID_EDIT_INSERTIMAGES         , OnEditInserIimages         )
    ON_COMMAND(ID_EDIT_REMOVEALLIMAGES      , OnEditRemoveAllImages      )
    ON_COMMAND(ID_EDIT_ADDTOGIF             , OnEditAddToGif             )
    ON_COMMAND(ID_EDIT_SETTINSGS            , OnEditSettinsgs            )
    ON_COMMAND(ID_VIEW_PLAY                 , OnViewPlay                 )
    ON_COMMAND(ID_VIEW_STOP                 , OnViewStop                 )
    ON_COMMAND(ID_VIEW_SHOWALLIMAGES        , OnViewShowAllImages        )
    ON_COMMAND(ID_VIEW_SHOWRAWPICTURES      , OnViewShowRawPictures      )
    ON_COMMAND(ID_VIEW_SHOWSCALEDPICTURES   , OnViewShowScaledPictures   )
    ON_COMMAND(ID_VIEW_SHOWQUANTIZEDPICTURES, OnViewShowQuantizedPictures)
    ON_WM_CLOSE()
    ON_COMMAND(ID_APP_EXIT, OnAppExit)
END_MESSAGE_MAP()

static UINT indicators[] = {
  ID_SEPARATOR      
 ,ID_INDICATOR_CAPS
 ,ID_INDICATOR_NUM
};

CMainFrame::CMainFrame() {
  m_registeredAsListener = false;
}

CMainFrame::~CMainFrame() {
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(CFrameWnd::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }
  
  if(!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
      | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
  {
    TRACE0("Failed to create toolbar\n");
    return -1;      // fail to create
  }

  if(!m_wndStatusBar.Create(this) ||
     !m_wndStatusBar.SetIndicators(indicators,
      sizeof(indicators)/sizeof(UINT)))
  {
    TRACE0("Failed to create status bar\n");
    return -1;      // fail to create
  }

  m_bAutoMenuEnable = false;
  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
  EnableDocking(CBRS_ALIGN_ANY);
  DockControlBar(&m_wndToolBar);

  setShowFormat(SHOW_RAW);
  return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
  if(!CFrameWnd::PreCreateWindow(cs)) {
    return FALSE;
  }
  return TRUE;
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const {
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const {
  CFrameWnd::Dump(dc);
}

#endif //_DEBUG

// -------------------------------------- File Menu -------------------------------------

void CMainFrame::OnFileOpen() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrTitle      = _T("Load GIF");
  dlg.m_ofn.lpstrFilter     = _T("Gif files (*.gif)\0*.gif\0"
                                 "All files (*.*)\0*.*\0\0");
  if(dlg.DoModal() != IDOK) {
    return;
  }
  load(dlg.m_ofn.lpstrFile);
}

void CMainFrame::OnFileCloseGiffile() {
  if(!confirmDiscard()) return;
  getDoc()->closeGif();
}

void CMainFrame::OnAppExit() {
  OnClose();
}

void CMainFrame::OnClose() {
  if(!confirmDiscard()) return;
  CFrameWnd::OnClose();
}

bool CMainFrame::confirmDiscard() { // should return true if user does not want to save or save not needed
  if(!getDoc()->needSave()) {
    return true;
  }
  bool showAgain;
  return confirmDialogBox(_T("Gif file not saved. Do you want to continue"), _T("MakeGif"), showAgain, MB_YESNO | MB_ICONQUESTION) 
         == IDYES;
}

void CMainFrame::OnFileMruFile1()  { onFileMruFile(0);}
void CMainFrame::OnFileMruFile2()  { onFileMruFile(1);}
void CMainFrame::OnFileMruFile3()  { onFileMruFile(2);}
void CMainFrame::OnFileMruFile4()  { onFileMruFile(3);}
void CMainFrame::OnFileMruFile5()  { onFileMruFile(4);}
void CMainFrame::OnFileMruFile6()  { onFileMruFile(5);}
void CMainFrame::OnFileMruFile7()  { onFileMruFile(6);}
void CMainFrame::OnFileMruFile8()  { onFileMruFile(7);}
void CMainFrame::OnFileMruFile9()  { onFileMruFile(8);}
void CMainFrame::OnFileMruFile10() { onFileMruFile(9);}

void CMainFrame::onFileMruFile(int index) {
  if(!load(theApp.getRecentFile(index))) {
    theApp.removeFromRecentFile(index);
  }
}

bool CMainFrame::load(const String &name) {
  try {
    getDoc()->loadGif(name);
    showAllImages();
    theApp.AddToRecentFileList(name.cstr());
    return true;
  } catch(Exception e) {
    MessageBox(e.what(), _T("Error"), MB_ICONWARNING);
    return false;
  }
}

void CMainFrame::OnFileSave() {
  CMakeGifDoc *doc = getDoc();
  if(doc->hasDefaultName()) {
    OnFileSaveAs();
  } else {
    save(doc->getName());
  }
}

static const TCHAR *saveFileDialogExtensions = _T("Gif files (*.gif)\0*.gif;\0\0");

void CMainFrame::OnFileSaveAs() {
  CMakeGifDoc *doc = getDoc();
  String name = doc->getName();
  const TCHAR *tmpName = name.cstr();
  CFileDialog dlg(FALSE,_T("gif"), tmpName);
  dlg.m_ofn.lpstrTitle  = _T("Save gif");
  dlg.m_ofn.lpstrFilter = saveFileDialogExtensions;
  if((dlg.DoModal() == IDOK) && (dlg.m_ofn.lpstrFile != NULL)) {
    name = dlg.m_ofn.lpstrFile;
  } else {
    return;
  }
  save(name);
}

void CMainFrame::save(const String &name) { // returns true if succeeded
  try {
    FileNameSplitter info(name);
    const String fullName = info.setExtension(_T("gif")).getFullPath();
    getDoc()->saveGif(fullName);
    theApp.AddToRecentFileList(fullName.cstr());
  } catch(Exception e) {
    MessageBox(e.what());
  }
}

void CMainFrame::setTitle() {
  CMakeGifDoc *doc = getDoc();
  if(doc) {
    const String name      = doc->getName();
    int          fileSize  = doc->getFileSize();
    String       sizeStr   = (fileSize < 0) ? EMPTYSTRING : format(_T("FileSize:%s bytes"), format1000(fileSize).cstr());
    const CSize  imageSize = doc->getGifSize();
    const TCHAR *dirtyMark = doc->needSave() ? _T("*") : EMPTYSTRING;
    SetWindowText(format(_T("%s%s - %dx%d %s"), name.cstr(), dirtyMark, imageSize.cx,imageSize.cy, sizeStr.cstr()).cstr());
    ajourMenuItems();
  }
}

void CMainFrame::ajourMenuItems() {
  CMakeGifDoc *doc = getDoc();
  enableMenuItem(this, ID_FILE_CLOSE_GIFFILE   , doc->hasGifFile());
  enableMenuItem(this, ID_FILE_GEM             , doc->needSave());
  enableMenuItem(this, ID_FILE_GEM_SOM         , doc->hasGifFile());
  enableMenuItem(this, ID_EDIT_ADDTOGIF        , doc->getImageCount() > 0);
  enableMenuItem(this, ID_EDIT_REMOVEALLIMAGES , doc->getImageCount() > 0);
  enableMenuItem(this, ID_VIEW_PLAY            , doc->hasGifFile() && !getView()->isPlaying());
  enableMenuItem(this, ID_VIEW_STOP            , getView()->isPlaying());
  if(!doc->hasGifFile()) {
    enableMenuItem(this, ID_VIEW_SHOWALLIMAGES, false);
  } else {
    enableMenuItem(this, ID_VIEW_SHOWALLIMAGES, true);
    checkMenuItem( this, ID_VIEW_SHOWALLIMAGES, getView()->isShowingAll());
  }
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) {
  CFrameWnd::OnShowWindow(bShow, nStatus);
  setTitle();
  if(!m_registeredAsListener) {
    getDoc()->addPropertyChangeListener(this);
    getView()->addPropertyChangeListener(this);
    m_registeredAsListener = true;
  }
}

void CMainFrame::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(source == getDoc()) {
    switch(id) {
    case UPDATE_TIME:
      Invalidate();
      setTitle();
      break;
    case SAVE_TIME  :
      setTitle();
      break;
    case RAW_IMAGE_LIST      :
    case SCALED_IMAGE_LIST   :
    case QUANTIZED_IMAGE_LIST:
      ajourMenuItems();
      break;
    }
  } else if(source == getView()) {
    switch(id) {
    case GIFCTRL_STATUS:
      ajourMenuItems();
      break;
    }
  }
}

// -------------------------------------- Edit Menu -------------------------------------

static StringArray getFileNames(const TCHAR *fileNames) {
  StringArray result;
  const TCHAR *dir = fileNames;
  if(*dir == 0) {
    return result;
  }
  bool addCount = 0;
  for(const TCHAR *cp = dir + _tcslen(dir)+1; *cp; cp += _tcslen(cp) + 1) {
    result.add(FileNameSplitter::getChildName(dir, cp));
    addCount++;
  }
  if(addCount == 0) {
    result.add(dir);
  }
  return result;
}

void CMainFrame::OnEditInserIimages() {
  static const TCHAR *loadFileDialogExtensions = _T("Picture files\0*.bmp;*.dib;*.jpg;*.emf;*.gif;*.ico;*.wmf;\0"
                                                    "BMP-Files (*.bmp)\0*.bmp;\0"
                                                    "DIB-files (*.dib)\0*.dib;\0"
                                                    "JPG-files (*.jpg)\0*.jpg;\0"
                                                    "TIFF-files (*.tiff)\0*.tiff;\0"
                                                    "GIF-files (*.gif)\0*.gif;\0"
                                                    "ICO-files (*.ico)\0*.ico;\0"
                                                    "CUR-files (*.cur)\0*.cur;\0"
                                                    "All files (*.*)\0*.*\0\0");
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = loadFileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open file");
  dlg.m_ofn.Flags |=  OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLESIZING;
  TCHAR fileNameBuffer[4096];
  memset(fileNameBuffer,' ',sizeof(fileNameBuffer));
  fileNameBuffer[ARRAYSIZE(fileNameBuffer)-1] = 0;
  dlg.m_ofn.lpstrFile = fileNameBuffer;
  dlg.m_ofn.nMaxFile = ARRAYSIZE(fileNameBuffer);

  if((dlg.DoModal() != IDOK) || (_tcslen(fileNameBuffer) == 0) ) {
    return;
  }

  const StringArray fileNames = getFileNames(fileNameBuffer);
  StringArray errors;
  PixRectArray prArray(fileNames, errors);
  if(errors.size() > 0) {
    String msg;
    for(size_t i = 0; i < errors.size(); i++) {
      msg += errors[i] + _T("\r\n");
    }
    MessageBox(msg.cstr(), _T("Error"), MB_ICONWARNING);
  }
  if(prArray.size() > 0) {
    getDoc()->addPixRectArray(prArray);
    Invalidate();
  }
}

void CMainFrame::OnEditRemoveAllImages() {
  if(getDoc()->removeAllImages()) {
    Invalidate();
  }
}

void CMainFrame::OnEditAddToGif() {
  if(getView()->isPlaying()) {
    OnViewStop();
  }
  if(getDoc()->addImagesToGif()) {
    showAllImages();
  }
}

void CMainFrame::OnEditSettinsgs() {
  CSettingsDlg dlg(getDoc()->getImageSettings());
  if(dlg.DoModal() == IDOK) {
    getDoc()->setImageSettings(dlg.getImageSettings());
    Invalidate();
  }
}

// -------------------------------------- View Menu -------------------------------------

void CMainFrame::OnViewPlay() {
  getView()->startPlay();
}

void CMainFrame::OnViewStop() {
  getView()->stopPlay();
}

void CMainFrame::OnViewShowAllImages() {
  if(!isMenuItemChecked(this, ID_VIEW_SHOWALLIMAGES)) {
    showAllImages();
  }
}

void CMainFrame::showAllImages() {
  OnViewStop();
  getView()->showAll();
}

void CMainFrame::OnViewShowRawPictures() {        setShowFormat(SHOW_RAW      ); }
void CMainFrame::OnViewShowScaledPictures() {     setShowFormat(SHOW_SCALED   ); }
void CMainFrame::OnViewShowQuantizedPictures() {  setShowFormat(SHOW_QUANTIZED); }

void CMainFrame::setShowFormat(ShowFormat format) {
  checkMenuItem(this, ID_VIEW_SHOWRAWPICTURES,       format == SHOW_RAW      );
  checkMenuItem(this, ID_VIEW_SHOWSCALEDPICTURES,    format == SHOW_SCALED   );
  checkMenuItem(this, ID_VIEW_SHOWQUANTIZEDPICTURES, format == SHOW_QUANTIZED);
  m_currentShowFormat = format;
  Invalidate();
}
