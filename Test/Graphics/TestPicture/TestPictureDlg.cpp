#include "stdafx.h"
#include <MFCUtil/ColorSpace.h>
#include <ByteFile.h>
#include <ProcessTools.h>
#include "TestPicture.h"
#include "TestPictureDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();
  enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CTestPictureDlg::CTestPictureDlg(CWnd *pParent /*=NULL*/) : CDialog(CTestPictureDlg::IDD, pParent) {
  m_hIcon      = theApp.LoadIcon(IDR_MAINFRAME);
  m_workPr     = NULL;
  m_lastMouse  = CPoint(0,0);
}

void CTestPictureDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestPictureDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZE()
    ON_WM_CLOSE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
    ON_COMMAND(ID_FILE_LOAD           , OnFileLoad           )
    ON_COMMAND(ID_FILE_UNLOAD         , OnFileUnload         )
    ON_COMMAND(ID_FILE_SAVE_BMP       , OnFileSaveBmp        )
    ON_COMMAND(ID_FILE_SAVE_JPG       , OnFileSaveJpg        )
    ON_COMMAND(ID_FILE_SAVE_PNG       , OnFileSavePng        )
    ON_COMMAND(ID_FILE_SAVE_TIFF      , OnFileSaveTiff       )
    ON_COMMAND(ID_FILE_EXIT           , OnFileExit           )
    ON_COMMAND(ID_EDIT_CURRENT_1      , OnEditCurrent1       )
    ON_COMMAND(ID_EDIT_CURRENT_2      , OnEditCurrent2       )
    ON_COMMAND(ID_EDIT_PIC1PIC2       , OnEditPic1pic2       )
    ON_COMMAND(ID_EDIT_PIC2PIC1       , OnEditPic2pic1       )
    ON_COMMAND(ID_EDIT_SWAPREDANDBLUE , OnEditSwapRedAndBlue )
    ON_COMMAND(ID_EDIT_CLEARLOG       , OnEditClearLog       )
    ON_COMMAND(ID_VIEW_KEEPASPECTRATIO, OnViewKeepAspectRatio)
    ON_COMMAND(ID_VIEW_SHOWDEBUGINFO  , OnViewShowDebugInfo  )
    ON_WM_DROPFILES()
END_MESSAGE_MAP()

BOOL CTestPictureDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);
  
  theApp.m_device.attach(*this);
  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATICIMAGEFRAME, RELATIVE_SIZE);
  m_layoutManager.addControl(IDC_EDITINFO        , RELATIVE_Y_POS | RELATIVE_WIDTH);

  m_infoWindowHeight = getWindowSize(getInfoWin()).cy;
  DragAcceptFiles(true);

  CFont *f = GetFont();
  LOGFONT lf;
  f->GetLogFont(&lf);
  _tcscpy(lf.lfFaceName, _T("courier new"));
  BOOL ret = m_infoFont.CreateFontIndirect(&lf);
  getInfoWin()->SetFont(&m_infoFont);

  m_origTitle      = getWindowText(this);
  setCurrentImageIndex(0);
  m_extensionIndex = 0;
  m_isDragging     = false;

  TCHAR **argv = __targv;
  argv++;
  if(*argv) {
    try {
      getCurrentImage().load(*argv);
    } catch(Exception e) {
      MessageBox(e.what(), _T("Error"), MB_ICONWARNING);
    }
  }
  return TRUE;
}

void CTestPictureDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  } else {
    CDialog::OnSysCommand(nID, lParam);
  }
}

BOOL CTestPictureDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

void CTestPictureDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
  Invalidate(FALSE);
}

