#include "stdafx.h"
#include <ByteFile.h>
#include <Random.h>
#include "MandelbrotDlg.h"
#include "MaxCountDlg.h"
#include "JuliaDlg.h"
#include "ShowColorMapDlg.h"
#include "MBBigRealCalculator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CMandelbrotDlg::CMandelbrotDlg(DigitPool *digitPool, CWnd *pParent) 
: CDialog(CMandelbrotDlg::IDD, pParent)
, m_digitPool(digitPool)
, m_rect0(    digitPool)
, m_zoom1Rect(digitPool)
, m_bigRealTransform(LINEAR,LINEAR,digitPool)
, m_colorMap(ColorMapData::getDefault(), 2000)
{
  m_hIcon                     = theApp.LoadIcon(IDR_MAINFRAME);
  m_precisionMode             = ID_OPTIONS_AUTOPRECISION;
  m_state                     = STATE_IDLE;
  m_frameGenerator            = NULL;
  m_colorMapEditThread        = NULL;
  m_ccMatrix                  = NULL;
  m_cca                       = NULL;
  m_imageDC                   = NULL;
  m_imageCopy                 = NULL;
  setDigits();
}

BOOL CMandelbrotDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CMandelbrotDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZING()
    ON_WM_SIZE()
    ON_WM_CLOSE()
    ON_WM_TIMER()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
    ON_WM_NCLBUTTONDOWN()
    ON_WM_NCMOUSEMOVE()
    ON_COMMAND(ID_FILE_SAVERECTANGLE                   , OnFileSaveRectangle                  )
    ON_COMMAND(ID_FILE_SAVECOLORMAP                    , OnFileSaveColorMap                   )
    ON_COMMAND(ID_FILE_SAVEIMAGE                       , OnFileSaveImage                      )
    ON_COMMAND(ID_FILE_LOADRECTANGLE                   , OnFileLoadRectangle                  )
    ON_COMMAND(ID_FILE_LOADCOLORMAP                    , OnFileLoadColorMap                   )
    ON_COMMAND(ID_FILE_MAKEMOVIE                       , OnFileMakeMovie                      )
    ON_COMMAND(ID_FILE_STOPMOVIE                       , OnFileStopMovie                      )
    ON_COMMAND(ID_FILE_EXIT                            , OnFileExit                           )
    ON_COMMAND(ID_EDIT_CALCULATEIMAGE                  , OnEditCalculateImage                 )
    ON_COMMAND(ID_EDIT_SUSPENDCALCULATION              , OnEditSuspendCalculation             )
    ON_COMMAND(ID_EDIT_ABORTCALCULATION                , OnEditAbortCalculation               )
    ON_COMMAND(ID_EDIT_BACK                            , OnEditBack                           )
    ON_COMMAND(ID_OPTIONS_MAXITERATIONS                , OnOptionsMaxIterations               )
    ON_COMMAND(ID_OPTIONS_EDITCOLORMAP                 , OnOptionsEditColorMap                )
    ON_COMMAND(ID_OPTIONS_SHOWCOLORMAP                 , OnOptionsShowColorMap                )
    ON_COMMAND(ID_OPTIONS_AUTOPRECISION                , OnOptionsAutoPrecision               )
    ON_COMMAND(ID_OPTIONS_FORCEFPU                     , OnOptionsForceFPU                    )
    ON_COMMAND(ID_OPTIONS_FORCEBIGREAL                 , OnOptionsForceBigReal                )
    ON_COMMAND(ID_OPTIONS_ANIMATE_CALCULATION          , OnOptionsAnimateCalculation          )
    ON_COMMAND(ID_OPTIONS_PAINTORBIT                   , OnOptionsPaintOrbit                  )
    ON_COMMAND(ID_OPTIONS_USEEDGEDETECTION             , OnOptionsUseEdgeDetection            )
    ON_COMMAND(ID_OPTIONS_USEONLY1CPU                  , OnOptionsUseOnly1CPU                 )
    ON_COMMAND(ID_OPTIONS_RETAIN_ASPECTRATIO           , OnOptionsRetainAspectRatio           )
    ON_COMMAND(ID_OPTIONS_SHOWZOOMFACTOR               , OnOptionsShowZoomFactor              )
    ON_COMMAND(ID_OPTIONS_SHOWCALULATIONTIME           , OnOptionsShowCalculationTime         )
    ON_COMMAND(ID_OPTIONS_SHOWCHANGEFLAGS              , OnOptionsShowChangeFlags             )
    ON_COMMAND(ID_HELP_ABOUTMANDELBROT                 , OnHelpAboutMandelbrot                )
    ON_MESSAGE(ID_MSG_STARTCALCULATION                 , OnMsgStartCalculation                )
    ON_MESSAGE(ID_MSG_UPDATEWINDOWSTATE                , OnMsgUpdateWindowState               )
    ON_MESSAGE(ID_MSG_MOVIEDONE                        , OnMsgMovieDone                       )
END_MESSAGE_MAP()

void CMandelbrotDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

HCURSOR CMandelbrotDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

BOOL CMandelbrotDlg::OnInitDialog() {
  __super::OnInitDialog();

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

  SetIcon(m_hIcon, TRUE);   // Set big icon
  SetIcon(m_hIcon, FALSE);  // Set small icon

  m_crossIcon      = createIcon(theApp.m_hInstance, IDB_MARK_COLORBITMAP, IDB_MARK_MASKBITMAP);
  m_accelTable     = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
  m_rect0          = RealRectangle2D(-2.1,-2.1, 4.2,4.2);
  m_imageWindow    = GetDlgItem(IDC_STATIC_IMAGEWINDOW);

  theApp.m_device.attach(*this);

  m_dummyMatrix    = newCCM(CSize(10,10),1);
  m_mbContainer    = new DialogMBContainer(this, m_dummyMatrix);                 TRACE_NEW(m_mbContainer);
  m_calculatorPool = new CalculatorPool(*m_mbContainer);                         TRACE_NEW(m_calculatorPool);

  const CSize startSize(750,750);

  setClientRectSize(this, CSize(startSize.cx+50, startSize.cy+50));
  setWindowPosition(this, IDC_STATIC_IMAGEWINDOW, CPoint(0, 0));
  setClientRectSize(this, IDC_STATIC_IMAGEWINDOW, startSize);
  const CRect imageRect = ::getWindowRect(this, IDC_STATIC_IMAGEWINDOW);
  const int infoWidth = imageRect.Width() * 35 / 100;
  setWindowPosition(this, IDC_STATIC_INFOWINDOW, CPoint(0, imageRect.bottom));
  setWindowSize(    this, IDC_STATIC_INFOWINDOW, CSize(infoWidth, 20));
  const CRect infoRect = ::getWindowRect(this, IDC_STATIC_INFOWINDOW);
  setWindowPosition(this, IDC_STATIC_MOUSEINFO , CPoint(infoRect.right, infoRect.top));
  setWindowSize(    this, IDC_STATIC_MOUSEINFO , CSize(imageRect.right - infoRect.right, 20));

  setClientRectSize(this, CSize(imageRect.Width(), infoRect.bottom));

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATIC_IMAGEWINDOW, RELATIVE_SIZE                                       );
  m_layoutManager.addControl(IDC_STATIC_INFOWINDOW , RELATIVE_Y_POS | PCT_RELATIVE_RIGHT                 );
  m_layoutManager.addControl(IDC_STATIC_MOUSEINFO  , RELATIVE_Y_POS | PCT_RELATIVE_LEFT | RELATIVE_RIGHT );

  m_animateCalculation = isMenuItemChecked(this, ID_OPTIONS_ANIMATE_CALCULATION);
  m_calculateWithOrbit = isMenuItemChecked(this, ID_OPTIONS_PAINTORBIT         );
  m_useOnly1CPU        = isMenuItemChecked(this, ID_OPTIONS_USEONLY1CPU        );
  m_useEdgeDetection   = isMenuItemChecked(this, ID_OPTIONS_USEEDGEDETECTION   );
  m_retainAspectRatio  = isMenuItemChecked(this, ID_OPTIONS_RETAIN_ASPECTRATIO );
  m_showZoomFactor     = isMenuItemChecked(this, ID_OPTIONS_SHOWZOOMFACTOR     );
  m_suspendingMenuTextIsSuspending = true;

