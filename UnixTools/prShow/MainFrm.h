#pragma once

#include "ImageView.h"

class CMainFrame : public CFrameWnd {
private:
  HACCEL      m_accelTable;
  CStatusBar  m_wndStatusBar;

  CImageView *getView() {
    return (CImageView*)GetActiveView();
  }

  void scroll(int dx, int dy);
  CPoint getMaxScroll();
protected:
  CMainFrame();
  DECLARE_DYNCREATE(CMainFrame)

public:

  virtual BOOL PreTranslateMessage(MSG *pMsg);

public:
  virtual ~CMainFrame();

  CImageDoc *getDocument();
  void setTitle();

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnScrollLineDown();
    afx_msg void OnScrollLineUp();
    afx_msg void OnScrollPageDown();
    afx_msg void OnScrollPageUp();
    afx_msg void OnScrollLeft();
    afx_msg void OnScrollRight();
    afx_msg void OnScrollPageLeft();
    afx_msg void OnScrollPageRight();
    afx_msg void OnScrollToTop();
    afx_msg void OnScrollToBottom();
    afx_msg void OnScrollToLeft();
    afx_msg void OnScrollToRight();
    afx_msg void OnFileExit();
    afx_msg void OnHelpAboutprshow();
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    DECLARE_MESSAGE_MAP()
};

