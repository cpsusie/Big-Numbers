#pragma once

class StackSizeDlg : public CDialog {
public:
    StackSizeDlg(int oldSize, CWnd *pParent = nullptr);

    enum { IDD = IDD_DIALOGSTACKSIZE };

    UINT    m_stackSize;

    protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:

    virtual void OnOK();
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};

