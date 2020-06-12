#pragma once

#include "DefineFileFormatDlg.h"
#include "WinDiffDoc.h"
#include "WinDiffView.h"

class StatusBarPaneInfo {
public:
  int  m_index;
  UINT m_id;
  UINT m_style;
  int  m_width;
};

class CWinDiffSplitterWnd : public CSplitterWnd {
private:
  bool m_splitPointMoved;
public:
  CWinDiffSplitterWnd() : m_splitPointMoved(false) {
  }
  void RecalcLayout();
  void OnInvertTracker(const CRect& rect);
  CWinDiffView *getDiffView(int index);
  int           getPanelCount() {
    return GetColumnCount();
  }
  void setActivePanel(int index);
};

#define MAINHASFOCUS   0x01
#define PANEL0ACTIVE   0x02
#define PANEL1ACTIVE   0x04

class CMainFrame : public CFrameWnd, public OptionsUpdater {
private:
  bool                            m_statusPanesVisible;
  bool                            m_hasFocus;
  CompactArray<StatusBarPaneInfo> m_paneInfo;
  double                          m_relativeWidth;
  FindParameters                  m_findParameters;
  int                             m_savedActivePanelIndex;
  void OnFileOpen(int id);
  void onFileMruFile(int index);
  BOOL doPreTranslateMessage(MSG *pMsg);
  void updateLoadOptionsMenu();
  void updateCheckedMenuItems();
  void updateNameFontSizeMenuItems(int pct);
  void loadOptions(int id);
  void saveRelativeWidth();
  void setRelativeWidth(int cx);
  int  getActivePanelIndex();
  void setActivePanel(int index);
  int  getPanelCount();
  void setNameFontSize(int pct);
  void setFocus(bool value);
  BYTE getFocusFlags(); // combination of m_hasFocus and active panelsIndex
  CWinDiffView *getDiffView(int index) {
    return m_wndSplitter.getDiffView(index);
  }
  friend class CWinDiffSplitterWnd;
protected: // create from serialization only
  CMainFrame();
  DECLARE_DYNCREATE(CMainFrame)
// Attributes
protected:
  CWinDiffSplitterWnd m_wndSplitter;
public:
  TextView        *getActiveTextView();
  CWinDiffView    *getActiveDiffView();

  void activateOptions();

#if defined(_DEBUG)
  void showDebugMsg(_In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void dumpFlags();
  String flagsToString();
#endif
  void ajourMenuItems();

public:
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL PreTranslateMessage(MSG *pMsg);

    virtual ~CMainFrame();
    void enableToolbarButtonAndMenuItem(int id, bool enable);
    bool isToolbarButtonEnabled(int id);
    void showStatusBarPanes(bool show);

    CWinDiffDoc *getDoc() {
      return (CWinDiffDoc*)GetActiveDocument();
    }

    void refreshDoc(bool recomp);
#if defined(_DEBUG)
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
    CStatusBar     m_wndStatusBar;
    CToolBar       m_wndToolBar;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized);
    afx_msg void OnActivatePartner();
    afx_msg void OnFileOpenPanel0();
    afx_msg void OnFileOpenPanel1();
    afx_msg void OnFilePrintPreview();
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
    afx_msg void OnEditCopy();
    afx_msg void OnEditPaste();
    afx_msg void OnEditSelectAll();
    afx_msg void OnEditFind();
    afx_msg void OnEditFindNext();
    afx_msg void OnEditFindPrev();
    afx_msg void OnEditPrevDiff();
    afx_msg void OnEditNextDiff();
    afx_msg void OnEditShowDetails();
    afx_msg void OnEditGoto();
    afx_msg void OnEditRefreshFiles();
    afx_msg void OnEditSwapPanels();
    afx_msg void OnViewShowWhiteSpace();
    afx_msg void OnViewTabSize();
    afx_msg void OnViewFont();
    afx_msg void OnViewNameFontSize100();
    afx_msg void OnViewNameFontSize125();
    afx_msg void OnViewNameFontSize150();
    afx_msg void OnViewNameFontSize175();
    afx_msg void OnViewNameFontSize200();
    afx_msg void OnViewShow1000separator();
    afx_msg void OnViewHighlightCompareEqual();
    afx_msg void OnOptionsIgnoreWhiteSpace();
    afx_msg void OnOptionsIgnoreCase();
    afx_msg void OnOptionsIgnoreComments();
    afx_msg void OnOptionsIgnoreStrings();
    afx_msg void OnOptionsStripComments();
    afx_msg void OnOptionsIgnoreColumns();
    afx_msg void OnOptionsDefineColumns();
    afx_msg void OnOptionsIgnoreRegex();
    afx_msg void OnOptionsDefineRegex();
    afx_msg void OnOptionsSaveOptions();
    afx_msg void OnOptionsLoadOptions1();
    afx_msg void OnOptionsLoadOptions2();
    afx_msg void OnOptionsLoadOptions3();
    afx_msg void OnOptionsLoadOptions4();
    afx_msg void OnOptionsLoadOptions5();
    afx_msg void OnOptionsLoadOptions6();
    afx_msg void OnOptionsLoadOptions7();
    afx_msg void OnOptionsLoadOptions8();
    afx_msg void OnOptionsLoadOptions9();
    afx_msg void OnOptionsOrganizeOptions();
    afx_msg void OnCharLeft();
    afx_msg void OnCharRight();
    afx_msg void OnCtrlCharLeft();
    afx_msg void OnCtrlCharRight();
    afx_msg void OnLineDown();
    afx_msg void OnLineUp();
    afx_msg void OnPageDown();
    afx_msg void OnPageUp();
    afx_msg void OnHome();
    afx_msg void OnEnd();
    afx_msg void OnCtrlHome();
    afx_msg void OnCtrlEnd();
    afx_msg void OnDumpFlags();
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

