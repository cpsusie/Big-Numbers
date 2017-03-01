#pragma once

#include "PolygonTools.h"

class CPolygondistDlg : public CDialog {
private:
  HICON   m_hIcon;
  HACCEL  m_accelTable;
  polygon m_poly;
  int     m_maxIndex1, m_maxIndex2;
  CPoint  dlgToPanel(CPoint p) const;
  void    showInfo(const TCHAR *format,...);
  void    clearMax();
public:
  CPolygondistDlg(CWnd *pParent = NULL);

  enum { IDD = IDD_POLYGONDIST_DIALOG };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand( UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnLButtonUp(  UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(  UINT nFlags, CPoint point);
  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnFileExit();
  afx_msg void OnToolsFindmaxdistance();
  afx_msg void OnHelpAboutpolygondist();
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  afx_msg void OnClose();
  virtual void OnOK();
  virtual void OnCancel();
  afx_msg void OnSize(UINT nType, int cx, int cy);
};
