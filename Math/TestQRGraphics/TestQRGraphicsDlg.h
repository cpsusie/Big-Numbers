#pragma once

#include <Math/Matrix.h>

#define DEFAULTSIZE 15

class CTestQRGraphicsDlg : public CDialog {
private:
    HICON     m_hIcon;
public:
    CTestQRGraphicsDlg(CWnd* pParent = NULL);
    CFont     m_font;
    Matrix    a;

    enum { IDD = IDD_TESTQRGRAPHICS_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnFileQuit();
    afx_msg void OnFileNew();
    afx_msg void OnFileSolve();
    afx_msg void OnFileUn();
    afx_msg void OnFileSymmetric();
    afx_msg void OnFileSpecial();
    afx_msg void OnOptionsPause();
    afx_msg void OnOptionsDimension();
    afx_msg void OnFileIllconditioned();
    afx_msg void OnFileRandomAntiSymmetric();
    DECLARE_MESSAGE_MAP()
};