//  setWindowPosition(this, CPoint(0,0));
  centerWindow(this);
  m_calculatorPool->addPropertyChangeListener(m_mbContainer);
  initScale();
  setWorkSize();
  startCalculation();
  return TRUE;  // return TRUE  unless you set the focus to a control
}

// -------------------------------- File menu -------------------------------------------

static const TCHAR *rectangleFileExtensions = _T("Rectangle files (*.rect)\0*.rect;\0"
                                                 "All files (*.*)\0*.*\0\0");

static const TCHAR *colormapFileExtensions  = _T("Colormap files (*.map)\0*.map;\0"
                                                 "All files (*.*)\0*.*\0\0");

void CMandelbrotDlg::OnFileSaveRectangle() {
  try {
    CFileDialog dlg(FALSE);
    dlg.m_ofn.lpstrTitle  = _T("Save Rectangle");
    dlg.m_ofn.lpstrFilter = rectangleFileExtensions;

    const BigRealRectangle2D r    = m_bigRealTransform.getFromRectangle();
    const UINT               prec = (UINT)getDigits();

    String fileName = format(_T("[%s,%s]x[%s,%s].rect")
                            ,::toString(r.getMinX()  ,prec,0, ios::scientific).cstr()
                            ,::toString(r.getMinY()  ,prec,0, ios::scientific).cstr()
                            ,::toString(r.getWidth() ,5   ,0, ios::scientific).cstr()
                            ,::toString(r.getHeight(),5   ,0, ios::scientific).cstr()
                            );
    TCHAR nameBuffer[256];
    _tcscpy(nameBuffer, fileName.cstr());
    dlg.m_ofn.lpstrFile = nameBuffer;
    dlg.m_ofn.nMaxFile  = ARRAYSIZE(nameBuffer);
    if(dlg.DoModal() != IDOK || _tcslen(nameBuffer) == 0) {
      return;
    }

    FileNameSplitter nameInfo(nameBuffer);
    if(nameInfo.getExtension().length() == 0) {
      nameInfo.setExtension(_T("rect"));
    }
    saveRectangle(nameInfo.getAbsolutePath());
  } catch(Exception e) {
    showException(e);
  }
}

void CMandelbrotDlg::OnFileSaveColorMap() {
  try {
    CFileDialog dlg(FALSE);
    dlg.m_ofn.lpstrTitle  = _T("Save Colormap");
    dlg.m_ofn.lpstrFilter = colormapFileExtensions;

    if(dlg.DoModal() != IDOK || _tcslen(dlg.m_ofn.lpstrFile) == 0) {
      return;
    }
    FileNameSplitter nameInfo(dlg.m_ofn.lpstrFile);
    if(nameInfo.getExtension().length() == 0) {
      nameInfo.setExtension(_T("map"));
    }
    saveColorMap(nameInfo.getAbsolutePath());
  } catch(Exception e) {
    showException(e);
  }
}

void CMandelbrotDlg::OnFileSaveImage() {
  static const TCHAR *imageFileExtensions = _T("Bitmap files (*.bmp)\0*.bmp;\0"
                                               "All files (*.*)\0*.*\0\0");

  CFileDialog dlg(FALSE);
  dlg.m_ofn.lpstrTitle  = _T("Save image");
  dlg.m_ofn.lpstrFilter = imageFileExtensions;

  if(dlg.DoModal() != IDOK || _tcslen(dlg.m_ofn.lpstrFile) == 0) {
    return;
  }

  try {
//    m_pixRect->writeAsBMP(ByteOutputFile(FileNameSplitter(dlg.m_ofn.lpstrFile).setExtension(_T("bmp")).getFullPath())); TODO
  } catch(Exception e) {
    showException(e);
  }
}

void CMandelbrotDlg::OnFileLoadRectangle() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrTitle   = _T("Load Rectangle");
  dlg.m_ofn.lpstrFilter  = rectangleFileExtensions;
  dlg.m_ofn.Flags |= OFN_FILEMUSTEXIST;
  if(dlg.DoModal() != IDOK || _tcslen(dlg.m_ofn.lpstrFile) == 0) {
    return;
  }
  try {
    handleChangeCode(loadRectangle(dlg.m_ofn.lpstrFile));
  } catch(Exception e) {
    showException(e);
  }
}

void CMandelbrotDlg::OnFileLoadColorMap() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrTitle   = _T("Load Colormap");
  dlg.m_ofn.lpstrFilter  = colormapFileExtensions;
  dlg.m_ofn.Flags |= OFN_FILEMUSTEXIST;
  if(dlg.DoModal() != IDOK || _tcslen(dlg.m_ofn.lpstrFile) == 0) {
    return;
  }
  try {
    handleChangeCode(loadColorMap(dlg.m_ofn.lpstrFile));
  } catch(Exception e) {
    showException(e);
  }
}

void CMandelbrotDlg::OnFileMakeMovie() {
  if(isMakingMovie()) return;
  TCHAR cwd[1024];
  _tgetcwd(cwd, ARRAYSIZE(cwd));
  CSelectDirDlg dlg(cwd);
  if(dlg.DoModal() != IDOK) {
    return;
  }
  m_frameGenerator = new MBFrameGenerator(this, dlg.getSelectedDir()); TRACE_NEW(m_frameGenerator);
  updateMovieMenuItem();
  startTimer(TIMER_MOVIEMAKER, 5000);
}

void CMandelbrotDlg::OnFileStopMovie() {
  if(!isMakingMovie()) return;
  stopTimer(TIMER_MOVIEMAKER);
  SAFEDELETE(m_frameGenerator);
  updateWindowStateInternal();
  updateMovieMenuItem();
}

void CMandelbrotDlg::OnFileExit() {
  OnFileStopMovie();
  SAFEDELETE(m_calculatorPool);
  SAFEDELETE(m_mbContainer   );
  deleteCCM( m_dummyMatrix   );
  destroyImageCopy();
  resetImageStack();
  destroyImageDC();
  destroyCCM();
  DestroyIcon(m_crossIcon    );
  EndDialog(IDOK);
}

void CMandelbrotDlg::OnClose() {
  OnFileExit();
}

void CMandelbrotDlg::OnOK() {
}

void CMandelbrotDlg::OnCancel() {
}

// -------------------------------- Edit menu -------------------------------------------

void CMandelbrotDlg::OnEditCalculateImage() {
  startCalculation();
}

void CMandelbrotDlg::OnEditSuspendCalculation() {
  if(isCalculationActive()) {
    if(m_calculatorPool->getCalculatorsInState(CALC_RUNNING).isEmpty()) {
      m_calculatorPool->resumeCalculation();
    } else {
      m_calculatorPool->suspendCalculation();
    }
  }
}

void CMandelbrotDlg::OnEditAbortCalculation() {
  m_calculatorPool->killAll();
}

void CMandelbrotDlg::OnEditBack() {
  popImage();
}

// -------------------------------- Options menu ----------------------------------------

void CMandelbrotDlg::OnOptionsMaxIterations() {
  if(isCalculationActive()) {
    return;
  }
  CMaxCountDlg dlg(getMaxCount());
  if(dlg.DoModal() == IDOK) {
    handleChangeCode(setMaxCount(dlg.getMaxCount()));
  }
}

