#pragma once

#include <D3DGraphics/D3Scene.h>
#include "GraphicObjects.h"

#define DEVELOPER_MODE

class CQuartoDlg : public CDialog {
private:
  HICON                   m_hIcon;
  HACCEL                  m_accelTable;
  int                     m_adjustingCameraFlags;
  CPoint                  m_rbuttonDownPoint;
  D3DXVECTOR3             m_startCamPos;
  D3DXVECTOR3             m_boardCenter;
  D3Scene                 m_scene;
  String                  m_gameName;
#ifdef _DEBUG
  D3SceneObject          *m_coordinateSystem;
#endif
  GameBoardObject        *m_boardObject;
  Game                    m_game;
  Player                  m_startPlayer;

  void createScene();
  void createLight();
  void createBoard();
  void resetCamera();
  void render();

  CStatic *getGameWindow() const {
    return (CStatic*)GetDlgItem(IDC_STATICGAMEWINDOW);
  }
  CRect    getGameRect() {
    return getClientRect(this, IDC_STATICGAMEWINDOW);
  }
  void setCameraPosition(const D3DXVECTOR3 &pos);
  D3DXVECTOR3 getCameraPosition();
  const D3DXVECTOR3 &getBoardCenter() const {
    return m_boardCenter;
  }
  inline const D3DXVECTOR3 getFieldCenter(const Field &f) const {
    return m_boardObject->getFieldCenter(f);
  }

  void resetBrickPositions(bool colored);
  void flashWinnerBlocks();
  void updateGraphicsDoingMove(const Move &m);
  void refreshGraphics();
#ifdef DEVELOPER_MODE
  void showCameraData();
#endif
  void startAdjustCamera(const CPoint &p, int flags);
  void endAdjustCamera();
  void adjustCameraAngle(const CPoint &p);
  void adjustCameraPos(  const CPoint &p);

  void showCursor(bool show);
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
  void        showInfo(const TCHAR *format,...);
/*
  void        turnBoard(int degree);
*/
  void        toggleLight(int index, bool on);
public:
  CQuartoDlg(CWnd *pParent = NULL);
  
  enum { IDD = IDD_DIALOGQUARTO };

  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);

protected:
  virtual void OnCancel();
  virtual void OnOK();
  afx_msg LRESULT OnMsgRefreshView(WPARAM wp, LPARAM lp);
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg HCURSOR OnQueryDragIcon();
  virtual BOOL OnInitDialog();
  afx_msg void OnPaint();
  afx_msg void OnClose();
  afx_msg void OnMouseMove(  UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonUp(  UINT nFlags, CPoint point);
  afx_msg void OnFileNew();
  afx_msg void OnFileOpen();
  afx_msg void OnFileSave();
  afx_msg void OnFileSaveAs();
  afx_msg void OnFileExit();
/*
  afx_msg void OnViewLeft();
  afx_msg void OnViewRight();
*/
  afx_msg void OnViewResetView();
  afx_msg void OnViewLight1();
/*
  afx_msg void OnViewLight2();
*/
  afx_msg void OnOptionsLevelBeginner();
  afx_msg void OnOptionsLevelExpert();
  afx_msg void OnOptionsColoredGame();
  afx_msg void OnHelpAboutquarto();
  afx_msg void OnDumpSetup();
  DECLARE_MESSAGE_MAP()
};
