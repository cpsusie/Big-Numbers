#pragma once

class CAfstandDlg : public CDialog {
public:
    CAfstandDlg(CWnd* pParent = NULL);
    void Beregnafstand();
    void Adder();

    //{{AFX_DATA(CAfstandDlg)
    enum { IDD = IDD_AFSTANDDIALOG };
    int     m_afstand;
    CString m_dag1;
    CString m_dag2;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CAfstandDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

protected:

    //{{AFX_MSG(CAfstandDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnResult();
    afx_msg void OnRadioadder();
    afx_msg void OnRadioafstand();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
