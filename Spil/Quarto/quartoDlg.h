#pragma once

#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/Profile.h>
#include "GameBoardObject.h"

#define DEVELOPER_MODE

class CQuartoDlg : public CDialog {
public:
  CQuartoDlg(CWnd *pParent = NULL);
  
  enum { IDD = IDD_DIALOGQUARTO };

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);

private:
  HICON                   m_hIcon;
  HACCEL                  m_accelTable;
  bool                    m_initdone;
  int                     m_adjustingCameraFlags;
  CPoint                  m_rbuttonDownPoint;
  D3DXVECTOR3             m_startCamPos;
  D3DXVECTOR3             m_boardCenter;
  D3Scene                 m_scene;
  int                     m_selectedBrick;
  String                  m_gameName;
#ifdef _DEBUG
  D3SceneObject          *m_coordinateSystem;
#endif
  GameBoardObject        *m_boardObject;
//  GameSceneObject        *m_brickMarkerTable[16];
//  GameSceneObject        *m_brickObject[FIELDCOUNT];
  int                     m_boardFaceIndex[ROWCOUNT][COLCOUNT];
  D3DXVECTOR3             m_fieldCenter[ROWCOUNT][COLCOUNT];
  D3DMATERIAL             m_brickMaterial[2];
  Game                    m_game;
  Player                  m_startPlayer;

  CRect getViewportRect();

  void createScene();
  void createLight();
  void createBoard();
  void resetCamera();
  void render();
/*
  void createBricks();
  void createMaterials();
*/

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
  inline const D3DXVECTOR3 &getFieldCenter(const Field &f) const {
    return m_fieldCenter[f.m_row][f.m_col];
  }
/*
  void initFieldCenter();
  void flashWinnerBlocks();

  void resetBrickPositions(bool colored);
  void updateGraphicsDoingMove(const Move &m);
  void refreshGraphics();
*/
#ifdef DEVELOPER_MODE
  void showCameraData();
#endif
  void startAdjustCamera(const CPoint &p, int flags);
  void endAdjustCamera();
  void adjustCameraAngle(const CPoint &p);
  void adjustCameraPos(  const CPoint &p);

  void showCursor(bool show);
/*
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
  LPD3DXMESH  createBrick(bool big, bool black, bool square, bool top);
  D3DMATERIAL createMaterial(COLORREF emissive, COLORREF specular);
//  void      setMaterialColor( LPDIRECT3DRMMATERIAL material, COLORREF emissive, COLORREF specular);
//  void      setMaterial(const LPDIRECT3DRMMATERIAL &material);
  Profile     createProfile(const Point2DArray &points);
  Profile     createProfile(const Point2D      *data, int n);
  int         getBrickFromPoint(const CPoint &p) const;
*/
  Field       getFieldFromPoint(const CPoint &p) const;
/*
  void        markBrick(  int b);
  void        unmarkBrick(int b);
*/
  void        selectField(const Field &f);
/*
  void        selectBrick(int b);
  inline int  getSelectedBrick() const {
    return m_selectedBrick;
  }
  inline const Field &getSelectedField() const {
    return m_selectedField;
  }
  LPD3DXMESH  getBrickMarker(int b);
*/
  void        showInfo(const TCHAR *format,...);
/*
  void        turnBoard(int degree);
*/
  void        toggleLight(int index, bool on);
protected:
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg HCURSOR OnQueryDragIcon();
  virtual BOOL OnInitDialog();
  afx_msg void OnPaint();
  afx_msg void OnClose();
  afx_msg void OnMouseMove(  UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonUp(  UINT nFlags, CPoint point);
/*
  afx_msg void OnFileNew();
  afx_msg void OnFileOpen();
  afx_msg void OnFileSave();
  afx_msg void OnFileSaveAs();
*/
  afx_msg void OnFileExit();
/*
  afx_msg void OnViewLeft();
  afx_msg void OnViewRight();
*/
  afx_msg void OnViewResetView();
  afx_msg void OnViewLight1();
/*
  afx_msg void OnViewLight2();
  afx_msg void OnOptionsLevelBeginner();
  afx_msg void OnOptionsLevelExpert();
  afx_msg void OnOptionsColoredGame();
*/
  afx_msg void OnHelpAboutquarto();
  afx_msg void OnDumpSetup();
  afx_msg LRESULT OnMsgRefreshView(WPARAM wp, LPARAM lp);
  virtual void OnCancel();
  virtual void OnOK();
  DECLARE_MESSAGE_MAP()
public:
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
