#pragma once

#include "D3FunctionDoc.h"
#include "D3SceneView.h"
#include "InfoView.h"

class StatusBarPaneInfo {
public:
  int  m_index;
  UINT m_id;
  UINT m_style;
  int  m_width;
};

class CD3FunctionSplitterWnd : public CSplitterWnd {
private:
  bool m_splitPointMoved;
public:
  CD3FunctionSplitterWnd() : m_splitPointMoved(false) {
  }
  void RecalcLayout();
  void OnInvertTracker(const CRect& rect);
  CD3SceneView *get3DPanel();
  CInfoView    *getInfoPanel();
  int           getPanelCount() const {
    return GetRowCount();
  }
};

#define INFO_EDIT  0x01
#define INFO_MEM   0x02

#ifdef DEBUG_POLYGONIZER
#define INFO_DEBUG 0x04
#include "Debugger.h"
#else
#define INFO_DEBUG 0
#endif // DEBUG_POLYGONIZER

#define INFO_ALL  (INFO_EDIT|INFO_MEM|INFO_DEBUG)

class CMainFrame : public CFrameWnd
                 , public OptionsUpdater
                 , public D3SceneContainer
#ifdef DEBUG_POLYGONIZER
                 , public PropertyChangeListener
#endif // DEBUG_POLYGONIZER
{
private:
  bool                            m_statusPanesVisible;
  CStatusBar                      m_wndStatusBar;
  CToolBar                        m_wndToolBar;
  CD3FunctionSplitterWnd          m_wndSplitter;
  CompactArray<StatusBarPaneInfo> m_paneInfo;
  bool                            m_infoPanelVisible;
  double                          m_relativeHeight;
  D3Scene                        *m_scene;
  bool                            m_timerRunning;
  String                          m_editorInfo, m_memoryInfo, m_debugInfo;

  void OnFileOpen(int id);
  void onFileMruFile(int index);
  void updateLoadOptionsMenu();
  void updateCheckedMenuItems();
  void loadOptions(int id);
  void saveRelativeHeight();
  void setRelativeHeight(int cy);
  inline int getPanelCount() const {
    return m_wndSplitter.getPanelCount();
  }
  inline CD3SceneView *get3DPanel() {
    return m_wndSplitter.get3DPanel();
  }
  inline CInfoView *getInfoPanel() {
    return m_wndSplitter.getInfoPanel();
  }
  friend class CD3FunctionSplitterWnd;

public:

  D3Scene &getScene() {
    return *m_scene;
  }
  CWnd    *getMessageWindow() {
    return this;
  }
  CWnd    *get3DWindow() {
    return get3DPanel();
  }
  void render(BYTE renderFlags) {
    SendMessage(ID_MSG_RENDER, renderFlags, 0);
  }

  void startTimer();
  void stopTimer();
  void setInfoPanelVisible(bool visible);
  inline bool isInfoPanelVisible() const {
    return getPanelCount() > 1;
  }
  void updateEditorInfo();
  void updateMemoryInfo();
  void updateDebugInfo();
  void showInfo(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  // flags any combination of INFO_MEM, INFO_EDIT, INFO_DEBUG
  void show3DInfo(BYTE flags);

  void activateOptions();
  void ajourMenuItems();

  virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

  virtual ~CMainFrame();
  void enableToolbarButtonAndMenuItem(int id, bool enable);
  bool isToolbarButtonEnabled(int id);
  void showStatusBarPanes(bool show);

  CD3FunctionDoc *getDoc() {
    return (CD3FunctionDoc*)GetActiveDocument();
  }

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected: // create from serialization only
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
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
    afx_msg void OnViewStartTimer();
    afx_msg void OnViewStopTimer();
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
    DECLARE_MESSAGE_MAP()
};
