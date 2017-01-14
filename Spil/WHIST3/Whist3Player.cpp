#include "stdafx.h"
#include <MyUtil.h>
#include <Random.h>
#include <Date.h>
#include "Whist3Player.h"
#include "Whist3EPlayer.h"

Whist3Player::Whist3Player(const String &name, Whist3PlayerCallback *callback, const String &dealerName) 
: m_callback(*callback)
, m_isClient(dealerName.length() != 0)
, m_myName(name)
{
  m_myId     = 0;
  addPropertyChangeListener(callback);
  if(isClient()) {
    try {
      USES_CONVERSION;
      m_socket[0] = tcpOpen(123,T2A(dealerName.cstr()));
    } catch(Exception e) {
      throwException(_T("Kan ikke finde kortgivers computer (%s)"), dealerName.cstr());
    }
  } else { // I am the dealer
    try {
      m_listen = tcpCreate(123);
    } catch(Exception e) {
      throwException(_T("Kan ikke åbne tcp-port %d:%s"),123,e.what());
    }
    m_players.addPlayer(m_myName);
  }
}

UINT Whist3Player::run() {
  try {
    play();
  } catch(TcpException e) {
    if(isClient()) {
      setState(STATE_DEALER_DISCONNECTED);
    } else {
      setState(STATE_CLIENT_DISCONNECTED);
    }
  } catch(Exception e) {
    log(_T("Exception:%s"),e.what());
    exit(-1);
  }
  return 0;
}

void Whist3Player::play() {
  m_gameState = STATE_INIT;
  if(isDealer()) {
    setState(STATE_CONNECTING);
  } else {
    setState(STATE_INIT);
  }

  randomize();
  m_playerInTurn = 0;
  for(;;) {
    switch(m_gameState) {
    case STATE_INIT:
      handleStateInit();
      break;
    case STATE_CONNECTING:
      handleStateConnecting();
      break;
    case STATE_DEALING:
      handleStateDealing();
      break;
    case STATE_DECIDE_GAME:
      handleStateDecideGame();
      break;
    case STATE_SELECT_CARDS:
      handleStateSelectCards();
      break;
    case STATE_WAITING_FOR_DECIDE:
      handleStateWaitingForDecideGame();
      break;
    case STATE_WAITING_FOR_SELECT:
      handleStateWaitingForSelect();
      break;
    case STATE_ACCEPT_CARDS:
      handleStateAcceptCards();
      break;
    case STATE_PLAY_CARD:
      handleStatePlayCard();
      break;
    case STATE_WAITING_FOR_PLAY:
      handleStateWaitingForPlay();
      break;
    case STATE_COLLECT_CARDS:
      handleStateCollectCards();
      break;
    case STATE_END_GAME:
      handleStateEndGame();
      break;
    case STATE_ASK_PLAYAGAIN:
      handleStateAskPlayAgain();
      break;
    case STATE_DEALER_DISCONNECTED:
      setState(STATE_INIT);
      break;
    case STATE_CLIENT_DISCONNECTED:
      handleConnection();
      setState(m_prevState);
      break;
    default: 
      throwException(_T("Ukendt gamestate:%d"),m_gameState);
    }
  }
}

void Whist3Player::handleStateInit() {
  if(isClient()) {
    m_myId = Communication::readClientId(m_socket[0],m_myName);
    m_callback.init(*this);
    setState(STATE_CONNECTING);
  }
}

void Whist3Player::handleStateConnecting() {
  if(isDealer()) {
    handleConnection();
    if(m_players.getCount() == 3) {
      startGame();
      setState(STATE_DEALING);
    } else {
      setState(STATE_CONNECTING);
    }
  } else {
    NetPlayers received = Communication::readPlayers(m_socket[0]);
    m_players  = received.m_playerList;
    m_gameDesc = received.m_headPlayer;
    startGame();
    if(m_players.getCount() == 3) {
      setState(STATE_DEALING);
    }
  }
}

