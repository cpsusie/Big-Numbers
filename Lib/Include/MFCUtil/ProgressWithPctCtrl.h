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

public:
  virtual ~CProgressWithPctCtrl();

protected:
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

