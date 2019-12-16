#pragma once

#include <Thread.h>
#include "SortPanelWnd.h"

#define STATESHIFT(oldState, newState) (((oldState) << 4) | (newState))
#define GETOLDSTATE(stateShift)        ((SortJobState)((stateShift) >>  4))
#define GETNEWSTATE(stateShift)        ((SortJobState)((stateShift) & 0xf))

class CSortDemoDlg : public CDialog, public PropertyChangeListener {
private:
  HICON                       m_hIcon;
  HACCEL                      m_accelTable;
  String                      m_origMenuTextGo, m_origMenuTextPause;
  InitializeParameters        m_parameters;
  bool                        m_fast;
  CompactArray<SortPanelWnd*> m_panels;
  int                         m_ctrlId;
  SortPanelWnd               *m_selectedPanel;
  int                         m_movingPanelIndex;
  CRect                       m_dragRect;
  CPoint                      m_mouseDownPoint;
  bool addSortPanel(    int methodId);
  bool deleteSortPanel( int methodId);
  int  findPanelIndexByMethod(int methodId) const;
  void toggleSortMethod(int methodId, bool redraw=true);
  void enableMenuItemsOnSortGo(bool enabled);
  void buildSortMethodMenu();
  void checkActiveMethodsItems();
  void adjustLayout();
  int  getPanelCountByState(SortJobState state) const;
  bool allPanelsInState(SortJobState state) const;
  bool noPanelsInState( SortJobState state) const;
  int           findPanelIndexFromPoint(const CPoint &p);
  SortPanelWnd *findPanelFromPoint(const CPoint &p);
  inline bool isMoveingPanel() const {
    return m_movingPanelIndex >= 0;
  }
  void startDragRect(CPoint &p);
  void dragRect(CPoint &p);
  void endDragRect();
public:
    CSortDemoDlg(CWnd *pParent = NULL);
    const InitializeParameters &getInitParameters() const {
      return m_parameters;
    }

    bool &getFast() {
      return m_fast;
    }
    int getNextCtrlId() {
      return m_ctrlId++;
    };
    bool isAnimatedSort();
    void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);

    enum { IDD = IDD_SORTDEMO_DIALOG };

    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnClose();
    afx_msg void OnIdok();
    afx_msg void OnCancel();
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    virtual BOOL OnInitDialog();
    afx_msg void OnFileInitialize();
    afx_msg void OnFileGo();
    afx_msg void OnFilePause();
    afx_msg void OnFileResume();
    afx_msg void OnFileExit();
    afx_msg void OnSelectAllMethods();
    afx_msg void OnOptionsParameters();
    afx_msg void OnOptionsAnimate();
    afx_msg void OnOptionsSpeedSlow();
    afx_msg void OnOptionsSpeedFast();
    afx_msg void OnContextAnalyze();
    afx_msg void OnContextmenuRemove();
    afx_msg LRESULT OnReceiveStateShift(WPARAM wp, LPARAM lp);
    afx_msg LRESULT OnEnableGoMenuItems(WPARAM wp, LPARAM lp);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnToggleSortMethod(UINT id);
    DECLARE_MESSAGE_MAP()
};

