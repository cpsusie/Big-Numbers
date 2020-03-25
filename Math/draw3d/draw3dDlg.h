#pragma once

#include "Transform.h"

class CDraw3dDlg : public CDialog {
private:
  HICON  m_hIcon;
  HACCEL m_accelTable;
  Scene  m_scene;
  CPoint m_lastMouse;

  void resetCamera();
  void walkWithCamera(        float dist, float angle );
  void rotateCameraUpDown(    float angle );
  void rotateCameraLeftRight( float angle );
  void sidewalkWithCamera(    float up  , float right );
public:
  CDraw3dDlg(CWnd *pParent = NULL);

  enum { IDD = IDD_DRAW3D_DIALOG };

  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  virtual void OnOK();
  virtual void OnCancel();
  afx_msg void OnClose();
  afx_msg void OnFileExit();
  afx_msg void OnHelpAboutdraw3d();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnFileResetcamera();
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  DECLARE_MESSAGE_MAP()
};

