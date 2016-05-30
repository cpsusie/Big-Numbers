#pragma once

#include "WinTools.h"

class CEditWithSpinButton : public CEdit {
private:
    CSpinButtonCtrl m_spinButton;
public:
    CEditWithSpinButton();

public:

    //{{AFX_VIRTUAL(CEditWithSpinButton)
    //}}AFX_VIRTUAL

public:
    virtual ~CEditWithSpinButton();

protected:
    //{{AFX_MSG(CEditWithSpinButton)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
