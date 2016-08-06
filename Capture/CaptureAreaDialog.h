#pragma once

class CCaptureAreaDialog : public CDialog {

public:
  CCaptureAreaDialog(CWnd* pParent = NULL);

  inline const CRect &getCapturedRect() const {
    return m_rect;
  }

private:
  int    m_state;
  CSize  m_screenSize;
  CPoint m_lastMark;
  CRect  m_rect;
  
  void drawMarkLines(const CPoint &p);
  void removeMarkLines();
  enum { IDD = IDD_CAPTUREAREADIALOG };


protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  virtual BOOL OnInitDialog();
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  DECLARE_MESSAGE_MAP()
};
