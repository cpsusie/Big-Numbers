#include "stdafx.h"
#include <Random.h>
#include <Process.h>
#include "Whist3.h"
#include "CardBitmap.h"
#include "Whist3Dialog.h"
#include "DecideGameDialog.h"
#include "SummaryDialog.h"
#include "BacksideDialog.h"
#include "DecideGameTrainingDialog.h"
#include "RulesDialog.h"

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CWhist3Dialog::CWhist3Dialog(CWnd* pParent) : CDialog(CWhist3Dialog::IDD, pParent), m_sync(0) {

  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  IdentifyDialog dlg;
  dlg.DoModal();

  try {
    const Options &options = getOptions();
    m_player = new Whist3Player(options.m_myName, this, options.m_connected ? options.m_dealerName : _T(""));
  } catch(Exception e) {
    fatalError(_T("%s"),e.what());
  }
}

void CWhist3Dialog::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWhist3Dialog, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_QUERYDRAGICON()
  ON_WM_PAINT()
  ON_WM_KEYDOWN()
  ON_WM_LBUTTONDOWN()
  ON_COMMAND(ID_GAME_SUMMARY      , OnGameSummary     )
  ON_COMMAND(ID_GAME_LASTTRICK    , OnGameLastTrick   )
  ON_COMMAND(ID_GAME_BACKSIDE     , OnGameBackside    )
  ON_COMMAND(ID_GAME_AUTOPLAY     , OnGameAutoPlay    )
  ON_COMMAND(ID_GAME_EXIT         , OnGameExit        )
  ON_COMMAND(ID_GAME_TRAIN        , OnGameTrain       )
  ON_COMMAND(ID_HELP_REGLER       , OnHelpRegler      )
  ON_COMMAND(ID_HELP_ABOUT        , OnHelpAbout       )
  ON_COMMAND(ID_DECIDE_GAME       , OnDecideGame      )
  ON_COMMAND(ID_END_GAME          , OnEndGame         )
  ON_COMMAND(ID_ASK_PLAYAGAIN     , OnAskPlayAgain    )
  ON_BN_CLICKED(IDC_SUBSTITUTE_OK , OnSubstituteOk    )
  ON_BN_CLICKED(IDC_START_BUTTON  , OnStartButton     )
  ON_MESSAGE(ID_MSG_STATECHANGE   , OnMsgStateChanged )
END_MESSAGE_MAP()

void CWhist3Dialog::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  } else {
    CDialog::OnSysCommand(nID, lParam);
  }
}

HCURSOR CWhist3Dialog::OnQueryDragIcon() {
  return (HCURSOR) m_hIcon;
}

void CWhist3Dialog::fatalError(const TCHAR *format, ... ) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format,argptr);
  va_end(argptr);
  MessageBox(msg.cstr(),_T("Fatal fejl"), MB_ICONSTOP);
  exit(-1);
}

BOOL CWhist3Dialog::OnInitDialog() {
  CDialog::OnInitDialog();


  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);
  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if (!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon

  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
  randomize();
  CRect rect;
  GetClientRect(&rect);
  createWorkBitmap(rect.Size());
  m_backgroundBrush = CreateSolidBrush(RGB(0,128,0));

  m_player->start();

  return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CWhist3Dialog::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return TRUE;
  }
    
  return CDialog::PreTranslateMessage(pMsg);
}

void CWhist3Dialog::createWorkBitmap(const CSize &size) {
  if(m_workDC.m_hDC != NULL)
    m_workDC.DeleteDC();
  if(m_workBitmap.m_hObject != NULL)
    m_workBitmap.DeleteObject();

  CClientDC screen(this);
  m_workDC.CreateCompatibleDC(&screen);
  m_workSize = size;
  m_workBitmap.CreateBitmap(size.cx,size.cy,screen.GetDeviceCaps(PLANES),screen.GetDeviceCaps(BITSPIXEL),NULL);
  m_workBitmap.SetBitmapDimension(size.cx,size.cy);
  m_workDC.SelectObject(m_workBitmap);
}

void CWhist3Dialog::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  } else {  
    CPaintDC dc(this);

    paintAll(m_workDC);

    dc.BitBlt(0,0,m_workSize.cx,m_workSize.cy,&m_workDC,0,0,SRCCOPY);

    CDialog::OnPaint();
  }
}

