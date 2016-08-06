#pragma once

#include <afxWin.h>
#include "CaptureView.h"

class CMainFrame : public CFrameWnd {
protected:
  CMainFrame();
  DECLARE_DYNCREATE(CMainFrame)

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);

private:
  HWND            m_selectedWindow;
  HWND            m_capturedWindow;
  HDC             m_savedWindowDC;
  HBITMAP         m_savedBitmap;
  HPEN            m_blackPen;
  WINDOWPLACEMENT m_savedWindowPlacement;
  bool            m_capturingWindow;

  void repaint();
  void showDocSize();
  void restoreSelectedWindow();
  void selectWindow(HWND hwnd);
  void initCurrent();
  void releaseCurrent();

  bool hasSelectedWindow() const {
    return m_selectedWindow != NULL;
  }

  HWND getSelectedWindow() {
    return m_selectedWindow;
  }

  bool isCapturingWindow() const {
    return m_capturingWindow;
  }

  void setCaptureAllEvents(bool value);

  void saveWindowPlacement();
  void restoreWindowPlacement();
  void moveWindowOutsideScreen();

  void drawRectangle(HWND hwnd);
  HWND findWindow(const CPoint &point);

  CCaptureView *getView() {
    return (CCaptureView*)GetActiveView();
  }

  CCaptureDoc *GetDocument() {
    return getView()->GetDocument();
  }

public:
  virtual ~CMainFrame();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  CStatusBar m_wndStatusBar;
  CToolBar   m_wndToolBar;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnStartMSPaint();
    afx_msg void OnAppCaptureArea();
    afx_msg void OnAppCaptureWindow();
    afx_msg void OnAppCaptureWindowDummy();
    afx_msg void OnAppCaptureScreen();
    afx_msg void OnScrollLineDown();
    afx_msg void OnScrollLineUp();
    afx_msg void OnScrollLineRight();
    afx_msg void OnScrollLineLeft();
    afx_msg void OnScrollPageDown();
    afx_msg void OnScrollPageUp();
    afx_msg void OnScrollPageRight();
    afx_msg void OnScrollPageLeft();
    afx_msg void OnScrollToBottom();
    afx_msg void OnScrollToTop();
    afx_msg void OnScrollToRight();
    afx_msg void OnScrollToLeft();
    afx_msg void OnScrollToEnd();
    afx_msg void OnScrollToHome();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnViewSizePixels();
    afx_msg void OnViewSizeCentimeter();
    DECLARE_MESSAGE_MAP()
};

