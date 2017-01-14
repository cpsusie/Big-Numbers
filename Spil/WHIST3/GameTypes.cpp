#include "stdafx.h"
#include <Exception.h>
#include "GameTypes.h"

const TCHAR *gameStateString[] = {
  _T("INIT               ")
 ,_T("CONNECTING         ")
 ,_T("DEALING            ")
 ,_T("DECIDE_GAME        ")
 ,_T("SELECT_CARDS       ")
 ,_T("WAITING_FOR_DECIDE ")
 ,_T("WAITING_FOR_SELECT ")
 ,_T("ACCEPT_CARDS       ")
 ,_T("PLAY_CARD          ")
 ,_T("WAITING_FOR_PLAY   ")
 ,_T("COLLECT_CARDS      ")
 ,_T("SUMMARY            ")
 ,_T("PLAYAGAIN          ")
 ,_T("DEALER_DISCONNECTED")
 ,_T("CLIENT_DISCONNECTED")
};

const TCHAR *gameTypeString[] = { _T("Sol"), _T("Sans"), _T("Spar"), _T("Farve") };
const TCHAR *suitString[]     = { _T("Klør"), _T("Ruder"), _T("Hjerter"), _T("Spar") };

// ----------------------------------TrickCount------------------------------------

void TrickCount::clear() {
  m_trickCount[0] = m_trickCount[1] = m_trickCount[2] = 0;
}

int TrickCount::getTrickCount(UINT playerId) const {
  return m_trickCount[playerId];
}

UINT TrickCount::getTotalTrickCount() const {
  return m_trickCount[0] + m_trickCount[1] + m_trickCount[2];
}

void TrickCount::update(UINT trickWinner) {
  DEFINEMETHODNAME;
  if(trickWinner > 2) {
    throwException(_T("%s:Illegal trickWinner=%lu. Must be [0..2]"),method, trickWinner);
  }
  if(getTotalTrickCount() >= 13) {
    throwException(_T("%s:TotalTrickCCount=%d"),method, getTotalTrickCount());
  }
  m_trickCount[trickWinner]++;
}

void PlayerList::clear() {
  memset(this,0,sizeof(PlayerList));
}

void PlayerList::addPlayer(const String &name) {
  if(m_count >= 3) {
    throwException(_T("Cannot add more players. count=%d"), m_count);
  }
  if(name.length() > MAXNAMELENGTH) {
    throwException(_T("Name <%s> too long. Length=%d. maxLength=%d"),name.cstr(),name.length(),MAXNAMELENGTH);
  }
  USES_CONVERSION;
  strcpy(m_name[m_count++],T2A(name.cstr()));
}

String PlayerList::getName(UINT playerId) const {
  if(playerId > 2) {
    throwInvalidArgumentException(__TFUNCTION__,_T("playerId=%d. Must be [0..2]"),m_count);
  }
  if(playerId > m_count) {
    return _T("");
  }
  return m_name[playerId];
}

String PlayerList::toString(UINT myId) const {
  if(myId > 2) {
    throwInvalidArgumentException(__TFUNCTION__,_T("playerId=%d. Must be [0..2]"),myId);
  }
  return format(_T("%-*s %*.*s %*.*s %*.*s")
               ,LEFTFIELDWIDTH ,_T(" ")
               ,FIELDWIDTH,FIELDWIDTH,getName(MOD3(myId+0)).cstr()
               ,FIELDWIDTH,FIELDWIDTH,getName(MOD3(myId+1)).cstr()
               ,FIELDWIDTH,FIELDWIDTH,getName(MOD3(myId+2)).cstr());
}

// ----------------------------------CardsOnTable------------------------------------

void CardsOnTable::init(int player0) {
  memset(this,0,sizeof(CardsOnTable));
  m_player0 = player0;
}

void CardsOnTable::addCard(Card card) {
  if(m_count >= 3) {
    throwException(_T("%s:Cannot add more cards. m_count=%d"), __TFUNCTION__, m_count);
  }
  m_card[getPlayerId(m_count)] = card;
  m_count++;
}

Card CardsOnTable::getCard(UINT index) const {
  return m_card[getPlayerId(index)];
}

int CardsOnTable::getPlayerId(UINT index) const {
  DEFINEMETHODNAME;
  if(m_player0 < 0 || m_player0 > 2) {
    throwException(_T("%s:Illegal state. m_player0=%d"), method, m_player0);
  }
  if(index > 2) {
    throwException(_T("%s:Illegal argument:index=%lu. Must be [0..2]"),method, index);
  }
  return MOD3(m_player0+index);
}
