#pragma once

#include <Bpn.h>
#include <Iterator.h>

typedef enum {
  FIRST_GAMETYPE = 0
 ,GAMETYPE_SOL   = 0
 ,GAMETYPE_SANS  = 1
 ,GAMETYPE_SPAR  = 2
 ,GAMETYPE_FARVE = 3
 ,LAST_GAMETYPE  = 3
} GameType;

extern const TCHAR *gameTypeString[];

typedef enum {
  FIRST_SUIT = 0
 ,KLOER      = 0
 ,RUDER      = 1
 ,HJERTER    = 2
 ,SPAR       = 3
 ,LAST_SUIT  = 3
} Suit;
extern const TCHAR *suitString[];

typedef char Card; // can be negative

#define CARDSUIT(card)       (Suit)((card)%4)
#define CARDVALUE(card)      ((card)/4 + 1)
#define CARDVALUEAH(card)    ((CARDVALUE(card)==1)?14:CARDVALUE(card))
#define MAKECARD(suit,value) ((Card)(((value)-1)*4 + (suit)))

#define MOD3(x) ((x)%3)
#define INC3(x) x = MOD3((x)+1)

#define FIELDWIDTH      10
#define LEFTFIELDWIDTH  6

typedef enum {
  STATE_INIT
 ,STATE_CONNECTING
 ,STATE_DEALING
 ,STATE_DECIDE_GAME
 ,STATE_SELECT_CARDS
 ,STATE_WAITING_FOR_DECIDE
 ,STATE_WAITING_FOR_SELECT
 ,STATE_ACCEPT_CARDS
 ,STATE_PLAY_CARD
 ,STATE_WAITING_FOR_PLAY
 ,STATE_COLLECT_CARDS
 ,STATE_END_GAME
 ,STATE_ASK_PLAYAGAIN
 ,STATE_DEALER_DISCONNECTED
 ,STATE_CLIENT_DISCONNECTED
} GameState;

extern const TCHAR *gameStateString[];

class CardsOnTable {
private:
  int   m_player0;
  UINT  m_count;
  Card  m_card[3];

public:
  CardsOnTable() {
    init(0);
  }

  void init(int player0);
  void addCard(Card card);
  int  getPlayerId(UINT index) const; // return (m_player0+index)%3       , index=[0..2]
  Card getCard(    UINT index) const; // return m_card[getPlayerId(index)], index=[0..2]

  int getCount() const {
    return m_count;
  }

  int getPlayer0() const {
    return m_player0;
  }

  Card getCard0() const {
    return getCard(0);
  }

  Card getCard1() const {
    return getCard(1);
  }

  Card getCard2() const {
    return getCard(2);
  }
};

class GameDescription {
private:
  int      m_player0;        // The player who decides what game to play
  GameType m_gameType;
  Suit     m_suit;
  int      m_pointsPerTrick;

public:
  GameDescription(int player0, GameType type, Suit suit, int points);
  GameDescription(int player0 = -1);

  void setRandomStarter();
  void nextPlayer0();
  int  getStandardTrickCount(int playerId) const;
  int  findTrickWinner(const CardsOnTable &cards) const;

  int getPlayer0() const {
    return m_player0;
  }

  GameType getGameType() const {
    return m_gameType;
  }

  Suit getSuit() const {
    return m_suit;
  }

  int getPointsPerTrick() const {
    return m_pointsPerTrick;
  }

  String toString() const;
};

class CardIndexSet {
private:
  USHORT m_bits;
  friend class CardIndexSetIterator;

public:
  CardIndexSet() {
    init();
  }
  void clear() {
    init();
  }
  void init(    UINT count = 0);
  bool contains(UINT i) const;
  void add(     UINT i);
  void remove(  UINT i);
  int  selectRandom() const; // select random elmenent from bitSet. Assume set not empty
  int  size() const;
  Iterator<int> getIterator() const;
};

class CardHand {
private:
  UINT m_count;
  Card m_card[13];

public:
  CardHand() {
    clear();
  }
  void clear();
  void removeCards(UINT count);
  void removeCard(UINT index);
  void addCard(Card card);
  void sort(bool sortAceHigest = true);
  void substituteCardsInSet(const CardIndexSet &set, const CardHand &kat);
  int  findIndex(Card card) const;
  bool contains(Card card) const;

  Card getCard(UINT index) const {
    return m_card[index];
  }

  int getCount() const {
    return m_count;
  }

  bool isEmpty() const {
    return getCount() == 0;
  }

  double getCardAverage()   const;
  double getCardAverageAceHigest() const;
  int    getCountCardsLessThan(           int value) const;
  int    getCountCardsGreaterThan(        int value) const;
  bool   isRenonce(                       Suit suit) const;
  int    getSuitCount(                    Suit suit) const;
  int    getNotSuitCount(                 Suit suit) const;
  int    getSuitGreaterThanAceHigestCount(Suit suit, int value) const;
  double getCardSuitAverage(              Suit suit) const;
  Suit   getMaxSuit(int &count, double &bavg) const;
  int    findLowest()   const;
  int    findHigest()   const;
  int    findLowestAceHigest() const;
  int    findHigestAceHigest() const;
  int    findLowestInSuit(                Suit suit) const;
  int    findHigestInSuit(                Suit suit) const;
  int    findLowestInSuitAceHigest(       Suit suit) const;
  int    findHigestInSuitAceHigest(       Suit suit) const;
  int    findHigestLowerThan(             Card card) const;
  int    findLowestHigherThanAceHisgest(  Card card) const;
  int    findLowestNotInSuitAceHigest(    Suit suit) const;
};

