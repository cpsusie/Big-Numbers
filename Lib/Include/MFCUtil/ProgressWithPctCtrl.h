#pragma once

class CProgressWithPctCtrl : public CProgressCtrl {
private:
  DECLARECLASSNAME;
  bool     m_showPct;
public:
	CProgressWithPctCtrl();

    void substituteControl(CWnd *wnd, int id);
    void setShowPercent(bool show);
    void setBarColor(COLORREF color);
	//{{AFX_VIRTUAL(CProgressWithPctCtrl)
	//}}AFX_VIRTUAL

public:
  virtual ~CProgressWithPctCtrl();

protected:
	//{{AFX_MSG(CProgressWithPctCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
