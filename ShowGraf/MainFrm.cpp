#include "stdafx.h"
#include "ShowGraf.h"
#include "MainFrm.h"
#include "IntervalDlg.h"
#include "RollSizeDlg.h"
#include "ExprGraphDlg.h"
#include "IsoCurveGraphDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
  ON_WM_CREATE()
  ON_COMMAND(ID_FILE_NEW                       , OnFileNew                     )
  ON_COMMAND(ID_FILE_OPEN                      , OnFileOpen                    )
  ON_COMMAND(ID_FILE_MRU_FILE1                 , OnFileMruFile1                )
  ON_COMMAND(ID_FILE_MRU_FILE2                 , OnFileMruFile2                )
  ON_COMMAND(ID_FILE_MRU_FILE3                 , OnFileMruFile3                )
  ON_COMMAND(ID_FILE_MRU_FILE4                 , OnFileMruFile4                )
  ON_COMMAND(ID_FILE_MRU_FILE5                 , OnFileMruFile5                )
  ON_COMMAND(ID_FILE_MRU_FILE6                 , OnFileMruFile6                )
  ON_COMMAND(ID_FILE_MRU_FILE7                 , OnFileMruFile7                )
  ON_COMMAND(ID_FILE_MRU_FILE8                 , OnFileMruFile8                )
  ON_COMMAND(ID_FILE_MRU_FILE9                 , OnFileMruFile9                )
  ON_COMMAND(ID_FILE_MRU_FILE10                , OnFileMruFile10               )
  ON_COMMAND(ID_FILE_MRU_FILE11                , OnFileMruFile11               )
  ON_COMMAND(ID_FILE_MRU_FILE12                , OnFileMruFile12               )
  ON_COMMAND(ID_FILE_MRU_FILE13                , OnFileMruFile13               )
  ON_COMMAND(ID_FILE_MRU_FILE14                , OnFileMruFile14               )
  ON_COMMAND(ID_FILE_MRU_FILE15                , OnFileMruFile15               )
  ON_COMMAND(ID_FILE_MRU_FILE16                , OnFileMruFile16               )
  ON_COMMAND(ID_VIEW_GRID                      , OnViewGrid                    )
  ON_COMMAND(ID_VIEW_SCALE_X_LINEAR            , OnViewScaleXLinear            )
  ON_COMMAND(ID_VIEW_SCALE_X_LOGARITHMIC       , OnViewScaleXLogarithmic       )
  ON_COMMAND(ID_VIEW_SCALE_X_NORMALDISTRIBUTION, OnViewScaleXNormaldistribution)
  ON_COMMAND(ID_VIEW_SCALE_Y_LINEAR            , OnViewScaleYLinear            )
  ON_COMMAND(ID_VIEW_SCALE_Y_LOGARITHMIC       , OnViewScaleYLogarithmic       )
  ON_COMMAND(ID_VIEW_SCALE_Y_NORMALDISTRIBUTION, OnViewScaleYNormaldistribution)
  ON_COMMAND(ID_VIEW_SCALE_Y_DATETIME          , OnViewScaleYDatetime          )
  ON_COMMAND(ID_VIEW_SCALE_X_DATETIME          , OnViewScaleXDatetime          )
  ON_COMMAND(ID_VIEW_SCALE_RESETSCALE          , OnViewScaleResetScale         )
  ON_COMMAND(ID_VIEW_STYLE_CURVE               , OnViewStyleCurve              )
  ON_COMMAND(ID_VIEW_STYLE_POINT               , OnViewStylePoint              )
  ON_COMMAND(ID_VIEW_STYLE_CROSS               , OnViewStyleCross              )
  ON_COMMAND(ID_VIEW_INTERVAL                  , OnViewInterval                )
  ON_COMMAND(ID_VIEW_RETAINASPECTRATIO         , OnViewRetainAspectRatio       )
  ON_COMMAND(ID_VIEW_ROLLAVERAGE               , OnViewRollAverage             )
  ON_COMMAND(ID_VIEW_SETROLLSIZE               , OnViewSetRollSize             )
  ON_COMMAND(ID_VIEW_REFRESHFILES              , OnViewRefreshFiles            )
  ON_COMMAND(ID_TOOLS_FIT_POLYNOMIAL           , OnToolsFitPolynomial          )
  ON_COMMAND(ID_TOOLS_FIT_EXPO_FUNCTION        , OnToolsFitExpoFunction        )
  ON_COMMAND(ID_TOOLS_FIT_POTENS_FUNCTION      , OnToolsFitPotensFunction      )
  ON_COMMAND(ID_TOOLS_FIT_CUSTOM_FUNCTION      , OnToolsFitCustomFunction      )
  ON_COMMAND(ID_TOOLS_PLOT                     , OnToolsPlot                   )
  ON_COMMAND(ID_TOOLS_IMPLICITDEFINEDCURVE     , OnToolsImplicitDefinedCurve   )
  ON_COMMAND(ID_OPTIONS_IGNOREERRORS           , OnOptionsIgnoreErrors         )
  ON_COMMAND(ID_OPTIONS_RADIANS                , OnOptionsRadians              )
  ON_COMMAND(ID_OPTIONS_DEGREES                , OnOptionsDegrees              )
  ON_COMMAND(ID_OPTIONS_GRADS                  , OnOptionsGrads                )
