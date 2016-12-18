#pragma once

#include <MFCUtil/Coordinatesystem/CoordinateSystem.h>
#include "GraphArray.h"
#include "ExpressionGraph.h"
#include "IsoCurveGraph.h"
#include "DiffEquationGraph.h"
#include "CustomFitThread.h"

class CShowGrafView : public CFormView, public FunctionPlotter {
private:
    enum { IDD = IDD_SHOWGRAFVIEW };

    CCoordinateSystem           m_coordinateSystem;
    bool                        m_firstDraw;
    CFont                       m_axisFont;
    CFont                       m_buttonFont;
    GraphArray                  m_graphArray;
    GraphStyle                  m_grafStyle;
    AxisType                    m_XAxisType,m_YAxisType;
    DataReader                 *m_xReader, *m_yReader;
    int                         m_rollSize;
    COLORREF                    m_backgroundColor,m_axisColor;
    bool                        m_onePerLine;
    bool                        m_rangeSpecified;
    bool                        m_dragging;
    RectangleTransformation     m_mouseDownTransform;
    CPoint                      m_mouseDownPoint;
    bool                        m_xRelativeToFirst, m_yRelativeToFirst;
    DataRange                   m_explicitRange;
    CCustomFitThread           *m_fitThread;
    ExpressionGraphParameters   m_plotParam;
    IsoCurveGraphParameters     m_isoCurveParam;
    DiffEquationGraphParameters m_diffEqParam;

    void checkMenuItem(int id, bool checked);
    bool isMenuItemChecked(int id);
    void enableMenuItem(int id, bool enabled);
    void initScaleIfSingleGraph();
    void plotFunction(Function &f, const DoubleInterval &interval, COLORREF color);
    void plotFunction(Function &f, COLORREF color);
    bool paintAll(CDC &dc, const CRect &rect, CFont *axisFont, CFont *buttonFont);
    void stopFitThread();

    void startDragging(const CPoint &point);
    void stopDragging();
protected: // create from serialization only
    DECLARE_DYNCREATE(CShowGrafView)

    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(  UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(  UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnSize(       UINT nType, int cx, int cy);
    afx_msg void OnSelectMenuDelete();
    afx_msg void OnSelectMenuEdit();
    afx_msg void OnSelectMenuHide();
    afx_msg void OnSelectMenuShow();
  DECLARE_MESSAGE_MAP()

public:
    CShowGrafView();
    virtual ~CShowGrafView();
    void init();
    void initScale();
    void clear();
    void addGraphFromFile(const String &fileName);
    void readDataFile(    const String &fileName);
    void readExprFile(    const String &fileName);
    void readIsoFile(     const String &fileName);
    void readDiffEqFile(  const String &fileName);
    void setGraphStyle(GraphStyle newStyle);
    void setXAxisType(AxisType type);
    void setYAxisType(AxisType type);
    void setRollingAverage(bool on);
    void setRetainAspectRatio(bool retain);

    CCoordinateSystem &getCoordinateSystem() {
      return m_coordinateSystem;
    }

    int getRollSize() const {
      return m_rollSize;
    }

    void setRollSize(int value);

    ExpressionGraphParameters &getPlotParam() {
      return m_plotParam;
    }

    IsoCurveGraphParameters &getIsoCurveParam() {
      return m_isoCurveParam;
    }

    DiffEquationGraphParameters &getDiffEquationParam() {
      return m_diffEqParam;
    }

    void addExpressionGraph(  ExpressionGraphParameters   &param);
    void addIsoCurveGraph(    IsoCurveGraphParameters     &param);
    void addDiffEquationGraph(DiffEquationGraphParameters &param);
    void setTrigonometricMode(TrigonometricMode mode);
    void startCustomFitThread();
    void startPolynomialFitThread();
    void makeExpoFit();
    void makePotensFit();
  public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    protected:
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

};
