#pragma once

#include "WinTools.h"

class CEditWithSpinButton : public CEdit {
private:
    CSpinButtonCtrl m_spinButton;
public:
    CEditWithSpinButton();

public:

public:
    virtual ~CEditWithSpinButton();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
