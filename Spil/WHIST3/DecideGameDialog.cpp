#include "stdafx.h"
#include "gametypes.h"
#include "whist3.h"
#include "DecideGameDialog.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

DecideGameDialog::DecideGameDialog(int myId, const PlayerList &players, const GameHistory &gameHistory, CWnd *pParent) : CDialog(DecideGameDialog::IDD, pParent) {
  m_selectedPointButton = -1;
  m_playedGamesColours  = EMPTYSTRING;
  m_playedGamesNames    = EMPTYSTRING;
  m_playedGamesSol      = EMPTYSTRING;
  m_playedGamesSpades   = EMPTYSTRING;
  m_gameType            = -1;
  m_playedGamesSans     = EMPTYSTRING;
  m_suit                = -1;

  m_myId        = myId;
  m_players     = players;
  m_gameHistory = gameHistory;
}

void DecideGameDialog::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Radio(pDX, IDC_SOL_RADIO          , m_gameType);
  DDX_Radio(pDX, IDC_KLOER_RADIO        , m_suit);
  DDX_Radio(pDX, IDC_1POINT_RADIO       , m_selectedPointButton);
  DDX_Text( pDX, IDC_PLAYEDGAMES_COLOURS, m_playedGamesColours);
  DDX_Text( pDX, IDC_PLAYEDGAMES_NAMES  , m_playedGamesNames);
  DDX_Text( pDX, IDC_PLAYEDGAMES_SOL    , m_playedGamesSol);
  DDX_Text( pDX, IDC_PLAYEDGAMES_SPADES , m_playedGamesSpades);
  DDX_Text( pDX, IDC_PLAYEDGAMES_SANS   , m_playedGamesSans);
}

BEGIN_MESSAGE_MAP(DecideGameDialog, CDialog)
  ON_BN_CLICKED(IDC_SOL_RADIO, OnSolRadio)
  ON_BN_CLICKED(IDC_SANS_RADIO, OnSansRadio)
  ON_BN_CLICKED(IDC_SPAR_RADIO, OnSparRadio)
  ON_BN_CLICKED(IDC_FARVE_RADIO, OnFarveRadio)
END_MESSAGE_MAP()

GameDescription DecideGameDialog::getGameDescription() const {
  return GameDescription(m_myId,getGameType(),getSuit(), getPointsPerTrick());
}

void DecideGameDialog::setColourEnabled(BOOL s) {
    GetDlgItem(IDC_KLOER_RADIO  )->EnableWindow( s);
    GetDlgItem(IDC_RUDER_RADIO  )->EnableWindow( s);
    GetDlgItem(IDC_HJERTER_RADIO)->EnableWindow( s);
    GetDlgItem(IDC_TRUMF_STATIC )->EnableWindow( s);
}

BOOL DecideGameDialog::OnInitDialog() {
  WINDOWPLACEMENT wp;

  __super::OnInitDialog();
  GetWindowPlacement( &wp );
  wp.ptMinPosition.y -= 10;
  wp.ptMaxPosition.y -= 10;

  SetWindowPlacement( &wp );

  m_playedGamesNames   = m_players.toString(m_myId).cstr();
  m_playedGamesSol     = m_gameHistory.toString(m_myId,GAMETYPE_SOL) .cstr();
  m_playedGamesSans    = m_gameHistory.toString(m_myId,GAMETYPE_SANS ).cstr();
  m_playedGamesSpades  = m_gameHistory.toString(m_myId,GAMETYPE_SPAR ).cstr();
  m_playedGamesColours = m_gameHistory.toString(m_myId,GAMETYPE_FARVE).cstr();

  GetDlgItem(IDC_SOL_RADIO  )->EnableWindow(!m_gameHistory.isGameUsed(m_myId,GAMETYPE_SOL  ));
  GetDlgItem(IDC_SANS_RADIO )->EnableWindow(!m_gameHistory.isGameUsed(m_myId,GAMETYPE_SANS ));
  GetDlgItem(IDC_SPAR_RADIO )->EnableWindow(!m_gameHistory.isGameUsed(m_myId,GAMETYPE_SPAR ));
  GetDlgItem(IDC_FARVE_RADIO)->EnableWindow(!m_gameHistory.isGameUsed(m_myId,GAMETYPE_FARVE));

  if(!m_gameHistory.isGameUsed(m_myId,GAMETYPE_SOL)) {
    m_gameType = GAMETYPE_SOL;
  } else if(!m_gameHistory.isGameUsed(m_myId,GAMETYPE_SANS)) {
    m_gameType = GAMETYPE_SANS;
  } else if(!m_gameHistory.isGameUsed(m_myId,GAMETYPE_SPAR)) {
    m_gameType = GAMETYPE_SPAR;
  } else {
    m_gameType = GAMETYPE_FARVE;
  }

  bool pointUsed[4];
  for(int p = 1; p <= 4; p++) {
    pointUsed[p-1] = m_gameHistory.isPointsUsed(m_myId,p);
  }
  GetDlgItem(IDC_1POINT_RADIO)->EnableWindow(!pointUsed[0]);
  GetDlgItem(IDC_2POINT_RADIO)->EnableWindow(!pointUsed[1]);
  GetDlgItem(IDC_3POINT_RADIO)->EnableWindow(!pointUsed[2]);
  GetDlgItem(IDC_4POINT_RADIO)->EnableWindow(!pointUsed[3]);

  if(!pointUsed[0]) {
    m_selectedPointButton = 0;
  } else if(!pointUsed[1]) {
    m_selectedPointButton = 1;
  } else if(!pointUsed[2]) {
    m_selectedPointButton = 2;
  } else {
    m_selectedPointButton = 3;
  }

  m_suit = 0;
  setColourEnabled(m_gameType == GAMETYPE_FARVE);

  UpdateData(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

void DecideGameDialog::OnSolRadio() {
  setColourEnabled(FALSE);
}

void DecideGameDialog::OnSansRadio() {
  setColourEnabled(FALSE);
}

void DecideGameDialog::OnSparRadio() {
  setColourEnabled(FALSE);
}

void DecideGameDialog::OnFarveRadio() {
  setColourEnabled(TRUE);
}
