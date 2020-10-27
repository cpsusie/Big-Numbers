#pragma once

#include "PolygonTools.h"

typedef enum {
  POLYGONMODE
 ,LINEINTERSECTIONMODE
} DialogMode;

class CPolygondistDlg : public CDialog {
private:
  HICON        m_hIcon;
  HACCEL       m_accelTable;
  DialogMode   m_mode;
  polygon      m_poly;
  int          m_maxIndex1, m_maxIndex2;
  Point2DArray m_point4;
  CPoint       dlgToPanel(CPoint p) const;
  void         showInfo(_In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void         clearMax();
  void         clearPoly();
  void         clearLines();
  void         clearPanel();
  void         setDialogMode(DialogMode mode);
  void OnLButtonDownLines(UINT nFlags, CPoint point);
  void OnLButtonUpLines(  UINT nFlags, CPoint point);
  void OnRButtonDownLines(UINT nFlags, CPoint point);
  void OnLButtonDownPoly (UINT nFlags, CPoint point);
  void OnLButtonUpPoly(   UINT nFlags, CPoint point);
  void OnRButtonDownPoly( UINT nFlags, CPoint point);
public:
  CPolygondistDlg(CWnd *pParent = nullptr);

  enum { IDD = IDD_POLYGONDIST_DIALOG };

protected:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  virtual void OnCancel();
  afx_msg void OnSysCommand( UINT nID, LPARAM lParam);
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnPaint();
  afx_msg void OnClose();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(  UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(  UINT nFlags, CPoint point);
  afx_msg void OnFileExit();
  afx_msg void OnToolsIntersectionOfLines();
  afx_msg void OnToolsDrawPolygon();
  afx_msg void OnToolsFindMaxDistance();
  afx_msg void OnToolsFindIntersectionOfLines();
  afx_msg void OnToolsClear();
  afx_msg void OnHelpAboutPolygonDist();
  DECLARE_MESSAGE_MAP()
};
