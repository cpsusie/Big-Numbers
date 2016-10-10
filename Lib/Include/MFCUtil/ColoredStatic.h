#pragma once

#include "WinTools.h"

class CColoredStatic : public CStatic {
private:
    COLORREF m_bkColor, m_textColor;
    bool     m_bkColorSet;
    void repaint(CDC &dc);
public:
    CColoredStatic();

    void setBKColor(  COLORREF color);
    void setTextColor(COLORREF color);
    COLORREF getBKColor() const {
      return m_bkColor;
    }
    COLORREF getTextColor() const {
      return m_textColor;
    }

public:
    virtual ~CColoredStatic();

protected:
	afx_msg void OnPaint();

  DECLARE_MESSAGE_MAP()
};

