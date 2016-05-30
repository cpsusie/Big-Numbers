#pragma once

#include "WinTools.h"

class CColoredStatic : public CStatic {
private:
    COLORREF m_bkColor, m_textColor;
    bool     m_bkColorSet;
    void repaint(CDC &dc);
public:
    CColoredStatic();

    //{{AFX_DATA(CColoredStatic)
	//}}AFX_DATA

    void setBKColor(  COLORREF color);
    void setTextColor(COLORREF color);
    COLORREF getBKColor() const {
      return m_bkColor;
    }
    COLORREF getTextColor() const {
      return m_textColor;
    }

    //{{AFX_VIRTUAL(CColoredStatic)
    //}}AFX_VIRTUAL

public:
    virtual ~CColoredStatic();

protected:
    //{{AFX_MSG(CColoredStatic)
	afx_msg void OnPaint();
	//}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