void Whist3Player::handleStateDealing() {
  if(isDealer()) {
    dealCards();
    m_playerInTurn = m_gameDesc.getPlayer0();
    m_substituteCount = 0;
    if(m_playerInTurn == m_myId) {
      setState(STATE_DECIDE_GAME);
    } else {
      setState(STATE_WAITING_FOR_DECIDE);
    }
  } else {
    readNewHand(m_socket[0]);
    sortMyHand();
    unmarkAllCards();
    initTableCards(m_gameDesc.getPlayer0());
    m_playerInTurn = m_gameDesc.getPlayer0();
    if(m_playerInTurn == m_myId) {
      setState(STATE_DECIDE_GAME);
    } else {
      setState(STATE_WAITING_FOR_DECIDE);
    }
  }
}

void Whist3Player::handleStateDecideGame() {
  if(isDealer()) {
    m_gameDesc = m_callback.getDecidedGame(*this);
    writeGameDescription(m_socket[0]);
    writeGameDescription(m_socket[1]);
    writePlayerInTurn(m_socket[0]);
    writePlayerInTurn(m_socket[1]);
    m_gameHistory.addPlayedGame(m_gameDesc);
    setState(STATE_SELECT_CARDS);
  } else {
    m_gameDesc = m_callback.getDecidedGame(*this);
    writeGameDescription(m_socket[0]);
    m_gameHistory.addPlayedGame(m_gameDesc);
    setState(STATE_WAITING_FOR_SELECT);
  }
}

void Whist3Player::handleStateSelectCards() {
  m_markedCards = m_callback.getCardsToSubstitute(*this);
  substituteMarkedCards();
  setState(STATE_ACCEPT_CARDS);
}

void Whist3Player::handleStateWaitingForDecideGame() {
  if(isDealer()) {
    if(m_gameDesc.getPlayer0() == 1) {
      readGameDescription(m_socket[0]);
      writeGameDescription(m_socket[1]);
      writePlayerInTurn(m_socket[0]);
      writePlayerInTurn(m_socket[1]);
    } else { // must be player 2
      readGameDescription(m_socket[1]);
      writeGameDescription(m_socket[0]);
      writePlayerInTurn(m_socket[0]);
      writePlayerInTurn(m_socket[1]);
    }
  } else {
    readGameDescription(m_socket[0]);
  }
  m_gameHistory.addPlayedGame(m_gameDesc);
  setState(STATE_WAITING_FOR_SELECT);
}

void Whist3Player::handleStateWaitingForSelect() {
  if(isDealer()) {
    switch(m_playerInTurn) {
    case 0:
      writePlayerInTurn(m_socket[0]); 
      writePlayerInTurn(m_socket[1]);
      if(m_kat.isEmpty()) {
        unmarkAllCards();
        substituteMarkedCards();
        setState(STATE_ACCEPT_CARDS);
      } else {
        setState(STATE_SELECT_CARDS);
      }
      break;
    case 1:
      m_kat.removeCards(Communication::writeNewCards(m_socket[0],m_kat));
      m_substituteCount++;
      if(m_substituteCount < 3) {
        INC3(m_playerInTurn);
        writePlayerInTurn(m_socket[0]);
        writePlayerInTurn(m_socket[1]);
        setState(STATE_WAITING_FOR_SELECT);
      } else {
        m_playerInTurn = m_gameDesc.getPlayer0();
        writePlayerInTurn(m_socket[0]);
        writePlayerInTurn(m_socket[1]);
        setState(STATE_WAITING_FOR_PLAY);
      }
      break;
    case 2:
      m_kat.removeCards(Communication::writeNewCards(m_socket[1],m_kat));
      m_substituteCount++;
      if(m_substituteCount < 3) {
        INC3(m_playerInTurn);
        writePlayerInTurn(m_socket[0]);
        writePlayerInTurn(m_socket[1]);
        setState(STATE_WAITING_FOR_SELECT);
      } else {
        m_playerInTurn = m_gameDesc.getPlayer0();
        writePlayerInTurn(m_socket[0]);
        writePlayerInTurn(m_socket[1]);
        setState(STATE_WAITING_FOR_PLAY);
      }
    }
  } else { // client
    readPlayerInTurn(m_socket[0]);
    if(m_substituteCount == 3) {
      if(m_gameDesc.getPlayer0() == m_myId ) {
        setState(STATE_PLAY_CARD);
      } else {
        setState(STATE_WAITING_FOR_PLAY);
      }
    } else {
      if(m_playerInTurn != m_myId) {
        setState(STATE_WAITING_FOR_SELECT);
      } else {
        if(!m_kat.isEmpty()) {
          setState(STATE_SELECT_CARDS);
        } else {
          unmarkAllCards();
          substituteMarkedCards();
          setState(STATE_ACCEPT_CARDS);
        }
      }
    }
  }
}