#define MAXNAMELENGTH 29

class PlayerList {
private:
  USHORT  m_count;
  char    m_name[3][MAXNAMELENGTH+1]; // dont modify

public:
  PlayerList() {
    clear();
  }

  void clear();
  void addPlayer(const String &name);
  String getName(UINT playerId) const;

  int getCount() const {
    return m_count;
  }

  String toString(UINT myId) const;
};

class TrickCount {
private:
  unsigned char m_trickCount[3];     // Number of tricks for each player

public:
  TrickCount() {
    clear();
  }

  void clear();
  void update(UINT trickWinner);
  UINT getTotalTrickCount() const;
  int getTrickCount(UINT playerId) const;

  bool allTricksGone() const {
    return getTotalTrickCount() == 13;
  }
};

class CardCount {
private:
  CardIndexSet m_cardSet[3]; // sets containing cards on each players hand

public:
  CardCount() {
    setAll();
  }
  void setAll(UINT count = 0);
  void decrementPlayer(UINT playerId); // remove random card from players cardSet.
  int  getCount(       UINT playerId) const;
  bool hasCard(        UINT playerId, UINT index) const;
};

class CardStatistic {
private:
  bool m_cardsGone[4][14];      // m_cardsGone[s][v] = true if card with suit s and value v=[1..13] has been played or substituted
  bool m_playerIsRenonce[3][4]; // m_playerIsRenonce[p][s] = true if player p is renonce in suit s

  void validateCardIsNotGone(Card card) const;
  void setCardGone(Card card);
public:
  CardStatistic() {
    clear();
  }

  void clear();
  void update(const CardsOnTable &played);
  void update(const CardHand &hand, const CardIndexSet &set);
  bool isCardGone(          Card card) const;
  bool allHigherIsGoneah(   Card card) const;
  bool allCardsOfSuitIsGone(Suit suit) const; // are all cards of suit gone
  bool isPlayerRenonce(UINT playerId, Suit suit) const;
};

class GamesPlayed {
private:
  unsigned char m_playedGames[4]; // Containts the games already played
public:
  GamesPlayed() {
    clear();
  }
  void clear();
  void addPlayedGame(GameType gameType, int pointsPerTrick);
  bool isPointsUsed(int pointsPerTrick) const;
  bool isGameUsed(  GameType gameType) const;
  int  getPointsPerTrick(GameType gameType) const {
    return m_playedGames[gameType];
  }
  int getCount() const;
  void setRandom(UINT count);
};

class GameHistory {
private:
  int         m_gameCount;         // Number of played games (0..12)
  int         m_scoreList[12][3];
  GamesPlayed m_playedGames[3]; // Containts the games already played for each user
public:
  GameHistory() {
    clear();
  }

  void clear();
  void addResult(    const GameDescription &gameDesc, const TrickCount &trickCount);
  void addPlayedGame(const GameDescription &gameDesc);
  bool isPointsUsed(UINT playerId, int pointsPerTrick) const;
  bool isGameUsed(  UINT playerId, GameType gameType) const;

  int getGameCount() const {
    return m_gameCount;
  }

  bool allGamesPlayed() const {
    return getGameCount() == 12;
  }

  String toString(UINT myId, GameType gameType) const;
  String toString(UINT myId, int index) const;
};

class Options {
public:
  String m_myName;
  String m_dealerName;
  bool   m_connected; // er jeg normal client eller dealer
  int    m_backside;

  Options() {
    load();
  }

  void load();
  void save();

};

class OptionsAccessor {
public:
  Options &getOptions();
  const Options &getOptions() const;
};

class DecideGameTrainingSet {
private:
  CardHand      m_hand;
  GamesPlayed   m_gamesPlayed;
  GameType      m_selectedGameType;
  int           m_selectedPointsPerTrick;
  Array<double> m_bpnOutput;

  void validate();
  void validateSelection(GameType gameType, int pointsPerTrick) const;
  static String getFileName();
  void init(const CardHand &hand, const GamesPlayed &gamesPlayed);
public:
  DecideGameTrainingSet();
  DecideGameTrainingSet(const CardHand &hand, const GamesPlayed &gamesPlayed);
  DecideGameTrainingSet(const String &src);
  void generateRandomTestData();
  void setSelectedGameAndPoint(GameType type, int pointsPerTrick);
  void setSelectedGameAndPoint(const Array<double> &bpnOutput);
  Array<double> &getBpnInput( Array<double> &input ) const;
  Array<double> &getBpnOutput(Array<double> &output) const;

  const CardHand &getHand() const {
    return m_hand;
  }

  const GamesPlayed &getGamesPlayed() const {
    return m_gamesPlayed;
  }

  GameType getSelectedGameType() const {
    return m_selectedGameType;
  }

  int getSelectedPointsPerTrick() {
    return m_selectedPointsPerTrick;
  }

  double getGameTypeScore(GameType gameType)       const;
  double getPointScore(   int      pointsPerTrick) const;

  String toString() const;

  static Array<DecideGameTrainingSet> loadTrainingData();
  void save();
};


class DecideGameBpn : public Bpn {
private:
  static Array<int> getLayerCount();
  static String getFileName();
public:
  DecideGameBpn();
  void learn(const DecideGameTrainingSet &data);
  void recognize(DecideGameTrainingSet &data);

  double getPatternError(const DecideGameTrainingSet &data) const;
  void load();
  void save();
};

void log(const TCHAR *format, ...);
