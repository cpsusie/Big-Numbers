#pragma once

#include <PropertyContainer.h>
#include <D3DGraphics/D3Scene.h>
#include "GraphicObjects.h"
#include <D3DGraphics/D3SceneEditor.h>

#define DEVELOPER_MODE

typedef enum {
  DLG_IDLE
 ,DLG_ONTIMERDOUSERMOVE
 ,DLG_ONTIMERDOCOMPUTERMOVE
 ,DLG_ONTIMERSHOWRESULT
 ,DLG_EDITMODE
} DialogState;

class CQuartoDlg : public CDialog, public D3SceneContainer, public PropertyChangeListener {
private:
  HICON                   m_hIcon;
  HACCEL                  m_accelTable;
  bool                    m_timerRunning;
  DialogState             m_state;
  D3SceneEditor           m_editor;
  D3Scene                 m_scene;
  String                  m_gameName;
  GameBoardObject        *m_boardObject;
  UINT                    m_lightIndex[2];
  Game                    m_game;
  Player                  m_startPlayer;

  void unInitDialog();

  void startTimer(int msec);
  void startTimer(DialogState state, int msec=1000);
  void stopTimer();
  void createScene();
  void destroyScene();
  void createLight();
  void destroyLight();
  void createBoard();
  void destroyBoard();
  void resetCamera();
  void resetScene();
  void showEditorInfo();
  inline void setEditMode(bool value) {
    m_editor.setEnabled(value);
    showInfo(EMPTYSTRING);
  }
  inline CRect getGameRect() {
    return getClientRect(this, IDC_STATICGAMEWINDOW);
  }
  // point in Dialog-space
  CPoint   get3DPanelPoint(CPoint point) const;
  void     setCameraPosition(const D3DXVECTOR3 &pos);
  inline const D3DXVECTOR3 getFieldCenter(const Field &f) const {
    return m_boardObject->getFieldCenter(f);
  }
  void resetBrickPositions(bool colored);
  void flashWinnerBlocks();
  void updateGraphicsDoingMove(const Move &m);

  void setGameName(const String &name);
  const String &getGameName() const {
    return m_gameName;
  }
  void save(const String &name);
  bool isGameTitled() const {
    return !m_gameName.equalsIgnoreCase(_T("Untitled"));
  }
  void        newGame(bool colored, Player startPlayer, const String &name = _T("Untitled"));
  void        executeMove(const Move &m);

  Move        findMove();
  void        endGame();
  int         getBrickFromPoint(const CPoint &p) const;
  Field       getFieldFromPoint(const CPoint &p) const;
  void        markBrick(  int b);
  void        unmarkCurrentBrick();
  void        selectField(const Field &f);
  void        selectBrick(int b);
  inline int  getSelectedBrick() const {
    return m_boardObject->getCurrentBrick();
  }
  inline Field getSelectedField() const {
    return m_boardObject->getCurrentField();
  }
  void        showInfo(_In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void        turnBoard(int degree);
  void        render(BYTE renderFlags) {
    __super::render(renderFlags, m_editor.getActiveCameraSet());
  }
public:
  CQuartoDlg(CWnd *pParent = NULL);
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);

  D3Scene &getScene() {
    return m_scene;
  }
  UINT get3DWindowCount(void) const {
    return 1;
  }
  HWND getMessageWindow() const {
    return *this;
  }
  HWND get3DWindow(UINT index) const {
    return *GetDlgItem(IDC_STATICGAMEWINDOW);
  }
  void doRender(BYTE renderFlags, CameraSet cameraSet);
  enum { IDD = IDD_DIALOGQUARTO };


protected:
  virtual BOOL OnInitDialog();
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void OnOK();
  virtual void OnCancel();
  afx_msg void OnPaint();
  afx_msg void OnClose();
  afx_msg void    OnSysCommand(UINT   nID, LPARAM lParam);
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnLButtonDown(  UINT nFlags, CPoint point);
  afx_msg void OnFileNew();
  afx_msg void OnFileOpen();
  afx_msg void OnFileSave();
  afx_msg void OnFileSaveAs();
  afx_msg void OnFileExit();
  afx_msg void OnViewLeft();
  afx_msg void OnViewRight();
  afx_msg void OnViewResetView();
  afx_msg void OnOptionsLevelBeginner();
  afx_msg void OnOptionsLevelExpert();
  afx_msg void OnOptionsColoredGame();
  afx_msg void OnDumpSetup();
  afx_msg void OnHelpAbout();
  afx_msg LRESULT OnMsgRender( WPARAM wp , LPARAM lp    );
  afx_msg LRESULT OnMsgToggleEditMode( WPARAM wp , LPARAM lp    );
  DECLARE_MESSAGE_MAP()
public:
};
