#pragma once

#include <Array.h>
#include "CardButton.h"

class BacksideDialog : public CDialog, public OptionsAccessor {
private:
  Array<CardButton*> m_buttons;
public:
    BacksideDialog(CWnd* pParent = NULL); 

    enum { IDD = IDD_BACKSIDE_DIALOG };

public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:

    afx_msg void OnPaint();
    afx_msg void OnDestroy();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()
};

