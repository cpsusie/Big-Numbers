#pragma once

class StackSizeDlg : public CDialog {
public:
    StackSizeDlg(int oldSize, CWnd* pParent = NULL);

    //{{AFX_DATA(StackSizeDlg)
    enum { IDD = IDD_DIALOGSTACKSIZE };
    UINT    m_stackSize;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(StackSizeDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

protected:

    //{{AFX_MSG(StackSizeDlg)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
