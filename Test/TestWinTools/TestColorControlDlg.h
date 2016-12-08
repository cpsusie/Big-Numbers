#pragma once

#include <MFCUtil/LayoutManager.h>
#include "colormapctrl.h"

class CTestColorControlDlg : public CDialog {
private:
  HACCEL              m_accelTable;
  SimpleLayoutManager m_layoutManager;
  bool                m_pickingColor;
public:
    CTestColorControlDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_TESTCOLORCONTROL_DIALOG };
    CString m_caption;
    BOOL    m_hasBorder;
    BOOL    m_enabled;
    BOOL    m_visible;
    BOOL    m_clientEdge;
    BOOL    m_modalFrame;
    BOOL    m_staticEdge;
    BOOL    m_sunken;

public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    afx_msg void OnPaint();
    afx_msg void OnCheckBorder();
    afx_msg void OnChangeEditCaption();
    afx_msg void OnButtonpickcolor();
    afx_msg void OnCheckVisible();
    afx_msg void OnCheckEnabled();
    afx_msg void OnCheckClientEdge();
    afx_msg void OnCheckModalFrame();
    afx_msg void OnCheckStaticEdge();
    afx_msg void OnCheckSunken();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnGotoCaption();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()
private:
  CColormapctrl m_colormap;
public:
  DECLARE_EVENTSINK_MAP()
  void OnColorchangedColormapctrl();
};

