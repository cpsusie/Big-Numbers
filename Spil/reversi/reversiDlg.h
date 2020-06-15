#pragma once

class CReversiDlg : public CDialog {
private:
    HICON               m_hIcon;
    HACCEL              m_accelTable;
    bool                m_currentIsSystemCursor;
    GameState           m_game;
    bool                m_timerIsRunning;
    int                 m_score;
    bool                m_computerTurn;
    int                 m_whoStarts;

    void  drawLine(CDC &dc, int x1, int y1, int x2, int y2);
    CRect getFieldRect(int r, int c);
    void  markField(int r, int c, int who);
    int   findField(CPoint &point);
    void  startGame();
    void  showWinner();
    bool  gameOver();
    bool  computerMustPass();
    bool  userMustPass();
    void  startTimer();
    void  stopTimer();
    void  computerMove();
public:
    CReversiDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_REVERSI_DIALOG };

protected:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnSpilQuit();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnHjlpAboutreversi();
    afx_msg void OnOptionsVisScore();
    afx_msg void OnSpilNytspil();
    DECLARE_MESSAGE_MAP()
};

