#pragma once

class CLixDlg : public CDialog {
public:
    CLixDlg(int lix, BOOL lixfilter, CWnd *pParent = NULL);

    enum { IDD = IDD_LIXDIALOG };
    int     m_lix;
    BOOL    m_lixfilter;


protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};

