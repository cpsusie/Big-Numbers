#include "stdafx.h"
#include <Exception.h>
#include "GameTypes.h"

GameDescription::GameDescription(int player0, GameType type, Suit suit, int points) {
  m_player0        = player0;
  m_gameType       = type;
  m_suit           = suit;
  m_pointsPerTrick = points;
}

GameDescription::GameDescription(int player0) {
  memset(this,0,sizeof(GameDescription));
  m_player0 = player0;
}

void GameDescription::setRandomStarter() {
  m_player0 = rand() % 3;
}

void GameDescription::nextPlayer0() {
  INC3(m_player0);
}

int GameDescription::getStandardTrickCount(int player) const {
  static char tricks[3][3] = {
    7,4,2,
    2,7,4,
    4,2,7
  };
  static char tricksSol[3][3] = {
    2,4,7,
    7,2,4,
    4,7,2
  };

  return (m_gameType == GAMETYPE_SOL)
    ? tricksSol[m_player0][player]
    : tricks[m_player0][player];
}

String GameDescription::toString() const {
  if(m_gameType == GAMETYPE_FARVE) {
    return format(_T("%s (%s) til %d point"), gameTypeString[m_gameType], suitString[m_suit],m_pointsPerTrick);
  } else {
    return format(_T("%s til %d point"),gameTypeString[m_gameType], m_pointsPerTrick);
  }
}


// return player who wins the trick
int GameDescription::findTrickWinner(const CardsOnTable &cards) const {
  Card card0 = cards.getCard0();
  Card card1 = cards.getCard1();
  Card card2 = cards.getCard2();
  int v0,v1,v2;

  switch(getGameType()) {
  case GAMETYPE_SOL   :
    v0 = CARDVALUE(card0);
    if(CARDSUIT(card1) == CARDSUIT(card0)) {
      v1 = CARDVALUE(card1);
    } else {
      v1 = 0;
    }
    if(CARDSUIT(card2) == CARDSUIT(card0)) {
      v2 = CARDVALUE(card2);
    } else {
      v2 = 0;
    }
    break;
  case GAMETYPE_SANS  :
    v0 = CARDVALUEAH(card0);
    if(CARDSUIT(card1) == CARDSUIT(card0)) {
      v1 = CARDVALUEAH(card1);
    } else {
      v1 = 0;
    }
    if(CARDSUIT(card2) == CARDSUIT(card0)) {
      v2 = CARDVALUEAH(card2);
    } else {
      v2 = 0;
    }
    break;
  case GAMETYPE_SPAR  :
    v0 = CARDVALUEAH(card0);
    if(CARDSUIT(card0) == SPAR) {
      v0 += 14;
    }
    if(CARDSUIT(card1) == SPAR) {
      v1 = CARDVALUEAH(card1)+14;
    } else {
      if(CARDSUIT(card1) == CARDSUIT(card0)) {
        v1 = CARDVALUEAH(card1);
      } else {
        v1 = 0;
      }
    }
    if(CARDSUIT(card2) == SPAR) {
      v2 = CARDVALUEAH(card2)+14;
    } else {
      if(CARDSUIT(card2) == CARDSUIT(card0)) {
        v2 = CARDVALUEAH(card2);
      } else {
        v2 = 0;
      }
    }
    break;

  case GAMETYPE_FARVE :
    { Suit suit = getSuit();
      v0 = CARDVALUEAH(card0);
      if(CARDSUIT(card0) == suit) {
        v0 += 14;
      }
      if(CARDSUIT(card1) == suit) {
        v1 = CARDVALUEAH(card1)+14;
      } else {
        if(CARDSUIT(card1) == CARDSUIT(card0)) {
          v1 = CARDVALUEAH(card1);
        } else {
          v1 = 0;
        }
      }
      if(CARDSUIT(card2) == suit) {
        v2 = CARDVALUEAH(card2)+14;
      } else {
        if(CARDSUIT(card2) == CARDSUIT(card0)) {
          v2 = CARDVALUEAH(card2);
        } else {
          v2 = 0;
        }
      }
      break;
    }
  }

  if(v0 > v1 && v0 > v2) {
    return cards.getPlayerId(0);
  } else if(v1 > v0 && v1 > v2) {
    return cards.getPlayerId(1);
  } else { // v2 must be biggest
    return cards.getPlayerId(2);
  }
}
