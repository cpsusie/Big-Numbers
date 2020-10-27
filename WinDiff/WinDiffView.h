#pragma once

#include "TextView.h"

class CWinDiffView : public CFormView, public TextContainer, public OptionsAccessor {
protected: // create from serialization only
  CWinDiffView();
  DECLARE_DYNCREATE(CWinDiffView)

// Form Data
private:
  int     m_nameFontSizePct;
  int     m_nameHeight;
  LOGFONT m_origNameFont;
  CFont   m_currentNameFont;
  void adjustChildPositions();
public:
  enum { IDD = IDD_WINDIFFVIEW };

  int getId(); // 0 for left, 1 for right
public:
  CWinDiffDoc  *GetDocument();
  CWinDiffView *getPartner();
  bool hasPartner() { return getPartner() != nullptr; }
  void setScrollPos();
  void setScrollRange(bool redraw);
  void handleAnchor();
  void copyToClipboard();
  void updateTitle();
  void setActive(bool active);
  TextPositionPair searchText(const FindParameters &param);
  void find(const FindParameters &param);
  void gotoLine(UINT line);
  void startTimer();
  void stopTimer();
  inline void showCaret() {
    m_textView.showCaret();
  }
  inline void hideCaret() {
    m_textView.hideCaret();
  }
  inline void destroyCaret() {
    m_textView.destroyCaret();
  }
  void setNameFontSizePct(int pct, bool updatePartner=true);
  int  getNameFontSizePct() const {
    return m_nameFontSizePct;
  }

  TextView      m_textView;
  bool          m_timerIsRunning;
  MousePosition m_mousePos;

public:
    virtual ~CWinDiffView();
#if defined(_DEBUG)
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  virtual void OnDraw(           CDC *pDC);  // overridden to draw this view
  virtual BOOL PreCreateWindow(  CREATESTRUCT &cs);
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void OnInitialUpdate();
  virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
  virtual void OnBeginPrinting(  CDC *pDC, CPrintInfo *pInfo);
  virtual void OnEndPrinting(    CDC *pDC, CPrintInfo *pInfo);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void OnDropFiles(HDROP hDropInfo);
  afx_msg void OnDestroy();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  DECLARE_MESSAGE_MAP()
};

#if !defined(_DEBUG  )// debug version in WinDiffView.cpp
inline CWinDiffDoc* CWinDiffView::GetDocument()
   { return (CWinDiffDoc*)m_pDocument; }
#endif

