#pragma once

#include "Game.h"

class COneOrTwoDlg : public CDialog {
private:
  HACCEL         m_accelTable;
  HICON          m_hIcon;
  AbstractGame  *m_game;
  int            m_timerInterval;
  PositionSet    m_markedPositions;
  Player         m_startPlayer;
  bool           m_showMoveList;

  void newGame(Player starter);
  void newGame();
  void changePositionMark(int pos);
  int  getMarkedCount() const;
  GameType  getSelectedGameType();
  PlayLevel getSelectedLavel();
  void setGameType(int id);
  void selectLevel(int id);
  void enableRemoveButton();
  void startTimer(int msec);
  void stopTimer();
public:
  COneOrTwoDlg(CWnd *pParent = nullptr);
  void toggleShowMoveList();
  bool isShowingMoveList() const {
    return m_showMoveList;
  }
  enum { IDD = IDD_ONEORTWO_DIALOG };

protected:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual void OnOK();
  virtual void OnCancel();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnPaint();
  afx_msg void OnClose();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnFileNewGameHumanStart();
  afx_msg void OnFileNewGameComputerStart();
  afx_msg void OnFileExit();
  afx_msg void OnSpilPentagon();
  afx_msg void OnSpilHexagon();
  afx_msg void OnSpilOctagon();
  afx_msg void OnLevelBeginner();
  afx_msg void OnLevelIntermediate();
  afx_msg void OnLevelExpert();
  afx_msg void OnHelpAboutOneOrTwo();
  afx_msg void OnButtonRemove();
  afx_msg void OnButtonShowMoves();
  DECLARE_MESSAGE_MAP()
};
