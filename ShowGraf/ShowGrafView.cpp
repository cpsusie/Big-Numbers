#include "stdafx.h"
#include <Math/Polynomial.h>
#include "ShowGraf.h"
#include "MainFrm.h"
#include "ShowGrafDoc.h"
#include "ShowGrafView.h"
#include "DataGraphParameters.h"
#include "DataGraph.h"
#include "DiffEquationGraphDlg.h"
#include "FunctionGraphDlg.h"
#include "ParametricGraphDlg.h"
#include "IsoCurveGraphDlg.h"
#include "DataGraphDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CShowGrafView, CFormView)

BEGIN_MESSAGE_MAP(CShowGrafView, CFormView)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
    ON_WM_SIZE()
    ON_COMMAND(ID_SELECTMENU_DELETE , OnSelectMenuDelete           )
    ON_COMMAND(ID_SELECTMENU_EDIT   , OnSelectMenuEdit             )
    ON_COMMAND(ID_SELECTMENU_HIDE   , OnSelectMenuHide             )
    ON_COMMAND(ID_SELECTMENU_SHOW   , OnSelectMenuShow             )
    ON_COMMAND(ID_FILE_PRINT        , CFormView::OnFilePrint       )
    ON_COMMAND(ID_FILE_PRINT_DIRECT , CFormView::OnFilePrint       )
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

CShowGrafView::CShowGrafView() : CFormView(IDD) {
  m_buttonFont.CreateFont(10, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                          DEFAULT_PITCH | FF_MODERN,
                          _T("Arial") );
  m_axisFont.CreateFont(  10, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                          DEFAULT_PITCH | FF_MODERN,
                          _T("Arial") );
  m_fitThread        = NULL;
  m_firstDraw        = true;
  m_dragging         = false;
}

CShowGrafView::~CShowGrafView() {
}

