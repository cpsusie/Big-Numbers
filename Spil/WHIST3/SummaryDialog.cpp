#include "stdafx.h"
#include "gametypes.h"
#include "whist3.h"
#include "SummaryDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SummaryDialog::SummaryDialog(int myId, const PlayerList &players, const GameHistory &gameHistory, CWnd *pParent) : CDialog(SummaryDialog::IDD, pParent) {
  m_myId        = myId;
  m_players     = players;
  m_gameHistory = gameHistory;
}

void SummaryDialog::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(SummaryDialog, CDialog)
END_MESSAGE_MAP()

BOOL SummaryDialog::OnInitDialog() {
  __super::OnInitDialog();

  CListBox *lb = (CListBox*)GetDlgItem(IDC_SUMMARY_LIST);

  lb->InsertString(-1,m_players.toString(m_myId).cstr());
  lb->InsertString(-1,m_gameHistory.toString(m_myId,GAMETYPE_SOL  ).cstr());
  lb->InsertString(-1,m_gameHistory.toString(m_myId,GAMETYPE_SANS ).cstr());
  lb->InsertString(-1,m_gameHistory.toString(m_myId,GAMETYPE_SPAR ).cstr());
  lb->InsertString(-1,m_gameHistory.toString(m_myId,GAMETYPE_FARVE).cstr());
  lb->InsertString(-1,_T(" "));
  for(int i = 0; i < m_gameHistory.getGameCount(); i++) {
    lb->InsertString(-1,m_gameHistory.toString(m_myId,i).cstr());
  }

/*
  char tmp[100];
  sprintf(tmp,_T("%-10.10s %10.10s %10.10s %10.10s"),
      _T(" "),
      m_players.m_name[ m_myPlayerId     ],
      m_players.m_name[(m_myPlayerId+1)%3],
      m_players.m_name[(m_myPlayerId+2)%3]);
  for(int i=0;i<4;i++) { // sol .. farve
    char s1[3], s2[3], s3[3];
    sprintf(tmp,_T("%-10.10s %10s %10s %10s"),
          gameTypeString[i],
          pstr(s1,m_playedGames[ m_myPlayerId     ][i]),
          pstr(s2,m_playedGames[(m_myPlayerId+1)%3][i]),
          pstr(s3,m_playedGames[(m_myPlayerId+2)%3][i])
    );
    lb->InsertString(-1,tmp);
  }

  lb->InsertString(-1,_T(" "));
  for(i=0;i<m_gameCount;i++) {
    sprintf(tmp,_T("%-10d %10d %10d %10d"),
        i+1,
        m_scoreList[i][ m_myPlayerId     ],
        m_scoreList[i][(m_myPlayerId+1)%3],
        m_scoreList[i][(m_myPlayerId+2)%3]);
    lb->InsertString(-1,tmp);
  }
*/

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}