void Whist3Player::handleStateAcceptCards() {
  unmarkAllCards();
  if(isDealer()) {
    m_substituteCount++;
    if(m_substituteCount < 3) {
      INC3(m_playerInTurn);
    } else {
      m_playerInTurn = m_gameDesc.getPlayer0();
    }

    writePlayerInTurn(m_socket[0]);
    writePlayerInTurn(m_socket[1]);

    if(m_substituteCount == 3) {
      setState(STATE_WAITING_FOR_PLAY);
    } else {
      setState(STATE_WAITING_FOR_SELECT);
    }
  } else {
    setState(STATE_WAITING_FOR_SELECT);
  }
}

void Whist3Player::handleStatePlayCard() {
  const int index = m_callback.getCardToPlay(*this);

  String tmp;
  if(!validatePlayedCard(index, tmp)) {
    return;  // validering af renonce
  }

  Card card = m_myHand.getCard(index);
  m_played.addCard(card);
  m_myHand.removeCard(index);

  if(isDealer()) {
    updateCardCount();
    INC3(m_playerInTurn);
    writeTableCards(m_socket[0]);
    writeTableCards(m_socket[1]);
    if(m_played.getCount() == 3) {
      setState(STATE_COLLECT_CARDS);
    } else {
      setState(STATE_WAITING_FOR_PLAY);
    }
  } else {
    writeTableCards(m_socket[0]);
    setState(STATE_WAITING_FOR_PLAY);
  }
}

void Whist3Player::handleStateWaitingForPlay() {
  if(isDealer()) {
    switch(m_playerInTurn) {
    case 0:
      writeTableCards(m_socket[0]);
      writeTableCards(m_socket[1]);
      setState(STATE_PLAY_CARD);
      break;
    case 1:
      readTableCards(m_socket[0]);
      updateCardCount();
      if(m_played.getCount() == 3) {
        writeTableCards(m_socket[0]);
        writeTableCards(m_socket[1]);
        setState(STATE_COLLECT_CARDS);
      } else {
        INC3(m_playerInTurn);
        writeTableCards(m_socket[0]);
        writeTableCards(m_socket[1]);
        setState(STATE_WAITING_FOR_PLAY);
      }
      break;
    case 2:
      readTableCards(m_socket[1]);
      updateCardCount();
      if(m_played.getCount() == 3) {
        writeTableCards(m_socket[0]);
        writeTableCards(m_socket[1]);
        setState(STATE_COLLECT_CARDS);
      } else {
        INC3(m_playerInTurn);
        writeTableCards(m_socket[0]);
        writeTableCards(m_socket[1]);
        setState(STATE_WAITING_FOR_PLAY);
      }
      break;
    }
  } else { // client
    readTableCards(m_socket[0]);
    if(m_played.getCount() == 3) {
      setState(STATE_COLLECT_CARDS);
    } else if(m_playerInTurn != m_myId) {
      setState(STATE_WAITING_FOR_PLAY);
    } else {
      setState(STATE_PLAY_CARD);
    }
  }
}

