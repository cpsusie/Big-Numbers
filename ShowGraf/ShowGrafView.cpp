#include "stdafx.h"
#include "ShowGrafDoc.h"
#include "ShowGrafView.h"
#include "MouseTool.h"
#ifdef _DEBUG
#include <DebugLog.h>
#endif

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
    ON_WM_DESTROY()
    ON_COMMAND(ID_FILE_PRINT        , __super::OnFilePrint       )
    ON_COMMAND(ID_FILE_PRINT_DIRECT , __super::OnFilePrint       )
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, __super::OnFilePrintPreview)
END_MESSAGE_MAP()

CShowGrafView::CShowGrafView() : CFormView(IDD) {
  m_buttonFont.CreateFont(10, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
                         ,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
                         ,DEFAULT_PITCH | FF_MODERN
                         ,_T("Arial") );
  m_axisFont.CreateFont(  10, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
                         ,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
                         ,DEFAULT_PITCH | FF_MODERN
                         ,_T("Arial") );
}

void CShowGrafView::OnDestroy() {
  clearToolStack();
  __super::OnDestroy();
}

CShowGrafView::~CShowGrafView() {
}

void CShowGrafView::OnInitialUpdate() {
  __super::OnInitialUpdate();

  m_coordinateSystem.substituteControl(this, IDC_SYSTEMPANEL);
  pushMouseTool(IDLETOOL);
}

