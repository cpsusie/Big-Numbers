#pragma once

class SummaryDialog : public CDialog {
private:
  int         m_myId;            // my identification 0..2. dealer = 0
  PlayerList  m_players;         // contains the names of the players
  GameHistory m_gameHistory;

public:
    SummaryDialog(int myId, const PlayerList &players, const GameHistory &gameHistory, CWnd *pParent = NULL);

    enum { IDD = IDD_SUMMARY_DIALOG };

    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};


