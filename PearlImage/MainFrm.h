#pragma once

#include <Stack.h>
#include <MFCUtil/PropertyDlgThread.h>
#include "DrawTool.h"
#include "PearlImageView.h"
#include "GridDlg.h"

class CMainFrame : public CFrameWnd, public PropertyChangeListener {
private:
  HACCEL              m_accelTable;
  double              m_currentDegree;
  ScaleParameters     m_currentScale;
  Stack<DrawTool*>    m_toolStack;
  D3DCOLOR            m_currentColor;
  FontParameters      m_currentFontParameters;
  String              m_currentText;
  UINT                m_approximateFillTolerance;
  CSize               m_eraseToolSize;
  GridParameters      m_currentGridParam;
  CGridDlg           *m_gridDlg;
  CPropertyDlgThread *m_gridDlgThread;


  void onFileMruFile(int index);
  void setCurrentDrawTool(int id);
  void setCurrentDrawTool(DrawTool *newDrawTool);
  void checkToolItem(int id);
  void ajourRedoUndo();
  bool save(const CString &name);
  bool onFileSave();
  bool checkSave();
  CPearlImageView *getView() {
    return (CPearlImageView*)GetActiveView();
  }

  CPearlImageDoc *getDocument();
  void scroll(int dx, int dy);
  CPoint getMaxScroll();
  void applyFilter(PixRectFilter &filter);
  void applyMirror(bool vertical);
protected:
  CMainFrame();
  CStatusBar  m_wndStatusBar;
  CToolBar    m_wndToolBar;
  DECLARE_DYNCREATE(CMainFrame)

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual ~CMainFrame();

  // return true, if a new image was loaded into doc and showed in view
  bool loadFile(const String &fileName);
  void setCurrentZoomFactor(int id);
  void saveDocState();
  void updateTitle();
  void pushTool(DrawTool *tool);
  void popTool();
  bool hasGridDlg() const;
  void createGridDlg();
  void destroyGridDlg();
  DrawTool *getCurrentDrawTool() {
    return m_toolStack.top();
  }

  bool hasDrawTool() const {
    return !m_toolStack.isEmpty();
  }

  void resetCurrentDrawTool();

  D3DCOLOR getCurrentColor() const {
    return m_currentColor;
  }

  int getApproximateFillTolerance() const {
    return m_approximateFillTolerance;
  }
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnFileNew();
    afx_msg void OnFileOpen();
    afx_msg void OnFileInsert();
    afx_msg void OnAppExit();
    afx_msg void OnFileMruFile1();
    afx_msg void OnFileMruFile2();
    afx_msg void OnFileMruFile3();
    afx_msg void OnFileMruFile4();
    afx_msg void OnFileMruFile5();
    afx_msg void OnFileMruFile6();
    afx_msg void OnFileMruFile7();
    afx_msg void OnFileMruFile8();
    afx_msg void OnFileMruFile9();
    afx_msg void OnFileMruFile10();
    afx_msg void OnFileMruFile11();
    afx_msg void OnFileMruFile12();
    afx_msg void OnFileMruFile13();
    afx_msg void OnFileMruFile14();
    afx_msg void OnFileMruFile15();
    afx_msg void OnFileMruFile16();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnEditUndo();
    afx_msg void OnEditRedo();
    afx_msg void OnEditCut();
    afx_msg void OnEditCopy();
    afx_msg void OnEditPaste();
    afx_msg void OnOptionsColor();
    afx_msg void OnOptionsZoomX1();
    afx_msg void OnOptionsZoomX2();
    afx_msg void OnOptionsZoomX4();
    afx_msg void OnOptionsZoomX8();
    afx_msg void OnToolsMoveRectangle();
    afx_msg void OnToolsMoveRegion();
    afx_msg void OnFunctionRotate();
    afx_msg void OnFunctionScale();
    afx_msg void OnFunctionMirrorHorizontal();
    afx_msg void OnFunctionMirrorVertical();
    afx_msg void OnFunctionMakegrayscale();
    afx_msg void OnFunctionsMakePearlGrid();
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
    afx_msg void OnPopTool();
    afx_msg void OnDelete();
    afx_msg LRESULT OnMsgCalculateImage(WPARAM wp, LPARAM lp);
    DECLARE_MESSAGE_MAP()
};