void CWhist3Dialog::repaint() {
  Invalidate(false);
}

void CWhist3Dialog::paintAll(CDC &dc) {
  dc.SelectObject(m_backgroundBrush);
  dc.Rectangle(0,0,m_workSize.cx,m_workSize.cy);
  dc.SetBkColor(RGB(0,128,0));
  paintCards(dc);
  const int myId = m_player->getPlayerId();

  dc.TextOut(10 ,m_workSize.cy - 20,getPlayerInfo(MOD3(myId+0)).cstr());
  dc.TextOut(10 ,1                 ,getPlayerInfo(MOD3(myId+1)).cstr());

  const String s = getPlayerInfo(MOD3(myId+2));
  dc.TextOut(m_workSize.cx-(int)s.length()*8-10,1,s.cstr());

  paintCenteredText(dc,50,m_message);
  paintCenteredText(dc,70,m_gameTypeMessage);
}

void CWhist3Dialog::paintCenteredText(CDC &dc, int y, const String &s) {
  if(s.length() > 0) {
    CSize size = dc.GetTextExtent(s.cstr());
    dc.TextOut((m_workSize.cx-size.cx)/2,y,s.cstr());
  }
}

void CWhist3Dialog::myMessage(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  m_message = vformat(format,argptr);
  repaint();
  va_end(argptr);
}

CRect CWhist3Dialog::getCardRect(int index) {
  CPoint p = getCardPosition(index);
  return CRect(p.x,p.y,p.x+CardBitmap::getCardWidth(),p.y+CardBitmap::getCardWidth());
}

CPoint CWhist3Dialog::getCardPosition(int index) {
#define CARDDIST 15
#define CARDXPOS(i) m_workSize.cx / 2 + (i-7)*CARDDIST
#define CARDYPOS(i) m_workSize.cy - CardBitmap::getCardHeight() - 2 - (m_player->isCardMarked(i)?20:0)

  return CPoint(CARDXPOS(index),CARDYPOS(index));
}

int CWhist3Dialog::findCardIndex(const CPoint &p) { // find index of selected card
  const CardHand &hand = m_player->getHand();

  for(int i = 12; i >= 0; i--) {
    if(hand.getCard(i) >= 0) { // maybe it isnt there anymore
      CRect r = getCardRect(i);
      if(r.PtInRect(p)) {
        return i;
      }
    }
  }
  return -1; // point not on a card
}

static const CPoint tableCardPositions[] = {
  CPoint(250,200)  // played card nearest myself
 ,CPoint(225,150)  // played card to the left
 ,CPoint(275,160)  // played card to the right
};

static const CPoint *tableCardPos[3] = { // will be set in updateplayerid
  tableCardPositions+0
 ,tableCardPositions+1
 ,tableCardPositions+2
};

void CWhist3Dialog::paintCards(CDC &dc) {
  const CardHand &hand = m_player->getHand();

  for(int i = 0; i < 13; i++) { // display my own cards
    const Card card = hand.getCard(i);
    if(card >= 0) {
      CardBitmap::paintCard(dc, getCardPosition(i),card);
    }
  }
  const int myId    = m_player->getPlayerId();
  const int lPlayer = MOD3(myId+1);
  const int rPlayer = MOD3(myId+2);

  const CardCount cardCount = m_player->getCardCount();

  const int backSide = getOptions().m_backside;
  for(int i = 0; i < 13; i++) { // display cards of left player 
    if(cardCount.hasCard(lPlayer,i)) {
      CardBitmap::paintCard(dc, 2,(i+4)*CARDDIST,backSide);
    }
  }
  for(int i = 0; i < 13; i++) { // display cards of right player
    if(cardCount.hasCard(rPlayer,i)) {
      CardBitmap::paintCard(dc, m_workSize.cx - CardBitmap::getCardWidth() - 2, (i+4)*CARDDIST,backSide);
    }
  }

  const CardsOnTable &played = m_player->getPlayed();
  int j = played.getPlayer0();
  for(int i = 0; i < played.getCount(); i++, INC3(j)) {
    CardBitmap::paintCard(dc, *tableCardPos[j],played.getCard(i));
  }
}

