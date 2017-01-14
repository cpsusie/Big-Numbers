#pragma once

#include <MyString.h>
#include <Thread.h>
#include <PropertyContainer.h>
#include <Tcp.h>
#include "GameTypes.h"
#include "Communication.h"

class Whist3Player;

typedef enum {
  PLAYER_STATE
} Whist3PlayerProperty;

class Whist3PlayerCallback : public PropertyChangeListener {
public:
  virtual void            init(                 const Whist3Player &player) {
  }
  virtual void            startGame(            const Whist3Player &player) {
  }
  virtual void            startRound(           const Whist3Player &player) {
  }
  virtual GameDescription getDecidedGame(       const Whist3Player &player) = 0;
  virtual CardIndexSet    getCardsToSubstitute( const Whist3Player &player) = 0;
  virtual UINT            getCardToPlay(        const Whist3Player &player) = 0;
  void handlePropertyChanged(const class PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  }
};

class Whist3Player : public Thread, public PropertyContainer {
private:
  const bool             m_isClient;
  const String           m_myName;
  int                    m_myId;      // [0..2]. dealer is 0
  SOCKET                 m_listen;    // Used by the dealer to get connections from other players
  SOCKET                 m_socket[2];
  PlayerList             m_players;
  GameDescription        m_gameDesc;
  CardHand               m_myHand;
  CardHand               m_kat;
  CardsOnTable           m_played;
  CardsOnTable           m_lastTrick;
  GameHistory            m_gameHistory;
  GameState              m_prevState;
  GameState              m_gameState;
  int                    m_playerInTurn;     
  int                    m_substituteCount;
  TrickCount             m_trickCount;
  CardIndexSet           m_markedCards;
  CardCount              m_cardCount;
  CardStatistic          m_cardStatistic;
  Whist3PlayerCallback  &m_callback;

  void handleConnection();
  void sortMyHand();
  void unmarkAllCards();
  void markCard(  UINT index);
  void unmarkCard(UINT index);

  void startGame();
  void startRound();
  void dealCards();
  void writeNewHand(         SOCKET s, const CardHand &hand);
  void readNewHand(          SOCKET s);
  void writeGameDescription( SOCKET s);
  void readGameDescription(  SOCKET s);
  void writePlayerInTurn(    SOCKET s);
  void readPlayerInTurn(     SOCKET s);
  void writeTableCards(      SOCKET s);
  void readTableCards(       SOCKET s);

  void initTableCards(int player0);
  void updateCardCount();
  void updatePlayedGames();
  void updateScoreList();
  void substituteMarkedCards();
  void playCard(UINT index);

  UINT collectTableCards(); // return player who wins the trick
  void setState(GameState newState);

  void handleStateInit();
  void handleStateConnecting();
  void handleStateDealing();
  void handleStateDecideGame();
  void handleStateSelectCards();
  void handleStateWaitingForDecideGame();
  void handleStateWaitingForSelect();
  void handleStateAcceptCards();
  void handleStatePlayCard();
  void handleStateWaitingForPlay();
  void handleStateCollectCards();
  void handleStateEndGame();
  void handleStateAskPlayAgain();

  void play();
public:
  Whist3Player(const String &name, Whist3PlayerCallback *callback, const String &dealerName = _T("")); // if dealerName = "" I am the dealer
  UINT run();

  void markUnmarkCard(UINT index);
  
  bool isCardMarked(  UINT index) {
    return m_markedCards.contains(index);
  }

  bool isClient() const {
    return m_isClient;
  }

  bool isDealer() const {
    return !m_isClient;
  }

  int getPlayerId() const {
    return m_myId;
  }

  bool validatePlayedCard(UINT index, String &msg) const; // validering af renonce

  const PlayerList &getPlayerList() const {
    return m_players;
  }

  const GameDescription &getGameDesc() const {
    return m_gameDesc;
  }

  UINT getPlayerInTurn() const {
    return m_playerInTurn;
  }

  GameState getState() const {
    return m_gameState;
  }

  const CardHand &getHand() const {
    return m_myHand;
  }

  const CardHand &getKat() const {
    return m_kat;
  }

  const CardIndexSet &getMarkedCards() const {
    return m_markedCards;
  }

  const CardsOnTable &getPlayed() const {
    return m_played;
  }

  const CardsOnTable &getLastTrick() const {
    return m_lastTrick;
  }

  const GameHistory &getGameHistory() const {
    return m_gameHistory;
  }

  const TrickCount &getTrickCount() const {
    return m_trickCount;
  }

  const CardCount &getCardCount() const {
    return m_cardCount;
  }

  const CardStatistic getCardStatistic() const {
    return m_cardStatistic;
  }
};
