#include "stdafx.h"
#include <io.h>
#include <process.h>
#include <MFCUtil/Clipboard.h>
#include "CaptureView.h"
#include "CaptureAreaDialog.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
    ON_WM_CREATE()
//    ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
//    ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_SIZE()
    ON_COMMAND(ID_FILEPRINT                      , OnFilePrint                )
    ON_COMMAND(ID_FILE_PRINT_PREVIEW             , OnFilePrintPreview         )
    ON_COMMAND(ID_FILESAVE                       , OnFileSave                 )
    ON_COMMAND(ID_EDITCOPY                       , OnEditCopy                 )
    ON_COMMAND(ID_MSPAINT                        , OnStartMSPaint             )
    ON_COMMAND(ID_APP_CAPTURE_AREA               , OnAppCaptureArea           )
    ON_COMMAND(ID_APP_CAPTURE_WINDOW             , OnAppCaptureWindowDummy    )
    ON_COMMAND(ID_APP_CAPTURE_SCREEN             , OnAppCaptureScreen         )
    ON_COMMAND(ID_SCROLL_LINEDOWN                , OnScrollLineDown           )
    ON_COMMAND(ID_SCROLL_LINEUP                  , OnScrollLineUp             )
    ON_COMMAND(ID_SCROLL_LINERIGHT               , OnScrollLineRight          )
    ON_COMMAND(ID_SCROLL_LINELEFT                , OnScrollLineLeft           )
    ON_COMMAND(ID_SCROLL_PAGEDOWN                , OnScrollPageDown           )
    ON_COMMAND(ID_SCROLL_PAGEUP                  , OnScrollPageUp             )
    ON_COMMAND(ID_SCROLL_PAGERIGHT               , OnScrollPageRight          )
    ON_COMMAND(ID_SCROLL_PAGELEFT                , OnScrollPageLeft           )
    ON_COMMAND(ID_SCROLL_TOBOTTOM                , OnScrollToBottom           )
    ON_COMMAND(ID_SCROLL_TOTOP                   , OnScrollToTop              )
    ON_COMMAND(ID_SCROLL_TORIGHT                 , OnScrollToRight            )
    ON_COMMAND(ID_SCROLL_TOLEFT                  , OnScrollToLeft             )
    ON_COMMAND(ID_SCROLL_TOEND                   , OnScrollToEnd              )
    ON_COMMAND(ID_SCROLL_TOHOME                  , OnScrollToHome             )
    ON_COMMAND(ID_VIEW_SIZE_PIXELS               , OnViewSizePixels           )
    ON_COMMAND(ID_VIEW_SIZE_CENTIMETERS          , OnViewSizeCentimeters      )
    ON_UPDATE_COMMAND_UI(ID_FILEPRINT            , OnUpdateFilePrint          )
    ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW   , OnUpdateFilePrintPreview   )
    ON_UPDATE_COMMAND_UI(ID_FILESAVE             , OnUpdateFileSave           )
    ON_UPDATE_COMMAND_UI(ID_EDITCOPY             , OnUpdateEditCopy           )
    ON_UPDATE_COMMAND_UI(ID_MSPAINT              , OnUpdateStartMSPaint       )
    ON_UPDATE_COMMAND_UI(ID_VIEW_SIZE_PIXELS     , OnUpdateViewSizePixels     )
    ON_UPDATE_COMMAND_UI(ID_VIEW_SIZE_PIXELS     , OnUpdateViewSizePixels     )
    ON_UPDATE_COMMAND_UI(ID_VIEW_SIZE_PIXELS     , OnUpdateViewSizePixels     )
    ON_UPDATE_COMMAND_UI(ID_VIEW_SIZE_CENTIMETERS, OnUpdateViewSizeCentimeters)
END_MESSAGE_MAP()

static UINT indicators[] = {
  ID_SEPARATOR           // status line indicator
 ,ID_INDICATOR_SIZE
};

#define PENSIZE 4