String CWhist3Dialog::getPlayerInfo(int playerId) const {
  const GameDescription &gd         = m_player->getGameDesc();
  const PlayerList      &playerList = m_player->getPlayerList();
  const TrickCount      &trickCount = m_player->getTrickCount();

  if(gd.getPlayer0() < 0) {
    return playerList.getName(playerId);
  } else {
    return format(_T("%s (%d/%d)"),playerList.getName(playerId).cstr(),trickCount.getTrickCount(playerId),gd.getStandardTrickCount(playerId));
  }
}

void CWhist3Dialog::handlePropertyChanged(const class PropertyContainer *source, int id, const void *oldValue, const void *newValue) { // Executed by player-thread. not WinThread.
  const Whist3Player &player   = *(const Whist3Player*)source;
  const GameState     oldState = *(const GameState*)oldValue;
  const GameState     newState = *(const GameState*)newValue;
  switch(newState) {
  case STATE_ACCEPT_CARDS:
    postStateChange(oldState, newState);
    m_sync.wait();
    break;

  case STATE_COLLECT_CARDS:
    postStateChange(oldState, newState);
    Sleep(1000);
    break;

  case STATE_END_GAME:
    m_tmpHistory = player.getGameHistory();
    postStateChange(oldState, newState);
    m_sync.wait();
    break;

  case STATE_ASK_PLAYAGAIN:
    saveScore();
    postStateChange(oldState, newState);
    m_sync.wait();
    break;

  default:
    postStateChange(oldState, newState);
    break;
  }
}

void CWhist3Dialog::postStateChange(GameState oldState, GameState newState) {
  PostMessage(ID_MSG_STATECHANGE, oldState, newState);
}

void CWhist3Dialog::postCommand(int command) {
  PostMessage(WM_COMMAND, MAKELONG(command, 0), 0);
}

LRESULT CWhist3Dialog::OnMsgStateChanged(WPARAM wp, LPARAM lp) {
  const GameState oldState = (GameState)wp;
  const GameState newState = (GameState)lp;

  m_stateMessage = gameStateString[newState];

  const PlayerList &playerList   = m_player->getPlayerList();
  const int         playerInTurn = m_player->getPlayerInTurn();
  const String      playerName   = playerList.getName(playerInTurn);

  switch(newState) {
  case STATE_INIT      :
    GetDlgItem(IDC_SUBSTITUTE_OK)->ShowWindow(SW_HIDE);
    break;

  case STATE_CONNECTING:
    if(!m_player->isClient()) {
      myMessage(_T("Venter på andre spillere"));
      GetDlgItem(IDC_START_BUTTON)->ShowWindow(SW_SHOW);
    }
    GetDlgItem(IDC_SUBSTITUTE_OK)->ShowWindow(SW_HIDE);
    break;

  case STATE_DEALING   :
    initGameTypeMessage();
    GetDlgItem(IDC_START_BUTTON)->ShowWindow(SW_HIDE);
    if(!m_player->isClient()) {
      myMessage(_T("Deler kort ud"));
    } else {
      myMessage(_T("Modtager kort"));
    }
    break;

  case STATE_DECIDE_GAME:
    initGameTypeMessage();
    myMessage(_T("Vælg spil"));
    break;

  case STATE_WAITING_FOR_DECIDE:
    initGameTypeMessage();
    myMessage(_T("Venter på at %s skal vælge spil"), playerName.cstr());
    break;

  case STATE_SELECT_CARDS:
    setGameTypeMessage();
    myMessage(_T("Vælg 0-%d kort at bytte"),m_player->getKat().getCount());
    SetDlgItemText(IDC_SUBSTITUTE_OK,_T("Byt"));
    GetDlgItem(IDC_SUBSTITUTE_OK)->ShowWindow(SW_SHOW);
    break;

  case STATE_WAITING_FOR_SELECT:
    setGameTypeMessage();
    myMessage(_T("Venter på at %s skal bytte kort"), playerName.cstr());
    GetDlgItem(IDC_SUBSTITUTE_OK)->ShowWindow(SW_HIDE);
    break;

  case STATE_ACCEPT_CARDS:
    setGameTypeMessage();
    if(oldState == STATE_SELECT_CARDS) {
      myMessage(_T("Godkend kort"));
    } else {
      myMessage(_T("Der er ikke flere kort at bytte"));
    }
    SetDlgItemText(IDC_SUBSTITUTE_OK,_T("Ok"));
    GetDlgItem(IDC_SUBSTITUTE_OK)->ShowWindow(SW_SHOW);
    break;

  case STATE_PLAY_CARD:
    myMessage(_T("Vælg et kort at spille"));
    GetDlgItem(IDC_SUBSTITUTE_OK)->ShowWindow(SW_HIDE);
    break;

  case STATE_WAITING_FOR_PLAY:
    myMessage(_T("Venter på at %s skal spille"), playerName.cstr());
    GetDlgItem(IDC_SUBSTITUTE_OK)->ShowWindow(SW_HIDE);
    break;

  case STATE_COLLECT_CARDS:
    myMessage(_T("Indsamler kort"));
    break;

  case STATE_END_GAME:
    myMessage(_T("Viser stilling"));
    postCommand(ID_END_GAME);
    break;

  case STATE_ASK_PLAYAGAIN:
    myMessage(_T(""));
    postCommand(ID_ASK_PLAYAGAIN);
    break;

  case STATE_DEALER_DISCONNECTED:
    fatalError(_T("Kortgiveren har forladt spillet. Kan ikke fortsætte"));
    break;

  case STATE_CLIENT_DISCONNECTED:
    fatalError(_T("En spiller har forladt spillet. Kan ikke fortsætte"));
    break;

  default:
    fatalError(_T("Invalid gamestate:%d"), newState);
    break;
  }
  return 0;
}

