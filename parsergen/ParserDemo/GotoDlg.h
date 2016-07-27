#pragma once

class GotoDlg : public CDialog {
public:
    GotoDlg(CWnd* pParent = NULL);

    //{{AFX_DATA(GotoDlg)
    enum { IDD = IDD_DIALOGGOTO };
    UINT    m_line;
    //}}AFX_DATA


    //{{AFX_VIRTUAL(GotoDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

protected:

    //{{AFX_MSG(GotoDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