void Whist3Player::handleStateCollectCards() {
  m_playerInTurn = collectTableCards();
  if(m_trickCount.allTricksGone()) {
    m_gameHistory.addResult(m_gameDesc,m_trickCount);
    setState(STATE_END_GAME);
  } else if(m_playerInTurn == m_myId) {
    setState(STATE_PLAY_CARD);
  } else {
    setState(STATE_WAITING_FOR_PLAY);
  }
}

void Whist3Player::handleStateEndGame() {
  m_gameDesc.nextPlayer0();
  if(m_gameHistory.allGamesPlayed()) {
    setState(STATE_ASK_PLAYAGAIN);
  } else {
    setState(STATE_DEALING);
    startRound();
  }
}

void Whist3Player::handleStateAskPlayAgain() {
  startGame();
  setState(STATE_DEALING);
}

// --------------------------------------------------------------------------------------

void Whist3Player::handleConnection() {
  SOCKET s = tcpAccept(m_listen);

  String clientName = Communication::writeClientId(s, m_players.getCount());
  m_socket[m_players.getCount()-1] = s;
  m_players.addPlayer(clientName);
  
  m_gameDesc.setRandomStarter();                      // decide whos starting
  for(int i = 1; i < m_players.getCount(); i++) {
    Communication::writePlayers(m_socket[i-1],m_players,m_gameDesc.getPlayer0());
  }
}

void Whist3Player::startGame() {
  m_callback.startGame(*this);
  m_gameHistory.clear();
  startRound();
}

void Whist3Player::startRound() {
  m_trickCount.clear();
  m_cardCount.setAll(0);
  m_cardStatistic.clear();
  initTableCards(0);

  m_callback.startRound(*this);
}

void Whist3Player::sortMyHand() {
  m_myHand.sort(m_gameDesc.getGameType() != GAMETYPE_SOL || m_gameState == STATE_DEALING);
}

void Whist3Player::dealCards() {
  Card deck[52];

  for(int i = 0; i < 52; i++) {
    deck[i] = (Card)i;
  }
  for(int i = 0; i < 52; i++) { // shuffle the cards
    int r = rand() % 52;
    Card tmp = deck[i];
    deck[i] = deck[r];
    deck[r] = tmp;
  }

  m_cardCount.setAll(13);

  m_myHand.clear();
  int c = 0;
  for(int i = 0; i < 13; i++) { // give 13 cards to player0 (myself)
    m_myHand.addCard(deck[c++]);
  }

  m_kat.clear();
  for(int i = 0; i < 13; i++) { // put 13 cards into the kat
    m_kat.addCard(deck[c++]);
  }

  CardHand hand;
  for(int i = 0; i < 13; i++) { // give 13 cards to player1
    hand.addCard(deck[c++]);
  }

  writeNewHand(m_socket[0],hand);

  hand.clear();
  for(int i = 0;i < 13; i++) { // give 13 cards to player2
    hand.addCard(deck[c++]);
  }
  writeNewHand(m_socket[1],hand);

  sortMyHand();

  unmarkAllCards();
  initTableCards(m_gameDesc.getPlayer0());
}

void Whist3Player::writeNewHand(SOCKET s, const CardHand &hand) {
  Communication::writePlayerHand(s,hand,m_cardCount);
}

void Whist3Player::readNewHand(SOCKET s) {
  NetPlayerHand received = Communication::readPlayerHand(s);
  m_myHand    = received.m_hand;
  m_cardCount = received.m_cardCount;
}

void Whist3Player::writeGameDescription(SOCKET s) {
  Communication::writeGameDescription(s,m_gameDesc);
}

void Whist3Player::readGameDescription(SOCKET s) {
  m_gameDesc = Communication::readGameDescription(s);
}

