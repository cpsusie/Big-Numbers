#pragma once

class DecideGameDialog : public CDialog {
public:
  DecideGameDialog(int myId, const PlayerList &players, const GameHistory &gameHistory, CWnd *pParent = nullptr);
  GameDescription getGameDescription() const;

private:
  int         m_myId;
  PlayerList  m_players;     // contains the names of the players
  GameHistory m_gameHistory; // containts the games already played

  GameType getGameType() const {
    return (GameType)m_gameType;
  }

  int getPointsPerTrick() const {
    return m_selectedPointButton + 1;
  }

  Suit getSuit() const {
    return (Suit)m_suit;
  }

    enum { IDD = IDD_DECIDE_GAME_DIALOG };
    int     m_selectedPointButton;
    CString m_playedGamesColours;
    CString m_playedGamesNames;
    CString m_playedGamesSol;
    CString m_playedGamesSpades;
    int     m_gameType;
    CString m_playedGamesSans;
    int     m_suit;

    void setColourEnabled(BOOL s);

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSolRadio();
    afx_msg void OnSansRadio();
    afx_msg void OnSparRadio();
    afx_msg void OnFarveRadio();
    afx_msg void OnButton1();
    DECLARE_MESSAGE_MAP()
};


