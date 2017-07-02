#pragma once

#include "WinTools.h"

class CEditWithSpinButton : public CEdit {
private:
    CSpinButtonCtrl m_spinButton;
public:
    CEditWithSpinButton();

    virtual ~CEditWithSpinButton();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()
};