void CTestPictureDlg::OnPaint() {
  if(IsIconic()) {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    const int   cxIcon = GetSystemMetrics(SM_CXICON);
    const int   cyIcon = GetSystemMetrics(SM_CYICON);
    const CRect rect   = getClientRect(this);
    const int   x      = (rect.Width()  - cxIcon + 1) / 2;
    const int   y      = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    CDialog::OnPaint();
    Image   &image    = getCurrentImage();
    HDC      wdc      = getWorkDC();
    CWnd    *imageWin = getImageWin();
    CRect    clRect   = getClientRect(imageWin);
    if(image.isLoaded()) {
      image.show(wdc, clRect, isMenuItemChecked(this, ID_VIEW_KEEPASPECTRATIO));
    }
    BitBlt(CClientDC(imageWin),0,0,clRect.Width(), clRect.Height(), wdc, 0,0,SRCCOPY);
    releaseWorkDC(wdc);
    ajourMenuItems();
    showInfo();
    showTitle();
  }
}

void CTestPictureDlg::allocateWorkPr(const CSize &sz) {
  deallocateWorkPr();
  m_workPr = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, sz, D3DPOOL_DEFAULT, D3DFMT_A8R8G8B8);
}

void CTestPictureDlg::deallocateWorkPr() {
  if(m_workPr) {
    delete m_workPr;
    m_workPr = NULL;
  }
}

HDC CTestPictureDlg::getWorkDC() {
  const CSize sz = getClientRect(getImageWin()).Size();
  if((m_workPr == NULL) || (m_workPr->getSize() != sz)) {
    allocateWorkPr(sz);
  }
  D3DCOLOR backColor = ::GetSysColor(COLOR_BTNFACE);
  m_workPr->fillColor(backColor);
  return m_workPr->getDC();
}

void CTestPictureDlg::releaseWorkDC(HDC dc) {
  m_workPr->releaseDC(dc);
}

void CTestPictureDlg::ajourMenuItems() {
  const bool loaded = getCurrentImage().isLoaded();
  enableSubMenuContainingId(this, ID_FILE_SAVE_BMP, loaded);
}

String toString(const CRect &r) {
  return format(_T("(%3d,%3d,%3d,%3d)"), r.left,r.top,r.Width(),r.Height());
}

String toString(const CSize &s) {
  return format(_T("(%3d,%3d)"), s.cx, s.cy);
}

String toString(const CPoint &p) {
  return format(_T("(%3d,%3d)"), p.x, p.y);
}

void CTestPictureDlg::showInfo() {
  const PROCESS_MEMORY_COUNTERS memCounters = getProcessMemoryUsage();
  const ResourceCounters        resCounters = getProcessResources();
  const Image                  &image       = getCurrentImage();
  String imageInfo;
  if(image.isLoaded()) {
    const CPoint offset     = image.getOffset();
    const CPoint maxOffset  = image.getMaxOffset();
    const CSize  size       = image.getSize();
    const CSize  zoomedSize = image.getZoomedSize();
    const CRect  visR       = image.getVisiblePart();
    const CRect  cliR       = getClientRect(getImageWin());
    const CRect  imgR       = image.getImageRect();
    const CPoint imgP       = image.getImagePoint(m_lastMouse);

    imageInfo = format(_T(": OFFSET:%s. MAX.OFF:%s\r\n"
                       "ZOOM:%.3lf, MINZOOM:%.3lf\r\n"
                       "SIZE :%s. ZSIZE:%s, ASR:%.4lf hasAlpha:%s\r\n"
                       "MOUSE:%s -> %s\r\n"
                       "VIS.R:%s  ASR:%.4lf\r\n"
                       "CLI.R:%s, ASR:%.4lf\r\n"
                       "IMG.R:%s, ASR:%.4lf\r\n"
                       "MARG :(%3d,%3d,%3d,%3d)\r\n"
                       "Filesize:%s bytes. ")
                      ,toString(offset).cstr(), toString(maxOffset).cstr()
                      ,image.getZoomFactor(), image.getMinZoomFactor()
                      ,toString(size).cstr(), toString(zoomedSize).cstr(), getAspectRatio(zoomedSize), boolToStr(image.hasAlpha())
                      ,toString(m_lastMouse).cstr(), toString(imgP).cstr()
                      ,toString(visR).cstr(), getAspectRatio(visR)
                      ,toString(cliR).cstr(), getAspectRatio(cliR)
                      ,toString(imgR).cstr(), getAspectRatio(imgR)
                      ,imgR.left-cliR.left, imgR.top-cliR.top, cliR.right-imgR.right, cliR.bottom-imgR.bottom
                      ,format1000(image.getWeight()).cstr()
                      );
  } else {
    imageInfo = _T("\r\n");
  }
 
  const String msg = format(_T("Current image:%d%sMem.Usage:%s\r\nGDI-objects:%d, User-objects:%d")
//                             "Current image:%d%sMem.Usage:%s"
                           ,getCurrentImageIndex() + 1
                           ,imageInfo.cstr()
                           ,format1000(memCounters.WorkingSetSize).cstr()
                           ,resCounters.m_gdiObjectCount
                           ,resCounters.m_userObjectCount
                           );

  setWindowText(getInfoWin(), msg);
}

