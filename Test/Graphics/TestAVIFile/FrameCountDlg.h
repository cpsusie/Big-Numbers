#pragma once

class CFrameCountDlg : public CDialog {
public:
    CFrameCountDlg(UINT frameCount, CWnd *pParent = nullptr);

    enum { IDD = IDD_FRAMECOUNT_DIALOG };
    UINT    m_frameCount;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual void OnOK();
    virtual void OnCancel();
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};

