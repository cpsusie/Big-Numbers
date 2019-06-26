#include "stdafx.h"
#include "MainFrm.h"
#include "DataGraphDlg.h"
#include "FunctionGraphDlg.h"
#include "ParametricGraphDlg.h"
#include "IsoCurveGraphDlg.h"
#include "DiffEquationGraphDlg.h"
#include "IntervalDlg.h"
#include "RollAvgSizeDlg.h"
#include "MouseTool.h"

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
  ON_COMMAND(ID_BOTH_SCALE_LINEAR              , OnBothScaleLinear             )
  ON_COMMAND(ID_BOTH_SCALE_LOGARITHMIC         , OnBothScaleLogarithmic        )
  ON_COMMAND(ID_BOTH_SCALE_NORMALDIST          , OnBothScaleNormaldistribution )
  ON_COMMAND(ID_BOTH_SCALE_DATETIME            , OnBothScaleDatetime           )
  ON_COMMAND(ID_BOTH_SHOWVALUES                , OnBothShowValues              )
  ON_COMMAND(ID_BOTH_SHOWVALUEMARKS            , OnBothShowValueMarks          )
  ON_COMMAND(ID_BOTH_SHOWGRIDLINES             , OnBothShowGridLines           )
  ON_COMMAND(ID_X_SCALE_LINEAR                 , OnXScaleLinear                )
  ON_COMMAND(ID_X_SCALE_LOGARITHMIC            , OnXScaleLogarithmic           )
  ON_COMMAND(ID_X_SCALE_NORMALDIST             , OnXScaleNormaldistribution    )
  ON_COMMAND(ID_X_SCALE_DATETIME               , OnXScaleDatetime              )
  ON_COMMAND(ID_X_SHOWVALUES                   , OnXShowValues                 )
  ON_COMMAND(ID_X_SHOWVALUEMARKS               , OnXShowValueMarks             )
  ON_COMMAND(ID_X_SHOWGRIDLINES                , OnXShowGridLines              )
  ON_COMMAND(ID_Y_SCALE_LINEAR                 , OnYScaleLinear                )
  ON_COMMAND(ID_Y_SCALE_LOGARITHMIC            , OnYScaleLogarithmic           )
  ON_COMMAND(ID_Y_SCALE_NORMALDIST             , OnYScaleNormaldistribution    )
  ON_COMMAND(ID_Y_SCALE_DATETIME               , OnYScaleDatetime              )
  ON_COMMAND(ID_Y_SHOWVALUES                   , OnYShowValues                 )
  ON_COMMAND(ID_Y_SHOWVALUEMARKS               , OnYShowValueMarks             )
  ON_COMMAND(ID_Y_SHOWGRIDLINES                , OnYShowGridLines              )
  ON_COMMAND(ID_VIEW_SCALE_RESETSCALE          , OnViewScaleResetScale         )
  ON_COMMAND(ID_VIEW_STYLE_CURVE               , OnViewStyleCurve              )
  ON_COMMAND(ID_VIEW_STYLE_POINT               , OnViewStylePoint              )
  ON_COMMAND(ID_VIEW_STYLE_CROSS               , OnViewStyleCross              )
  ON_COMMAND(ID_VIEW_INTERVAL                  , OnViewInterval                )
  ON_COMMAND(ID_VIEW_RETAINASPECTRATIO         , OnViewRetainAspectRatio       )
  ON_COMMAND(ID_VIEW_ROLLAVG                   , OnViewRollAvg                 )
  ON_COMMAND(ID_VIEW_SETROLLAVGSIZE            , OnViewSetRollAvgSize          )
  ON_COMMAND(ID_VIEW_REFRESHFILES              , OnViewRefreshFiles            )
  ON_COMMAND(ID_TOOLS_FIT_POLYNOMIAL           , OnToolsFitPolynomial          )
  ON_COMMAND(ID_TOOLS_FIT_EXPO_FUNCTION        , OnToolsFitExpoFunction        )
  ON_COMMAND(ID_TOOLS_FIT_POTENS_FUNCTION      , OnToolsFitPotensFunction      )
  ON_COMMAND(ID_TOOLS_FIT_CUSTOM_FUNCTION      , OnToolsFitCustomFunction      )
  ON_COMMAND(ID_TOOLS_PLOTFUNCTION             , OnToolsPlotFunction           )
  ON_COMMAND(ID_TOOLS_PARAMETRICCURVE          , OnToolsParametricCurve        )
  ON_COMMAND(ID_TOOLS_IMPLICITDEFINEDCURVE     , OnToolsImplicitDefinedCurve   )
  ON_COMMAND(ID_TOOLS_DIFFERENTIALEQUATIONS    , OnToolsDifferentialEquations  )
  ON_COMMAND(ID_FIND_ZEROES                    , OnFindZeroes                  )
  ON_COMMAND(ID_FIND_MINIMUM                   , OnFindMinimum                 )
  ON_COMMAND(ID_FIND_MAXIMUM                   , OnFindMaximum                 )
  ON_COMMAND(ID_FIND_INTERSECTION              , OnFindIntersection            )
  ON_COMMAND(ID_OPTIONS_IGNOREERRORS           , OnOptionsIgnoreErrors         )
  ON_COMMAND(ID_OPTIONS_RADIANS                , OnOptionsRadians              )
  ON_COMMAND(ID_OPTIONS_DEGREES                , OnOptionsDegrees              )
  ON_COMMAND(ID_OPTIONS_GRADS                  , OnOptionsGrads                )
  ON_COMMAND(ID_SELECTMENU_DELETE              , OnSelectMenuDelete            )
  ON_COMMAND(ID_SELECTMENU_EDIT                , OnSelectMenuEdit              )
  ON_COMMAND(ID_SELECTMENU_HIDE                , OnSelectMenuHide              )
  ON_COMMAND(ID_SELECTMENU_SHOW                , OnSelectMenuShow              )
  ON_COMMAND(ID_SELECTMENU_STYLE_CURVE         , OnSelectMenuStyleCurve        )
  ON_COMMAND(ID_SELECTMENU_STYLE_POINT         , OnSelectMenuStylePoint        )
  ON_COMMAND(ID_SELECTMENU_STYLE_CROSS         , OnSelectMenuStyleCross        )
  ON_MESSAGE(ID_MSG_SEARCHZEROESININTERVAL     , OnMsgSearchZeroesInInterval   )
  ON_MESSAGE(ID_MSG_SEARCHMAXININTERVAL        , OnMsgSearchMaxInInterval      )
  ON_MESSAGE(ID_MSG_SEARCHMINININTERVAL        , OnMsgSearchMinInInterval      )