void CTestPictureDlg::showTitle() {
  const FileNameSplitter info(getCurrentImage().getFileName());
  const String name = info.getFileName() + info.getExtension();
  if(name.length() != 0) {
    const String title = m_origTitle + _T(" - ") + name;
    setWindowText(this, title);
  } else {
    setWindowText(this, m_origTitle);
  }
}

HCURSOR CTestPictureDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CTestPictureDlg::OnCancel() {
}

void CTestPictureDlg::OnOK() {
}

void CTestPictureDlg::OnClose() {
  OnFileExit();
}

void CTestPictureDlg::OnFileExit() {
  EndDialog(IDOK);
}

void CTestPictureDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  CDialog::OnLButtonDown(nFlags, point);
  Image &image = getCurrentImage();
  point = getImagePoint(point);
  if(image.getImageRect().PtInRect(point)) {
    m_mouseDown       = m_lastMouse = point;
    m_mouseDownOffset = image.getOffset();
    setDragging(true);
    showInfo();
  }
}

void CTestPictureDlg::OnLButtonUp(UINT nFlags, CPoint point) {
  CDialog::OnLButtonUp(nFlags, point);
  setDragging(false);
}

void CTestPictureDlg::OnMouseMove(UINT nFlags, CPoint point) {
  CDialog::OnMouseMove(nFlags, point);
  if((nFlags & MK_LBUTTON) == 0) {
    setDragging(false);
    m_lastMouse = getImagePoint(point);
    showInfo();
    return;
  }
  if(!m_isDragging) {
    m_lastMouse = getImagePoint(point);
    showInfo();
    return;
  }
  Image &image = getCurrentImage();
  point = getImagePoint(point);
  if(image.getImageRect().PtInRect(point)) {
    CSize dp = point - m_mouseDown;
    dp.cx = -dp.cx;
    const double zoom = image.getZoomFactor();
    dp.cx = (int)((double)dp.cx / zoom);
    dp.cy = (int)((double)dp.cy / zoom);
    m_lastMouse = point;
    if(image.setOffset(m_mouseDownOffset + dp)) {
      Invalidate(FALSE);
    }
  }
}

void CTestPictureDlg::setDragging(bool on) {
  m_isDragging = on;
  const int cursorId[] = { OCR_NORMAL, OCR_HAND };
  setWindowCursor(getImageWin(), MAKEINTRESOURCE(cursorId[on?1:0]));
  setWindowCursor(this         , MAKEINTRESOURCE(cursorId[on?1:0]));
}

BOOL CTestPictureDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  Image &image = getCurrentImage();
  if(image.isLoaded() && isMenuItemChecked(this, ID_VIEW_KEEPASPECTRATIO)) {
    ScreenToClient(&pt);
    m_lastMouse = getImagePoint(pt);
    if(image.zoom(m_lastMouse, zDelta > 0)) {
      Invalidate(FALSE);
    }
  }
  return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

