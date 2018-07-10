#pragma once

#include <Registry.h>

typedef enum {
  DOCSIZE_IN_PIXELS
 ,DOCSIZE_IN_CENTIMETERS
} DocSizeFormat;

class Options {
private:
  DocSizeFormat m_docSizeFormat;
  int           m_nFilterIndex;
  static RegistryKey getRootKey();
public:
  void load();
  void save();
  void setDocSizeFormat(DocSizeFormat docSizeFormat);
  inline DocSizeFormat getDocSizeFormat() const {
    return m_docSizeFormat;
  }
  void setFilterIndex(int index);
  inline int getFilterIndex() const {
    return m_nFilterIndex;
  }
};

class CMainFrame : public CFrameWndEx {
protected:
  CMainFrame();
  DECLARE_DYNCREATE(CMainFrame)

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd *pParentWnd = NULL, CCreateContext *pContext = NULL);

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

  inline bool hasSelectedWindow() const {
    return m_selectedWindow != NULL;
  }

  inline HWND getSelectedWindow() {
    return m_selectedWindow;
  }

  inline bool isCapturingWindow() const {
    return m_capturingWindow;
  }

  void setCaptureAllEvents(bool value);

  void saveWindowPlacement();
  void restoreWindowPlacement();
  void moveWindowOutsideScreen();

  void drawRectangle(HWND hwnd);
  HWND findWindow(const CPoint &point);

  inline CCaptureView *getView() {
    return (CCaptureView*)GetActiveView();
  }
  inline CCaptureDoc *GetDocument() {
    return (CCaptureDoc*)GetActiveDocument();
  }
  inline const CCaptureDoc *GetDocument() const {
    return (CCaptureDoc*)((CMainFrame*)this)->GetActiveDocument();
  }
  bool hasDocument() const {
    return GetDocument() != NULL;
  }
  inline bool hasImage() const {
    return hasDocument() && GetDocument()->hasImage();
  }
public:
  virtual ~CMainFrame();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif
  Options m_options;

protected:  // control bar embedded members
    CMFCMenuBar       m_wndMenuBar;
    CMFCToolBar       m_wndToolBar;
    CMFCStatusBar     m_wndStatusBar;
    CMFCToolBarImages m_UserImages;

    afx_msg int     OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void    OnViewCustomize();
    afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
    afx_msg void OnFilePrint();
    afx_msg void OnFilePrintPreview();
    afx_msg void OnFileSave();
    afx_msg void OnEditCopy();
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
    afx_msg void OnViewSizeCentimeters();
    afx_msg void OnUpdateFilePrint(          CCmdUI *pCmdUI);
    afx_msg void OnUpdateFilePrintPreview(   CCmdUI *pCmdUI);
    afx_msg void OnUpdateFileSave(           CCmdUI *pCmdUI);
    afx_msg void OnUpdateEditCopy(           CCmdUI *pCmdUI);
    afx_msg void OnUpdateStartMSPaint(       CCmdUI *pCmdUI);
    afx_msg void OnUpdateViewSizePixels(     CCmdUI *pCmdUI);
    afx_msg void OnUpdateViewSizeCentimeters(CCmdUI *pCmdUI);
    DECLARE_MESSAGE_MAP()
};
