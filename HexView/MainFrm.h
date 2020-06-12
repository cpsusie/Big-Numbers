#pragma once

class StatusBarPaneInfo {
public:
  int  m_index;
  UINT m_id;
  UINT m_style;
  int  m_width;
};

class CMainFrame : public CFrameWnd, SettingsAccessor {
private:
  bool                     m_settingsActivated;
  bool                     m_statusPanesVisible;
  Array<StatusBarPaneInfo> m_paneInfo;
  SearchMachine            m_searchMachine;

  bool editColor(COLORREF &color);
  void selectAndOpenFile(bool readOnly);
  bool saveFile();
  void errorMsg(const Exception &e);
  void enableToolbarButtonAndMenuItem(int id, bool enable);
  void showStatusBarPanes(bool show);
  void searchText(unsigned __int64 startPos, bool forwardSearch);
  void OnFileMruFile(int menuid);
protected: // create from serialization only
  CMainFrame();
  DECLARE_DYNCREATE(CMainFrame)

public:
  bool setDataRadix(int radix, bool hex3Pos=false); // radix must be 8,10,16
  bool setAddrRadix(int radix);              // radix must be 8,10,16
  bool newFile(const String &fname, bool readOnly);
  bool checkSave(); // return true if the operation should continue, false if cancelled by user
  void updateTitle();
  void ajourMenuItems();

  CHexViewDoc *getDoc() {
    return (CHexViewDoc*)GetActiveDocument();
  }

  CHexViewView *getView() {
    return (CHexViewView*)GetActiveView();
  }

public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);

public:
  virtual ~CMainFrame();
#if defined(_DEBUG)
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
  CStatusBar  m_wndStatusBar;
  CToolBar    m_wndToolBar;

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnKillFocus(CWnd *pNewWnd);
    afx_msg void OnSetFocus(CWnd *pOldWnd);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
  afx_msg void OnFileOpen();
    afx_msg void OnFileOpenForUpdate();
    afx_msg void OnFileCloseDoc();
    afx_msg void OnFileSaveDoc();
    afx_msg void OnFileSaveDocAs();
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
    afx_msg void OnAppExit();
    afx_msg void OnEditAllowEdit();
    afx_msg void OnEditUndo();
    afx_msg void OnEditRedo();
    afx_msg void OnEditCopy();
  afx_msg void OnEditFind();
  afx_msg void OnEditFindNext();
  afx_msg void OnEditFindPrev();
    afx_msg void OnEditGotoAddress();
    afx_msg void OnEditRefreshFile();
  afx_msg void OnViewDataOct();
  afx_msg void OnViewDataDec();
  afx_msg void OnViewDataHex3Pos();
  afx_msg void OnViewDataHex2Pos();
  afx_msg void OnViewShowAddr();
  afx_msg void OnViewShowAscii();
  afx_msg void OnViewLineSize();
  afx_msg void OnViewAddrOct();
  afx_msg void OnViewAddrDec();
  afx_msg void OnViewAddrHex();
    afx_msg void OnActivateSettings();
    afx_msg void OnViewColorsDataText();
    afx_msg void OnViewColorsDataBack();
    afx_msg void OnViewColorsAsciiBack();
    afx_msg void OnViewColorsAsciiText();
    afx_msg void OnViewColorsAddrText();
    afx_msg void OnViewColorsAddrBack();
    afx_msg void OnViewWrapEndOfLine();
    afx_msg void OnViewDataHexUppercase();
    afx_msg void OnViewAddrHexUppercase();
    afx_msg void OnViewAddrSeparators();
  afx_msg void OnCharLeft();
  afx_msg void OnCharRight();
  afx_msg void OnCtrlCharLeft();
  afx_msg void OnCtrlCharRight();
  afx_msg void OnLineUp();
  afx_msg void OnLineDown();
  afx_msg void OnHome();
  afx_msg void OnEnd();
  afx_msg void OnCtrlHome();
  afx_msg void OnCtrlEnd();
  afx_msg void OnPageLeft();
  afx_msg void OnPageRight();
  afx_msg void OnPageUp();
  afx_msg void OnPageDown();
  afx_msg void OnShiftCharLeft();
  afx_msg void OnShiftCharRight();
  afx_msg void OnShiftCtrlCharLeft();
  afx_msg void OnShiftCtrlCharRight();
  afx_msg void OnShiftLineUp();
  afx_msg void OnShiftLineDown();
  afx_msg void OnShiftHome();
  afx_msg void OnShiftEnd();
  afx_msg void OnShiftCtrlHome();
  afx_msg void OnShiftCtrlEnd();
  afx_msg void OnShiftPageLeft();
  afx_msg void OnShiftPageRight();
  afx_msg void OnShiftPageUp();
  afx_msg void OnShiftPageDown();
  DECLARE_MESSAGE_MAP()
};

