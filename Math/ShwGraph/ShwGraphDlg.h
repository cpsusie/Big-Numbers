#pragma once

#include <Stack.h>
#include <MFCUtil/LayoutManager.h>
#include <MFCUTil/Viewport2D.h>

typedef enum {
  STATE_IDLE
 ,STATE_ZOOM_START
 ,STATE_ZOOM_RESIZE
 ,STATE_LBUTTONDOWN
 ,STATE_MOVEPOINT
} DialogState;

#define BLACK RGB(0,0,0)
#define WHITE RGB(255,255,255)

#define MAX_ZOOM_COUNT 10

typedef CompactArray<DataPoint> DataPointArray;

class CShwGraphDlg : public CDialog {
private:
  HICON               m_hIcon;
  HACCEL              m_accelTable;
  CBrush              m_whiteBrush;
  HCURSOR             m_systemCursor;
  SimpleLayoutManager m_layout;
  CCoordinateSystem   m_cs;

public:
  CShwGraphDlg(CWnd *pParent = NULL);

  void readData(FILE *f);
  void readTextFile(const String &fname);
  void addPoint(const Point2DP &p);
  void removePoint(intptr_t index);
  void movePoint(  intptr_t index, const CPoint &point);
  intptr_t findDataPoint(const CPoint &point);
  void adjustTransform();
  inline RectangleTransformation &getTr() {
    return m_cs.getTransformation();
  }
  inline const RectangleTransformation &getTr() const {
    return m_cs.getTransformation();
  }
  CPoint mouseToCS(const CPoint &p) const;
  bool showFunctionIsChecked();
  bool showD1IsChecked();
  bool showD2IsChecked();
  bool showD3IsChecked();
  bool gridIsChecked();
  bool showMousePositionIsChecked();
  bool showCoefIsChecked();
  bool autoUpdateScreen();
  void drawDerived( Viewport2D &vp, int d);
  void drawFunction(Viewport2D &vp, Function &f);
  void showCoef();
  void showMousePosition(const CPoint &p);
  void setChecked(int id1, int id2);
  void setInterpolType(int id);
  FittingType getFittingType();
  bool uselssd() const { return m_iptype == ID_EDIT_LEASTSUMOFSQUARES; }
  void pushZoom(CRect &rect);
  void pushZoom(double minx, double maxx, double miny, double maxy);
  void popZoom();
  DoubleInterval getXInterval() const;
  DoubleInterval findYInterval(const DoubleInterval &xInterval);
  void           setState(DialogState newstate);
  double         evaluate(      double x);
  void           solve();

  void setCurrentFont(CFont *newfont);

  Point2DArray              m_points;
  DataPointArray            m_dataPoints;
  mutable DataRange         m_pointRange;
  mutable bool              m_needUpdateRange;
  bool                      m_needSolve;
  bool                      m_canDraw;
  int                       m_iptype;
  DialogState               m_state;
  CRect                     m_zoomRect;
  Stack<DataRange>          m_zoomStack;
  intptr_t                  m_movePoint;
  CFont                     m_defaultFont,*m_currentFont;
  int                       m_preferredDegree;
  DataFit                   m_dataFit;

  enum { IDD = IDD_SHWGRAPH_DIALOG };

public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDropFiles(HDROP hDropInfo);
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(  UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(  UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnFileOpen();
    afx_msg void OnFileQuit();
    afx_msg void OnEditDegree();
    afx_msg void OnEditIncrdegree();
    afx_msg void OnEditDecrdegree();
    afx_msg void OnEditLeastSumOfSquares();
    afx_msg void OnEditCubicSplineNatural();
    afx_msg void OnEditCubicSplineMatchdydx();
    afx_msg void OnEditCubicSplineINoEndPoints();
    afx_msg void OnEditAkimaSpline();
    afx_msg void OnEditFritschCarlsonSpline();
    afx_msg void OnEditClear();
    afx_msg void OnToolsShowFunction();
    afx_msg void OnToolsShowD1();
    afx_msg void OnToolsShowD2();
    afx_msg void OnToolsShowD3();
    afx_msg void OnToolsShowMousePosition();
    afx_msg void OnToolsZoomin();
    afx_msg void OnToolsZoomout();
    afx_msg void OnToolsSetinterval();
    afx_msg void OnToolsGrid();
    afx_msg void OnToolsFont();
    afx_msg void OnToolsDisplaycoefficients() ;
    afx_msg void OnToolsGeneratecfunction();
    afx_msg void OnHelpAbout();
    DECLARE_MESSAGE_MAP()
};