CMainFrame::CMainFrame()
{
  m_blackPen = ::CreatePen(PS_SOLID, PENSIZE, RGB(0,0,0));
  m_docSizeFormat = DOCSIZE_IN_PIXELS;
  setCaptureAllEvents(false);
  initCurrent();
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if(__super::OnCreate(lpCreateStruct) == -1)
    return -1;

  BOOL bNameValid;

  if (!m_wndMenuBar.Create(this))
  {
      TRACE0("Failed to create menubar\n");
      return -1;      // fail to create
  }

  m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

  // prevent the menu bar from taking the focus on activation
  CMFCPopupMenu::SetForceMenuFocus(FALSE);

  if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
  {
      TRACE0("Failed to create toolbar\n");
      return -1;      // fail to create
  }

  CString strToolBarName;
  bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
  ASSERT(bNameValid);
  m_wndToolBar.SetWindowText(strToolBarName);

/*
  CString strCustomize;
  bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
  ASSERT(bNameValid);
  m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

  // Allow user-defined toolbars operations:
  InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);
*/

  if (!m_wndStatusBar.Create(this))
  {
      TRACE0("Failed to create status bar\n");
      return -1;      // fail to create
  }
  m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

  // TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
  m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
  EnableDocking(CBRS_ALIGN_ANY);
  DockPane(&m_wndMenuBar);
  DockPane(&m_wndToolBar);


  // enable Visual Studio 2005 style docking window behavior
  CDockingManager::SetDockingMode(DT_SMART);
  // enable Visual Studio 2005 style docking window auto-hide behavior
  EnableAutoHidePanes(CBRS_ALIGN_ANY);

  // set the visual manager used to draw all user interface elements
  CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
/*
  // Enable toolbar and docking window menu replacement
  EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

  // enable quick (Alt+drag) toolbar customization
  CMFCToolBar::EnableQuickCustomization();

  if (CMFCToolBar::GetUserImages() == NULL)
  {
      // load user-defined toolbar images
      if (m_UserImages.Load(_T(".\\UserImages.bmp")))
      {
          CMFCToolBar::SetUserImages(&m_UserImages);
      }
  }

  // enable menu personalization (most-recently used commands)
  // TODO: define your own basic commands, ensuring that each pulldown menu has at least one basic command.
  CList<UINT, UINT> lstBasicCommands;

  lstBasicCommands.AddTail(ID_FILE_NEW);
  lstBasicCommands.AddTail(ID_FILE_OPEN);
  lstBasicCommands.AddTail(ID_FILE_SAVE);
  lstBasicCommands.AddTail(ID_FILE_PRINT);
  lstBasicCommands.AddTail(ID_APP_EXIT);
  lstBasicCommands.AddTail(ID_EDIT_CUT);
  lstBasicCommands.AddTail(ID_EDIT_PASTE);
  lstBasicCommands.AddTail(ID_EDIT_UNDO);
  lstBasicCommands.AddTail(ID_APP_ABOUT);
  lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
  lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);

  CMFCToolBar::SetBasicCommands(lstBasicCommands);
*/

  m_bAutoMenuEnable = false;
  return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if(!__super::PreCreateWindow(cs) )
        return FALSE;
    return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
  __super::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
  __super::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers
#ifdef __NEVER__
void CMainFrame::OnViewCustomize()
{
    CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
    pDlgCust->EnableUserDefinedToolbars();
    pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
    LRESULT lres = __super::OnToolbarCreateNew(wp,lp);
    if (lres == 0)
    {
        return 0;
    }

    CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
    ASSERT_VALID(pUserToolbar);

    BOOL bNameValid;
    CString strCustomize;
    bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
    ASSERT(bNameValid);

    pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
    return lres;
}
#endif

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd *pParentWnd, CCreateContext *pContext) {
    if (!__super::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext)) {
        return FALSE;
    }


    // enable customization button for all user toolbars
    BOOL bNameValid;
    CString strCustomize;
    bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
    ASSERT(bNameValid);

    for (int i = 0; i < iMaxUserToolbars; i ++) {
        CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
        if (pUserToolbar != NULL) {
            pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
        }
    }

    return TRUE;
}

void CMainFrame::OnFilePrint() {
  if(!hasImage()) {
    MessageBox(_T("No image to print"), _T("Error"), MB_ICONINFORMATION);
    return;
  }
  getView()->OnFilePrint();
}

void CMainFrame::OnFilePrintPreview() {
  if(!hasImage()) {
    MessageBox(_T("No image to show"), _T("Error"), MB_ICONINFORMATION);
    return;
  }
  getView()->OnFilePrintPreview();
}

