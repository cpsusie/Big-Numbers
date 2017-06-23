#pragma once

class RollAvgSizeDlg : public CDialog {
public:
    RollAvgSizeDlg(int rollAvgSize, CWnd *pParent = NULL);

    enum { IDD = IDD_ROLLAVGSIZE_DIALOG };
    UINT    m_rollAvgSize;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};