END_MESSAGE_MAP()

static UINT indicators[] = {
  ID_SEPARATOR,           // status line indicator
  ID_INDICATOR_POSITION
};

CMainFrame::CMainFrame() {
  m_fitThread = NULL;
  m_flushLevel = 0;
}

CMainFrame::~CMainFrame() {
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(__super::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }

  if(!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(IDR_DUMMYTOOLBAR))
  {
    TRACE0("Failed to create toolbar\n");
    return -1;      // fail to create
  }

  HBITMAP bitmap = ::LoadBitmap(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
  m_wndToolBar.SetBitmap(bitmap);
//  m_wndToolBar.SetSizes(CSize(32, 32), getBitmapSize(bitmap));

  if(!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, ARRAYSIZE(indicators))) {
    TRACE0("Failed to create status bar\n");
    return -1;      // fail to create
  }

  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
  EnableDocking(CBRS_ALIGN_ANY);
  DockControlBar(&m_wndToolBar);
  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
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

BOOL CMainFrame::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CMainFrame::updatePositionText(const String &str) {
  m_wndStatusBar.SetPaneText(1,format(_T("(%s)"), str.cstr()).cstr());
}

void CMainFrame::activateInitialOptions() {
  const InitialOptions &options = getDoc()->getOptions();
  pushLevel();
  getView()->setBackgroundColor(options.m_backgroundColor);
  setGraphStyle(  options.m_graphStyle    );
  FOR_BOTH_AXIS(i) {
    setAxisType(            i, options.m_axisOptions[i].m_type );
    getView()->setAxisColor(i, options.m_axisOptions[i].m_color);
  }
  setShowValues(    options.m_axisOptions[XAXIS_INDEX].m_showValues    , options.m_axisOptions[YAXIS_INDEX].m_showValues    );
  setShowValueMarks(options.m_axisOptions[XAXIS_INDEX].m_showValueMarks, options.m_axisOptions[YAXIS_INDEX].m_showValueMarks);
  setShowGridLines( options.m_axisOptions[XAXIS_INDEX].m_showGridLines , options.m_axisOptions[YAXIS_INDEX].m_showGridLines );
  setTrigoMode(     options.m_trigoMode     );
  setIgnoreErrors(  options.m_ignoreErrors  );
  setRollAvg(       options.m_rollAvg       );
  getView()->initScale();
  popLevel();
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const {
  __super::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
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
  pushLevel();
  getDoc()->addGraphFromFile(dlg.m_ofn.lpstrFile);
  theApp.AddToRecentFileList(dlg.m_ofn.lpstrFile);
  popLevel();
}

void CMainFrame::onFileMruFile(int index) {
  pushLevel();
  String fname = theApp.getRecentFile(index);
  getDoc()->addGraphFromFile(fname);
  popLevel();
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

void CMainFrame::OnBothScaleLinear() {
  pushLevel();
  FOR_BOTH_AXIS(i) {
    setAxisType(i,AXIS_LINEAR);
  }
  popLevel();
}

void CMainFrame::OnBothScaleLogarithmic() {
  pushLevel();
  FOR_BOTH_AXIS(i) {
    setAxisType(i,AXIS_LOGARITHMIC);
  }
  popLevel();
}

void CMainFrame::OnBothScaleNormaldistribution() {
  pushLevel();
  FOR_BOTH_AXIS(i) {
    setAxisType(i,AXIS_NORMAL_DISTRIBUTION);
  }
  popLevel();
}

void CMainFrame::OnBothScaleDatetime() {
  pushLevel();
  FOR_BOTH_AXIS(i) {
    setAxisType(i,AXIS_DATE);
  }
  popLevel();
}

void CMainFrame::OnBothShowValues() {
  pushLevel();
  const bool v = toggleMenuItem(this, ID_BOTH_SHOWVALUES);
  setShowValues(v,v);
  popLevel();
}
void CMainFrame::OnBothShowValueMarks() {
  pushLevel();
  const bool v = toggleMenuItem(this, ID_BOTH_SHOWVALUEMARKS);
  setShowValueMarks(v,v);
  popLevel();
}
void CMainFrame::OnBothShowGridLines() {
  pushLevel();
  const bool v = toggleMenuItem(this, ID_BOTH_SHOWGRIDLINES);
  setShowGridLines(v, v);
  popLevel();
}

void CMainFrame::OnXScaleLinear() {
  pushLevel();
  setAxisType(XAXIS_INDEX, AXIS_LINEAR);
  popLevel();
}

void CMainFrame::OnXScaleLogarithmic() {
  pushLevel();
  setAxisType(XAXIS_INDEX, AXIS_LOGARITHMIC);
  popLevel();
}

void CMainFrame::OnXScaleNormaldistribution() {
  pushLevel();
  setAxisType(XAXIS_INDEX, AXIS_NORMAL_DISTRIBUTION);
  popLevel();
}

void CMainFrame::OnXScaleDatetime() {
  pushLevel();
  setAxisType(XAXIS_INDEX, AXIS_DATE);
  popLevel();
}

void CMainFrame::OnXShowValues() {
  pushLevel();
  setShowValues(!getShowValues(XAXIS_INDEX), getShowValues(YAXIS_INDEX));
  popLevel();
}
void CMainFrame::OnXShowValueMarks() {
  pushLevel();
  setShowValueMarks(!getShowValueMarks(XAXIS_INDEX), getShowValueMarks(YAXIS_INDEX));
  popLevel();
}
void CMainFrame::OnXShowGridLines() {
  pushLevel();
  setShowGridLines(!getShowGridLines(XAXIS_INDEX), getShowGridLines(YAXIS_INDEX));
  popLevel();
}

void CMainFrame::OnYScaleLinear() {
  pushLevel();
  setAxisType(YAXIS_INDEX, AXIS_LINEAR);
  popLevel();
}

void CMainFrame::OnYScaleLogarithmic() {
  pushLevel();
  setAxisType(YAXIS_INDEX, AXIS_LOGARITHMIC);
  popLevel();
}

void CMainFrame::OnYScaleNormaldistribution() {
  pushLevel();
  setAxisType(YAXIS_INDEX, AXIS_NORMAL_DISTRIBUTION);
  popLevel();
}

void CMainFrame::OnYScaleDatetime() {
  pushLevel();
  setAxisType(YAXIS_INDEX, AXIS_DATE);
  popLevel();
}

void CMainFrame::OnYShowValues() {
  pushLevel();
  setShowValues(getShowValues(XAXIS_INDEX), !getShowValues(YAXIS_INDEX));
  popLevel();
}
void CMainFrame::OnYShowValueMarks() {
  pushLevel();
  setShowValueMarks(getShowValueMarks(XAXIS_INDEX), !getShowValueMarks(YAXIS_INDEX));
  popLevel();
}
void CMainFrame::OnYShowGridLines() {
  pushLevel();
  setShowGridLines(getShowGridLines(XAXIS_INDEX), !getShowGridLines(YAXIS_INDEX));
  popLevel();
}

void CMainFrame::setShowValues(bool xAxis, bool yAxis) {
  pushLevel();
  checkMenuItem(this, ID_X_SHOWVALUES       , xAxis       );
  checkMenuItem(this, ID_Y_SHOWVALUES       , yAxis       );
  checkMenuItem(this, ID_BOTH_SHOWVALUES    , xAxis&&yAxis);
  popLevel();
}
bool CMainFrame::getShowValues(AxisIndex axis) const {
  return isMenuItemChecked(this, (axis==XAXIS_INDEX) ? ID_X_SHOWVALUES : ID_Y_SHOWVALUES);
}

void CMainFrame::setShowValueMarks(bool xAxis, bool yAxis) {
  pushLevel();
  checkMenuItem(this, ID_X_SHOWVALUEMARKS   , xAxis       );
  checkMenuItem(this, ID_Y_SHOWVALUEMARKS   , yAxis       );
  checkMenuItem(this, ID_BOTH_SHOWVALUEMARKS, xAxis&&yAxis);
  popLevel();
}

bool CMainFrame::getShowValueMarks(AxisIndex axis) const {
  return isMenuItemChecked(this, (axis==XAXIS_INDEX) ? ID_X_SHOWVALUEMARKS : ID_Y_SHOWVALUEMARKS);
}

void CMainFrame::setShowGridLines( bool xAxis, bool yAxis) {
  pushLevel();
  checkMenuItem(this, ID_X_SHOWGRIDLINES    , xAxis       );
  checkMenuItem(this, ID_Y_SHOWGRIDLINES    , yAxis       );
  checkMenuItem(this, ID_BOTH_SHOWGRIDLINES , xAxis&&yAxis);
  popLevel();
}

bool CMainFrame::getShowGridLines(AxisIndex axis) const {
  return isMenuItemChecked(this, (axis==XAXIS_INDEX) ? ID_X_SHOWGRIDLINES : ID_Y_SHOWGRIDLINES);
}

template<class T> class RadioMenuItem {
public:
  const int m_menuId;
  const T   m_value;
};

template<class T> void uncheckAllRadioMenuItem(CWnd *wnd, const RadioMenuItem<T> *items, int n) {
  for(int i = 0; i < n; i++) {
    const RadioMenuItem<T> &item = items[i];
    checkMenuItem(wnd, item.m_menuId, false);
  }
}

template<class T> void checkRadioMenuItem(CWnd *wnd, T v, const RadioMenuItem<T> *items, int n) {
  for(int i = 0; i < n; i++) {
    const RadioMenuItem<T> &item = items[i];
    checkMenuItem(wnd, item.m_menuId, item.m_value == v);
  }
}

template<class T> T getRadioItemValue(const CWnd *wnd, const RadioMenuItem<T> *items, int n) {
  for(int i = 0; i < n; i++) {
    const RadioMenuItem<T> &item = items[i];
    if(isMenuItemChecked(wnd, item.m_menuId)) {
      return item.m_value;
    }
  }
  return items[0].m_value;
}

static const RadioMenuItem<AxisType> XAxisTypeItems[] = {
  ID_X_SCALE_LINEAR      , AXIS_LINEAR
 ,ID_X_SCALE_LOGARITHMIC , AXIS_LOGARITHMIC
 ,ID_X_SCALE_NORMALDIST  , AXIS_NORMAL_DISTRIBUTION
 ,ID_X_SCALE_DATETIME    , AXIS_DATE
};

static const RadioMenuItem<AxisType> YAxisTypeItems[] = {
  ID_Y_SCALE_LINEAR      , AXIS_LINEAR
 ,ID_Y_SCALE_LOGARITHMIC , AXIS_LOGARITHMIC
 ,ID_Y_SCALE_NORMALDIST  , AXIS_NORMAL_DISTRIBUTION
 ,ID_Y_SCALE_DATETIME    , AXIS_DATE
};

static const RadioMenuItem<AxisType> BothAxisTypeItems[] = {
  ID_BOTH_SCALE_LINEAR      , AXIS_LINEAR
 ,ID_BOTH_SCALE_LOGARITHMIC , AXIS_LOGARITHMIC
 ,ID_BOTH_SCALE_NORMALDIST  , AXIS_NORMAL_DISTRIBUTION
 ,ID_BOTH_SCALE_DATETIME    , AXIS_DATE
};

static const RadioMenuItem<GraphStyle> graphStyleTypeItems[] = {
  ID_VIEW_STYLE_CURVE         , GSCURVE
 ,ID_VIEW_STYLE_POINT         , GSPOINT
 ,ID_VIEW_STYLE_CROSS         , GSCROSS
};

static const RadioMenuItem<TrigonometricMode> trigonometricItems[] = {
  ID_OPTIONS_RADIANS          , RADIANS
 ,ID_OPTIONS_DEGREES          , DEGREES
 ,ID_OPTIONS_GRADS            , GRADS
};

void CMainFrame::setXAxisType(AxisType type) {
  pushLevel();
  checkRadioMenuItem(this, type, XAxisTypeItems, ARRAYSIZE(XAxisTypeItems));
  updateBothAxisType();
  switch(type) {
  case AXIS_LINEAR              :
  case AXIS_NORMAL_DISTRIBUTION :
  case AXIS_DATE                :
    break;
  case AXIS_LOGARITHMIC         :
    { DataRange cr = getView()->getDataRange();
      DataRange dr = getDoc()->getDataRange();
      if(cr.getMinX() < 0) {
        if(dr.getMinX() > 0) {
          getView()->setDataRange(DataRange(dr.getMinX(), max(cr.getMaxX(),dr.getMaxX()), cr.getMinY(), cr.getMaxY()), false);
        } else if(dr.getMaxX() > 0) {
          getView()->setDataRange(DataRange(getDoc()->getGraphArray().getSmallestPositiveX(), max(cr.getMaxX(),dr.getMaxX()), cr.getMinY(), cr.getMaxY()), false);
        } else if(cr.getMaxX() > 0) {
          getView()->setDataRange(DataRange(cr.getMaxX() / 10, cr.getMaxX(), cr.getMinY(), cr.getMaxY()), false);
        } else {
          getView()->setDataRange(DataRange(1, 10, cr.getMinY(), cr.getMaxY()), false);
        }
      }
    }
    break;
  }
  getView()->setAxisType(XAXIS_INDEX, type);
  popLevel();
}

void CMainFrame::setYAxisType(AxisType type) {
  pushLevel();
  checkRadioMenuItem(this, type, YAxisTypeItems, ARRAYSIZE(YAxisTypeItems));
  updateBothAxisType();
  switch(type) {
  case AXIS_LINEAR              :
  case AXIS_NORMAL_DISTRIBUTION :
  case AXIS_DATE                :
    break;
  case AXIS_LOGARITHMIC         :
    { DataRange cr = getView()->getDataRange();
      DataRange dr = getDoc()->getDataRange();
      if(cr.getMinY() < 0) {
        if(dr.getMinY() > 0) {
          getView()->setDataRange(DataRange(cr.getMinX(), cr.getMaxX(), dr.getMinY(), max(cr.getMaxY(),dr.getMaxY())), false);
        } else if(dr.getMaxY() > 0) {
          getView()->setDataRange(DataRange(cr.getMinX(), cr.getMaxX(), getDoc()->getGraphArray().getSmallestPositiveY(), max(cr.getMaxY(),dr.getMaxY())), false);
        } else if(cr.getMaxY() > 0) {
          getView()->setDataRange(DataRange(cr.getMinX(), cr.getMaxX(), cr.getMaxY() / 10, cr.getMaxY()), false);
        } else {
          getView()->setDataRange(DataRange(cr.getMinX(), cr.getMaxX(), 1, 10), false);
        }
      }
    }
    break;
  }
  getView()->setAxisType(YAXIS_INDEX, type);
  popLevel();
}

void CMainFrame::updateBothAxisType() {
  const AxisType xt = getAxisType(XAXIS_INDEX), yt = getAxisType(YAXIS_INDEX);
  if(xt == yt) {
    checkRadioMenuItem(this, xt,BothAxisTypeItems, ARRAYSIZE(BothAxisTypeItems));
  } else {
    uncheckAllRadioMenuItem(this, BothAxisTypeItems, ARRAYSIZE(BothAxisTypeItems));
  }
}

AxisType CMainFrame::getAxisType(AxisIndex axis) const {
  return (axis == XAXIS_INDEX)
       ? getRadioItemValue(this, XAxisTypeItems, ARRAYSIZE(XAxisTypeItems))
       : getRadioItemValue(this, YAxisTypeItems, ARRAYSIZE(YAxisTypeItems));
}

void CMainFrame::setAxisType(AxisIndex axis, AxisType type) {
  pushLevel();
  if(axis == XAXIS_INDEX) {
    setXAxisType(type);
  } else {
    setYAxisType(type);
  }
  popLevel();
}

void CMainFrame::OnViewScaleResetScale() {
  pushLevel();
  getView()->initScale();
  popLevel();
}

void CMainFrame::OnViewStyleCurve() {
  pushLevel();
  setGraphStyle(GSCURVE);
  popLevel();
}

void CMainFrame::OnViewStylePoint() {
  pushLevel();
  setGraphStyle(GSPOINT);
  popLevel();
}

void CMainFrame::OnViewStyleCross() {
  pushLevel();
  setGraphStyle(GSCROSS);
  popLevel();
}

void CMainFrame::setGraphStyle(GraphStyle newStyle) {
  pushLevel();
  checkRadioMenuItem(this, newStyle, graphStyleTypeItems, ARRAYSIZE(graphStyleTypeItems));
  getDoc()->getGraphArray().setStyle(newStyle);
  popLevel();
}

GraphStyle CMainFrame::getGraphStyle() const {
  return getRadioItemValue(this, graphStyleTypeItems, ARRAYSIZE(graphStyleTypeItems));
}

void CMainFrame::OnViewInterval() {
  CCoordinateSystem &cs = getView()->getCoordinateSystem();
  CIntervalDlg dlg(cs.getTransformation());
  if(dlg.DoModal() == IDOK) {
    pushLevel();
    cs.setDataRange(dlg.getDataRange(), false);
    popLevel();
  }
}

void CMainFrame::OnViewRetainAspectRatio() {
  pushLevel();
  getView()->setRetainAspectRatio(toggleMenuItem(this, ID_VIEW_RETAINASPECTRATIO));
  popLevel();
}

void CMainFrame::setRollAvg(bool on) {
  checkMenuItem(this, ID_VIEW_ROLLAVG, on);
}

void CMainFrame::OnViewRollAvg() {
  pushLevel();
  getDoc()->setRollAvg(toggleMenuItem(this, ID_VIEW_ROLLAVG));
  popLevel();
}

void CMainFrame::OnViewSetRollAvgSize() {
  RollAvgSizeDlg dlg(getDoc()->getRollAvgSize());
  if(dlg.DoModal() == IDOK) {
    pushLevel();
    getDoc()->setRollAvgSize(dlg.m_rollAvgSize);
    popLevel();
  }
}

void CMainFrame::OnViewRefreshFiles() {
  if(getDoc()->refreshFiles()) {
    Invalidate(FALSE);
  }
}

void CMainFrame::OnFindZeroes() {
  getView()->pushMouseTool(FINDZEROTOOL);
}

void CMainFrame::OnFindMinimum() {
  getView()->pushMouseTool(FINDMINTOOL);
}

void CMainFrame::OnFindMaximum() {
  getView()->pushMouseTool(FINDMAXTOOL);
}

void CMainFrame::OnFindIntersection() {
}

DoubleInterval CMainFrame::wlParamToInterval(WPARAM wp, LPARAM lp) {
  int xFrom = (int)wp;
  int xTo   = (int)lp;
  if(xFrom > xTo) std::swap(xFrom,xTo);
  return getView()->getCoordinateSystem().getTransformation().getXTransformation().backwardTransform(DoubleInterval(xFrom,xTo));
}

LRESULT CMainFrame::OnMsgSearchZeroesInInterval(WPARAM wp, LPARAM lp) {
  const GraphItem *item = getDoc()->getGraphArray().getSelectedItem();
  if(item != NULL) {
    const DoubleInterval   interval = wlParamToInterval(wp, lp);
    GraphZeroesResultArray result(item->getGraph());
    result.addAll(item->getGraph().findZeroes(interval));
    if(result.isEmpty()) {
      showInformation(result.toString().cstr());
    } else {
      getDoc()->getGraphArray().addPointArray(result.getMoveablePointArray(), false);
      Invalidate(FALSE);
    }
  }
  getView()->popMouseTool();
  return 0;
}

void CMainFrame::showExtremaInInterval(WPARAM wp, LPARAM lp, ExtremaType extremaType) {
  const GraphItem *item = getDoc()->getGraphArray().getSelectedItem();
  if(item != NULL) {
    const DoubleInterval    interval = wlParamToInterval(wp, lp);
    GraphExtremaResultArray result(item->getGraph(), extremaType);
    result.addAll(item->getGraph().findExtrema(interval, extremaType));
    if(result.isEmpty()) {
      showInformation(result.toString().cstr());
    } else {
      getDoc()->getGraphArray().addPointArray(result.getMoveablePointArray(), true);
      Invalidate(FALSE);
    }
  }
  getView()->popMouseTool();
}

LRESULT CMainFrame::OnMsgSearchMaxInInterval(WPARAM wp, LPARAM lp) {
  showExtremaInInterval(wp, lp, EXTREMA_TYPE_MAX);
  return 0;
}

LRESULT CMainFrame::OnMsgSearchMinInInterval(WPARAM wp, LPARAM lp) {
  showExtremaInInterval(wp, lp, EXTREMA_TYPE_MIN);
  return 0;
}

void CMainFrame::OnToolsFitPolynomial() {
  startPolynomialFitThread();
}

void CMainFrame::OnToolsFitExpoFunction() {
  makeExpoFit();
}

void CMainFrame::OnToolsFitPotensFunction() {
  makePotensFit();
}

void CMainFrame::OnToolsFitCustomFunction() {
  startCustomFitThread();
}

void CMainFrame::OnToolsPlotFunction() {
  try {
    FunctionGraphParameters &param = m_functionParam;
    param.setColor(randomColor());
    param.setTrigonometricMode(getTrigoMode());
    param.setInteval(getView()->getCoordinateSystem().getDataRange().getXInterval());
    CFunctionGraphDlg dlg(param);
    if(dlg.DoModal() == IDOK) {
      pushLevel();
      param = dlg.getData();
      getDoc()->addFunctionGraph(param);
      popLevel();
    }
  } catch(Exception e) {
    showException(e);
  }
}

void CMainFrame::OnToolsParametricCurve() {
  try {
    ParametricGraphParameters &param = m_parametricCurveParam;
    param.setColor(randomColor());
    param.setTrigonometricMode(getTrigoMode());
    param.setInteval(getView()->getCoordinateSystem().getDataRange().getXInterval());
    CParametricGraphDlg dlg(param);
    if(dlg.DoModal() == IDOK) {
      pushLevel();
      param = dlg.getData();
      getDoc()->addParametricGraph(param);
      popLevel();
    }
  } catch(Exception e) {
    showException(e);
  }
}

void CMainFrame::OnToolsImplicitDefinedCurve() {
  try {
    IsoCurveGraphParameters &param = m_isoCurveParam;
    param.setColor(randomColor());
    param.setTrigonometricMode(getTrigoMode());
    const DoubleInterval xInterval = getView()->getCoordinateSystem().getDataRange().getXInterval();
    const DoubleInterval yInterval = getView()->getCoordinateSystem().getDataRange().getYInterval();
    CIsoCurveGraphDlg dlg(param);
    if(dlg.DoModal() == IDOK) {
      pushLevel();
      param = dlg.getData();
      getDoc()->addIsoCurveGraph(param);
      popLevel();
    }
  } catch(Exception e) {
    showException(e);
  }
}

void CMainFrame::OnToolsDifferentialEquations() {
  try {
    DiffEquationGraphParameters &param = m_diffEqParam;
    param.setTrigonometricMode(getTrigoMode());
    const DoubleInterval xInterval = getView()->getAxisInterval(XAXIS_INDEX);
    CDiffEquationGraphDlg dlg(param);
    if(dlg.DoModal() == IDOK) {
      pushLevel();
      getDoc()->addDiffEquationGraph(param);
      popLevel();
    }
  } catch(Exception e) {
    showException(e);
  }
}

void CMainFrame::OnOptionsIgnoreErrors() {
  pushLevel();
  toggleMenuItem(this, ID_OPTIONS_IGNOREERRORS);
  popLevel();
}

void CMainFrame::OnOptionsRadians() {  setTrigoMode(RADIANS); }
void CMainFrame::OnOptionsDegrees() {  setTrigoMode(DEGREES); }
void CMainFrame::OnOptionsGrads()   {  setTrigoMode(GRADS  ); }

void CMainFrame::setTrigoMode(TrigonometricMode mode) {
  const bool changed = getTrigoMode() != mode;
  checkRadioMenuItem(this, mode, trigonometricItems, ARRAYSIZE(trigonometricItems));
  if(changed) {
    pushLevel();
    getDoc()->setTrigoMode(mode);
    popLevel();
  }
}

TrigonometricMode CMainFrame::getTrigoMode() const {
  return getRadioItemValue(this, trigonometricItems, ARRAYSIZE(trigonometricItems));
}

void CMainFrame::setIgnoreErrors(bool ignore) {
  checkMenuItem(this, ID_OPTIONS_IGNOREERRORS, ignore);
}

bool CMainFrame::getIgnoreErrors() {
  return isMenuItemChecked(this, ID_OPTIONS_IGNOREERRORS);
}

void CMainFrame::startPolynomialFitThread() {
  stopFitThread();
  const GraphItem *item = getDoc()->getSelectedGraphItem();
  if(item && item->getGraph().isPointGraph()) {
    m_fitThread = ::startFitThread(POLYNOMIAL_FIT, getDoc()->getDataRange().getXInterval(), ((PointGraph&)(item->getGraph())).getDataPoints(), *getView());
  }
}

void CMainFrame::startCustomFitThread() {
  stopFitThread();
  const GraphItem *item = getDoc()->getSelectedGraphItem();
  if(item && item->getGraph().isPointGraph()) {
    m_fitThread = ::startFitThread(CUSTOM_FIT, getDoc()->getDataRange().getXInterval(), ((PointGraph&)(item->getGraph())).getDataPoints(), *getView());
  }
}

static COLORREF getShiftedColor(COLORREF c) {
  UINT r = GetRValue(c);
  UINT g = GetGValue(c);
  UINT b = GetBValue(c);
  if(r == g && g == b) {
    r+=50;
    g+=50;
    b+=50;
  }
  return RGB(g,b,r);
}

void CMainFrame::makeExpoFit() {
  const GraphItem *item = getDoc()->getSelectedGraphItem();
  if(item == NULL) {
    showWarning(_T("No data selected"));
    return;
  }
  if(!item->getGraph().isPointGraph()) {
    showWarning(_T("Select datapoints to make exponential regression"));
    return;
  }
  const DoubleInterval &yRange = item->getGraph().getDataRange().getYInterval();
  if(sign(yRange.getFrom()) != sign(yRange.getTo())) {
    showWarning(_T("There are both negative and positive y-values in the dataset"));
    return;
  }
  const Point2DArray &points = ((PointGraph&)item->getGraph()).getDataPoints();
  CompactArray<DataPoint> logPoints;
  const int s = sign(yRange.getFrom());
  for(size_t i = 0; i < points.size(); i++) {
    const Point2D &p = points[i];
    const double logy = log(p.y * s);
    logPoints.add(DataPoint(p.x, logy));
  }
  DataFit fit;
  fit.solve(LSSD, logPoints);
  fit.setDegree(1);
  Polynomial poly = fit;
  const double a = s * exp(poly.getCoef(0).re);
  const double b = exp(poly.getCoef(1).re);
  FunctionGraphParameters param;
  param.setName(format(_T("Exponential fit of %s"), item->getDisplayName().cstr()));
  param.setColor(getShiftedColor(item->getGraph().getParam().getColor()));
  param.setExprText(format(_T("a = %lg;\r\nb = %lg;\r\na * b^x"), a, b));
  param.setInteval(item->getGraph().getDataRange().getXInterval());
  getDoc()->addFunctionGraph(param);
  Invalidate(TRUE);
}

void CMainFrame::makePotensFit() {
  const GraphItem *item = getDoc()->getSelectedGraphItem();
  if(item == NULL) {
    showWarning(_T("No data selected"));
    return;
  }
  if(!item->getGraph().isPointGraph()) {
    showWarning(_T("Select datapoints to make potens regression"));
    return;
  }
  const DoubleInterval &yRange = item->getGraph().getDataRange().getYInterval();
  if(sign(yRange.getFrom()) != sign(yRange.getTo())) {
    showWarning(_T("There are both negative and positive y-values in the dataset"));
    return;
  }
  const DoubleInterval &xRange = item->getGraph().getDataRange().getXInterval();
  if(xRange.getMin() < 0) {
    showWarning(_T("There cannot be negative x-values"));
    return;
  }

  double xMinPlus;
  if(xRange.getMin() == 0) {
    xMinPlus = item->getGraph().getSmallestPositiveX() / 1000;
  }
  const Point2DArray &points = ((PointGraph&)item->getGraph()).getDataPoints();
  CompactArray<DataPoint> logPoints;
  const int s = sign(yRange.getFrom());
  for(size_t i = 0; i < points.size(); i++) {
    const Point2D &p = points[i];
    const double logx = (p.x > 0) ? log(p.x) : log(xMinPlus);
    const double logy = log(p.y * s);
    logPoints.add(DataPoint(logx, logy));
  }
  DataFit fit;
  fit.solve(LSSD, logPoints);
  fit.setDegree(1);
  Polynomial poly = fit;
  const double a = s * exp(poly.getCoef(0).re);
  const double b = poly.getCoef(1).re;
  FunctionGraphParameters param;
  param.setName(format(_T("potens fit of %s"), item->getDisplayName().cstr()));
  param.setColor(getShiftedColor(item->getGraph().getParam().getColor()));
  param.setExprText(format(_T("a = %lg;\r\nb = %lg;\r\na * x^b"), a, b));
  param.setInteval(item->getGraph().getDataRange().getXInterval());
  getDoc()->addFunctionGraph(param);
  Invalidate(TRUE);
}

void CMainFrame::stopFitThread() {
  if(m_fitThread) {
    m_fitThread->PostThreadMessage(WM_QUIT,0,0);
    m_fitThread = NULL;
  }
}

void CMainFrame::OnSelectMenuDelete() {
  GraphArray &ga = getDoc()->getGraphArray();
  if(ga.getCurrentSelection() >= 0) {
    pushLevel();
    ga.remove(ga.getCurrentSelection());
    popLevel();
  }
}

void CMainFrame::OnSelectMenuEdit() {
  pushLevel();
  try {
    GraphArray &ga = getDoc()->getGraphArray();
    if(ga.getCurrentSelection() >= 0) {
      Graph &g = ga[ga.getCurrentSelection()].getGraph();
      switch(g.getType()) {
      case DATAGRAPH        :
        { CDataGraphDlg dlg((DataGraph&)g);
          dlg.DoModal();
        }
        break;

      case FUNCTIONGRAPH  :
        { FunctionGraphParameters &param = (FunctionGraphParameters&)g.getParam();
          CFunctionGraphDlg dlg(param);
          if(dlg.DoModal() == IDOK) {
            param = dlg.getData();
            g.calculate();
          }
        }
        break;

      case PARAMETRICGRAPH:
        { ParametricGraphParameters &param = (ParametricGraphParameters&)g.getParam();
          CParametricGraphDlg dlg(param);
          if(dlg.DoModal() == IDOK) {
            param = dlg.getData();
            g.calculate();
          }
        }
        break;

      case ISOCURVEGRAPH    :
        { IsoCurveGraphParameters &param = (IsoCurveGraphParameters&)g.getParam();
          CIsoCurveGraphDlg dlg(param);
          if(dlg.DoModal() == IDOK) {
            param = dlg.getData();
            g.calculate();
          }
        }
        break;

      case DIFFEQUATIONGRAPH:
        { DiffEquationGraphParameters *param = (DiffEquationGraphParameters*)&g.getParam();
          CDiffEquationGraphDlg dlg(*param);
          if(dlg.DoModal() == IDOK) {
            g.calculate();
          }
        }
        break;

      default:
        throwException(_T("%s:Unknown graph type:%d"), __TFUNCTION__, g.getType());
      }
    }
    popLevel();
  } catch(Exception e) {
    popLevel();
    showException(e);
  } catch(...) {
    popLevel();
    throw;
  }
}

void CMainFrame::OnSelectMenuHide() {
  GraphArray &ga = getDoc()->getGraphArray();
  if(ga.getCurrentSelection() >= 0) {
    pushLevel();
    ga.getSelectedItem()->getGraph().setVisible(false);
    popLevel();
  }
}

void CMainFrame::OnSelectMenuShow() {
  GraphArray &ga = getDoc()->getGraphArray();
  if(ga.getCurrentSelection() >= 0) {
    pushLevel();
    ga.getSelectedItem()->getGraph().setVisible(true);
    popLevel();
  }
}

void CMainFrame::OnSelectMenuStyleCurve() {
  GraphArray &ga = getDoc()->getGraphArray();
  if(ga.getCurrentSelection() >= 0) {
    pushLevel();
    ga.getSelectedItem()->getGraph().setStyle(GSCURVE);
    popLevel();
  }
}

void CMainFrame::OnSelectMenuStylePoint() {
  GraphArray &ga = getDoc()->getGraphArray();
  if(ga.getCurrentSelection() >= 0) {
    pushLevel();
    ga.getSelectedItem()->getGraph().setStyle(GSPOINT);
    popLevel();
  }
}

void CMainFrame::OnSelectMenuStyleCross() {
  GraphArray &ga = getDoc()->getGraphArray();
  if(ga.getCurrentSelection() >= 0) {
    pushLevel();
    ga.getSelectedItem()->getGraph().setStyle(GSCROSS);
    popLevel();
  }
}

#ifdef __NEVER__
#include <Scandir.h>

template<class T> class FileConverter {
private:
  void convertFile(const String &name) {
    T v;
    try {
      v.oldLoad(name);
    } catch (Exception e) {
      throwException(_T("Error loading %s:%s"), name.cstr(), e.what());
    }
    FileNameSplitter fs(name);
    fs.setDir(fs.getDir() + _T("New"));
    const String outName = fs.getAbsolutePath();
    try {
      v.save(outName);
    } catch (Exception e) {
      throwException(_T("Error saving %s:%s"), outName.cstr(), e.what());
    }
  }
public:
  void convertAllFiles(const String &ext) {
    const String dir = _T("c:\\mytools2015\\Showgraf\\samples\\");
    DirList list = scandir(dir + _T("*.") + ext);
    for (size_t i = 0; i < list.size(); i++) {
      convertFile(FileNameSplitter::getChildName(dir, list[i].name));
    }
  }
};

void CMainFrame::OnFileConvertscripts() {
  try {
    FileConverter<IsoCurveGraphParameters    >().convertAllFiles(_T("iso"));
    FileConverter<FunctionGraphParameters    >().convertAllFiles(_T("exp"));
    FileConverter<ParametricGraphParameters  >().convertAllFiles(_T("par"));
    FileConverter<DiffEquationGraphParameters>().convertAllFiles(_T("deq"));
  } catch (Exception e) {
    showException(e);
  }
}
#endif