void CMainFrame::OnFileSave() {
  if(!hasImage()) {
    MessageBox(_T("No image to save"), _T("Error"), MB_ICONINFORMATION);
    return;
  }

  static const TCHAR *FileDialogExtensions = _T("Bitmap files (*.bmp)\0*.bmp;\0"
                                                "JPEG files (*.jpg)\0*.jpg;\0"
                                                "TIFF files (*.tiff)\0*.tiff;\0"
                                                "PNG files (*.png)\0*.png;\0\0");


  CFileDialog dlg(FALSE);
  dlg.m_ofn.lpstrFilter = FileDialogExtensions;
  dlg.m_ofn.lpstrDefExt = _T(".jpg");
  dlg.m_ofn.lpstrTitle  = _T("Save picture");

  if(dlg.DoModal() != IDOK || _tcslen(dlg.m_ofn.lpstrFile) == 0) {
    return;
  }

  try {
    CCaptureDoc *doc = GetDocument();
    const TCHAR *ext = dlg.m_ofn.lpstrFilter;
    doc->save(dlg.m_ofn.lpstrFile);
  } catch(Exception e) {
    MessageBox(e.what());
  }
}

void CMainFrame::OnEditCopy() {
  if(!hasImage()) {
    MessageBox(_T("No image to copy"), _T("Error"), MB_ICONINFORMATION);
    return;
  }

  HBITMAP bitmap = GetDocument()->getBitmap();
  try {
    putClipboard(theApp.m_pMainWnd->m_hWnd,bitmap);
  } catch(Exception e) {
    MessageBox(format(_T("putClipboard failed:%s"), e.what()).cstr(), _T("Error"), MB_ICONWARNING);
  }
}

void CMainFrame::OnScrollLineDown() {
  CCaptureView *view = getView();
  CPoint p = view->GetScrollPosition();
  p.y += 30;
  view->ScrollToPosition(p);
}

void CMainFrame::OnScrollLineUp() {
  CCaptureView *view = getView();
  CPoint p = view->GetScrollPosition();
  p.y -= 30;
  view->ScrollToPosition(p);
}

void CMainFrame::OnScrollLineRight() {
  CCaptureView *view = getView();
  CPoint p = view->GetScrollPosition();
  p.x += 30;
  view->ScrollToPosition(p);
}

void CMainFrame::OnScrollLineLeft() {
  CCaptureView *view = getView();
  CPoint p = view->GetScrollPosition();
  p.x -= 30;
  view->ScrollToPosition(p);
}

void CMainFrame::OnScrollPageDown() {
  CCaptureView *view = getView();
  CRect clientRect;
  view->GetClientRect(clientRect);
  CPoint p = view->GetScrollPosition();
  p.y += clientRect.Height();
  view->ScrollToPosition(p);
}

void CMainFrame::OnScrollPageUp() {
  CCaptureView *view = getView();
  CRect clientRect;
  view->GetClientRect(clientRect);
  CPoint p = view->GetScrollPosition();
  p.y -= clientRect.Height();
  view->ScrollToPosition(p);
}

void CMainFrame::OnScrollPageRight() {
  CCaptureView *view = getView();
  CRect clientRect;
  view->GetClientRect(clientRect);
  CPoint p = view->GetScrollPosition();
  p.x += clientRect.Width();
  view->ScrollToPosition(p);
}

void CMainFrame::OnScrollPageLeft() {
  CCaptureView *view = getView();
  CRect clientRect;
  view->GetClientRect(clientRect);
  CPoint p = view->GetScrollPosition();
  p.x -= clientRect.Width();
  view->ScrollToPosition(p);
}

void CMainFrame::OnScrollToBottom() {
  CCaptureView *view = getView();
  CRect clientRect;
  view->GetClientRect(clientRect);
  CPoint p = view->GetScrollPosition();
  p.y = max(0,GetDocument()->getSize().cy - clientRect.Height());
  view->ScrollToPosition(p);
}

void CMainFrame::OnScrollToTop() {
  CCaptureView *view = getView();
  CPoint p = view->GetScrollPosition();
  p.y = 0;
  view->ScrollToPosition(p);
}