void CMandelbrotDlg::OnOptionsEditColorMap() {
  if(isCalculationActive()) {
    return;
  }
  startColorMapEditor();
}

void CMandelbrotDlg::startColorMapEditor() {
  m_gate.wait();
  if(!hasColorMapEditor()) {
    m_colorMapEditThread = CEditColorMapDlgThread::startThread(this, m_colorMap.getColorMapData());
    remoteUpdateWindowState();
  }
  m_gate.signal();
}

void CMandelbrotDlg::stopColorMapEditor() {
  m_gate.wait();
  if(hasColorMapEditor()) {
    m_colorMapEditThread = NULL;
    remoteUpdateWindowState();
  }
  m_gate.signal();
}


void CMandelbrotDlg::OnOptionsShowColorMap() {
  CShowColorMapDlg dlg(m_colorMap);
  dlg.DoModal();
}

void CMandelbrotDlg::OnOptionsAutoPrecision() {
  setPrecision(ID_OPTIONS_AUTOPRECISION);
}

void CMandelbrotDlg::OnOptionsForceFPU() {
  setPrecision(ID_OPTIONS_FORCEFPU);
}

void CMandelbrotDlg::OnOptionsForceBigReal() {
  setPrecision(ID_OPTIONS_FORCEBIGREAL);
}

void CMandelbrotDlg::OnOptionsAnimateCalculation() {
  m_animateCalculation = toggleMenuItem(this, ID_OPTIONS_ANIMATE_CALCULATION);
}

void CMandelbrotDlg::OnOptionsPaintOrbit() {
  if(isCalculationActive()) {
    m_calculatorPool->suspendCalculation();
    m_calculatorPool->waitUntilNoRunning();
    m_calculateWithOrbit = toggleMenuItem(this, ID_OPTIONS_PAINTORBIT);
    clearCCA();
    getCCA();
    m_calculatorPool->resumeCalculation();
  } else {
    m_calculateWithOrbit = toggleMenuItem(this, ID_OPTIONS_PAINTORBIT);
  }
}

void CMandelbrotDlg::OnOptionsUseOnly1CPU() {
  m_useOnly1CPU = toggleMenuItem(this, ID_OPTIONS_USEONLY1CPU);
}

void CMandelbrotDlg::OnOptionsUseEdgeDetection() {
  m_useEdgeDetection = toggleMenuItem(this, ID_OPTIONS_USEEDGEDETECTION);
}

void CMandelbrotDlg::OnOptionsRetainAspectRatio() {
  if(m_retainAspectRatio = toggleMenuItem(this, ID_OPTIONS_RETAIN_ASPECTRATIO)) {
    handleChangeCode(handleTransformChanged(true));
  }
}

void CMandelbrotDlg::OnOptionsShowZoomFactor() {
  m_showZoomFactor = toggleMenuItem(this, ID_OPTIONS_SHOWZOOMFACTOR);
  Invalidate(false);
}

void CMandelbrotDlg::OnOptionsShowCalculationTime() {
  checkMenuItem(this,ID_OPTIONS_SHOWCALULATIONTIME, true );
  checkMenuItem(this,ID_OPTIONS_SHOWCHANGEFLAGS   , false);
  showCalculationState();
}

void CMandelbrotDlg::OnOptionsShowChangeFlags() {
  checkMenuItem(this,ID_OPTIONS_SHOWCALULATIONTIME, false);
  checkMenuItem(this,ID_OPTIONS_SHOWCHANGEFLAGS   , true );
  showCalculationState();
}

void CMandelbrotDlg::OnHelpAboutMandelbrot() {
  CAboutDlg().DoModal();
}

// ----------------------------------- Mouse --------------------------------------------

CPoint CMandelbrotDlg::getImagePointFromMousePoint(const CPoint &p) const {
  CPoint tmp = p;
  ClientToScreen(&tmp);
  m_imageWindow->ScreenToClient(&tmp);
  return tmp;
}

void CMandelbrotDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  __super::OnLButtonDown(nFlags, point);
  if(!isCalculationActive()) {
    m_mouseDownPoint = getImagePointFromMousePoint(point);
#ifdef SAVE_CALCULATORINFO
    if(nFlags & MK_CONTROL) {
      const CalculatorInfo *info = m_calculatorPool->findInfo(m_mouseDownPoint);
      if(info != NULL) {
        paintPointSet(info->getEdgeSet() , RGB(255,255,255));
        paintPointSet(info->getInnerSet(), RGB(255,255,0));
        showInformation(info->toString());
        paintPointSet(info->getEdgeSet() , BLACK);
        paintPointSet(info->getInnerSet(), BLACK);
      }
      __super::OnLButtonDown(nFlags, point);
      return;
    }
#endif
    setDragRect(m_mouseDownPoint,m_mouseDownPoint);
    setState(STATE_DRAGGING);
  }
}

#ifdef SAVE_CALCULATORINFO
void CMandelbrotDlg::paintPointSet(const PointSet &ps, COLORREF color) {
  CClientDC dc(m_imageWindow);
  for(Iterator<CPoint> it = ps.getIterator(); it.hasNext();) {
    dc.SetPixel(it.next(), color);
  }
}
#endif

void CMandelbrotDlg::OnLButtonUp(UINT nFlags, CPoint point) {
  __super::OnLButtonUp(nFlags, point);
  if(getState() == STATE_DRAGGING) {
    setState(STATE_IDLE);
    if(getArea(m_dragRect) != 0) {
      try {
        pushImage();
        handleChangeCode(setScale(m_bigRealTransform.backwardTransform(toBigRealRect(m_dragRect,getDigitPool()))));
      } catch(Exception e) {
        showException(e);
      }
    }
  }
}

void CMandelbrotDlg::OnLButtonDblClk(UINT nFlags, CPoint point) {
  __super::OnLButtonDblClk(nFlags, point);
  m_mouseDownPoint = getImagePointFromMousePoint(point);
  const BigRealPoint2D p = m_bigRealTransform.backwardTransform(toBigRealPoint(m_mouseDownPoint, getDigitPool()));
//  CJuliaDlg dlg(p); // TODO
//  dlg.DoModal();
}

void CMandelbrotDlg::OnRButtonDown(UINT nFlags, CPoint point) {
  __super::OnRButtonDown(nFlags, point);
  if(!isCalculationActive()) {
    m_mouseDownPoint = getImagePointFromMousePoint(point);
    createImageCopy();
    setState(STATE_MOVING);
  }
}

void CMandelbrotDlg::OnRButtonUp(UINT nFlags, CPoint point) {
  __super::OnRButtonUp(nFlags, point);
  if(getState() == STATE_MOVING) {
    setState(STATE_IDLE);
    try {
      destroyImageCopy();
      calculateMovedImage(getImagePointFromMousePoint(point) - m_mouseDownPoint);
    } catch (Exception e) {
      showException(e);
    }
  }
}

void CMandelbrotDlg::OnMouseMove(UINT nFlags, CPoint point) {
  __super::OnMouseMove(nFlags, point);
  const CPoint p = getImagePointFromMousePoint(point);
  switch(getState()) {
  case STATE_IDLE    :
    break;
  case STATE_DRAGGING:
    if(nFlags & MK_LBUTTON) {
      CPoint rb = setDragRect(m_mouseDownPoint,p);
      if(rb != p) {
        m_imageWindow->ClientToScreen(&rb);
        SetCursorPos(rb.x,rb.y);
      }
    } else {
      removeDragRect();
      setState(STATE_IDLE);
    }
    break;

  case STATE_MOVING  :
    if(nFlags & MK_RBUTTON) {
      paintMovedImage(p - m_mouseDownPoint);
    } else {
      paintMovedImage(CSize(0,0));
      setState(STATE_IDLE);
    }
    break;
  }
  showMousePoint(p);
}