void Whist3Player::writePlayerInTurn(SOCKET s) {
  Communication::writePlayerInTurn(s,m_playerInTurn,m_kat,m_substituteCount);
}

void Whist3Player::readPlayerInTurn(SOCKET s) {
  NetPlayerInTurn received = Communication::readPlayerInTurn(s);
  m_playerInTurn    = received.m_playerInTurn;
  m_kat             = received.m_kat;
  m_substituteCount = received.m_substituteCount;
}

void Whist3Player::writeTableCards(SOCKET s) {
  Communication::writeTableCards(s,m_playerInTurn,m_played,m_cardCount);
}

void Whist3Player::readTableCards(SOCKET s) {
  NetTableCards received = Communication::readTableCards(s);
  m_played          = received.m_played;
  m_playerInTurn    = received.m_playerInTurn;
  m_cardCount       = received.m_cardCount;
}

void Whist3Player::initTableCards(int player0) {
  m_played.init(player0);
}

void Whist3Player::unmarkAllCards() {
  m_markedCards.clear();
}

void Whist3Player::markCard(UINT index) {
  m_markedCards.add(index);
}

void Whist3Player::unmarkCard(UINT index) {
  m_markedCards.remove(index);
}

void Whist3Player::markUnmarkCard(UINT index) {
  if(index < 13) {
    if(m_markedCards.contains(index)) {
      unmarkCard(index);
    } else if(m_markedCards.size() < m_kat.getCount()) {
      markCard(index);
    }
  }
}

void Whist3Player::updateCardCount() {
  m_cardCount.decrementPlayer(m_playerInTurn);
}

void Whist3Player::substituteMarkedCards() {
  const int count = m_markedCards.size();
  if(count > m_kat.getCount()) {
    throwException(_T("Du ønsker at bytte for mange kort (%d) Max=%d"), count, m_kat.getCount());
  }

  m_cardStatistic.update(m_myHand,m_markedCards);

  CardHand newCards;
  switch(isClient()) {
  case false:
    { for(int i = 0; i < count; i++) {
        newCards.addCard(m_kat.getCard(i));
      }
      m_kat.removeCards(count);
    }
    break;

  case true:
    newCards = Communication::readNewCards(m_socket[0],count);
    break;
  }

  m_myHand.substituteCardsInSet(m_markedCards, newCards);
  sortMyHand();

  // Now mark the recieved cards
  unmarkAllCards();
  for(int i = 0; i < newCards.getCount(); i++) {
    markCard(m_myHand.findIndex(newCards.getCard(i)));
  }
}

bool Whist3Player::validatePlayedCard(UINT index, String &msg) const { // validering af renonce
  if(index > 12 || m_myHand.getCard(index) < 0) {
    return false;
  }

  if(m_played.getCount() == 0) {
    return true; // no further validation needed
  }

  const Card thisCard = m_myHand.getCard(index);
  const Card card0    = m_played.getCard0();
  if(CARDSUIT(card0) == CARDSUIT(thisCard)) {
    return true; // OK
  }

  if(m_myHand.getSuitCount(CARDSUIT(card0)) > 0) {
    msg = _T("Du skal spille samme farve som det først spillede kort");
    return false; // got one of the same suit as the first m_played
  }

  return true;
}

UINT Whist3Player::collectTableCards() { // return player who wins the trick
  const int winner = m_gameDesc.findTrickWinner(m_played);
  m_cardStatistic.update(m_played);
  m_trickCount.update(winner);
  m_lastTrick = m_played;
  initTableCards(winner);
  return winner;
}

void Whist3Player::setState(GameState newState) {
  switch(newState) {
  case STATE_SELECT_CARDS:
  case STATE_WAITING_FOR_SELECT:
    sortMyHand();
    break;
  }

  m_prevState = m_gameState;
  m_gameState = newState;
  notifyPropertyChanged(PLAYER_STATE, &m_prevState, &m_gameState);
}
