#pragma once

class RollsizeDlg : public CDialog {
public:
    RollsizeDlg(int rollSize, CWnd* pParent = NULL);

    enum { IDD = IDD_ROLLSIZE_DIALOG };
    UINT    m_rollSize;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:

    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};
