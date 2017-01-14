#include "stdafx.h"
#include <MyUtil.h>
#include <Tcp.h>
#include "GameTypes.h"

class NetInitializer {
public:
  char m_myName[30]; 
  char m_version[30];
  NetInitializer(const String &myName, const String &version);
  NetInitializer() {}
};

class NetPlayers {
public:
  PlayerList m_playerList;
  int        m_headPlayer;
  NetPlayers(const PlayerList &playerList, int headPlayer);
  NetPlayers() {}
};

class NetPlayerHand {
public:
  CardHand  m_hand;
  CardCount m_cardCount;
  NetPlayerHand(const CardHand &hand, const CardCount &cardCount);
  NetPlayerHand() {};
};

class NetPlayerInTurn {
public:
  int      m_playerInTurn;
  CardHand m_kat;
  int      m_substituteCount;
  NetPlayerInTurn(int playerInTurn, const CardHand &kat, int substituteCount);
  NetPlayerInTurn() {}
};

class NetTableCards {
public:
  int          m_playerInTurn;
  CardsOnTable m_played;
  CardCount    m_cardCount;
  NetTableCards(int playerInTurn, const CardsOnTable &played, const CardCount &cardCount);
  NetTableCards() {}
};

class Communication {
private:
  static const char *gameVersion;

public:

  static int             readClientId(        SOCKET s, const String &myName); // returns playerId
  static String          writeClientId(       SOCKET s, int id);               // returns players name
  static NetPlayers      readPlayers(         SOCKET s);
  static void            writePlayers(        SOCKET s, const PlayerList &playerList, int headPlayer);
  static GameDescription readGameDescription( SOCKET s);
  static void            writeGameDescription(SOCKET s, const GameDescription &gameDescription);
  static NetPlayerHand   readPlayerHand(      SOCKET s);
  static void            writePlayerHand(     SOCKET s, const CardHand &hand, const CardCount &cardCount);
  static NetPlayerInTurn readPlayerInTurn(    SOCKET s);
  static void            writePlayerInTurn(   SOCKET s, int playerInTurn, const CardHand &kat, int substituteCount);
  static NetTableCards   readTableCards(      SOCKET s);
  static void            writeTableCards(     SOCKET s, int playerInTurn, const CardsOnTable &played, const CardCount &cardCount);
  static CardHand        readNewCards(        SOCKET s, int wanted);
  static int             writeNewCards(       SOCKET s, const CardHand &kat); // returns how many was send
};
