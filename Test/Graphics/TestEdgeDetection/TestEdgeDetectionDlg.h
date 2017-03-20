#pragma once

#include <Thread.h>
#include <CompactStack.h>
#include <MFCUtil/Layoutmanager.h>
#include <MatrixTemplate.h>
#include "EdgeMatrix.h"

#define PIXELSIZE 14
#define MARKSIZE  6
#define MARGIN    4


#define RED   RGB(255,  0,  0)
#define BLUE  RGB( 38, 96,179)
#define GREEN RGB(  0,255,  0)
#define WHITE RGB(255,255,255)
#define BLACK RGB(  0,  0,  0)

#define COMPLEMENTCOLOR(c) RGB(255-GetRValue(c),255-GetGValue(c), 255-GetBValue(c))

class PixelInfo {
public:
  COLORREF m_color;
  COLORREF m_markColor;
  bool     m_marked;
  PixelInfo(int id=0) : m_color(BLUE), m_markColor(BLACK), m_marked(false) {
  }
};

class CTestEdgeDetectionDlg;

class FollowEdgeThread : public Thread {
private:
  CTestEdgeDetectionDlg &m_dlg;
  const CRect            m_currentRect;
  const CPoint           m_startPoint;
  FillInfo              *m_fillInfo;
  void pause();
  bool followBlackEdge(const CPoint &p);
  void fillInnerArea(PointSet &innerSet);
public:
  FollowEdgeThread(CTestEdgeDetectionDlg *dlg, const CPoint &startPoint);
  unsigned int run();
};

String toString(const PixelInfo &pi);

typedef MatrixTemplate<PixelInfo> PixelMatrix;

class CTestEdgeDetectionDlg : public CDialog {
private:
  HICON               m_hIcon;
  HACCEL              m_accelTable;
  SimpleLayoutManager m_layoutManager;
  CWnd               *m_pixelWindow;
  CClientDC          *m_dc;
  CSize               m_logicalSize;
  PixelMatrix         m_pixelMatrix, m_savedCopy;
  CPoint              m_lastMousePoint;
  COLORREF            m_drawColor;
  Direction           m_currentSearchDir;
  CPoint              m_arrowPoint;
  Direction           m_arrowDirection;
  CPoint              m_edgeMatrixCenter;
  bool                m_breakPoint;
  bool                m_doFill;
  HBITMAP             m_currentDirBitmap;
  FollowEdgeThread   *m_edgeThread;
  FillInfo           *m_fillInfo;

  void adjustPixelMatrix();
  CTestEdgeDetectionDlg &paintCircel(const CPoint &center, int r, COLORREF color, bool filled=true);
  CTestEdgeDetectionDlg &paintLine(const CPoint &p1, const CPoint &p2, COLORREF color);
  inline CTestEdgeDetectionDlg &paintLine(int x1, int y1, int x2, int y2, COLORREF color) {
    return paintLine(CPoint(x1,y1), CPoint(x2,y2), color);
  }
  void paintPointSet(const PointSet &ps, COLORREF color);
  void setSearchDir(Direction dir);
  void setBackground(COLORREF color);
  void paintPixelMatrix();
  void paintPixel(unsigned int x, unsigned int y);
  void drawEdgeMatrix(const CPoint &center, COLORREF color);
  void showCount(int count);
  CPoint getPixelPointFromMousePoint(const CPoint &p) const;
  void paintPixelMark(unsigned int x, unsigned int y);
  inline bool isMarked(unsigned int x, unsigned int y) const {
    return m_pixelMatrix(x, y).m_marked;
  }
  inline bool isMarked(const CPoint &p) const {
    return m_pixelMatrix(p.x, p.y).m_marked;
  }
  inline bool isPointInside(const CPoint &p) const{
    return ((UINT)p.x < (UINT)m_logicalSize.cx) && ((UINT)p.y < (UINT)m_logicalSize.cy);
  }
  void setMark(unsigned int x, unsigned int y, bool marked=true, COLORREF color = BLACK);
  void resetMarkMatrix();
  void savePixelMatrix();
  void restorePixelMatrix();
  void releaseFillInfo();
  void enableContinueInternal(bool enabled);
  CPoint findEdge(  const CPoint &p);
  void followEdge(  const CPoint &p, Direction searchDir);
  void killEdgeThread();
  bool isEdgeThreadActive() const {
    return m_edgeThread && m_edgeThread->stillActive();
  }
  CDC &getDC();
public:
    CTestEdgeDetectionDlg(CWnd *pParent = NULL);
    
    enum { IDD = IDD_TESTEDGEDETECTION_DIALOG };

protected:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);

public:
  const CSize &getWindowSize() const {
    return m_logicalSize;
  }
  void setPixel(unsigned int x, unsigned int y, COLORREF c);
  inline void setPixel(const CPoint &p, COLORREF c) {
    setPixel(p.x, p.y, c);
  }
  inline COLORREF getPixel(unsigned int x, unsigned int y) const {
    return m_pixelMatrix(x, y).m_color;
  }
  inline COLORREF getPixel(const CPoint &p) const {
    return getPixel(p.x, p.y);
  }
  inline void markPixel(unsigned int x, unsigned int y, COLORREF color = BLACK) {
    setMark(x,y,true,color);
  }
  inline void markPixel(const CPoint &p, COLORREF color = BLACK) {
    setMark(p.x,p.y,true,color);
  }
  inline void unmarkPixel(unsigned int x, unsigned int y) {
    setMark(x,y,false);
  }
  inline void unmarkPixel(const CPoint &p) {
    setMark(p.x, p.y, false);
  }
  void unmarkAllPixels();
  CTestEdgeDetectionDlg &paintArrow( const CPoint &p, Direction dir, COLORREF color = RGB(255,255,0));
  CTestEdgeDetectionDlg &removeArrow();
  CTestEdgeDetectionDlg &paintEdgeMatrix(const CPoint &center);
  CTestEdgeDetectionDlg &removeEdgeMatrix();
  void flush();
  void enableContinueThread(bool enabled);
  Direction getSearchDir() const {
    return m_currentSearchDir;
  }
  inline bool hasBreakPoint() const {
    return m_breakPoint;
  }
  inline bool doFill() const {
    return m_doFill;
  }
  FillInfo &getFillInfo();
protected:
    
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg HCURSOR OnQueryDragIcon();
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
    afx_msg void OnClose();
    virtual void OnCancel();
    virtual void OnOK();
    afx_msg void OnLButtonDown(  UINT nFlags  , CPoint point);
    afx_msg void OnRButtonDown(  UINT nFlags  , CPoint point);
    afx_msg void OnMouseMove(    UINT nFlags  , CPoint point);
    afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
    afx_msg void OnFileExit();
    afx_msg void OnEditReset();
    afx_msg void OnEditCircel();
    afx_msg void OnEditLine();
    afx_msg void OnEditN();
    afx_msg void OnEditS();
    afx_msg void OnEditE();
    afx_msg void OnEditW();
    afx_msg void OnEditNoSearch();
    afx_msg void OnEditGo();
    afx_msg void OnEditToggleBreakpoint();
    afx_msg void OnEditSingleStep();
    afx_msg void OnEditDoFill();
    afx_msg void OnEditShowFillInfo();
    afx_msg void OnEditTestArrrow();
    afx_msg void OnEditTestEdgeMatrix();
    afx_msg LRESULT OnMsgEnableContinueThread(WPARAM wp, LPARAM lp);
    DECLARE_MESSAGE_MAP()
};

