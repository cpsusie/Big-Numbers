#include "stdafx.h"
#include "ShowGrafDoc.h"
#include "ShowGrafView.h"

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
  m_firstDraw        = true;
  m_mouseTool        = TOOL_DRAG;
  m_dragging         = false;
}

CShowGrafView::~CShowGrafView() {
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
  const GraphArray &ga = getDoc()->getGraphArray();
  WINDOWPLACEMENT wpl;
  systemPanel->GetWindowPlacement(&wpl);
  int buttonPanelWidth = ga.getMaxButtonWidth(dc,*buttonFont) + 30;

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
  m_coordinateSystem.setGrid(theApp.getMainWindow()->hasGrid());
  try {
    m_coordinateSystem.OnPaint();
    CClientDC dc(&m_coordinateSystem);
    m_coordinateSystem.setDC(dc);
    ga.paint(m_coordinateSystem, *buttonFont, getRelativeClientRect(this,IDC_BUTTONPANEL));
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

void CShowGrafView::OnDraw(CDC *pDC) {
  try {
    if(m_firstDraw) {
      m_coordinateSystem.substituteControl(this, IDC_SYSTEMPANEL);
      m_firstDraw = false;
    }
    const CRect rect = getClientRect(this);

    if(paintAll(*pDC, rect, &m_axisFont, &m_buttonFont)) {
      DoubleInterval maxNormInterval(0,1);
      const DataRange dr = m_coordinateSystem.getDataRange();
      enableMenuItem(ID_VIEW_SCALE_X_LOGARITHMIC       , dr.getMinX() > 0);
      enableMenuItem(ID_VIEW_SCALE_Y_LOGARITHMIC       , dr.getMinY() > 0);
      enableMenuItem(ID_VIEW_SCALE_X_NORMALDIST        , maxNormInterval.contains(dr.getXInterval()));
      enableMenuItem(ID_VIEW_SCALE_Y_NORMALDIST        , maxNormInterval.contains(dr.getYInterval()));
      checkMenuItem( ID_VIEW_RETAINASPECTRATIO         , m_coordinateSystem.isRetainingAspectRatio());
      enableMenuItem(ID_VIEW_RETAINASPECTRATIO         , m_coordinateSystem.canRetainAspectRatio()  );
      checkMenuItem( ID_VIEW_ROLLAVG                   , getDoc()->getRollAvg()                     );
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
  switch(m_mouseTool) {
  case TOOL_DRAG              :
    if(getClientRect(this, IDC_SYSTEMPANEL).PtInRect(point)) {
      lbuttonDownDragging(nFlags, point);
    }
    break;
  case TOOL_FINDSEARCHINTERVAL:
    if(getClientRect(this, IDC_SYSTEMPANEL).PtInRect(point)) {
      lbuttonDownMarkInterval(nFlags, point);
    }
    break;
  }
}

void CShowGrafView::OnLButtonUp(UINT nFlags, CPoint point) {
  __super::OnLButtonUp(nFlags, point);
  switch(m_mouseTool) {
  case TOOL_DRAG              :
    lbuttonUpDragging(nFlags, point);
    break;
  case TOOL_FINDSEARCHINTERVAL:
    lbuttonUpMarkInterval(nFlags, point);
    break;
  }
}

void CShowGrafView::OnMouseMove(UINT nFlags, CPoint point) {
  __super::OnMouseMove(nFlags, point);
  switch(m_mouseTool) {
  case TOOL_DRAG              :
    mouseMoveDragging(nFlags, point);
    break;
  case TOOL_FINDSEARCHINTERVAL:
    mouseMoveMarkInterval(nFlags, point);
    break;
  }
  theApp.getMainWindow()->showPosition(m_coordinateSystem.getTransformation().backwardTransform((Point2DP)point));
}

void CShowGrafView::lbuttonDownDragging(UINT nFlags, const CPoint &point) {
  m_dragging           = true;
  m_mouseDownPoint     = point;
  m_mouseDownTransform = m_coordinateSystem.getTransformation();
  CRect cr             = getClientRect(this, IDC_SYSTEMPANEL);
  ClientToScreen(&cr);
  ClipCursor(&cr);
  setWindowCursor(this, MAKEINTRESOURCE(OCR_HAND));
}

void CShowGrafView::lbuttonUpDragging(UINT nFlags, const CPoint &point) {
  m_dragging = false;
  setWindowCursor(this, MAKEINTRESOURCE(OCR_NORMAL));
  ClipCursor(NULL);
}

void CShowGrafView::mouseMoveDragging(UINT nFlags, const CPoint &point) {
  if(m_dragging) {
    if(nFlags && MK_LBUTTON) {
      Rectangle2D   fr         = m_mouseDownTransform.getFromRectangle();
      Point2D       startPoint = m_mouseDownTransform.backwardTransform((Point2DP)m_mouseDownPoint);
      Point2D       newPoint   = m_mouseDownTransform.backwardTransform((Point2DP)point);
      const Point2D dp         = newPoint - startPoint;
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

void CShowGrafView::lbuttonDownMarkInterval(UINT nFlags, const CPoint &point) {
  m_mouseDownPoint = point;
  m_dragging       = true;
  CRect cr   = getClientRect(this, IDC_SYSTEMPANEL);
  m_dragRect = CRect(m_mouseDownPoint.x, cr.bottom, m_mouseDownPoint.x, cr.top);
  ClientToScreen(&cr);
  ClipCursor(&cr);
  CClientDC(GetDlgItem(IDC_SYSTEMPANEL)).DrawDragRect(&m_dragRect, CSize(1,1), NULL, CSize(1,1));
}

void CShowGrafView::lbuttonUpMarkInterval(UINT nFlags, const CPoint &point) {
  CClientDC(GetDlgItem(IDC_SYSTEMPANEL)).DrawDragRect(&m_dragRect, CSize(1,1), NULL, CSize(1,1));
  m_dragging = false;
  ClipCursor(NULL);
  getMainFrame()->PostMessage(ID_MSG_SEARCHINTERVAL, m_dragRect.left,m_dragRect.right);
}

void CShowGrafView::mouseMoveMarkInterval(UINT nFlags, const CPoint &point) {
  if(m_dragging) {
    if(nFlags && MK_LBUTTON) {
      const CRect cr      = getClientRect(this, IDC_SYSTEMPANEL);
      const CRect newRect = CRect(m_mouseDownPoint.x, cr.bottom, point.x, cr.top);
      CClientDC(GetDlgItem(IDC_SYSTEMPANEL)).DrawDragRect(&newRect, CSize(1,1), &m_dragRect, CSize(1,1));
      m_dragRect = newRect;
    } else {
      CClientDC(GetDlgItem(IDC_SYSTEMPANEL)).DrawDragRect(&m_dragRect, CSize(1,1), NULL, CSize(1,1));
      m_dragging = false;
      ClipCursor(NULL);
    }
  }
}

void CShowGrafView::OnRButtonDown(UINT nFlags, CPoint point) {
  GraphArray &ga = getDoc()->getGraphArray();
  if(ga.OnLButtonDown(nFlags, point, m_coordinateSystem.getTransformation())) {
    CMenu menu;
    if(!menu.LoadMenu(IDR_MENUSELECTGRAF)) {
      showWarning(_T("Loadmenu failed"));
      return;
    }
    removeMenuItem(menu, ga.getSelectedItem()->getGraph().isVisible() ? ID_SELECTMENU_SHOW : ID_SELECTMENU_HIDE);
    ClientToScreen(&point);
    menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x+10,point.y, theApp.getMainWindow());
  } else {
    __super::OnRButtonDown(nFlags, point);
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
  return __super::OnMouseWheel(nFlags, zDelta, pt);
}

void CShowGrafView::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  ShowScrollBar(SB_BOTH,FALSE);
}

void CShowGrafView::clear() {
  initScale();
  Invalidate();
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
  const AxisType        xt      = theApp.getMainWindow()->getXAxisType();
  const AxisType        yt      = theApp.getMainWindow()->getYAxisType();
  if(options.m_rangeSpecified) {
    dr = options.m_explicitRange;
  } else if(ga.size() == 0) {
    dr = CCoordinateSystem::getDefaultDataRange(xt, yt);
  } else {
    dr = ga.getDataRange();
  }
  int makeSpaceFlags = ((xt == AXIS_LINEAR) ? X_AXIS : 0) | ((yt == AXIS_LINEAR) ? Y_AXIS : 0);
  m_coordinateSystem.setFromRectangle(dr, makeSpaceFlags);
  setXAxisType(xt);
  setYAxisType(yt);
}
