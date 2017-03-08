#pragma once

class CLevelDlg : public CDialog {
public:
    CLevelDlg(GameLevel level, CWnd *pParent = NULL);


    enum { IDD = IDD_DIALOGLEVEL };
   GameLevel m_level;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:

    virtual BOOL OnInitDialog();
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
};

