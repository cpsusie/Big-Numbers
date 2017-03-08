#pragma once

class CSelectGameDlg : public CDialog {
public:
    CSelectGameDlg(CWnd *pParent = NULL);


    enum { IDD = IDD_DIALOGSELECTGAME };
    UINT    m_seed;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:

    virtual void OnOK();
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};

