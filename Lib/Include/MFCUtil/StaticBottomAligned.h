#pragma once

#include "WinTools.h"

class CStaticBottomAligned : public CStatic {
private:
  CString m_text;
  int     m_lineHeight;
  void repaint(CDC &dc);
public:
	CStaticBottomAligned();
    void substituteControl(CWnd *parent, int id);
    void SetWindowText(LPCTSTR string);
    void GetWindowText(CString &str) {
      str = m_text;
    }
    int getLineHeight() const {
      return m_lineHeight;
    }
public:

public:
	virtual ~CStaticBottomAligned();

protected:
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