BOOL CMandelbrotDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  if(!isCalculationActive()) {
    if(zDelta > 0) {
      pushImage();
      m_imageWindow->ScreenToClient(&pt);
      handleChangeCode(setScale(m_bigRealTransform.zoom(BigRealPoint2D(pt.x,pt.y, getDigitPool()),BigReal(0.25,getDigitPool()))));
    } else {
      popImage();
    }
  }
  return __super::OnMouseWheel(nFlags, zDelta, pt);
}

// ----------------------------------- Message handlers ---------------------------------

LRESULT CMandelbrotDlg::OnMsgStartCalculation(WPARAM wp, LPARAM lp) {
  startCalculation();
  return 0;
}

LRESULT CMandelbrotDlg::OnMsgUpdateWindowState(WPARAM wp, LPARAM lp) {
  updateWindowStateInternal();
  return 0;
}

LRESULT CMandelbrotDlg::OnMsgMovieDone(WPARAM wp, LPARAM lp) {
  OnFileStopMovie();
  return 0;
}

void CMandelbrotDlg::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(source == m_colorMapEditThread) {
    switch(id) {
    case DIALOGACTIVE:
      if(!*(bool*)newValue) {
        stopColorMapEditor();
      }
      break;
    case COLORMAPDATA:
      { const ColorMapData &cmd = *(ColorMapData*)newValue;
        handleChangeCode(setColorMapData(cmd));
        break;
      }
    }
  } else if(source == m_calculatorPool) {
    switch(id) {
    case CALCULATIONACTIVE:
      { const bool oldActive = *(bool*)oldValue;
        const bool newActive = *(bool*)newValue;
        if(oldActive && !newActive) {
          m_jobQueue.clear();
          stopTimer(TIMER_CALCULATION);
          DLOG(_T("CalculationACtive=FALSE. Now POST WM_PAINT\n"));
          PostMessage(WM_PAINT);
        } else if(newActive) {
          remoteUpdateWindowState();
        }
      }
      break;
    case RUNNINGSET:
      { const CalculatorSet oldRunSet = *(CalculatorSet*)oldValue;
        const CalculatorSet newRunSet = *(CalculatorSet*)newValue;
        if(oldRunSet.isEmpty() != newRunSet.isEmpty()) {
          remoteUpdateWindowState();
        }
      }
      break;
    }
  }
}

void CMandelbrotDlg::OnPaint() {
  if (IsIconic())   {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    const int   cxIcon = GetSystemMetrics(SM_CXICON);
    const int   cyIcon = GetSystemMetrics(SM_CYICON);
    const CRect rect   = getClientRect(this);
    const int   x      = (rect.Width()  - cxIcon + 1) / 2;
    const int   y      = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  } else {
    if(!isCalculationActive(false)) {
      updateWindowStateInternal();
      flushCCM();
      if(showZoomFactor()) {
        CPaintDC dc(this);
        paintZoomFactor(dc);
      }
      m_hasResized = false;
      if(isMakingMovie()) {
        m_frameGenerator->notifyFrameReady();
      }
    }
    __super::OnPaint();
  }
}

// --------------------------------------------------------------------------------------------------

void CMandelbrotDlg::setState(DlgState newState) {
  m_state = newState;
  showWindowState();
  switch(m_state) {
  case STATE_IDLE:
    setWindowCursor(m_imageWindow, MAKEINTRESOURCE(OCR_NORMAL));
    break;
  case STATE_DRAGGING:
    setWindowCursor(m_imageWindow, MAKEINTRESOURCE(OCR_CROSS));
    break;
  case STATE_MOVING:
    setWindowCursor(m_imageWindow, MAKEINTRESOURCE(OCR_HAND));
    break;
  }
}

const TCHAR *CMandelbrotDlg::s_stateName[] = {
  _T("IDLE")
 ,_T("DRAGGING")
 ,_T("MOVING")
};

void CMandelbrotDlg::dumpHistogram() {
  static int dumpCount = 0;
  const String histoFileName = format(_T("c:\\temp\\mandel\\histo%04d.txt"),dumpCount);
  const String mapFileName   = format(_T("c:\\temp\\mandel\\map%04d.txt" ),dumpCount);
  const CellCountHistogram &h = m_ccMatrix->getHistogram();
  redirectDebugLog(false,histoFileName.cstr());
  debugLog(_T("%s"), h.toString().cstr());

  redirectDebugLog(false,mapFileName.cstr());
  debugLog(_T("%s"), m_colorMap.toString().cstr());

  dumpCount++;
}

void CMandelbrotDlg::flushCCM() {
//  dumpHistogram();
  m_gate.wait();
  PixRect *pr = NULL;
  try {
    clearCCA();
    pr = newPixRect(m_ccMatrix->getSize());
    m_ccMatrix->convertToPixRect(pr, m_colorMap);
    CClientDC dc(m_imageWindow);
    pixRectToWindow(pr, dc);
    deletePixRect(pr);
  } catch (Exception e) {
    deletePixRect(pr);
    showException(e);
  }
  m_gate.signal();
}

static String zoomToStr(double z) {
  return (z < 100000) ? format(_T("%.1lf"),z) : format(_T("%.2lg"),z);
}

void CMandelbrotDlg::paintZoomFactor(CDC &dc) {
  const Size2D &zoom    = getZoomFactor();
  String        zoomStr = (zoom.cx == zoom.cy) 
                        ? zoomToStr(zoom.cx)
                        : format(_T("(%s,%s)"), zoomToStr(zoom.cx).cstr(), zoomToStr(zoom.cy).cstr());
  const CSize   sz      = getWindowSize();
  const CSize   tsz     = getTextExtent(dc, zoomStr);
  textOut(dc, 10, sz.cy-tsz.cy-10,zoomStr);
}

void CMandelbrotDlg::updateWindowStateInternal() {
  const bool isActive = isCalculationActive();

  enableMenuItem(this, ID_FILE_SAVEIMAGE                   ,!isActive   );
  enableMenuItem(this, ID_FILE_MAKEMOVIE                   ,!isActive   );
  enableMenuItem(this, ID_EDIT_CALCULATEIMAGE              ,!isActive   );
  enableMenuItem(this, ID_EDIT_ABORTCALCULATION            ,isActive    );
  enableMenuItem(this, ID_EDIT_SUSPENDCALCULATION          ,isActive    );
  enableMenuItem(this, ID_EDIT_BACK                        ,!isActive && !m_imageStack.isEmpty());
  setSuspendingMenuText(!isCalculationSuspended());
  enableMenuItem(this, ID_OPTIONS_MAXITERATIONS            ,!isActive   );
  enableMenuItem(this, ID_OPTIONS_EDITCOLORMAP             ,!isActive   && !hasColorMapEditor());
  enableMenuItem(this, ID_OPTIONS_AUTOPRECISION            ,!isActive   );
  enableMenuItem(this, ID_OPTIONS_FORCEFPU                 ,!isActive   );
  enableMenuItem(this, ID_OPTIONS_FORCEBIGREAL             ,!isActive   );
  enableSubMenuContainingId(this, ID_OPTIONS_AUTOPRECISION ,!isActive   );
  enableMenuItem(this, ID_OPTIONS_ANIMATE_CALCULATION      ,!isActive   );
//enableMenuItem(this, ID_OPTIONS_PAINTORBIT               ,!isActive   );
  enableMenuItem(this, ID_OPTIONS_USEEDGEDETECTION         ,!isActive   );
  enableMenuItem(this, ID_OPTIONS_USEONLY1CPU              ,!isActive   );
  enableMenuItem(this, ID_OPTIONS_RETAIN_ASPECTRATIO       ,!isActive   );
  enableMenuItem(this, ID_OPTIONS_SHOWZOOMFACTOR           ,!isActive   );
  showWindowState();
}