void CShowGrafView::init() {
//  redirectDebugLog();
  m_coordinateSystem.substituteControl(this, IDC_SYSTEMPANEL);

  m_backgroundColor  = RGB(255,255,255);
  m_axisColor        = RGB(0,0,0);
  setGraphStyle(GSCURVE);
  m_XAxisType        = AXIS_LINEAR;
  m_xReader          = &DataReader::LinearDataReader;
  m_YAxisType        = AXIS_LINEAR;
  m_yReader          = &DataReader::LinearDataReader;
  m_onePerLine       = false;
  m_rangeSpecified   = false;
  m_rollSize         = 10;
  m_xRelativeToFirst = m_yRelativeToFirst = false;

  TCHAR **argv       = __targv;
  int argc           = __argc;
  argvExpand(argc,argv);
  TCHAR *cp;

  for(argv++;*argv && *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
      case 'p':
        setGraphStyle(GSPOINT);  continue;
      case 'k':
        setGraphStyle(GSCROSS);  continue;
      case 'i':
        checkMenuItem(ID_OPTIONS_IGNOREERRORS,true);
        continue;
      case '1':
        m_onePerLine   = true;   continue;
      case 'R':
        if(cp[1] == 'x') {
          m_xRelativeToFirst = true;
          cp++;
        }
        if(cp[1] == 'y') {
          m_yRelativeToFirst = true;
          cp++;
        };
        continue;

      case 'r':
        ((CMainFrame*)GetParent())->setRollAverage();
        if(_stscanf(cp+1,_T("%d"), &m_rollSize) == 1) {
          if(m_rollSize < 1 || m_rollSize > 10000) {
            usage();
          }
          break;
        }
        continue;
      case 'I':
        { double minX,maxX,minY,maxY;
          if(_stscanf(cp+1,_T("%le,%le,%le,%le"),&minX,&maxX,&minY,&maxY) != 4) {
            usage();
          }
          m_explicitRange.init(minX,maxX,minY,maxY);
          m_rangeSpecified = true;
          break;
        }
      case 'g':
        ((CMainFrame*)GetParent())->setGrid();
        continue;
      case 'L':
        if(cp[1] == 'x') {
          m_XAxisType = AXIS_LOGARITHMIC;
          m_xReader = &DataReader::LogarithmicDataReader;
          cp++;
        }
        if(cp[1] == 'y') {
          m_YAxisType = AXIS_LOGARITHMIC;
          m_yReader = &DataReader::LogarithmicDataReader;
          cp++;
        };
        continue;
      case 'N':
        if(cp[1] == 'x') {
          m_XAxisType = AXIS_NORMAL_DISTRIBUTION;
          m_xReader = &DataReader::NormalDistributionDataReader;
          cp++;
        }
        if(cp[1] == 'y') {
          m_YAxisType = AXIS_NORMAL_DISTRIBUTION;
          m_yReader = &DataReader::NormalDistributionDataReader;
          cp++;
        };
        continue;

      case 'D':
        if(cp[1] == 'x') {
          m_XAxisType = AXIS_DATE;
          m_xReader = &DataReader::DateTimeDataReader;
          cp++;
        }
        if(cp[1] == 'y') {
          m_YAxisType = AXIS_DATE;
          m_yReader = &DataReader::DateTimeDataReader;
          cp++;
        };
        continue;

      case 'c':
        if(!(*(++argv))) {
          usage();
        } else {
          const ColorName *bc = findColorByName(*argv);
          if(bc == NULL) {
            usage();
          }
          m_backgroundColor = bc->m_color;
        }
        if(!(*(++argv))) {
          usage(); 
        } else {
          const ColorName *ac = findColorByName(*argv);
          if(ac == NULL) {
            usage();
          }
          m_axisColor = ac->m_color;
        }
        break;

      default:usage();
      }
      break;
    }
  }
  if(*argv == NULL) {
    DataGraphParameters param("stdin"
                             ,getColor(0)
                             ,m_onePerLine
                             ,isMenuItemChecked(ID_OPTIONS_IGNOREERRORS)
                             ,m_xRelativeToFirst
                             ,m_yRelativeToFirst 
                             ,*m_xReader
                             ,*m_yReader
                             ,isMenuItemChecked(ID_VIEW_ROLLAVERAGE) ? m_rollSize : 0
                             ,m_grafStyle);
    Graph *g = new DataGraph(param);
    if(!g->isEmpty()) {
      m_graphArray.add(g);
    } else {
      delete g;
    }
  } else {
    int colorIndex = 0;
    for(;*argv;argv++) {
      DataGraphParameters param(*argv
                               ,getColor(colorIndex)
                               ,m_onePerLine
                               ,isMenuItemChecked(ID_OPTIONS_IGNOREERRORS)
                               ,m_xRelativeToFirst
                               ,m_yRelativeToFirst 
                               ,*m_xReader
                               ,*m_yReader
                               ,isMenuItemChecked(ID_VIEW_ROLLAVERAGE) ? m_rollSize : 0
                               ,m_grafStyle);
      Graph *g = new DataGraph(param);
      if(!g->isEmpty()) {
        colorIndex++;
        m_graphArray.add(g);
      } else {
        delete g;
      }
    }
  }
  if(m_graphArray.size() == 1) {
    m_graphArray.select(0);
  }
  initScale();
}