bool CShowGrafView::paintAll(CDC &dc, const CRect &rect) {
  CWnd *systemPanel = GetDlgItem(IDC_SYSTEMPANEL);
  CWnd *buttonPanel = GetDlgItem(IDC_BUTTONPANEL);

  if(systemPanel == NULL || buttonPanel == NULL) {
    return false;
  }
  const GraphArray &ga = getDoc()->getGraphArray();
  WINDOWPLACEMENT wpl;
  systemPanel->GetWindowPlacement(&wpl);
  int buttonPanelWidth = ga.getMaxButtonWidth(dc,m_buttonFont) + 30;

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

  m_coordinateSystem.SetFont(&m_axisFont,FALSE);
  FOR_BOTH_AXIS(i) {
    m_coordinateSystem.setShowAxisValues( i, theApp.getMainWindow()->getShowValues(    i));
    m_coordinateSystem.showAxisValueMarks(i, theApp.getMainWindow()->getShowValueMarks(i));
    m_coordinateSystem.showAxisGridLines( i, theApp.getMainWindow()->getShowGridLines( i));
  }
  try {
    m_coordinateSystem.OnPaint();
    CClientDC dc(&m_coordinateSystem);
    ga.paintItems(dc, getButtonFont(), getRelativeClientRect(this,IDC_BUTTONPANEL));
    ga.paintPointArray(dc, getAxisFont());
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

void CShowGrafView::addFunctionGraph(FunctionGraphParameters &param) {
  getDoc()->addFunctionGraph(param);
}

void CShowGrafView::pushMouseTool(MouseToolType toolType) {
  switch(toolType) {
  case IDLETOOL            : m_toolStack.push(new IdleTool(      this)); break;
  case DRAGTOOL            : m_toolStack.push(new DragTool(     *m_toolStack.top())); break;
  case MOVEPOINTTOOL       : m_toolStack.push(new MovePointTool(*m_toolStack.top())); break;
  case FINDZEROTOOL        : m_toolStack.push(new FindZeroTool(  this)); break;
  case FINDMAXTOOL         : m_toolStack.push(new FindMaxTool(   this)); break;
  case FINDMINTOOL         : m_toolStack.push(new FindMinTool(   this)); break;
  case FINDINTERSECTIONTOOL:
  default                  :
    errorMessage(_T("Invalid MouseTool:%d"), toolType);
    break;
  }
  DUMPTOOLSTACK();
}

void CShowGrafView::popMouseTool() {
  MouseTool *mt = m_toolStack.pop();
  delete mt;
  DUMPTOOLSTACK();
}

#ifdef _DEBUG
void CShowGrafView::dumpToolStack() const {
  String result;
  for (int i = m_toolStack.getHeight(); i--;) {
    result += toString(m_toolStack.top(i)->getType()) + _T(" ");
  }
  debugLog(_T("ToolStack:%s\n"), result.cstr());
}
#endif

void CShowGrafView::clearToolStack() {
  while(!hasMouseTool()) {
    popMouseTool();
  }
}

void CShowGrafView::OnDraw(CDC *pDC) {
  try {
    const CRect rect = getClientRect(this);

    if(paintAll(*pDC, rect)) {
      DoubleInterval maxNormInterval(0,1);
      const DataRange dr = m_coordinateSystem.getDataRange();
      const bool xlogEnabled = dr.getMinX() > 0, ylogEnabled = dr.getMinY() > 0;
      enableMenuItem(ID_X_SCALE_LOGARITHMIC       , xlogEnabled);
      enableMenuItem(ID_Y_SCALE_LOGARITHMIC       , ylogEnabled);
      enableMenuItem(ID_BOTH_SCALE_LOGARITHMIC    , xlogEnabled && ylogEnabled);

      const bool xnormDistEnabled = maxNormInterval.contains(dr.getXInterval()), ynormDistEnabled = maxNormInterval.contains(dr.getYInterval());
      enableMenuItem(ID_X_SCALE_NORMALDIST        , xnormDistEnabled);
      enableMenuItem(ID_Y_SCALE_NORMALDIST        , ynormDistEnabled);
      enableMenuItem(ID_BOTH_SCALE_NORMALDIST     , xnormDistEnabled && ynormDistEnabled);
      checkMenuItem( ID_VIEW_RETAINASPECTRATIO    , m_coordinateSystem.isRetainingAspectRatio());
      enableMenuItem(ID_VIEW_RETAINASPECTRATIO    , m_coordinateSystem.canRetainAspectRatio()  );
      checkMenuItem( ID_VIEW_ROLLAVG              , getDoc()->getRollAvg()                     );
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

#ifdef _DEBUG
void CShowGrafView::AssertValid() const {
  __super::AssertValid();
}

void CShowGrafView::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
}

#endif //_DEBUG

void CShowGrafView::OnLButtonDown(UINT nFlags, CPoint point) {
  __super::OnLButtonDown(nFlags, point);
  if(hasMouseTool() && ptInPanel(IDC_SYSTEMPANEL, point)) {
    getCurrentTool().OnLButtonDown(nFlags, point);
  }
}

void CShowGrafView::OnLButtonUp(UINT nFlags, CPoint point) {
  __super::OnLButtonUp(nFlags, point);
  if(hasMouseTool()) getCurrentTool().OnLButtonUp(nFlags, point);
}

void CShowGrafView::OnMouseMove(UINT nFlags, CPoint point) {
  __super::OnMouseMove(nFlags, point);
  if(!ptInPanel(IDC_SYSTEMPANEL, point)) {
    theApp.getMainWindow()->updatePositionText(EMPTYSTRING);
  } else  {
    if(hasMouseTool()) {
      getCurrentTool().OnMouseMove(nFlags, point);
    }
    theApp.getMainWindow()->updatePositionText(m_coordinateSystem.getPointText(m_coordinateSystem.getMouseToSystem(point)));
  }
}

void CShowGrafView::OnRButtonDown(UINT nFlags, CPoint point) {
  GraphArray &ga = getDoc()->getGraphArray();
  if(ga.OnLButtonDown(nFlags, point)) {
    CMenu menu;
    if(!menu.LoadMenu(IDR_MENUSELECTGRAF)) {
      showWarning(_T("Loadmenu failed"));
      return;
    }
    const GraphItem *gi = ga.getSelectedGraphItem();
    if(gi) {
      removeMenuItem(menu, gi->getGraph().isVisible() ? ID_SELECTMENU_SHOW : ID_SELECTMENU_HIDE);
    } else {
      removeMenuItem(menu, ID_SELECTMENU_SHOW);
      removeMenuItem(menu, ID_SELECTMENU_HIDE);
    }
    ClientToScreen(&point);
    menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x+10,point.y, theApp.getMainWindow());
  } else {
    __super::OnRButtonDown(nFlags, point);
  }
}

BOOL CShowGrafView::OnMouseWheel(UINT nFlags, short zDelta, CPoint point) {
  CPoint pt = point;
  ScreenToClient(&pt);
  if(ptInPanel(IDC_SYSTEMPANEL, pt)) {
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
    m_coordinateSystem.getTransformation().zoom(tmppt, (zDelta < 0) ? -0.05 : 0.05, zoomFlags);
    Invalidate(FALSE);
  }
  return __super::OnMouseWheel(nFlags, zDelta, point);
}

void CShowGrafView::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  ShowScrollBar(SB_BOTH,FALSE);
}

void CShowGrafView::clear() {
  initScale();
  Invalidate();
  clearToolStack();
  pushMouseTool(IDLETOOL);
}

bool CShowGrafView::isMenuItemChecked(int id) {
  return ::isMenuItemChecked(theApp.getMainWindow(), id);
}

void CShowGrafView::enableMenuItem(int id, bool enabled) {
  ::enableMenuItem(theApp.getMainWindow(), id, enabled);
}

void CShowGrafView::checkMenuItem(int id, bool checked) {
  ::checkMenuItem(theApp.getMainWindow(), id, checked);
}

CShowGrafDoc *CShowGrafView::getDoc() {
  return theApp.getMainWindow()->getDoc();
}

void CShowGrafView::initScale() {
  const InitialOptions &options = getDoc()->getOptions();
  const GraphArray     &ga      = getDoc()->getGraphArray();
  DataRange             dr;
  const AxisType        xt      = theApp.getMainWindow()->getAxisType(XAXIS_INDEX);
  const AxisType        yt      = theApp.getMainWindow()->getAxisType(YAXIS_INDEX);
  if(options.m_rangeSpecified) {
    dr = options.m_explicitRange;
  } else if(ga.size() == 0) {
    dr = CCoordinateSystem::getDefaultDataRange(xt, yt);
  } else {
    dr = ga.getDataRange();
  }
  int makeSpaceFlags = ((xt == AXIS_LINEAR) ? X_AXIS : 0) | ((yt == AXIS_LINEAR) ? Y_AXIS : 0);
  m_coordinateSystem.setFromRectangle(dr, makeSpaceFlags);
  setAxisType(XAXIS_INDEX, xt);
  setAxisType(YAXIS_INDEX, yt);
}