void CMainFrame::OnScrollToRight() {
  CCaptureView *view = getView();
  CRect clientRect;
  view->GetClientRect(clientRect);
  CPoint p = view->GetScrollPosition();
  p.x = max(0,GetDocument()->getSize().cx - clientRect.Width());
  view->ScrollToPosition(p);
}

void CMainFrame::OnScrollToLeft() {
  CCaptureView *view = getView();
  CPoint p = view->GetScrollPosition();
  p.x = 0;
  view->ScrollToPosition(p);
}

void CMainFrame::OnScrollToEnd() {
  CCaptureView *view = getView();
  CRect clientRect;
  view->GetClientRect(clientRect);
  CSize docSize = GetDocument()->getSize();
  CPoint p = view->GetScrollPosition();
  p.x = max(0,docSize.cx - clientRect.Width());
  p.y = max(0,docSize.cy - clientRect.Height());
  view->ScrollToPosition(p);
}

void CMainFrame::OnScrollToHome() {
  getView()->ScrollToPosition(CPoint(0,0));
}

void CMainFrame::repaint() {
  getView()->repaint();
  showDocSize();
}

void CMainFrame::showDocSize() {
  switch(m_docSizeFormat) {
  case DOCSIZE_IN_CENTIMETERS:
    { const CSize size = GetDocument()->getSizeInMillimeters();
      m_wndStatusBar.SetPaneText(1, format(_T("%.1lf x %.1lf centimeters"), (double)size.cx/10, (double)size.cy/10).cstr());
    }
    break;
  case DOCSIZE_IN_PIXELS     :
    { const CSize size = GetDocument()->getSize();
      m_wndStatusBar.SetPaneText(1, format(_T("%d x %d pixels"), size.cx, size.cy).cstr());
    }
    break;
  }
}

String getTempFileName(const String &fileName) {
  return FileNameSplitter::getChildName(_T("c:\\temp"), fileName);
}

String createTempFileName(const String &ext) {
  String fileName = getTempFileName(_T("cXXXXXX"));
  _tmktemp(fileName.cstr());
  return FileNameSplitter(fileName).setExtension(ext).getAbsolutePath();
}

void CMainFrame::OnStartMSPaint() {
  USES_CONVERSION;
  try {
    CCaptureDoc *doc = GetDocument();
    if(!doc->hasImage()) {
      MessageBox(_T("No image"), _T("Error"), MB_ICONINFORMATION);
      return;
    }
    const String tempName = createTempFileName(_T("bmp"));
    const char *tempNameA = T2A(tempName.cstr());
    doc->save(tempName);
    const TCHAR *msPaint = _T("c:\\windows\\system32\\mspaint.exe");
    const char *msPaintA = T2A(msPaint);
    if(_spawnlp(_P_NOWAITO, msPaintA, msPaintA, tempNameA, NULL) == -1) {
      throwErrNoOnSysCallException(_T("_spawnlp(\"mapaint.exe\")"));
    }
  } catch(Exception e) {
    MessageBox(e.what());
  }
}

void CMainFrame::OnAppCaptureArea() {
  moveWindowOutsideScreen();
//  ShowWindow(SW_HIDE);
  CCaptureAreaDialog dlg;
  if(dlg.DoModal() == IDOK) {
    GetDocument()->setImage(captureScreen(dlg.getCapturedRect()));
  }
  restoreWindowPlacement();
//  ShowWindow(SW_SHOW);
  repaint();
}

void CMainFrame::OnAppCaptureScreen() {
  moveWindowOutsideScreen();
  GetDocument()->setImage(captureScreen(0,0,getScreenSize()));
  restoreWindowPlacement();
  repaint();
}

void CMainFrame::OnAppCaptureWindowDummy() {
}

void CMainFrame::OnAppCaptureWindow() {
  setCaptureAllEvents(true);
  moveWindowOutsideScreen();
}

void CMainFrame::setCaptureAllEvents(bool value) {
  m_capturingWindow = value;
  if(m_capturingWindow) {
    SetCapture();
  } else {
    ReleaseCapture();
  }
}