END_MESSAGE_MAP()

static UINT indicators[] = {
  ID_SEPARATOR,           // status line indicator
  ID_INDICATOR_POSITION
};

CMainFrame::CMainFrame() {
}

CMainFrame::~CMainFrame() {
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(CFrameWnd::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }

  if(!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(IDR_DUMMYTOOLBAR))
  {
    TRACE0("Failed to create toolbar\n");
    return -1;      // fail to create
  }

  HBITMAP bitmap = ::LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
  m_wndToolBar.SetBitmap(bitmap);
//  m_wndToolBar.SetSizes(CSize(32, 32), getBitmapSize(bitmap));

  if(!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, ARRAYSIZE(indicators))) {
    TRACE0("Failed to create status bar\n");
    return -1;      // fail to create
  }

  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
  EnableDocking(CBRS_ALIGN_ANY);
  DockControlBar(&m_wndToolBar);
  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
  m_bAutoMenuEnable = FALSE;
  initMenuItems();
  theApp.m_device.attach(*this);

  return 0;
}

typedef struct {
  int m_command;
  int m_bitmapId;
} BitmapMenuItem;

void CMainFrame::initMenuItems() {
  static const BitmapMenuItem itemArray[] = {
    ID_TOOLS_FIT_POLYNOMIAL     , IDB_BITMAP_POLYNOMIAL
   ,ID_TOOLS_FIT_EXPO_FUNCTION  , IDB_BITMAP_EXPO_FUNCTION
   ,ID_TOOLS_FIT_POTENS_FUNCTION, IDB_BITMAP_POTENS_FUNCTION
   ,ID_TOOLS_FIT_CUSTOM_FUNCTION, IDB_BITMAP_CUSTOM_FUNCTION
  };
  static CBitmap bitmap[ARRAYSIZE(itemArray)];
  for(int i = 0; i < ARRAYSIZE(itemArray); i++) {
    const BitmapMenuItem &item = itemArray[i];
    bitmap[i].LoadBitmap(item.m_bitmapId);
    GetMenu()->SetMenuItemBitmaps(item.m_command, MF_BYCOMMAND, &bitmap[i], &bitmap[i]);
  }
/*    int index;

    HMENU menu = findMenuContainingId(*GetMenu(), item.m_command, index);
    menu
    removeMenuItem(this, item.m_command);
    InsertMenu(menu, index, MF_BYPOSITION | MF_BITMAP, item.m_command, (char*)(HBITMAP)bitmap[i].m_hObject);
  }
*/
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
  if(!CFrameWnd::PreCreateWindow(cs)) {
    return FALSE;
  }
  return TRUE;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::showPositon(const CPoint &p) {
  m_wndStatusBar.SetPaneText(1,format(_T("%3d,%3d"),p.x,p.y).cstr());
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const {
  CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const {
  CFrameWnd::Dump(dc);
}
#endif //_DEBUG


void CMainFrame::OnFileNew() {
  getView()->clear();
}

void CMainFrame::OnFileOpen() {
  static const TCHAR *FileDialogExtensions = _T("Text files (*.txt, *.dat)\0*.txt;*.dat\0"
                                                "Expression-files (*.exp)\0*.exp\0"
                                                "All files (*.*)\0*.*\0\0");
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = FileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open file");

  if(dlg.DoModal() != IDOK || _tcslen(dlg.m_ofn.lpstrFile) == 0) {
    return;
  }
  getView()->addGraphFromFile(dlg.m_ofn.lpstrFile);
  AfxGetApp()->AddToRecentFileList(dlg.m_ofn.lpstrFile);

  Invalidate();
}

void CMainFrame::onFileMruFile(int index) {
  String fname = ((CShowGrafApp*)AfxGetApp())->getRecentFile(index);
  getView()->addGraphFromFile(fname);
  Invalidate();
}

void CMainFrame::OnFileMruFile1()  {  onFileMruFile( 0); }
void CMainFrame::OnFileMruFile2()  {  onFileMruFile( 1); }
void CMainFrame::OnFileMruFile3()  {  onFileMruFile( 2); }
void CMainFrame::OnFileMruFile4()  {  onFileMruFile( 3); }
void CMainFrame::OnFileMruFile5()  {  onFileMruFile( 4); }
void CMainFrame::OnFileMruFile6()  {  onFileMruFile( 5); }
void CMainFrame::OnFileMruFile7()  {  onFileMruFile( 6); }
void CMainFrame::OnFileMruFile8()  {  onFileMruFile( 7); }
void CMainFrame::OnFileMruFile9()  {  onFileMruFile( 8); }
void CMainFrame::OnFileMruFile10() {  onFileMruFile( 9); }
void CMainFrame::OnFileMruFile11() {  onFileMruFile(10); }
void CMainFrame::OnFileMruFile12() {  onFileMruFile(11); }
void CMainFrame::OnFileMruFile13() {  onFileMruFile(12); }
void CMainFrame::OnFileMruFile14() {  onFileMruFile(13); }
void CMainFrame::OnFileMruFile15() {  onFileMruFile(14); }
void CMainFrame::OnFileMruFile16() {  onFileMruFile(15); }

void CMainFrame::setGrid() {
  OnViewGrid();
}

void CMainFrame::OnViewGrid() {
  toggleMenuItem(this,ID_VIEW_GRID);
  Invalidate(FALSE);
}

void CMainFrame::OnViewScaleXLinear() {
  getView()->setXAxisType(AXIS_LINEAR);
  Invalidate();
}

void CMainFrame::OnViewScaleXLogarithmic() {
  getView()->setXAxisType(AXIS_LOGARITHMIC);
  Invalidate();
}

void CMainFrame::OnViewScaleXNormaldistribution() {
  getView()->setXAxisType(AXIS_NORMAL_DISTRIBUTION);
  Invalidate();
}

void CMainFrame::OnViewScaleXDatetime() {
  getView()->setXAxisType(AXIS_DATE);
  Invalidate();
}

void CMainFrame::OnViewScaleYLinear() {
  getView()->setYAxisType(AXIS_LINEAR);
  Invalidate();
}

void CMainFrame::OnViewScaleYLogarithmic() {
  getView()->setYAxisType(AXIS_LOGARITHMIC);
  Invalidate();
}

void CMainFrame::OnViewScaleYNormaldistribution() {
  getView()->setYAxisType(AXIS_NORMAL_DISTRIBUTION);
  Invalidate();
}

void CMainFrame::OnViewScaleYDatetime() {
  getView()->setYAxisType(AXIS_DATE);
  Invalidate();
}

void CMainFrame::OnViewScaleResetScale() {
  getView()->initScale();
  Invalidate();
}

void CMainFrame::OnViewStyleCurve() {
  getView()->setGraphStyle(GSCURVE);
  Invalidate(FALSE);
}

void CMainFrame::OnViewStylePoint() {
  getView()->setGraphStyle(GSPOINT);  	
  Invalidate(FALSE);
}

void CMainFrame::OnViewStyleCross() {
  getView()->setGraphStyle(GSCROSS);
  Invalidate(FALSE);
}

void CMainFrame::OnViewInterval() {
  CCoordinateSystem &cs = getView()->getCoordinateSystem();

  CIntervalDlg dlg(cs.getTransformation());
  if(dlg.DoModal() == IDOK) {
    cs.setDataRange(dlg.getDataRange(), false);
    Invalidate();
  }
}

void CMainFrame::OnViewRetainAspectRatio() {
  getView()->setRetainAspectRatio(toggleMenuItem(this, ID_VIEW_RETAINASPECTRATIO));
  Invalidate();
}

void CMainFrame::setRollAverage() {
  OnViewRollAverage();
}

void CMainFrame::OnViewRollAverage() {
  bool checked = toggleMenuItem(this,ID_VIEW_ROLLAVERAGE);
  getView()->setRollingAverage(checked);
  Invalidate(FALSE);
}

void CMainFrame::OnViewSetRollSize() {
  RollsizeDlg dlg(getView()->getRollSize());
  if(dlg.DoModal() == IDOK) {
    getView()->setRollSize(dlg.m_rollSize);
    checkMenuItem(this,ID_VIEW_ROLLAVERAGE,true);
    Invalidate(FALSE);
  }
}

void CMainFrame::OnViewRefreshFiles() {
}

void CMainFrame::OnToolsFitPolynomial() {
  getView()->startPolynomialFitThread();
}

void CMainFrame::OnToolsFitExpoFunction() {
  getView()->makeExpoFit();

}

void CMainFrame::OnToolsFitPotensFunction() {
  getView()->makePotensFit();
}

void CMainFrame::OnToolsFitCustomFunction() {
  getView()->startCustomFitThread();
}

void CMainFrame::OnToolsPlot() {
  try {
    ExpressionGraphParameters &param = getView()->getPlotParam();
    param.m_color             = randomColor();
    param.m_trigonometricMode = getTrigonometricMode();
    param.m_interval          = getView()->getCoordinateSystem().getDataRange().getXInterval();
    CExprGraphDlg dlg(param);
    if(dlg.DoModal() == IDOK) {
      Expression expr(param.m_trigonometricMode);
      expr.compile(param.m_expr, true);
      if(!expr.isOk()) {
        MessageBox(expr.getErrors()[0].cstr(),_T("Error"),MB_ICONEXCLAMATION);
        return;
      }
      getView()->addExpressionGraph(param);
    }
  } catch(Exception e) {
    showException(e);
  }
}

void CMainFrame::OnToolsImplicitDefinedCurve() {
  try {
    IsoCurveGraphParameters &param = getView()->getIsoCurveParam();
    param.m_color             = randomColor();
    param.m_trigonometricMode = getTrigonometricMode();
    const DoubleInterval xInterval = getView()->getCoordinateSystem().getDataRange().getXInterval();
    const DoubleInterval yInterval = getView()->getCoordinateSystem().getDataRange().getYInterval();
    CIsoCurveGraphDlg dlg(param);
    if(dlg.DoModal() == IDOK) {
      Expression expr(param.m_trigonometricMode);
      expr.compile(param.m_expr, true);
      if(!expr.isOk()) {
        MessageBox(expr.getErrors()[0].cstr(), _T("Error"), MB_ICONEXCLAMATION);
        return;
      }
      getView()->addIsoCurveGraph(param);
    }
  } catch(Exception e) {
    showException(e);
  }

  
}

void CMainFrame::OnOptionsIgnoreErrors() {
  toggleMenuItem(this,ID_OPTIONS_IGNOREERRORS);
  Invalidate(FALSE);
}

TrigonometricMode CMainFrame::getTrigonometricMode() {
  if(isMenuItemChecked(this, ID_OPTIONS_RADIANS)) {
    return RADIANS;
  } else if(isMenuItemChecked(this, ID_OPTIONS_DEGREES)) {
    return DEGREES;
  } else if(isMenuItemChecked(this, ID_OPTIONS_GRADS  )) {
    return GRADS;
  } else {
    return RADIANS;
  }
}

void CMainFrame::setTrigonometricMode(TrigonometricMode mode) {
  bool changed = getTrigonometricMode() != mode;

  checkMenuItem(this, ID_OPTIONS_RADIANS, false);
  checkMenuItem(this, ID_OPTIONS_DEGREES, false);
  checkMenuItem(this, ID_OPTIONS_GRADS  , false);
  switch(mode) {
  case RADIANS: checkMenuItem(this, ID_OPTIONS_RADIANS, true); break;
  case DEGREES: checkMenuItem(this, ID_OPTIONS_DEGREES, true); break;
  case GRADS  : checkMenuItem(this, ID_OPTIONS_GRADS  , true); break;
  }
  if(changed) {
    getView()->setTrigonometricMode(mode);
    Invalidate(FALSE);
  }
}

void CMainFrame::OnOptionsRadians() {  setTrigonometricMode(RADIANS); }
void CMainFrame::OnOptionsDegrees() {  setTrigonometricMode(DEGREES); }
void CMainFrame::OnOptionsGrads()   {  setTrigonometricMode(GRADS  ); }