void CMandelbrotDlg::setSuspendingMenuText(bool isSuspendingText) {
  if(isSuspendingText != m_suspendingMenuTextIsSuspending) {
    setMenuItemText(this
                   ,ID_EDIT_SUSPENDCALCULATION
                   ,isSuspendingText ? _T("Suspend calculation\tAlt+S")
                                     : _T("Resume calculation\tAlt+S")
                   );
    m_suspendingMenuTextIsSuspending = isSuspendingText;
  }
}

static String changeFlagsToString(int flags) {
  String result;
  TCHAR *delim = NULL;
#define ADDFLAG(f)                                 \
if(flags & f##_CHANGED) {                          \
  if(delim) result += delim; else delim = _T(" "); \
  result += _T(#f);                                \
}

  ADDFLAG(MAXCOUNT );
  ADDFLAG(COLORDATA);
  ADDFLAG(IMAGE    );
  ADDFLAG(WORKSIZE );
  ADDFLAG(SCALE    );

  return result;
}

void CMandelbrotDlg::showWindowState() {
  if(!isMakingMovie()) {
    const BigRealRectangle2D r    = m_bigRealTransform.getFromRectangle();
    const UINT               prec = (UINT)getDigits();
    setWindowText(this, format(_T("[%s;%s]x[%s,%s] (digits=%u)")
                              ,::toString(r.getMinX()  ,prec,0, ios::scientific).cstr()
                              ,::toString(r.getMinY()  ,prec,0, ios::scientific).cstr()
                              ,::toString(r.getWidth() ,5   ,0, ios::scientific).cstr()
                              ,::toString(r.getHeight(),5   ,0, ios::scientific).cstr()
                              ,prec
                              )
                 );

    showCalculationState();
  } else {
    setWindowText(this, _T("Generating image list"));

    setWindowText(this, IDC_STATIC_INFOWINDOW
                      , format(_T("Calculating frame %d/%d")
                              ,m_frameGenerator->getFrameIndex()
                              ,m_frameGenerator->getTotalFrameCount()
                              )
                 );
  }
}
void CMandelbrotDlg::showCalculationState() {
  const bool calculating = isCalculationActive();
  String stateStr, infoText;
  if(!isCalculationActive()) {
    stateStr = getStateName();
  } else {
    stateStr = format(_T("%s %4.1lf%%")
                     ,isCalculationSuspended() ? _T("Suspended") : _T("Calculating")
                     ,getPercentDone()
                     );
  }
  if(isMenuItemChecked(this, ID_OPTIONS_SHOWCHANGEFLAGS)) {
    infoText = format(_T("Level:%2d. %s flags=%s")
                     ,m_imageStack.getHeight()
                     ,stateStr.cstr()
                     ,changeFlagsToString(m_lastChangeFlags).cstr()
                     );
  } else {
    infoText = format(_T("Level:%2d. %5.2lfsec. %s [%s]")
                     ,m_imageStack.getHeight()
                     ,m_calculatorPool->getTimeUsed()/1000000.0
                     ,stateStr.cstr()
                     ,m_calculatorPool->getStatesString().cstr()
                     );
  }
  setWindowText(this, IDC_STATIC_INFOWINDOW, infoText);

}

void CMandelbrotDlg::showMousePoint(const CPoint &p) {
  if(hasCCM() && m_ccMatrix->contains(p)) {
    const BigRealPoint2D rp       = m_bigRealTransform.backwardTransform(toBigRealPoint(p, getDigitPool()));
    const UINT           prec     = (UINT)getDigits();
    const String         pixelStr = m_ccMatrix->getPixelInfo(p);
    const String msg = format(_T(" (%3d,%3d) [%s,%s] Count=%s")
                             , p.x,p.y
                             ,::toString(rp.x,prec,prec+8,ios::scientific|ios::showpos).cstr()
                             ,::toString(rp.y,prec,prec+8,ios::scientific|ios::showpos).cstr()
                             ,pixelStr.cstr()
                             );
    setWindowText(this, IDC_STATIC_MOUSEINFO, msg);
  }
}

void CMandelbrotDlg::remoteStartCalculation() {
  DLOG(_T("Post STARTCALCULATION\n"));
  PostMessage(ID_MSG_STARTCALCULATION);
}

void CMandelbrotDlg::remoteUpdateWindowState() {
  DLOG(_T("Post UPDATEWINDOWSTATE\n"));
  PostMessage(ID_MSG_UPDATEWINDOWSTATE);
}

int CMandelbrotDlg::setWorkSize() {
  return setWorkSize(getClientRect(m_imageWindow).Size());
}

int CMandelbrotDlg::setWorkSize(const CSize &size) {
  createImageDC(size);
  int flags = 0;
  if(size != getWorkSize()) {
    flags |= WORKSIZE_CHANGED;
  }
  if (!hasCCM() || (getMaxCount() != m_ccMatrix->getMaxCount())) {
    flags |= MAXCOUNT_CHANGED;
  }
  if(flags == 0) {
    return 0;
  }

  createCCM(size);
  if(m_imageStack.isEmpty()) {
    initScale();
  }
  if(hasCCM()) {
    BigRealRectangle2D r(getDigitPool());
    r = RealRectangle2D(0,0,size.cx,size.cy);
    m_bigRealTransform.setToRectangle(r);
    flags |= handleTransformChanged(isRetainAspectRatio());
    m_zoom1Rect = m_bigRealTransform.getFromRectangle();
  }
  return flags;
}

void CMandelbrotDlg::createImageDC(const CSize &size) {
  destroyImageDC();
  m_imageRGN        = CreateRectRgn(0,0,size.cx, size.cy);
  m_imageDC         = ::GetDCEx(m_imageWindow->m_hWnd, m_imageRGN, DCX_CACHE | DCX_LOCKWINDOWUPDATE);
}

void CMandelbrotDlg::destroyImageDC() {
  if(m_imageDC != NULL) {
    ::ReleaseDC(m_imageWindow->m_hWnd, m_imageDC);
    m_imageDC = NULL;
  }
}

void CMandelbrotDlg::createCCM(const CSize &size) {
  destroyCCM();
  if(size.cx <= 0 || size.cy <= 0) {
    return;
  }
  m_ccMatrix = newCCM(size, getMaxCount());
}

void CMandelbrotDlg::destroyCCM() {
  if(hasCCM()) {
    deleteCCM(m_ccMatrix);
    m_ccMatrix = NULL;
  }
}

int CMandelbrotDlg::copyCCM(const CellCountMatrix *ccm) {
  int flags = 0;
  clearCCA();
  if(getWorkSize() != ccm->getSize()) {
    flags = setWorkSize(ccm->getSize());
  }
  *m_ccMatrix = *ccm;
  return flags | IMAGE_CHANGED;
}

CellCountMatrix *CMandelbrotDlg::newCCM(const CSize &size, UINT maxCount) { // static
  CellCountMatrix *ccm = new CellCountMatrix(size, maxCount); TRACE_NEW(ccm);
  return ccm;
}

void CMandelbrotDlg::deleteCCM(CellCountMatrix *ccm) { // static
  SAFEDELETE(ccm);
}

PixRect *CMandelbrotDlg::newPixRect(const CSize &size) { // static
  if((size.cx <= 0) || (size.cy <= 0)) {
    throwInvalidArgumentException(__TFUNCTION__,_T("size=(%d,%d)"), size.cx, size.cy);
  }
  PixRect *pr = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, size); TRACE_NEW(pr);
  return pr;
}

void CMandelbrotDlg::deletePixRect(PixRect *pr) { // static
  SAFEDELETE(pr);
}

void CMandelbrotDlg::createImageCopy() { // take a screenshut of m_imageDC, for dragging
  destroyImageCopy();
  m_imageCopy = windowToPixRect();
}