void CWhist3Dialog::initGameTypeMessage() {
  m_gameTypeMessage = _T("");
}

void CWhist3Dialog::setGameTypeMessage() {
  const int              myId = m_player->getPlayerId();
  const GameDescription &gd   = m_player->getGameDesc();
  const PlayerList      &pl   = m_player->getPlayerList();

  String p = gd.getPlayer0()==myId?_T("Du"):pl.getName(gd.getPlayer0());
  m_gameTypeMessage = format(_T("%s har valgt %s"),p.cstr(),gd.toString().cstr()).cstr();
}

void CWhist3Dialog::OnAskPlayAgain() {
  if(MessageBox( _T("Vil du spille igen"),_T(""), MB_YESNO + MB_ICONQUESTION) != IDYES) {
    exit(0);
  }
  m_sync.signal();
}

void CWhist3Dialog::initTablePos(int myId) {
  for(int i = 0; i < 3; i++) {
    tableCardPos[MOD3(myId+i)] = tableCardPositions + i;
  }
}

void CWhist3Dialog::init(const Whist3Player &player) {
  initTablePos(m_player->getPlayerId());
}

GameDescription CWhist3Dialog::getDecidedGame(const Whist3Player &player) {
  postCommand(ID_DECIDE_GAME);
  m_sync.wait();
  return m_tmpDescription;
}

CardIndexSet CWhist3Dialog::getCardsToSubstitute(const Whist3Player &player) {
  m_sync.wait();
  return m_tmpCardSet;
}

UINT CWhist3Dialog::getCardToPlay(const Whist3Player &player) {
  m_sync.wait();
  return m_tmpCardIndex;
}

void CWhist3Dialog::OnDecideGame() {
  DecideGameDialog dlg(m_player->getPlayerId(), m_player->getPlayerList(), m_player->getGameHistory());
  dlg.DoModal();
  m_tmpDescription = dlg.getGameDescription();
  m_sync.signal();
}

void CWhist3Dialog::OnSubstituteOk() {
  switch(m_player->getState()) {
  case STATE_SELECT_CARDS:
    m_tmpCardSet = m_player->getMarkedCards();
    m_sync.signal();
    break;
  case STATE_ACCEPT_CARDS:
    m_sync.signal();
    break;
  }
}

void CWhist3Dialog::OnLButtonDown(UINT nFlags, CPoint point) {
  switch(m_player->getState()) {
  case STATE_SELECT_CARDS:
    markUnmarkCard(point);
    break;

  case STATE_PLAY_CARD:
    playCard(point);
    break;
  }
    
  CDialog::OnLButtonDown(nFlags, point);
}

