#pragma once

#include <CompactStack.h>
#include "ShowGrafDoc.h"
#include "CoordinateSystemContainer.h"

class CMainFrame;
class MouseTool;

#define FOR_BOTH_AXIS(i) for(AxisIndex i = XAXIS_INDEX; i <= YAXIS_INDEX; ((int&)i)++)

class CShowGrafView : public CFormView, public FunctionPlotter, public CoordinateSystemContainer {
private:
  enum { IDD = IDD_SHOWGRAFVIEW };

  CCoordinateSystem           m_coordinateSystem;
  CFont                       m_axisFont;
  CFont                       m_buttonFont;
  CompactStack<MouseTool*>    m_toolStack;
  void plotFunction(Function &f, const DoubleInterval &interval, COLORREF color);
  void plotFunction(Function &f, COLORREF color);
  bool paintAll(CDC &dc, const CRect &rect);
  bool isMenuItemChecked(int id);
  void enableMenuItem(int id, bool enabled);
  void checkMenuItem( int id, bool checked);
  inline bool ptInPanel(int id, const CPoint &point) const {
    return getRelativeClientRect(this, id).PtInRect(point);
  }
  inline bool hasMouseTool() const {
    return !m_toolStack.isEmpty();
  }
  inline MouseTool &getCurrentTool() const {
    return *m_toolStack.top();
  }
  void clearToolStack();
#if defined(_DEBUG)
  void dumpToolStack() const;
#define DUMPTOOLSTACK() dumpToolStack()
#else
#define DUMPTOOLSTACK()
#endif
  CShowGrafDoc *getDoc();

  inline CMainFrame *getMainFrame() {
    return (CMainFrame*)GetParent();
  }

  inline const CMainFrame *getMainFrame() const {
    return (const CMainFrame*)GetParent();
  }

protected: // create from serialization only
  DECLARE_DYNCREATE(CShowGrafView)

  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(  UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(  UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt);
  afx_msg void OnSize(       UINT nType, int cx, int cy);
  afx_msg void OnDestroy();
  DECLARE_MESSAGE_MAP()

public:
    CShowGrafView();
    virtual ~CShowGrafView();
    void initScale();
    void clear();

    // virtual functions that implements CoordinateSystemContainer interface
    CWnd *getWin() {
      return this;
    }
    CCoordinateSystem &getCoordinateSystem() {
      return m_coordinateSystem;
    }
    CFont &getAxisFont() {
      return m_axisFont;
    }
    CFont &getButtonFont() {
      return m_buttonFont;
    }
    void repaint() {
      Invalidate();
    }
    void setBackgroundColor(COLORREF color) {
      getCoordinateSystem().setBackGroundColor(color);
    }
    void setRetainAspectRatio(bool retain) {
      m_coordinateSystem.setRetainAspectRatio(retain);
    }
    RectangleTransformation &getTransformation() {
      return getCoordinateSystem().getTransformation();
    }
    DataRange getDataRange() {
      return m_coordinateSystem.getDataRange();
    }
    void setDataRange(const DataRange &dr, bool makeSpace) {
      getCoordinateSystem().setDataRange(dr, makeSpace);
    }
    DoubleInterval getAxisInterval(AxisIndex axis) {
      return (axis == XAXIS_INDEX) ? getDataRange().getXInterval() : getDataRange().getYInterval();
    }
    void setAxisType(AxisIndex axis, AxisType type) {
      getCoordinateSystem().setAxisType(axis, type);
    }
    void setAxisColor(AxisIndex axis, COLORREF color) {
      getCoordinateSystem().setAxisColor(axis, color);
    }
    void addFunctionGraph(FunctionGraphParameters &param);
    void pushMouseTool(MouseToolType toolType);
    void popMouseTool();
    virtual void OnInitialUpdate();
    virtual void OnDraw(CDC *pDC);  // overridden to draw this view
    virtual BOOL OnPreparePrinting(          CPrintInfo *pInfo);
    virtual void OnBeginPrinting(  CDC *pDC, CPrintInfo *pInfo);
    virtual void OnEndPrinting(    CDC *pDC, CPrintInfo *pInfo);

#if defined(_DEBUG)
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext &dc) const;
#endif
};