void CMandelbrotDlg::destroyImageCopy() {
  SAFEDELETE(m_imageCopy);
}


PixRect *CMandelbrotDlg::windowToPixRect() {
  PixRect *pr = newPixRect(getWorkSize());
  PixRect::bitBlt(pr, ORIGIN, pr->getSize(), SRCCOPY, m_imageDC, ORIGIN);
  return pr;
}

void CMandelbrotDlg::pixRectToWindow(PixRect *pr, HDC hdc) {
  PixRect::bitBlt(hdc?hdc:m_imageDC,0,0,pr->getWidth(),pr->getHeight(),SRCCOPY,pr,0,0);
}

int CMandelbrotDlg::setMaxCount(UINT maxCount) {
  m_gate.wait();
  int ret = 0;
  if(maxCount != getMaxCount()) {
    m_colorMap.setMaxCount(maxCount);
    if(hasCCM()) {
      m_ccMatrix->setMaxCount(maxCount);
    }
    ret = MAXCOUNT_CHANGED;
  }
  m_gate.signal();
  return ret;
}

int CMandelbrotDlg::setColorMapData(const ColorMapData &cdm) {
  m_gate.wait();
  int ret = 0;
  if(cdm != m_colorMap.getColorMapData()) {
    m_colorMap.setColorMapData(cdm);
    ret = COLORDATA_CHANGED;
  }
  m_gate.signal();
  return ret;
}

void CMandelbrotDlg::saveColorMap(const String &fileName) {
  showInformation(_T("%s not implemented"), __TFUNCTION__);
}

int CMandelbrotDlg::loadColorMap(const String &fileName) {
  showInformation(_T("%s not implemented"), __TFUNCTION__);
  return 0;
}

Packer &operator<<(Packer &p, const BigRealRectangle2D &r) {
  p << r.getMinX() << r.getMinY() << r.getWidth() << r.getHeight();
  return p;
}

Packer &operator>>(Packer &p, BigRealRectangle2D &r) {
  DigitPool *dp = r.getDigitPool();
  BigReal x(dp),y(dp),w(dp), h(dp);
  p >> x >> y >> w >> h;
  r = BigRealRectangle2D(x,y,w,h,dp);
  return p;
}

void CMandelbrotDlg::saveRectangle(const String &fileName) {
  Packer p;
  const BigRealRectangle2D r = m_bigRealTransform.getFromRectangle();
  p << r;
  p.write(ByteOutputFile(fileName));
}

int CMandelbrotDlg::loadRectangle(const String &fileName) {
  Packer p;
  p.read(ByteInputFile(fileName));
  BigRealRectangle2D r(getDigitPool());
  p >> r;
  return setScale(r);
}

void CMandelbrotDlg::setPrecision(int id) {
  if(isCalculationActive()) {
    return;
  }
  assert((id == ID_OPTIONS_AUTOPRECISION) || (id == ID_OPTIONS_FORCEFPU) || (id == ID_OPTIONS_FORCEBIGREAL));

  const bool oldFPUUsage = useFPUCalculators();
  checkMenuItem(this, ID_OPTIONS_AUTOPRECISION,false);
  checkMenuItem(this, ID_OPTIONS_FORCEFPU     ,false);
  checkMenuItem(this, ID_OPTIONS_FORCEBIGREAL ,false);
  checkMenuItem(this, id, true);
  m_precisionMode = id;
  if(useFPUCalculators() != oldFPUUsage) {
    startCalculation();
  }
}

int CMandelbrotDlg::setScale(const BigRealRectangle2D &scale, bool allowAdjustAspectRatio) {
  return setScale(scale.getMinX(),scale.getMaxX(),scale.getMinY(),scale.getMaxY(), allowAdjustAspectRatio);
}

int CMandelbrotDlg::setScale(const BigReal &minX, const BigReal &maxX, const BigReal &minY, const BigReal &maxY, bool allowAdjustAspectRatio) {
  int flags = 0;
  const BigRealRectangle2D old = m_bigRealTransform.getFromRectangle();
  m_bigRealTransform.setFromRectangle(BigRealRectangle2D(minX, maxY, maxX-minX, minY-maxY));
  if (m_bigRealTransform.getFromRectangle() != old) {
    flags |= SCALE_CHANGED;
  }
  flags |= handleTransformChanged(isRetainAspectRatio() && allowAdjustAspectRatio);
  return flags;
}

int CMandelbrotDlg::handleTransformChanged(bool adjustAspectRatio) {
  int flags = 0;
  if(adjustAspectRatio && m_bigRealTransform.adjustAspectRatio()) {
    flags |= SCALE_CHANGED;
  }
  setDigits();
  if(useFPUCalculators()) {
    const RealRectangleTransformation old = m_realTransform;
    m_realTransform.setToRectangle(  m_bigRealTransform.getToRectangle());
    m_realTransform.setFromRectangle(m_bigRealTransform.getFromRectangle());
    if(m_realTransform != old) {
      flags |= SCALE_CHANGED;
    }
  }
  return flags;
}

void CMandelbrotDlg::setDigits() {
  const BigRealRectangle2D r        = m_bigRealTransform.getFromRectangle();
  const BigReal            w        = r.getXInterval().getLength();
  const BigReal            h        = r.getYInterval().getLength();
  const size_t             xDigits  = 1 - BigReal::getExpo10(w) + 9;
  const size_t             yDigits  = 1 - BigReal::getExpo10(h) + 9;
  m_digits = max(xDigits, yDigits);
}

void CMandelbrotDlg::updateZoomFactor() {
  const BigRealRectangle2D r = m_bigRealTransform.getFromRectangle();
  if(isRetainAspectRatio()) {
    const double z = getDouble(m_zoom1Rect.getWidth())/getDouble(r.getWidth());
    m_zoomFactor = Size2D(z,z);
  } else {
    m_zoomFactor = Size2D(getDouble(m_zoom1Rect.getWidth()) /getDouble(r.getWidth())
                         ,getDouble(m_zoom1Rect.getHeight())/getDouble(r.getHeight()));
  }
}

CPoint CMandelbrotDlg::setDragRect(const CPoint &topLeft, const CPoint &bottomRight) {
  CClientDC dc(m_imageWindow);

  const CRect newDragRect = createDragRect(topLeft, bottomRight);
  const CSize size(1,1);

  if(getState() == STATE_DRAGGING) {
    dc.DrawDragRect(&newDragRect, size, &m_dragRect, size);
    m_dragRect = newDragRect;
  } else {
    m_dragRect = newDragRect;
    dc.DrawDragRect(&m_dragRect, size, &m_dragRect, size);
  }
  if(newDragRect.TopLeft() == topLeft) {
    return newDragRect.BottomRight();
  } else if(newDragRect.BottomRight() == topLeft) {
    return newDragRect.TopLeft();
  } else {
    const CPoint nTopRight(newDragRect.right, newDragRect.top), nBottomLeft(newDragRect.left,newDragRect.bottom);
    if(topLeft == nTopRight) {
      return nBottomLeft;
    } else if(topLeft == nBottomLeft) {
      return nTopRight;
    }
  }
  return newDragRect.BottomRight();
}

CRect CMandelbrotDlg::createDragRect(const CPoint &topLeft, const CPoint &bottomRight) {
  if(!isRetainAspectRatio()) {
    return makePositiveRect(CRect(topLeft,bottomRight));
  } else {
    const CSize size = getWorkSize();
    return makePositiveRect(CRect(topLeft.x, topLeft.y, bottomRight.x, topLeft.y + (bottomRight.x - topLeft.x) * size.cy / size.cx));
  }
}