bool CShowGrafView::paintAll(CDC &dc, const CRect &rect, CFont *axisFont, CFont *buttonFont) {
  if(axisFont == NULL) {
    axisFont = &m_axisFont;
  }

  if(buttonFont == NULL) {
    buttonFont = &m_buttonFont;
  }

  CWnd *systemPanel = GetDlgItem(IDC_SYSTEMPANEL);
  CWnd *buttonPanel = GetDlgItem(IDC_BUTTONPANEL);

  if(systemPanel == NULL || buttonPanel == NULL) {
    return false;
  }

  WINDOWPLACEMENT wpl;
  systemPanel->GetWindowPlacement(&wpl);
  int buttonPanelWidth = m_graphArray.getMaxButtonWidth(dc,*buttonFont) + 30;

  wpl.rcNormalPosition.left   = 0;
  wpl.rcNormalPosition.top    = 0;
  wpl.rcNormalPosition.right  = rect.right - buttonPanelWidth;
  wpl.rcNormalPosition.bottom = rect.bottom;
  systemPanel->SetWindowPlacement(&wpl);

  buttonPanel->GetWindowPlacement(&wpl);
  wpl.rcNormalPosition.left   = rect.right - buttonPanelWidth;
  wpl.rcNormalPosition.top    = 0;
  wpl.rcNormalPosition.right  = rect.right;
  wpl.rcNormalPosition.bottom = rect.bottom;
  buttonPanel->SetWindowPlacement(&wpl);

  m_coordinateSystem.SetFont(axisFont,FALSE);
  m_coordinateSystem.setBackGroundColor(m_backgroundColor);
  m_coordinateSystem.setAxisColor(m_axisColor);
  m_coordinateSystem.setXAxisType(m_XAxisType);
  m_coordinateSystem.setYAxisType(m_YAxisType);
  m_coordinateSystem.setGrid(isMenuItemChecked(ID_VIEW_GRID));
  try {
    m_coordinateSystem.OnPaint();
    CClientDC dc(&m_coordinateSystem);
    m_coordinateSystem.setDC(dc);
    m_graphArray.paint(m_coordinateSystem, *buttonFont, getRelativeClientRect(this,IDC_BUTTONPANEL));
//    debugLog(_T("Cells Occupied:\n%s"), m_coordinateSystem.getOccupationMap().toString().cstr());
    return true;
  } catch(Exception e) {
    showException(e);
    return false;
  }
}

void CShowGrafView::plotFunction(Function &f, const DoubleInterval &interval, COLORREF color) {
  m_coordinateSystem.addFunctionObject(f, &interval, color);
}

void CShowGrafView::plotFunction(Function &f, COLORREF color) {
  plotFunction(f, getCoordinateSystem().getDataRange().getXInterval(), color);
}

void CShowGrafView::OnDraw(CDC *pDC) {
  try {
    if(m_firstDraw) {
      init();
      m_firstDraw = false;
    }
    CRect rect = getClientRect(this);

    if(paintAll(*pDC, rect, &m_axisFont, &m_buttonFont)) {
      DoubleInterval maxNormInterval(0,1);
      const DataRange dr = m_coordinateSystem.getDataRange();
      enableMenuItem(ID_VIEW_SCALE_X_LOGARITHMIC       , dr.getMinX() > 0);
      enableMenuItem(ID_VIEW_SCALE_Y_LOGARITHMIC       , dr.getMinY() > 0);
      enableMenuItem(ID_VIEW_SCALE_X_NORMALDIST        , maxNormInterval.contains(dr.getXInterval()));
      enableMenuItem(ID_VIEW_SCALE_Y_NORMALDIST        , maxNormInterval.contains(dr.getYInterval()));
      checkMenuItem( ID_VIEW_RETAINASPECTRATIO         , m_coordinateSystem.isRetainingAspectRatio());
      enableMenuItem(ID_VIEW_RETAINASPECTRATIO         , m_coordinateSystem.canRetainAspectRatio());
    }
  } catch(Exception e) {
    showException(e);
  }
}

BOOL CShowGrafView::OnPreparePrinting(CPrintInfo *pInfo) {
  return DoPreparePrinting(pInfo);
}

void CShowGrafView::OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo) {
}

void CShowGrafView::OnEndPrinting(CDC *pDC, CPrintInfo *pInfo) {
}

void CShowGrafView::checkMenuItem(int id, bool checked) {
  ::checkMenuItem(GetParent(), id, checked);
}

