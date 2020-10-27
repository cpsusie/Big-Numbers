#pragma once

#include <Array.h>
#include "CardButton.h"

class BacksideDialog : public CDialog, public OptionsAccessor {
private:
  Array<CardButton*> m_buttons;
public:
    BacksideDialog(CWnd *pParent = nullptr);

    enum { IDD = IDD_BACKSIDE_DIALOG };

    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg void OnPaint();
    afx_msg void OnDestroy();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()
};