void CMainFrame::OnMouseMove(UINT nFlags, CPoint point) {
  if(isCapturingWindow()) {
    HWND window = findWindow(point);
    if(window == getSelectedWindow()) {
      return;
    }

    try {
      if(hasSelectedWindow()) {
        restoreSelectedWindow();
      }
      if(window != NULL) {
        selectWindow(window);
      }
    } catch(Exception e) {
      MessageBox(e.what(), _T("Exception"), MB_ICONERROR);
    }
  }
  __super::OnMouseMove(nFlags, point);
}

void CMainFrame::OnLButtonUp(UINT nFlags, CPoint point) {
  try {
    if(isCapturingWindow()) {
      if(hasSelectedWindow()) {
        HWND hwnd = getSelectedWindow();
        restoreSelectedWindow();
        GetDocument()->setImage(captureWindow(hwnd));
      }
      restoreWindowPlacement();
      setCaptureAllEvents(false);
      repaint();
    }
    __super::OnLButtonUp(nFlags, point);
  } catch(Exception e) {
    MessageBox(format(_T("Exception:%s"), e.what()).cstr(), _T("Error"), MB_ICONWARNING);
  }
}

void CMainFrame::moveWindowOutsideScreen() {
  saveWindowPlacement();
  const CSize          screenSize = getScreenSize();
  WINDOWPLACEMENT      tmp        = m_savedWindowPlacement;
  tmp.rcNormalPosition.left       = tmp.rcNormalPosition.right  = screenSize.cx;
  tmp.rcNormalPosition.top        = tmp.rcNormalPosition.bottom = screenSize.cy;
  SetWindowPlacement(&tmp);
}

void CMainFrame::saveWindowPlacement() {
  GetWindowPlacement(&m_savedWindowPlacement);
}

void CMainFrame::restoreWindowPlacement() {
  SetWindowPlacement(&m_savedWindowPlacement);
}

void CMainFrame::initCurrent() {
  m_savedWindowDC  = NULL;
  m_savedBitmap    = NULL;
  m_selectedWindow = NULL;
}

void CMainFrame::releaseCurrent() {
  if(m_savedWindowDC != NULL) {
    DeleteDC(m_savedWindowDC);
    m_savedWindowDC = NULL;
  }

  if(m_savedBitmap != NULL) {
    DeleteObject(m_savedBitmap);
    m_savedBitmap   = NULL;
  }
  m_selectedWindow = NULL;
}

void CMainFrame::restoreSelectedWindow() {
  HDC hdc = ::GetWindowDC(m_selectedWindow);
  if(hdc == NULL) {
    throwException(_T("GetWindowDC failed:%s"), getLastErrorText().cstr());
  }
  CRect rect = getWindowRect(m_selectedWindow);

  ::BitBlt(hdc,0                   , 0                    ,rect.Width()        ,PENSIZE               , m_savedWindowDC
              ,0                   , 0                                                                , SRCCOPY);
  ::BitBlt(hdc,rect.Width()-PENSIZE, PENSIZE              ,PENSIZE             ,rect.Height()         , m_savedWindowDC
              ,rect.Width()-PENSIZE, PENSIZE                                                          , SRCCOPY);
  ::BitBlt(hdc,0                   , rect.Height()-PENSIZE,rect.Width()-PENSIZE,PENSIZE               , m_savedWindowDC
              ,0                   , rect.Height()-PENSIZE                                            , SRCCOPY);
  ::BitBlt(hdc,0                   , PENSIZE              ,PENSIZE             ,rect.Height()-PENSIZE , m_savedWindowDC
              ,0                   , PENSIZE                                                          , SRCCOPY);

  ::ReleaseDC(m_selectedWindow,hdc);
  releaseCurrent();
}

void CMainFrame::selectWindow(HWND hwnd) {
  HDC hdc = ::GetWindowDC(hwnd);
  if(hdc == NULL) {
    throwException(_T("GetWindowDC failed:%s"),getLastErrorText().cstr());
  }
  CRect rect = getWindowRect(hwnd);

  m_savedWindowDC = CreateCompatibleDC(hdc);
  m_savedBitmap   = CreateCompatibleBitmap(hdc, rect.Width(), rect.Height()); // Use hdc, not m_savedWindowDC. which gives af monochrom bitmap

  SelectObject(m_savedWindowDC, m_savedBitmap);
  ::BitBlt(m_savedWindowDC,0,0,rect.Width(),rect.Height(),hdc,0,0,SRCCOPY);

  ::ReleaseDC(hwnd, hdc);
  m_selectedWindow = hwnd;
  drawRectangle(hwnd);
}

