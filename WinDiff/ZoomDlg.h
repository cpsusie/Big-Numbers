#pragma once

#include "WinDiffDoc.h"
#include "Edit2Lines.h"

class CZoomDlg: public CDialog, private OptionsAccessor {
private:
  HICON               m_hIcon;
  CFont               m_font;
  double              m_fontScale;
  CEdit2Lines         m_editBox;
  const String       &m_s1;
  const String       &m_s2;
  StrDiff             m_diff;
  WinDiffComparator   m_cmp;

  void zoomIn();
  void zoomOut();
  void scrollRight();
  void scrollLeft();
   // n characters. positive scrolls right, negative scroll left
  void scrollHorizontal(int n);
  void createAndSetFont(double scale);
public:
  CZoomDlg(const String &s1, const String &s2, CWnd *pParent = NULL);

  enum { IDD = IDD_DIALOGZOOM };
  BOOL  m_ignorecase;
  CString m_lines;

  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  afx_msg void OnCheckIgnorecase();
  DECLARE_MESSAGE_MAP()
};
