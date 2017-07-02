#include "stdafx.h"
#include <MyUtil.h>
#include "Communication.h"

const char *Communication::gameVersion = "Vers 1.1";

NetInitializer::NetInitializer(const String &myName, const String &version) {
  if(myName.length() >= sizeof(m_myName)) {
    throwException(_T("Name <%s> too long. Length=%d. max length=%d"),myName.cstr(), myName.length(), sizeof(m_myName)-1);
  }
  if(version.length() >= sizeof(m_version)) {
    throwException(_T("Version <%s> too long. Length=%d. max length=%d"),version.cstr(), version.length(), sizeof(m_version)-1);
  }

  USES_CONVERSION;
  strcpy(m_myName,T2A(myName.cstr()));
  strcpy(m_version,T2A(version.cstr()));
};

NetPlayers::NetPlayers(const PlayerList &playerList, int headPlayer) {
  m_playerList = playerList;
  m_headPlayer = headPlayer;
}

NetPlayerHand::NetPlayerHand(const CardHand &hand, const CardCount &cardCount) {
  m_hand      = hand;
  m_cardCount = cardCount;
}

NetPlayerInTurn::NetPlayerInTurn(int playerInTurn, const CardHand &kat, int substituteCount) {
  m_playerInTurn    = playerInTurn;
  m_kat             = kat;
  m_substituteCount = substituteCount;
}

NetTableCards::NetTableCards(int playerInTurn, const CardsOnTable &played, const CardCount &cardCount) {
  m_playerInTurn = playerInTurn;
  m_played       = played;
  m_cardCount    = cardCount;
}

int Communication::readClientId(SOCKET s, const String &myName) {
  NetInitializer buf(myName,gameVersion);
  tcpWrite(s, &buf, sizeof(buf));
  int myId;
  tcpRead(s,&myId,sizeof(myId));
  return myId;
}

String Communication::writeClientId(SOCKET s, int id) {
  NetInitializer buf;
  tcpRead(s,&buf,sizeof(buf));
  if(strcmp(buf.m_version,gameVersion) != 0) {
    const String gameVers = gameVersion;
    throwException(_T("Wrong version=<%s> received from dealer. this version is <%s>"),buf.m_version,gameVers.cstr());
  }
  tcpWrite(s,&id,sizeof(id));
  return buf.m_myName;
}

NetPlayers Communication::readPlayers(SOCKET s) {
  NetPlayers buf;
  tcpRead(s,&buf,sizeof(buf));
  return buf;
}

void Communication::writePlayers(SOCKET s, const PlayerList &playerList, int headPlayer) {
  NetPlayers buf(playerList,headPlayer);
  tcpWrite(s,&buf,sizeof(buf));
}

void Communication::writePlayerHand(SOCKET s, const CardHand &hand, const CardCount &cardCount) {
  NetPlayerHand buf(hand,cardCount);
  tcpWrite(s,&buf,sizeof(buf));
}

NetPlayerHand Communication::readPlayerHand(SOCKET s) {
  NetPlayerHand buf;
  tcpRead(s,&buf,sizeof(buf));
  return buf;
}

void Communication::writeGameDescription(SOCKET s, const GameDescription &gameDescription) {
  tcpWrite(s,&gameDescription,sizeof(GameDescription));
}

GameDescription Communication::readGameDescription(SOCKET s) {
  GameDescription buf;
  tcpRead(s,&buf,sizeof(buf));
  return buf;
}

void Communication::writePlayerInTurn(SOCKET s, int playerInTurn, const CardHand &kat, int substituteCount) {
  NetPlayerInTurn buf(playerInTurn, kat, substituteCount);
  tcpWrite(s,&buf,sizeof(buf));
}

NetPlayerInTurn Communication::readPlayerInTurn(SOCKET s) {
  NetPlayerInTurn buf;
  tcpRead(s,&buf,sizeof(buf));
  return buf;
}

void Communication::writeTableCards(SOCKET s, int playerInTurn, const CardsOnTable &played, const CardCount&cardCount) {
  NetTableCards buf(playerInTurn,played,cardCount);
  tcpWrite(s,&buf,sizeof(buf));
}

NetTableCards Communication::readTableCards(SOCKET s) {
  NetTableCards buf;
  tcpRead(s,&buf,sizeof(buf));
  return buf;
}

CardHand Communication::readNewCards(SOCKET s, int wanted) {
  tcpWrite(s,&wanted,sizeof(wanted));  // tell how many cards I want
  CardHand buf;
  tcpRead(s,&buf,sizeof(buf)); // get them
  return buf;
}

int Communication::writeNewCards(SOCKET s, const CardHand &kat) {
  int wanted;

  tcpRead(s,&wanted,sizeof(wanted)); // read how many cards he want
  if(wanted > kat.getCount())
    throwException(_T("Player wants too many cards(%d). I got only %d"), wanted, kat.getCount());

  CardHand hand;
  for(int i = 0; i < wanted; i++) {
    hand.addCard(kat.getCard(i));
  }
  tcpWrite(s,&hand,sizeof(CardHand)); // giv him the new cards
  return wanted;
}