CPoint CTestPictureDlg::getImagePoint(const CPoint &p) const {
  CPoint p1 = p;
  ClientToScreen(&p1);
  getImageWin()->ScreenToClient(&p1);
  return p1;
}

void CTestPictureDlg::OnFileLoad() {
  try {
    CFileDialog dlg(TRUE);
    dlg.m_ofn.lpstrTitle      = _T("Load Image");
    dlg.m_ofn.lpstrFilter     = _T("BMP files (*.bmp)\0*.bmp\0"
                                   "JPG files (*.jpg)\0*.jpg\0"
                                   "GIF files (*.gif)\0*.gif\0"
                                   "PNG files (*.png)\0*.png\0"
                                   "TIF files (*.tif)\0*.tif; *.tiff\0"
                                   "ICO files (*.ico)\0*.ico\0"
                                   "CUR files (*.cur)\0*.cur\0"
                                   "DIB files (*.dib)\0*.dib\0"
                                   "EMF files (*.emf)\0*.emf\0"
                                   "WMF files (*.wmf)\0*.wmf\0"
                                   "All files (*.*)\0*.*\0"
                                   "\0");
    dlg.m_ofn.nFilterIndex = m_extensionIndex;

    if(dlg.DoModal() != IDOK) {
      return;
    }
    const String fileName = dlg.m_ofn.lpstrFile;
    m_extensionIndex = dlg.m_ofn.nFilterIndex;

    Image &image = getCurrentImage();
    image.load(fileName);
  } catch(Exception e) {
    MessageBox(e.what(), _T("Error"), MB_ICONWARNING);
    return;
  }
  Invalidate();
}

void CTestPictureDlg::OnFileUnload() {
  Image &image = getCurrentImage();
  image.unload();
  Invalidate();
}

void CTestPictureDlg::OnFileSaveBmp() {
  savePicture(PFF_BMP);
}

void CTestPictureDlg::OnFileSaveJpg() {
  savePicture(PFF_JPG);
}

void CTestPictureDlg::OnFileSavePng() {
  savePicture(PFF_PNG);
}

void CTestPictureDlg::OnFileSaveTiff() {
  savePicture(PFF_TIFF);
}

static String getFileExtension(PictureFileFormat format) {
  switch(format) {
  case PFF_BMP : return _T("bmp");
  case PFF_JPG : return _T("jpg");
  case PFF_PNG : return _T("png");
  case PFF_TIFF: return _T("tiff");
  default      : return _T("dat");
  }
}

static void saveBitmap(HBITMAP bm, const String &fileName, PictureFileFormat fileFormat) {
  switch(fileFormat) {
  case PFF_BMP : writeAsBMP( bm, ByteOutputFile(fileName)); break;
  case PFF_JPG : writeAsJPG( bm, ByteOutputFile(fileName)); break;
  case PFF_PNG : writeAsPNG( bm, ByteOutputFile(fileName)); break;
  case PFF_TIFF: writeAsTIFF(bm, ByteOutputFile(fileName)); break;
  }
}

void CTestPictureDlg::savePicture(PictureFileFormat fileFormat) {
  const String ext      = getFileExtension(fileFormat);
  String       fileName = FileNameSplitter(getCurrentImage().getFileName()).setExtension(ext).getAbsolutePath();

  String       filter   = format(_T("%s files (*.%s)%c*.%s%c%c"), toUpperCase(ext).cstr(), ext.cstr(), 0, ext.cstr(), 0, 0);

  CFileDialog  dlg(FALSE, ext.cstr(), fileName.cstr());
  dlg.m_ofn.lpstrTitle  = _T("Save Image");
  dlg.m_ofn.lpstrFilter = filter.cstr();
  if(dlg.DoModal() == IDOK) {
    fileName = dlg.m_ofn.lpstrFile;
    HBITMAP bm = getCurrentImage();
    if(bm) {
      saveBitmap(bm, fileName, fileFormat);
      DeleteObject(bm);
    }
  } 
}

