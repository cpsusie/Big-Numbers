#pragma once

#include "game.h"

class C4PaaStribeDlg : public CDialog {
public:
    C4PaaStribeDlg(CWnd *pParent = NULL);
    CRect getHoleRect(int r, int c);
    void paintFieldValues();
    void paintHole(int r, int c, COLORREF color);
    void paintHole(const Position &p, COLORREF color) { paintHole(p.m_r,p.m_c,color); }
    void paintGame();
    void paintMessage();
    void animateMove(Move m, Player who);
    void executeMove(Move m, Player who);
    void markStribe(Move lastmove);
    int  point2column(CPoint point);
    Move executeMachineMove();
    void showMessage(TCHAR *format, ... );
    void newGame();
    void checkgameover(Move lastmove);
    String messageString;
    Game m_game;
    Player m_starter;

    enum { IDD = IDD_4PAASTRIBE_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:
    HICON m_hIcon;

    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnFileAfslut();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnFileNytSpil();
    afx_msg void OnHelpAbout();
    afx_msg void OnOptionsAnimation();
    afx_msg void OnOptionsVisevalueringsfunktion();
    afx_msg void OnOptionsBegynder();
    afx_msg void OnOptionsExpert();
    afx_msg void OnOptionsVisfeltvrdier();
    DECLARE_MESSAGE_MAP()
};

