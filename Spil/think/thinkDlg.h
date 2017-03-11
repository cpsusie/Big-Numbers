#pragma once

#include "Game.h"

class CThinkDlg : public CDialog {
private:
  HICON m_hIcon;
  Game  g;
  void  drawLine(CClientDC &dc, int x1, int y1, int x2, int y2);
  void  markField(int r, int c, bool current);
  CRect getRect(int r, int c) const;
  void  drawBoard();
  bool  findField(const CPoint &point, int &r, int &c) const;
  void  showWinner();
public:
  CThinkDlg(CWnd *pParent = NULL);
  enum { IDD = IDD_THINK_DIALOG };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnFileQuit();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  DECLARE_MESSAGE_MAP()
};