void CTestPictureDlg::OnEditCurrent1() {
  setCurrentImageIndex(0);
}

void CTestPictureDlg::OnEditCurrent2() {
  setCurrentImageIndex(1);
}

void CTestPictureDlg::setCurrentImageIndex(int index) {
  const int menuItem[] = { ID_EDIT_CURRENT_1, ID_EDIT_CURRENT_2 };
  for(int i = 0; i < 2; i++) {
    checkMenuItem(this, menuItem[i], i == index);
  }
  m_currentImageIndex = index;
  Invalidate(FALSE);
}

void CTestPictureDlg::OnEditPic1pic2() {
  m_image[0] = m_image[1];
  Invalidate(FALSE);
}

void CTestPictureDlg::OnEditPic2pic1() {
  m_image[1] = m_image[0];
  Invalidate(FALSE);
}

static HBITMAP swapRB(HBITMAP bm) {
  ByteArray ba;;
  BITMAPINFO info;
  getBitmapBits(bm, ba, info);
  int pixelCount = (int)(ba.size() / sizeof(DWORD));
  for(DWORD *p = (DWORD*)ba.getData(); pixelCount--;) {
    const DWORD c = *p;
    *(p++) = COLORREF2D3DCOLOR(c);
  }
  setBitmapBits(bm, ba, info);
  return bm;
}

void CTestPictureDlg::OnEditSwapRedAndBlue() {
  CPicture &pic = getCurrentImage();
  if(pic.isLoaded()) {
    HBITMAP bm = NULL;
    try {
      bm = swapRB(pic);
      pic = bm;
      DeleteObject(bm);
      Invalidate(FALSE);
    } catch(Exception e) {
      MessageBox(e.what(), _T("Error"), MB_ICONWARNING);
    }
  }
}

void CTestPictureDlg::OnEditClearLog() {
  redirectDebugLog();
}

void CTestPictureDlg::OnViewKeepAspectRatio() {
  toggleMenuItem(this, ID_VIEW_KEEPASPECTRATIO);
  Invalidate(FALSE);
}

void CTestPictureDlg::OnViewShowDebugInfo() {
  setInfoWindowVisible(toggleMenuItem(this, ID_VIEW_SHOWDEBUGINFO));
}

void CTestPictureDlg::setInfoWindowVisible(bool visible) {
  CWnd       *imageWin     = getImageWin();
  CWnd       *infoWin      = getInfoWin();
  const bool  isVisible    = infoWin->IsWindowVisible() ? true : false;
  const CRect cr           = getClientRect(this);
  const CRect imageWinRect = getWindowRect(imageWin);
  const int   marg         = imageWinRect.left;

  if(visible) {
    if(!isVisible) {
      setWindowSize(imageWin, CSize(imageWinRect.Width(), cr.Height() - 2*marg - m_infoWindowHeight));
      const int y = getWindowRect(imageWin).bottom;
      setWindowPosition(infoWin, CPoint(imageWinRect.left, y));
      setWindowSize(    infoWin, CSize( imageWinRect.Width(), m_infoWindowHeight));
      infoWin->ShowWindow(SW_SHOW);
    }
  } else {
    infoWin->ShowWindow(SW_HIDE);
    setWindowSize(imageWin, CSize(imageWinRect.Width(), cr.Height() - 2*marg));
  }
}

void CTestPictureDlg::OnDropFiles(HDROP hDropInfo) {
  TCHAR fileName[256];
  const int count = DragQueryFile(hDropInfo,-1,fileName,ARRAYSIZE(fileName));

  if(count >= 1) {
    DragQueryFile(hDropInfo,0,fileName,ARRAYSIZE(fileName));
    try {
      Image &image = getCurrentImage();
      image.load(fileName);
      Invalidate();
    } catch(Exception e) {
      MessageBox(e.what(), _T("Error"), MB_ICONWARNING);
    }
  }
  CDialog::OnDropFiles(hDropInfo);
}
