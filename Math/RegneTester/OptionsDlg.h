#pragma once

#include "HighScore.h"

class COptionsDlg : public CDialog {
private:
  Options &m_options;
public:
    COptionsDlg(Options &options, CWnd *pParent = NULL);

    enum { IDD = IDD_OPTIONS_DIALOG };
    CString m_confirmPassword;
    CString m_highscorePath;
    CString m_oldPassword;
    CString m_password;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnButtonFile();
    DECLARE_MESSAGE_MAP()
};

