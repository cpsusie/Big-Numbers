#pragma once

class RollsizeDlg : public CDialog {
public:
    RollsizeDlg(int rollSize, CWnd* pParent = NULL);

    //{{AFX_DATA(RollsizeDlg)
    enum { IDD = IDD_ROLLSIZE_DIALOG };
    UINT    m_rollSize;
    //}}AFX_DATA


    //{{AFX_VIRTUAL(RollsizeDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

protected:

    //{{AFX_MSG(RollsizeDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
