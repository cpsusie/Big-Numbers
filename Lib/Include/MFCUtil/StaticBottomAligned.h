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

	//{{AFX_VIRTUAL(CStaticBottomAligned)
	//}}AFX_VIRTUAL

public:
	virtual ~CStaticBottomAligned();

protected:
	//{{AFX_MSG(CStaticBottomAligned)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
