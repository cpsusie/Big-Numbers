#pragma once

#include "Transform.h"

class CDraw3dDlg : public CDialog {
private:
    HICON  m_hIcon;
    HACCEL m_accelTable;
    Scene  m_scene;
    CPoint m_lastMouse;

    void resetCamera();
    void walkWithCamera(        double dist, double angle );
    void rotateCameraUpDown(    double angle );
    void rotateCameraLeftRight( double angle );
    void sidewalkWithCamera(    double up  , double right );
public:
    CDraw3dDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_DRAW3D_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
    virtual void OnOK();
    virtual void OnCancel();
public:
  afx_msg void OnClose();
  afx_msg void OnFileExit();
  afx_msg void OnHelpAboutdraw3d();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnFileResetcamera();
  virtual BOOL PreTranslateMessage(MSG* pMsg);
};

