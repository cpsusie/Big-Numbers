#pragma once

#include "MazeApp.h"
#include <Thread.h>
#include "Maze.h"
#include "PathFinder.h"

class CMazeDlg : public CDialog {
public:
  CMazeDlg(CWnd *pParent = nullptr);

  HICON       m_hIcon;
  HACCEL      m_accelTable;
  bool        m_initialized;
  CDC         m_cleanMazeDC,m_workDC;
  CBitmap     m_cleanMazeBM,m_workBM;
  int         m_doorWidth;
  Maze       *m_maze;
  PathFinder *m_pathFinder;

  void destroyDC();
  void createDC();
  void stopPathFinder();
  bool hasPathFinder() const {
    return m_pathFinder != nullptr;
  }
  void newMaze();
  void DCToWork(CDC &dc);
  void workToScreen();
  void setDoorSize(int item, int i);
  void setDoorChoice(int choice);
  void setLevel(int level);

  enum { IDD = IDD_MAINDIALOG };

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
  virtual void DoDataExchange(CDataExchange *pDX);

protected:

  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnOK();
  afx_msg void OnCancel();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnFileQuit();
  afx_msg void OnFileNewMaze();
  afx_msg void OnEditFindPath();
  afx_msg void OnHelpAboutMaze();
  afx_msg void OnOptionsSpeedSlow();
  afx_msg void OnOptionsSpeedFast();
  afx_msg void OnOptionsKeepTrack();
  afx_msg void OnOptionsPathchoiceLeftFirst();
  afx_msg void OnOptionsPathchoiceRightFirst();
  afx_msg void OnOptionsPathchoiceMix();
  afx_msg void OnOptionsDoorSize1();
  afx_msg void OnOptionsDoorSize2();
  afx_msg void OnOptionsDoorSize3();
  afx_msg void OnOptionsDoorSize4();
  afx_msg void OnOptionsDoorSize5();
  afx_msg void OnOptionsDoorSize6();
  afx_msg void OnOptionsDoorSize10();
  afx_msg void OnEditClear();
  afx_msg void OnFilePrint();
  afx_msg void OnClose();
  afx_msg void OnOptionsLevelEasy();
  afx_msg void OnOptionsLevelHard();
  afx_msg void OnOptionsLevelHexagonal();
  afx_msg void OnOptionsLevelTriangular();
  DECLARE_MESSAGE_MAP()
};