void CMandelbrotDlg::removeDragRect() {
  CClientDC dc(m_imageWindow);
  const CSize size(1,1), newSize(0,0);
  dc.DrawDragRect(&m_dragRect,newSize,&m_dragRect,size);
}

void CMandelbrotDlg::paintMovedImage(const CSize &dp) {
  PixRect *tmp = NULL;
  try {
    tmp = newPixRect(m_imageCopy->getSize());
    copyVisiblePart(tmp, m_imageCopy, dp);
    CClientDC dc(m_imageWindow);
    pixRectToWindow(tmp, dc);
    deletePixRect(tmp);
  } catch(Exception e) {
    deletePixRect(tmp);
    showException(e);
  }
}

void CMandelbrotDlg::calculateMovedImage(const CSize &dp) {
  if(dp.cx == 0 && dp.cy == 0) {
    return;
  }
  const int  adx = abs(dp.cx);
  const int  ady = abs(dp.cy);
  const int  w   = m_ccMatrix->getWidth();
  const int  h   = m_ccMatrix->getHeight();
  CellCountMatrix *tmp = getCalculatedPart(dp);
  m_ccMatrix->rop(m_ccMatrix->getRect(), SRCCOPY, tmp, ORIGIN);
  deleteCCM(tmp);
  CompactArray<CRect> uncalculatedRectangles;
  switch(getMoveDirection(dp)) {
  case NW:
    uncalculatedRectangles.add(CRect(w-adx, 0    , w   , h-ady ));
    uncalculatedRectangles.add(CRect(0    , h-ady, w   , h     ));
    break;
  case SW:
    uncalculatedRectangles.add(CRect(0    , 0    , w   , ady   ));
    uncalculatedRectangles.add(CRect(w-adx, ady  , w   , h     ));
    break;
  case NE:
    uncalculatedRectangles.add(CRect(0   , 0     , adx , h-ady ));
    uncalculatedRectangles.add(CRect(0   , h-ady , w   , h     ));
    break;
  case SE:
    uncalculatedRectangles.add(CRect(0   , 0    , w    , ady   ));
    uncalculatedRectangles.add(CRect(0   , ady  , adx  , h     ));
    break;
  }
  setRectanglesToCalculate(uncalculatedRectangles);
  BigRealRectangle2D s(toBigRealRect(m_ccMatrix->getRect(), getDigitPool()));
  s -= toBigRealPoint(CPoint(dp), getDigitPool());
  setScale(m_bigRealTransform.backwardTransform(s),false);
  startCalculation();
}

CellCountMatrix *CMandelbrotDlg::getCalculatedPart(const CSize &dp) {
  CellCountMatrix *src    = m_ccMatrix;
  CellCountMatrix *result = newCCM(src->getSize(), src->getMaxCount());
  copyVisiblePart(result, src, dp);
  return result;
}

void CMandelbrotDlg::copyVisiblePart(PixRect *dst, const PixRect *src, const CSize &dp) {
  const int adx = abs(dp.cx);
  const int ady = abs(dp.cy);
  const int w   = dst->getWidth();
  const int h   = dst->getHeight();
  switch(getMoveDirection(dp)) {
  case NW: // point.x <  mousedownpoint.x && point.y <  mousedownpoint.y
    dst->rop(0  ,0  ,w,h,SRCCOPY,src,adx,ady);
    break;
  case SW: // point.x <  mousedownpoint.x && point.y >= mousedownpoint.y
    dst->rop(0  ,ady,w,h,SRCCOPY,src,adx,0  );
    break;
  case NE: // point.x >= mousedownpoint.x && point.y <  mousedownpoint.y
    dst->rop(adx,0  ,w,h,SRCCOPY,src,0  ,ady);
    break;
  case SE: // point.x >= mousedownpoint.x && point.y >= mousedownpoint.y
    dst->rop(adx,ady,w,h,SRCCOPY,src,0  ,0  );
    break;
  }
}

MoveDirection CMandelbrotDlg::getMoveDirection(const CSize &dp) { // static
  const int code = dp.cx >= 0 ? 1 : 0;
  return (MoveDirection)((code << 1) + ((dp.cy >= 0) ? 1 : 0));
}

void CMandelbrotDlg::pushImage() {
  if(!isCalculationActive()) {
    m_imageStack.push(ImageStackEntry(getScale(), m_ccMatrix->clone(), getColorMap().getColorMapData()));
    updateWindowStateInternal();
  }
}

void CMandelbrotDlg::popImage() {
  if(m_imageStack.isEmpty() || isCalculationActive()) {
    return;
  }
  ImageStackEntry e = m_imageStack.pop();
  int flags = setColorMapData(e.m_cmd);
  flags |= setMaxCount(e.m_ccm->getMaxCount());
  flags |= copyCCM(e.m_ccm);
  deleteCCM(e.m_ccm);
  setScale(e.m_scale, false); // don't set ScaleChanged bit
  updateZoomFactor();
  handleChangeCode(flags);
}

void CMandelbrotDlg::resetImageStack() {
  if(isCalculationActive()) {
    return;
  }
  while(!m_imageStack.isEmpty()) {
    ImageStackEntry image = m_imageStack.pop();
    deletePixRect(image.m_ccm);
  }
}

CellCountAccessor *CMandelbrotDlg::getCCA() {
  if(!hasCCA()) {
    m_cca = m_ccMatrix->getCCA();
    DLOG(_T("ccMatrix locked with CellCountAccessor\n"));
  }
  return (calculateWithOrbit() || animateCalculation()) ? m_mbContainer : m_cca;
}

void CMandelbrotDlg::clearCCA() {
  if(hasCCA()) {
    m_ccMatrix->releaseCCA();
    m_cca = NULL;
    DLOG(_T("ccMatrix unlocked\n"));
  }
}

COLORREF CMandelbrotDlg::setOrbitPixel(const CPoint &p, COLORREF color) {
  if(m_ccMatrix->contains(p.x,p.y)) {
    const COLORREF result = GetPixel(m_imageDC, p.x, p.y);
    SetPixel(m_imageDC, p.x,p.y, color);
    return result;
  }
  return 0;
}

void CMandelbrotDlg::resetOrbitPixels(const OrbitPoint *op, size_t count) {
  for(const OrbitPoint *p = op + count; p-- > op;) {
    SetPixel(m_imageDC, p->x,p->y, p->m_oldColor);
  }
}

void CMandelbrotDlg::paintMark(const CPoint &p) {
  if(!DrawIcon(m_imageDC, p.x-15,p.y-15, m_crossIcon)) {
    const String str = getLastErrorText();
  }
}

bool CMandelbrotDlg::useFPUCalculators() const {
  switch (m_precisionMode) {
  case ID_OPTIONS_AUTOPRECISION: return getDigits() < 18;
  case ID_OPTIONS_FORCEFPU     : return true;
  case ID_OPTIONS_FORCEBIGREAL : return false;
  default                      : return true;
  }
}

bool CMandelbrotDlg::getJobToDo(CRect &rect) {
  bool result = true;

  m_gate.wait();

  if(m_jobQueue.isEmpty()) {
    result = false;
  } else {
    rect = m_jobQueue.get();
  }

  m_gate.signal();

  return result;
}

// -----------------------------------------------------------------------------------------

void CMandelbrotDlg::handleChangeCode(int flags) {
  m_lastChangeFlags = flags;
  if(flags & WORKSIZE_CHANGED) {
    setWorkSize();
    startCalculation();
  } else if(flags & (MAXCOUNT_CHANGED | SCALE_CHANGED)) {
    startCalculation();
  } else if(flags & (COLORDATA_CHANGED | IMAGE_CHANGED)) {
    PostMessage(WM_PAINT);
  }
}

void CMandelbrotDlg::initScale() {
  setScale(m_rect0, isRetainAspectRatio());
}