void CWhist3Dialog::markUnmarkCard(const CPoint &point) {
  const int index = findCardIndex(point);
  if(index >= 0) {
    m_player->markUnmarkCard(index);
    repaint();
  }
}

void CWhist3Dialog::playCard(const CPoint &point) {
  m_tmpCardIndex = findCardIndex(point);
  String msg;
  if(!m_player->validatePlayedCard(m_tmpCardIndex,msg)) {
    myMessage(_T("%s"),msg.cstr());
  } else {
    m_sync.signal();
  }
}

void CWhist3Dialog::saveScore() {
  const int          myId = m_player->getPlayerId();
  const PlayerList  &pl   = m_player->getPlayerList();
  const GameHistory &gh   = m_player->getGameHistory();

  log(_T("%-30s:%s"),pl.toString(myId).cstr(),gh.toString(myId,11).cstr());
}

void CWhist3Dialog::OnEndGame() {
  OnGameSummary();
  m_sync.signal();
}

void CWhist3Dialog::OnGameSummary() {
  const int          myId = m_player->getPlayerId();
  const PlayerList  &pl   = m_player->getPlayerList();
  const GameHistory &gh   = m_player->getGameHistory();

  SummaryDialog dlg(myId, pl, gh);
  dlg.DoModal();
}

void CWhist3Dialog::OnHelpAbout() {
  CAboutDlg dlg;
  dlg.DoModal();
}

void CWhist3Dialog::OnGameExit() {
  if(MessageBox( _T("Er du sikker på at du vil stoppe"),_T(""), MB_YESNO + MB_ICONQUESTION) == IDYES) {
    PostMessage(WM_QUIT);
  }
}

// catch ENTER and ESC
void CWhist3Dialog::OnOK() {
//  CDialog::OnOK();
}

void CWhist3Dialog::OnCancel() {
  OnGameExit();
}

static const TCHAR *someNames[] = {
  _T("Kaj")
 ,_T("Andrea")
 ,_T("Thelma")
 ,_T("Louise")
 ,_T("Mads")
 ,_T("Kurt")
 ,_T("Paul")
 ,_T("John")
 ,_T("George")
 ,_T("Steen")
 ,_T("Stina")
 ,_T("Alex")
 ,_T("Marianne")
 ,_T("Anne")
 ,_T("Ane")
 ,_T("Morten")
 ,_T("Bente")
 ,_T("Torben")
 ,_T("Jens")
 ,_T("Hans")
 ,_T("Erik")
};

static String selectRandomName(const String &name1, const String &name2) {
  for(;;) {
    int index = rand() % ARRAYSIZE(someNames);
    String result = someNames[index];
    if(result != name1 && result != name2) {
      return result;
    }
  }
}

void CWhist3Dialog::OnStartButton() {
  const PlayerList &playerList = m_player->getPlayerList();
  const int count = playerList.getCount();
  String myName = playerList.getName(0);

  if(count < 3) {   // start computer-spillere
    String name1 = selectRandomName(myName,playerList.getName(1));
    startComputerPlayer(name1);
    if(count == 1) {
      String name2 = selectRandomName(myName,name1);
      startComputerPlayer(name2);
    }
  }
}

void CWhist3Dialog::OnGameLastTrick() {
  CardsOnTable lastTrick = m_player->getLastTrick();
  if(lastTrick.getCount() == 0)
    return;

  repaint();
  CClientDC dc(this);
  for(int i = 0, j = lastTrick.getPlayer0(); i < lastTrick.getCount(); i++, INC3(j)) {
    CardBitmap::paintCard(dc, *tableCardPos[j], lastTrick.getCard(i));
  }
  Sleep(2000);
  repaint();
}

void CWhist3Dialog::OnGameBackside() {
  BacksideDialog dlg;
  if(dlg.DoModal() == IDOK) {
    repaint();
  }
}

void CWhist3Dialog::OnGameAutoPlay() {
  if(toggleMenuItem(this, ID_GAME_AUTOPLAY)) {
  }
}

void CWhist3Dialog::OnHelpRegler() {
  RulesDialog dlg;
  dlg.DoModal();
}

void CWhist3Dialog::OnGameTrain() {
  DecideGameTrainingDialog dlg;
  dlg.DoModal();
}
