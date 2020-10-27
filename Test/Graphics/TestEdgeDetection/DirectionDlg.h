#pragma once

#include "EdgeMatrix.h"

class CDirectionDlg : public CDialog {
public:
    CDirectionDlg(Direction dir, CWnd *pParent = nullptr);

    Direction getDirection() const {
      return m_dir;
    }
    enum { IDD = IDD_DIRECTION_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

    Direction m_dir;

    virtual BOOL OnInitDialog();
    afx_msg void OnButtonS();
    afx_msg void OnButtonE();
    afx_msg void OnButtonN();
    afx_msg void OnButtonW();
    virtual void OnCancel();
    DECLARE_MESSAGE_MAP()
};