void CMandelbrotDlg::startCalculation() {
  m_gate.wait();

  if(isCalculationActive(false)) {
    m_gate.signal();
    return;
  }

  if(m_jobQueue.isEmpty()) { // calculate the whole visible area
    setRectangleToCalculate(toCRect(m_bigRealTransform.getToRectangle()));
  }

  clearCCA();
  m_totalPixelsInJob = setUncalculatedRectsToEmpty();
  if(calculateWithOrbit() || animateCalculation()) {
    clearUncalculatedWindowArea();
  }
  updateZoomFactor();
  getCCA();

  startTimer(TIMER_CALCULATION, 500);
  m_calculatorPool->startCalculators(getCPUCountToUse());
  m_gate.signal();
}

size_t CMandelbrotDlg::setUncalculatedRectsToEmpty() {
  size_t pixelCount = 0;
  for(size_t i = 0; i < m_jobQueue.size(); i++) {
    const CRect &r = m_jobQueue[i];
    pixelCount += getArea(r);
    m_ccMatrix->clearRect(r);
  }
  return pixelCount;
}

void CMandelbrotDlg::clearUncalculatedWindowArea() {
  HBRUSH whiteBrush = CreateSolidBrush(WHITE);
  for(size_t i = 0; i < m_jobQueue.size(); i++) {
    FillRect(m_imageDC, &m_jobQueue[i], whiteBrush);
  }
}

void CMandelbrotDlg::setRectanglesToCalculate(const CompactArray<CRect> &rectangles) {
  m_gate.wait();
  const size_t n = rectangles.size();
  for(size_t i = 0; i < n; i++) {
    setRectangleToCalculate(rectangles[i], i == n-1);
  }
  m_gate.signal();
}

void CMandelbrotDlg::setRectangleToCalculate(const CRect &rectangle, bool splitLast) {
  if(rectangle.Width() <= 0 || rectangle.Height() <= 0) {
    return;
  }
  const UINT cpuCount = getCPUCountToUse();
  if(cpuCount == 1) splitLast = false;
  int height = (cpuCount == 1) ? rectangle.Height() : (rectangle.Height() / (7 * cpuCount));
  if(height == 0) {
    height = 1;
  }
  int lastBottom = rectangle.top;
  while(lastBottom < rectangle.bottom) {
    CRect rect;
    rect.left   = rectangle.left;
    rect.right  = rectangle.right;
    rect.top    = lastBottom;
    rect.bottom = rect.top + height;
    if((rect.bottom >= rectangle.bottom) && splitLast) {
      height    = 1;
      splitLast = false;
      continue;
    }
    if(rect.bottom > rectangle.bottom) {
      rect.bottom = rectangle.bottom;
    }
    if(rect.top < rect.bottom) {
      m_jobQueue.put(rect);
    }
    lastBottom = rect.bottom;
  }
}

UINT CMandelbrotDlg::getCPUCountToUse() const {
  return (calculateWithOrbit() || m_useOnly1CPU) ? 1 : CalculatorPool::getCPUCount();
}

void CMandelbrotDlg::OnNcLButtonDown(UINT nHitTest, CPoint point) {
  if(isBorderHit(nHitTest)) {
    if(!m_imageStack.isEmpty() || isCalculationActive()) {
      return;
    }
  }
  m_hasResized = false;
  __super::OnNcLButtonDown(nHitTest, point);
}

void CMandelbrotDlg::OnSizing(UINT fwSide, LPRECT pRect) {
  __super::OnSizing(fwSide, pRect);
  m_hasResized = true;
}

void CMandelbrotDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

void CMandelbrotDlg::OnNcMouseMove(UINT nHitTest, CPoint point) {
  __super::OnNcMouseMove(nHitTest, point);
  if(m_hasResized) {
    setWorkSize();
    if(hasCCM()) {
      startCalculation();
    }
  }
}

// ------------------------------------------ Make Movie Fuinctions --------------------------------

void CMandelbrotDlg::updateMovieMenuItem() {
  if(isMakingMovie()) {
    if(menuItemExists(this, ID_FILE_MAKEMOVIE)) {
      insertMenuItem(this, ID_FILE_MAKEMOVIE,0,_T("Stop &Movie...\tCtrl+O"), ID_FILE_STOPMOVIE);
      removeMenuItem(this, ID_FILE_MAKEMOVIE);
    }
  } else {
    if(menuItemExists(this, ID_FILE_STOPMOVIE)) {
      insertMenuItem(this, ID_FILE_STOPMOVIE,0,_T("Make &Movie...\tCtrl+M"), ID_FILE_MAKEMOVIE);
      removeMenuItem(this, ID_FILE_STOPMOVIE);
    }
  }
}

// --------------------------------------------------------------------------------------------------

void CMandelbrotDlg::startTimer(TimerId id, int msec) {
  if(!m_runningTimerSet.contains(id) && SetTimer(id,msec,NULL)) {
    m_runningTimerSet.add(id);
  }
}

void CMandelbrotDlg::stopTimer(TimerId id) {
  if(m_runningTimerSet.contains(id) ) {
    KillTimer(id);
    m_runningTimerSet.remove(id);
  }
}

void CMandelbrotDlg::OnTimer(UINT_PTR nIDEvent) {
  switch(nIDEvent) {
  case TIMER_CALCULATION:
    showCalculationState();
    break;
  case TIMER_MOVIEMAKER :
    showWindowState();
    break;
  }
  __super::OnTimer(nIDEvent);
}

DialogMBContainer::DialogMBContainer(CMandelbrotDlg *dlg, CellCountMatrix *m) : MBContainer(m), m_dlg(dlg) {
  m_colorMap = m_dlg->getColorMap().getBuffer();
}
const RealRectangleTransformation &DialogMBContainer::getRealTransformation() const {
  return m_dlg->getRealTransformation();
}
const BigRealRectangleTransformation &DialogMBContainer::getBigRealTransformation() const {
  return m_dlg->getBigRealTransformation();
}
UINT DialogMBContainer::getMaxCount() const {
  return m_dlg->getMaxCount();
}
size_t DialogMBContainer::getDigits() const {
  return m_dlg->getDigits();
}
bool DialogMBContainer::useFPUCalculators() const {
  return m_dlg->useFPUCalculators();
}
CellCountAccessor *DialogMBContainer::getCCA() {
  return m_dlg->getCCA();
}
bool DialogMBContainer::calculateWithOrbit() const {
  return m_dlg->calculateWithOrbit();
}
bool DialogMBContainer::useEdgeDetection() const {
  return m_dlg->useEdgeDetection();
}
bool DialogMBContainer::getJobToDo(CRect &rect) {
  return m_dlg->getJobToDo(rect);
}
CSize DialogMBContainer::getWindowSize() const {
  return m_dlg->getWindowSize();
}
void DialogMBContainer::paintMark(const CPoint &p) {
  m_dlg->paintMark(p);
}
COLORREF DialogMBContainer::setOrbitPixel(const CPoint &p, COLORREF color) {
  return m_dlg->setOrbitPixel(p, color);
}
void DialogMBContainer::resetOrbitPixels(const OrbitPoint *op, size_t count) {
  m_dlg->resetOrbitPixels(op, count);
}
// overrides DWordpixelAccessor::setPixel. color actually count
void DialogMBContainer::setPixel(UINT x, UINT y, D3DCOLOR color) {
  m_dlg->setCount(x,y, color);
  SetPixel(m_dlg->getImageDC(), x,y,m_colorMap[color].getColorRef());
}
// overrides DWordPixelAccessor::getPixel. return count
D3DCOLOR DialogMBContainer::getPixel(UINT x, UINT y) const {
  return m_dlg->getCount(x,y);
}
void DialogMBContainer::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  m_dlg->handlePropertyChanged(source, id, oldValue, newValue);
}
