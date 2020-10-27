#pragma once

#include <Array.h>
#include <MFCUtil/PixRect.h>

class GlyphRect : public CRect {
public:
  int m_row, m_col;
  inline GlyphRect() : m_row(0), m_col(0) {
  }
  inline GlyphRect(CRect &r, int row, int col) : CRect(r), m_row(row), m_col(col) {
  }
};

class CSelectGlyphDlg : public CDialog {
private:
  const LOGFONT          &m_logFont;
  PixRectFont            *m_prFont;
  HACCEL                  m_accelTable;
  CBrush                  m_selectedBrush,m_unselectedBrush;
  int                     m_currentSelection;
  GlyphCurveData          m_selectedGlyphCurveData;
  CompactArray<GlyphRect> m_rectArray;
  int   findSelection(const CPoint &p) const;
  void  markSelection(      int index);
  void  unmarkSelection(    int index);
  void  setCurrentSelection(int index);
  int   getSelectedRow();
  int   getSelectedCol();
  void  selectPosition(int row, int col);
  void  selectPosition(const CPoint &p);
  CRect getClientRect(int id);
  CRect getWindowRect(int id);

public:
  CSelectGlyphDlg(const LOGFONT &logFont, CWnd *pParent = nullptr);
  ~CSelectGlyphDlg();
  const GlyphCurveData &getSelectedGlyphCurveData() const { return m_selectedGlyphCurveData; }

  enum { IDD = IDR_SELECTGLYPH };

protected:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnPaint();
  afx_msg void OnLButtonDown(  UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnGoDown();
  afx_msg void OnGoLeft();
  afx_msg void OnGoRight();
  afx_msg void OnGoUp();
  DECLARE_MESSAGE_MAP()
};