void CMainFrame::drawRectangle(HWND hwnd) {
  HDC   hdc  = ::GetWindowDC(hwnd);
  CRect rect =   getWindowRect(hwnd);
  const int w        = rect.Width();
  const int h        = rect.Height();
  const int fromEdge = PENSIZE/2;
  HGDIOBJ   oldPen   = ::SelectObject(hdc, m_blackPen);

  MoveToEx(hdc, fromEdge, fromEdge, NULL);
  LineTo(hdc, w-fromEdge, fromEdge  );
  LineTo(hdc, w-fromEdge, h-fromEdge);
  LineTo(hdc, fromEdge  , h-fromEdge);
  LineTo(hdc, fromEdge  , fromEdge  );
  ::SelectObject(hdc, oldPen);
  ::ReleaseDC(hwnd, hdc);
}

HWND CMainFrame::findWindow(const CPoint &point) {
  CPoint p = point;
  ClientToScreen(&p);
  HWND parent = ::WindowFromPoint(p);
  if(parent == NULL) {
    return NULL;
  }
  ::ScreenToClient(parent, &p);
  HWND child = ::ChildWindowFromPointEx(parent, p, CWP_ALL);
  return child != NULL ? child : parent;
}

BOOL CMainFrame::PreTranslateMessage(MSG *pMsg) {
  try {
    if(!isCapturingWindow()) {
      switch(pMsg->message) {
      case WM_LBUTTONDOWN:
        { const CPoint &p = pMsg->pt;
          for(int i = 0; i < m_wndToolBar.GetCount(); i++) {
            CRect r;
            m_wndToolBar.GetItemRect(i,&r);
            m_wndToolBar.ClientToScreen(&r);
            if(r.PtInRect(p)) {
              const int id = m_wndToolBar.GetItemID(i);
              if(id == ID_APP_CAPTURE_WINDOW) {
                OnAppCaptureWindow();
  //              __super::PreTranslateMessage(pMsg);
                return true;
              }
            }
          }
        }
        break;
      case WM_MOUSEWHEEL:
        { const INT_PTR zDelta = pMsg->wParam >> 16;
          if(pMsg->wParam & MK_SHIFT) {
            if(zDelta > 0) {
              OnScrollLineLeft();
            } else {
              OnScrollLineRight();
            }
            return TRUE;
          }
        }
        break;
      }
    }
    return __super::PreTranslateMessage(pMsg);
  } catch(Exception e) {
    MessageBox(format(_T("Exception:%s"), e.what()).cstr(), _T("Error"), MB_ICONWARNING);
    return TRUE;
  }
}

void CMainFrame::OnViewSizePixels() {
  m_docSizeFormat = DOCSIZE_IN_PIXELS;
  repaint();
}

void CMainFrame::OnViewSizeCentimeters() {
  m_docSizeFormat = DOCSIZE_IN_CENTIMETERS;
  repaint();
}

void CMainFrame::OnUpdateFilePrint(CCmdUI *pCmdUI) {
  pCmdUI->Enable(hasImage());
}
void CMainFrame::OnUpdateFilePrintPreview(CCmdUI *pCmdUI) {
  pCmdUI->Enable(hasImage());
}
void CMainFrame::OnUpdateFileSave(CCmdUI *pCmdUI) {
  pCmdUI->Enable(hasImage());
}
void CMainFrame::OnUpdateEditCopy(CCmdUI *pCmdUI) {
  pCmdUI->Enable(hasImage());
}
void CMainFrame::OnUpdateStartMSPaint(CCmdUI *pCmdUI) {
  pCmdUI->Enable(hasImage());
}
void CMainFrame::OnUpdateViewSizePixels(CCmdUI *pCmdUI) {
  pCmdUI->SetCheck(m_docSizeFormat == DOCSIZE_IN_PIXELS);
}
void CMainFrame::OnUpdateViewSizeCentimeters(CCmdUI *pCmdUI) {
  pCmdUI->SetCheck(m_docSizeFormat == DOCSIZE_IN_CENTIMETERS);
}