void CShowGrafView::enableMenuItem(int id, bool enabled) {
  ::enableMenuItem(GetParent(), id, enabled);
}

bool CShowGrafView::isMenuItemChecked(int id) {
  return ::isMenuItemChecked(GetParent(), id);
}

void CShowGrafView::initScaleIfSingleGraph() {
  if (m_graphArray.size() == 1) initScale();
}

#ifdef _DEBUG
void CShowGrafView::AssertValid() const {
  CFormView::AssertValid();
}

void CShowGrafView::Dump(CDumpContext& dc) const {
  CFormView::Dump(dc);
}

#endif //_DEBUG

void CShowGrafView::OnLButtonDown(UINT nFlags, CPoint point) {
  CFormView::OnLButtonDown(nFlags, point);
  CRect cr = getClientRect(this, IDC_SYSTEMPANEL);
  if(cr.PtInRect(point)) {
    startDragging(point);
  }
}

void CShowGrafView::OnLButtonUp(UINT nFlags, CPoint point) {
  CFormView::OnLButtonUp(nFlags, point);
  stopDragging();
}

void CShowGrafView::OnMouseMove(UINT nFlags, CPoint point) {
  CFormView::OnMouseMove(nFlags, point);
  if(m_dragging) {
    if(nFlags && MK_LBUTTON) {
      Rectangle2D   fr         = m_mouseDownTransform.getFromRectangle();
      Point2D       startPoint = m_mouseDownTransform.backwardTransform((Point2DP)m_mouseDownPoint);
      Point2D       newPoint   = m_mouseDownTransform.backwardTransform((Point2DP)point);
      const Point2D dp = newPoint - startPoint;
      fr -= dp;
      try {
        m_coordinateSystem.getTransformation().setFromRectangle(fr);
        Invalidate(FALSE);
      } catch(Exception e) {
        // ignore
      }
    }
  }
}

void CShowGrafView::startDragging(const CPoint &point) {
  m_dragging           = true;
  m_mouseDownPoint     = point;
  m_mouseDownTransform = m_coordinateSystem.getTransformation();
  CRect cr = getClientRect(this, IDC_SYSTEMPANEL);
  ClientToScreen(&cr);
  ClipCursor(&cr);
  setWindowCursor(this, MAKEINTRESOURCE(OCR_HAND));
}

void CShowGrafView::stopDragging() {
  m_dragging = false;
  setWindowCursor(this, MAKEINTRESOURCE(OCR_NORMAL));
  ClipCursor(NULL);
}

void CShowGrafView::OnRButtonDown(UINT nFlags, CPoint point) {
  if(m_graphArray.OnLButtonDown(nFlags, point, m_coordinateSystem.getTransformation())) {
    CMenu menu;
    if(!menu.LoadMenu(IDR_MENUSELECTGRAF)) {
      MessageBox(_T("Loadmenu failed"), _T("Error"), MB_ICONEXCLAMATION);
      return;
    }
    removeMenuItem(&menu, m_graphArray.getSelectedItem()->getGraph().isVisible() ? ID_SELECTMENU_SHOW : ID_SELECTMENU_HIDE);
    ClientToScreen(&point);
    menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x+10,point.y, this );
  } else {
    CFormView::OnRButtonDown(nFlags, point);
  }
}

BOOL CShowGrafView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  ScreenToClient(&pt);
  const Point2DP tmppt(pt);
  int zoomFlags = 0;
  if(nFlags & MK_CONTROL) {
    zoomFlags |= X_AXIS;
  }
  if(nFlags & MK_SHIFT) {
    zoomFlags |= Y_AXIS;
  }
  if(zoomFlags == 0) {
    zoomFlags = X_AXIS | Y_AXIS;
  }
  m_coordinateSystem.getTransformation().zoom(tmppt,(zDelta < 0) ? -0.05 : 0.05,zoomFlags);
  Invalidate(FALSE);
  return CFormView::OnMouseWheel(nFlags, zDelta, pt);
}

