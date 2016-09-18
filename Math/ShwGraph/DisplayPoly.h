#pragma once

class DisplayPoly : public CDialog {
public:
    DisplayPoly(CWnd* pParent = NULL);

    enum { IDD = IDD_DISPLAYPOLY };
    CString m_fisk;


    protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    int counter;

    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

