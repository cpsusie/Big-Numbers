#pragma once

class DisplayPoly : public CDialog {
public:
    DisplayPoly(CWnd *pParent = nullptr);

    enum { IDD = IDD_DISPLAYPOLY };
    CString m_fisk;
    int counter;

    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