void CShowGrafView::OnSelectMenuDelete() {
  if(m_graphArray.getCurrentSelection() >= 0) {
    m_graphArray.remove(m_graphArray.getCurrentSelection());
    Invalidate();
  }
}

void CShowGrafView::OnSelectMenuEdit() {
  if(m_graphArray.getCurrentSelection() >= 0) {
    Graph &g = m_graphArray[m_graphArray.getCurrentSelection()].getGraph();
    switch(g.getType()) {
    case FUNCTIONGRAPH  :
      { FunctionGraphParameters *param = (FunctionGraphParameters*)&g.getParam();
        CFunctionGraphDlg dlg(*param);
        if(dlg.DoModal() == IDOK) {
          g.calculate();
        }
      }
      break;

    case PARAMETRICGRAPH:
      { ParametricGraphParameters *param = (ParametricGraphParameters*)&g.getParam();
        CParametricGraphDlg dlg(*param);
        if(dlg.DoModal() == IDOK) {
          g.calculate();
        }
      }
      break;

    case ISOCURVEGRAPH    :
      { IsoCurveGraphParameters *param = (IsoCurveGraphParameters*)&g.getParam();
        CIsoCurveGraphDlg dlg(*param);
        if(dlg.DoModal() == IDOK) {
          g.calculate();
        }
      }
      break;

    case DATAGRAPH        :
      { CDataGraphDlg dlg((DataGraph&)g);
        dlg.DoModal();
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
      MessageBox(format(_T("%s:Unknown graph type:%d"), __TFUNCTION__, g.getType()).cstr());
      return;
    }
    Invalidate();
  }
}

void CShowGrafView::OnSelectMenuHide() {
  if(m_graphArray.getCurrentSelection() >= 0) {
    m_graphArray.getSelectedItem()->getGraph().setVisible(false);
    Invalidate();
  }
}

void CShowGrafView::OnSelectMenuShow() {
  if(m_graphArray.getCurrentSelection() >= 0) {
    m_graphArray.getSelectedItem()->getGraph().setVisible(true);
    Invalidate();
  }
}

void CShowGrafView::OnSize(UINT nType, int cx, int cy) {
  CFormView::OnSize(nType, cx, cy);
  ShowScrollBar(SB_BOTH,FALSE);
}

void CShowGrafView::clear() {
  m_graphArray.clear();
  initScale();
  Invalidate();
}

void CShowGrafView::initScale() {
  DataRange dataRange;
  if(m_rangeSpecified) {
    dataRange = m_explicitRange;
  } else if(m_graphArray.size() == 0) {
    dataRange = CCoordinateSystem::getDefaultDataRange(m_XAxisType,m_YAxisType);
  } else {
    dataRange = m_graphArray.getDataRange();
  }
  m_coordinateSystem.setFromRectangle(dataRange, true);
  setXAxisType(m_XAxisType);
  setYAxisType(m_YAxisType);
}

void CShowGrafView::addGraphFromFile(const String &fileName) {
  for (int i = 0; i < 5; i++) {
    try {
      switch(i) {
      case 0: readParametricFile(fileName); break;
      case 1: readFunctionFile(  fileName); break;
      case 2: readIsoFile(       fileName); break;
      case 3: readDataFile(      fileName); break;
      case 4: readDiffEqFile(    fileName); break;
      }
      return;
    } catch(Exception e) {
    }
  }
  MessageBox(format(_T("%s is not a valid format"), fileName.cstr()).cstr(), _T("Error"), MB_ICONWARNING);
}

void CShowGrafView::readDataFile(const String &fileName) {
  DataGraphParameters param(fileName
                           ,randomColor()
                           ,m_onePerLine
                           ,isMenuItemChecked(ID_OPTIONS_IGNOREERRORS)
                           ,m_xRelativeToFirst
                           ,m_yRelativeToFirst 
                           ,*m_xReader
                           ,*m_yReader
                           ,isMenuItemChecked(ID_VIEW_ROLLAVERAGE) ? m_rollSize : 0
                           ,m_grafStyle);
  Graph *g = new DataGraph(param);
  if(!g->isEmpty()) {
    m_graphArray.add(g);
    initScaleIfSingleGraph();
  } else {
    delete g;
  }
}

void CShowGrafView::readFunctionFile(const String &fileName) {
  FunctionGraphParameters param;
  param.load(fileName);
  Graph *g = new FunctionGraph(param);
  m_graphArray.add(g);
  initScaleIfSingleGraph();
}

void CShowGrafView::readParametricFile(const String &fileName) {
  ParametricGraphParameters param;
  param.load(fileName);
  Graph *g = new ParametricGraph(param);
  m_graphArray.add(g);
  initScaleIfSingleGraph();
}

void CShowGrafView::readIsoFile(const String &fileName) {
  IsoCurveGraphParameters param;
  param.load(fileName);
  Graph *g = new IsoCurveGraph(param);
  m_graphArray.add(g);
  initScaleIfSingleGraph();
}

void CShowGrafView::readDiffEqFile(const String &fileName) {
  DiffEquationGraphParameters param;
  param.load(fileName);
  Graph *g = new DiffEquationGraph(param);
  m_graphArray.add(g);
  initScaleIfSingleGraph();
}

void CShowGrafView::setRollingAverage(bool on) {
  m_graphArray.setRollSize(on ? m_rollSize : 0);
}

void CShowGrafView::setRollSize(int size) {
  m_rollSize = size;
  setRollingAverage(true);
}

void CShowGrafView::setXAxisType(AxisType type) {
  checkMenuItem(ID_VIEW_SCALE_X_LINEAR            , false);
  checkMenuItem(ID_VIEW_SCALE_X_LOGARITHMIC       , false);
  checkMenuItem(ID_VIEW_SCALE_X_NORMALDIST        , false);
  checkMenuItem(ID_VIEW_SCALE_X_DATETIME          , false);
  switch(type) {
  case AXIS_LINEAR              :
    checkMenuItem(ID_VIEW_SCALE_X_LINEAR          , true);
    break;
  case AXIS_LOGARITHMIC         : 
    { checkMenuItem(ID_VIEW_SCALE_X_LOGARITHMIC   , true);
      DataRange cr = m_coordinateSystem.getDataRange();
      DataRange dr = m_graphArray.getDataRange();
      if(cr.getMinX() < 0) {
        if(dr.getMinX() > 0) {
          m_coordinateSystem.setDataRange(DataRange(dr.getMinX(), max(cr.getMaxX(),dr.getMaxX()), cr.getMinY(), cr.getMaxY()), false);
        } else if(dr.getMaxX() > 0) {
          m_coordinateSystem.setDataRange(DataRange(m_graphArray.getSmallestPositiveX(), max(cr.getMaxX(),dr.getMaxX()), cr.getMinY(), cr.getMaxY()), false);
        } else if(cr.getMaxX() > 0) {
          m_coordinateSystem.setDataRange(DataRange(cr.getMaxX() / 10, cr.getMaxX(), cr.getMinY(), cr.getMaxY()), false);
        } else {
          m_coordinateSystem.setDataRange(DataRange(1, 10, cr.getMinY(), cr.getMaxY()), false);
        }
      }
    }
    break;
  case AXIS_NORMAL_DISTRIBUTION :
    checkMenuItem(ID_VIEW_SCALE_X_NORMALDIST, true);
    break;
  case AXIS_DATE                :
    checkMenuItem(ID_VIEW_SCALE_X_DATETIME          , true);
    break;
  }
  m_XAxisType = type;
}

void CShowGrafView::setYAxisType(AxisType type) {
  checkMenuItem(ID_VIEW_SCALE_Y_LINEAR            , false);
  checkMenuItem(ID_VIEW_SCALE_Y_LOGARITHMIC       , false);
  checkMenuItem(ID_VIEW_SCALE_Y_NORMALDIST        , false);
  checkMenuItem(ID_VIEW_SCALE_Y_DATETIME          , false);
  switch(type) {
  case AXIS_LINEAR              :
    checkMenuItem(ID_VIEW_SCALE_Y_LINEAR          , true);
    break;
  case AXIS_LOGARITHMIC         :
    { checkMenuItem(ID_VIEW_SCALE_Y_LOGARITHMIC   , true);
      DataRange cr = m_coordinateSystem.getDataRange();
      DataRange dr = m_graphArray.getDataRange();
      if(cr.getMinY() < 0) {
        if(dr.getMinY() > 0) {
          m_coordinateSystem.setDataRange(DataRange(cr.getMinX(), cr.getMaxX(), dr.getMinY(), max(cr.getMaxY(),dr.getMaxY())), false);
        } else if(dr.getMaxY() > 0) {
          m_coordinateSystem.setDataRange(DataRange(cr.getMinX(), cr.getMaxX(), m_graphArray.getSmallestPositiveY(), max(cr.getMaxY(),dr.getMaxY())), false);
        } else if(cr.getMaxY() > 0) {
          m_coordinateSystem.setDataRange(DataRange(cr.getMinX(), cr.getMaxX(), cr.getMaxY() / 10, cr.getMaxY()), false);
        } else {
          m_coordinateSystem.setDataRange(DataRange(cr.getMinX(), cr.getMaxX(), 1, 10), false);
        }
      }
    }
    break;
  case AXIS_NORMAL_DISTRIBUTION :
    checkMenuItem(ID_VIEW_SCALE_Y_NORMALDIST, true);
    break;
  case AXIS_DATE                :
    checkMenuItem(ID_VIEW_SCALE_Y_DATETIME          , true);
    break;
  }
  m_YAxisType = type;
}

void CShowGrafView::setGraphStyle(GraphStyle newStyle) {
  checkMenuItem(ID_VIEW_STYLE_CURVE, false);
  checkMenuItem(ID_VIEW_STYLE_POINT, false);
  checkMenuItem(ID_VIEW_STYLE_CROSS, false);
  switch(newStyle) {
  case GSCURVE :
    checkMenuItem(ID_VIEW_STYLE_CURVE, true); 
    break;
  case GSPOINT :
    checkMenuItem(ID_VIEW_STYLE_POINT, true);
    break;
  case GSCROSS :
    checkMenuItem(ID_VIEW_STYLE_CROSS, true);
    break;
  }
  m_grafStyle = newStyle;
  m_graphArray.setStyle(m_grafStyle);
}

void CShowGrafView::setRetainAspectRatio(bool retain) {
  m_coordinateSystem.setRetainAspectRatio(retain);
}

void CShowGrafView::addDiffEquationGraph(DiffEquationGraphParameters &param) {
  m_graphArray.add(new DiffEquationGraph(param));
  m_graphArray.select(m_graphArray.size()-1);
  initScaleIfSingleGraph();
  Invalidate(TRUE);
}

void CShowGrafView::addFunctionGraph(FunctionGraphParameters &param) {
  m_graphArray.add(new FunctionGraph(param));
  m_graphArray.select(m_graphArray.size()-1);
  initScaleIfSingleGraph();
  Invalidate(TRUE);
}

void CShowGrafView::addParametricGraph(ParametricGraphParameters &param) {
  m_graphArray.add(new ParametricGraph(param));
  m_graphArray.select(m_graphArray.size()-1);
  initScaleIfSingleGraph();
  Invalidate(TRUE);
}

void CShowGrafView::addIsoCurveGraph(IsoCurveGraphParameters &param) {
  m_graphArray.add(new IsoCurveGraph(param));
  m_graphArray.select(m_graphArray.size()-1);
  initScaleIfSingleGraph();
  Invalidate(TRUE);
}

void CShowGrafView::setTrigonometricMode(TrigonometricMode mode) {
  for(size_t i = 0; i < m_graphArray.size(); i++) {
    Graph &g = m_graphArray[i].getGraph();
    if(g.getType() == FUNCTIONGRAPH) {
      ((FunctionGraph&)g).setTrigonometricMode(mode);
    }
  }
}

void CShowGrafView::startPolynomialFitThread() {
  stopFitThread();
  const GraphItem *item = m_graphArray.getSelectedItem();
  if(item && item->getGraph().isPointGraph()) {
    m_fitThread = ::startFitThread(POLYNOMIAL_FIT, m_graphArray.getDataRange().getXInterval(), ((PointGraph&)(item->getGraph())).getDataPoints(), *this);
  }
}

void CShowGrafView::startCustomFitThread() {
  stopFitThread();
  const GraphItem *item = m_graphArray.getSelectedItem();
  if(item && item->getGraph().isPointGraph()) {
    m_fitThread = ::startFitThread(CUSTOM_FIT, m_graphArray.getDataRange().getXInterval(), ((PointGraph&)(item->getGraph())).getDataPoints(), *this);
  }
}

void CShowGrafView::stopFitThread() {
  if(m_fitThread) {
    m_fitThread->PostThreadMessage(WM_QUIT,0,0);
    m_fitThread = NULL;
  }
}

COLORREF getShiftedColor(COLORREF c) {
  unsigned int r = GetRValue(c);
  unsigned int g = GetGValue(c);
  unsigned int b = GetBValue(c);
  if(r == g && g == b) {
    r+=50;
    g+=50;
    b+=50;
  }
  return RGB(g,b,r);
}

void CShowGrafView::makeExpoFit() {
  const GraphItem *item = m_graphArray.getSelectedItem();
  if(item && item->getGraph().isPointGraph()) {
    const DoubleInterval &yRange = item->getGraph().getDataRange().getYInterval();
    if(sign(yRange.getFrom()) != sign(yRange.getTo())) {
      MessageBox(_T("There are both negative and positive y-values in the dataset"), _T("Error"),MB_ICONEXCLAMATION);
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
    param.setName(format(_T("Exponential fit of %s"), item->getPartialName().cstr()));
    param.m_color = getShiftedColor(item->getGraph().getParam().m_color);
    param.m_expr  = format(_T("a = %lg;\r\nb = %lg;\r\na * b^x"), a, b);
    param.m_interval = item->getGraph().getDataRange().getXInterval();
    addFunctionGraph(param);
    Invalidate(TRUE);
  }
}

void CShowGrafView::makePotensFit() {
  const GraphItem *item = m_graphArray.getSelectedItem();
  if(item && item->getGraph().isPointGraph()) {
    const DoubleInterval &yRange = item->getGraph().getDataRange().getYInterval();
    if(sign(yRange.getFrom()) != sign(yRange.getTo())) {
      MessageBox(_T("There are both negative and positive y-values in the dataset"), _T("Error"), MB_ICONEXCLAMATION);
      return;
    }
    const DoubleInterval &xRange = item->getGraph().getDataRange().getXInterval();
    if(xRange.getMin() < 0) {
      MessageBox(_T("There cannot be negative x-values"), _T("Error"), MB_ICONEXCLAMATION);
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
    param.setName(format(_T("potens fit of %s"), item->getPartialName().cstr()));
    param.m_color = getShiftedColor(item->getGraph().getParam().m_color);
    param.m_expr  = format(_T("a = %lg;\r\nb = %lg;\r\na * x^b"), a, b);
    param.m_interval = item->getGraph().getDataRange().getXInterval();
    addFunctionGraph(param);
    Invalidate(TRUE);
  }
}

