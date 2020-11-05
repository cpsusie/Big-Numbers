#include "stdafx.h"
#include <ThreadPool.h>
#include "Whist3EPlayer.h"

Whist3Player *startComputerPlayer(const String &name) {
  Whist3Player *player = new Whist3Player(name, new Whist3EPlayer(), _T("localhost"));
  ThreadPool::executeNoWait(*player);
  return player;
}

void Whist3EPlayer::init(const Whist3Player &player) {
  m_myId = player.getPlayerId();
}

GameDescription Whist3EPlayer::getDecidedGame(const Whist3Player &player) {
  const CardHand &hand = player.getHand();

  int point[4];

  double n = hand.getCardAverage();

  if(n < 5) {
    point[GAMETYPE_SOL] = 4;
  } else if(n < 6) {
    point[GAMETYPE_SOL] = 3;
  } else if(n < 7) {
    point[GAMETYPE_SOL] = 2;
  } else {
    point[GAMETYPE_SOL] = 1;
  }

  if(hand.getCountCardsLessThan(4) > 4) {
    point[GAMETYPE_SOL]++;      // bonus
  }

  n = hand.getCardAverageAceHigest();

  if(n > 9) {
    point[GAMETYPE_SANS] = 4;
  } else if(n > 8) {
    point[GAMETYPE_SANS] = 3;
  } else if(n > 7) {
    point[GAMETYPE_SANS] = 2;
  } else {
    point[GAMETYPE_SANS] = 1;
  }

  if(hand.getCountCardsGreaterThan(11) > 5) {
    point[GAMETYPE_SANS]++;  // bonus
  }

  int i = hand.getSuitCount(SPAR);

  if(i > 5) {
    point[GAMETYPE_SPAR] = 4;
  } else if(i > 4) {
    point[GAMETYPE_SPAR] = 3;
  } else if(i > 3) {
    point[GAMETYPE_SPAR] = 2;
  } else {
    point[GAMETYPE_SPAR] = 1;
  }

  n = hand.getCardSuitAverage(SPAR);
  if(n > 7.5) {
    point[GAMETYPE_SPAR]++; // bonus
  }

  Suit suit = hand.getMaxSuit(i,n);

  if(i > 5) {
    point[GAMETYPE_FARVE] = 4;
  } else if(i > 4) {
    point[GAMETYPE_FARVE] = 3;
  } else if(i > 3) {
    point[GAMETYPE_FARVE] = 2;
  } else {
    point[GAMETYPE_FARVE] = 1;
  }

  if(n > 8) {
    point[GAMETYPE_FARVE]++;                     // bonus
  }

// now decide which game to play

  const GameHistory &history = player.getGameHistory();

  for(int t = 0; t < 4; t++) {
    if(history.isGameUsed(m_myId,(GameType)t)) {
      point[t] = 0;
    }
  }
  GameType gameType = FIRST_GAMETYPE;
  for(int t = 0; t < 4; t++) {
    if(point[t] >= point[gameType]) {
      gameType = (GameType)t;
    }
  }

// now decide to how many points

// first mark the points used

  int pointsPerTrick;
  if(point[gameType] > 3) { // find the biggest that fits
    for(int p = 4; p >= 1; p--) {
      if(!history.isPointsUsed(m_myId,p)) {
        pointsPerTrick = p;
        break;
      }
    }
  } else { // find the smallest that fit
    for(int p = 1; p <= 4; p++) {
      if(!history.isPointsUsed(m_myId,p)) {
        pointsPerTrick = p;
        break;
      }
    }
  }
  return GameDescription(m_myId, gameType, suit, pointsPerTrick);
}

CardIndexSet Whist3EPlayer::getCardsToSubstitute(const Whist3Player &player) {
  const GameDescription &gameDesc = player.getGameDesc();
  const CardHand        &hand     = player.getHand();
  const int              max      = player.getKat().getCount();

  CardIndexSet result;

  switch(gameDesc.getGameType()) {
  case GAMETYPE_SOL  :
    { for(int i = 0;i < 13 && result.size() < max; i++) {
        if(CARDVALUE(hand.getCard(i)) > 7) {
          result.add(i);
        }
      }
    }
    break;

  case GAMETYPE_SANS :
    { bool hasTop[4];
      for(int i = 0; i < 4; i++) {
        hasTop[i] = hand.getSuitGreaterThanAceHigestCount((Suit)i,11) == 3;
      }
      for(int i = 0; i < 13 && result.size() < max; i++) {
        const Card c = hand.getCard(i);
        if(CARDVALUEAH(c) < 7 && !hasTop[CARDSUIT(c)]) {
          result.add(i);
        }
      }
    }
    break;

  case GAMETYPE_SPAR :
    { for(int i = 0; i < 13 && result.size() < max; i++) {
        const Card c = hand.getCard(i);
        if(CARDSUIT(c) != SPAR && CARDVALUEAH(c) < 12) {
          result.add(i);
        }
      }
    }
    break;

  case GAMETYPE_FARVE:
    { for(int i = 0; i < 13 && result.size() < max; i++) {
        const Card c = hand.getCard(i);
        if(CARDSUIT(c) != gameDesc.getSuit() && CARDVALUEAH(c) < 12) {
          result.add(i);
        }
      }
    }
    break;
  }
  return result;
}

// --------------------- HEURESTICS TO DECIDE CARD TO PLAY ----------------

int Whist3EPlayer::findTopCard(const Whist3Player &player) const {
  const CardStatistic &statistic = player.getCardStatistic();
  const CardHand      &hand      = player.getHand();

  int p1 = MOD3(m_myId+1);
  int p2 = MOD3(m_myId+2);

  for(int i = 0; i < 13; i++) {
    const Card card = hand.getCard(i);
    if(card >= 0 && statistic.isPlayerRenonce(p1,CARDSUIT(card)) && statistic.isPlayerRenonce(p2,CARDSUIT(card))) {
      return i; // found a card where the other m_players are renonce
    }
  }

  int cardsTotal[4];
  for(int i = 0; i < 4; i++) {
    cardsTotal[i] = 0;
  }
  for(Card card = 0; card < 52; card++) {                   // now count the cards gone
    if(statistic.isCardGone(card)) {
      cardsTotal[CARDSUIT(card)]++;
    }
  }

  for(int i = 0; i < 13; i++) {
    const Card card = hand.getCard(i);
    if(card >= 0) {
      cardsTotal[CARDSUIT(card)]++;
    }
  }

  // now cardsTotal contains the count of known cards gone in each suit

  for(int i = 0; i < 13; i++) {
    const Card card = hand.getCard(i);
    if(card >= 0 && cardsTotal[CARDSUIT(card)] == 13) { // this is good
      return i;
    }
  }

  for(int i = 0; i < 13; i++) {
    const Card card = hand.getCard(i);
    if(card >= 0 && statistic.allHigherIsGoneah(card)) {
      return i;
    }
  }
  return -1;
}

UINT Whist3EPlayer::getCardToPlay(const Whist3Player &player) {
  GameType gameType = player.getGameDesc().getGameType();
  switch(gameType) {
  case GAMETYPE_SOL  : return findCardToPlaySol(  player);
  case GAMETYPE_SANS : return findCardToPlaySans( player);
  case GAMETYPE_SPAR : return findCardToPlaySpar( player);
  case GAMETYPE_FARVE: return findCardToPlayFarve(player);
  default            :
    throwException(_T("findCardToPlay:Illegal gametype=%d"),gameType);
    return 0;
  }
}

UINT Whist3EPlayer::findCardToPlaySol(const Whist3Player &player) const {
  const GameDescription &gameDesc        = player.getGameDesc();
  const CardHand        &hand            = player.getHand();
  const CardsOnTable    &played          = player.getPlayed();
  const CardStatistic   &statistic       = player.getCardStatistic();
  const int              totalTrickCount = player.getTrickCount().getTotalTrickCount();

  int index;
  switch(played.getCount()) {
  case 0:
    return hand.findLowest();

  case 1:
    { Card card0 = played.getCard0();
      if(hand.isRenonce(CARDSUIT(card0))) {
        return hand.findHigest();
      } else { // not renonce
        index = hand.findHigestLowerThan(card0);
        return index >= 0 ? index : hand.findHigestInSuit(CARDSUIT(card0));
      }
    }
    break;

  case 2:
    { Card card0 = played.getCard0();
      Card card1 = played.getCard1();
      if(hand.isRenonce(CARDSUIT(card0))) {
        return hand.findHigest();
      } else { // not renonce
        if(CARDSUIT(card1) != CARDSUIT(card0)) {
          index = hand.findHigestLowerThan(card0);
        } else if(CARDVALUE(card1) < CARDVALUE(card0)) {
          index = hand.findHigestLowerThan(card0);
        } else {
          index = hand.findHigestLowerThan(card1);
        }
        return index >= 0 ? index : hand.findHigestInSuit(CARDSUIT(card0));
      }
    }
    break;
  }
  throwException(_T("%s dropped to the end. played.getCount()=%d"),__TFUNCTION__, played.getCount());
  return 0;
}

UINT Whist3EPlayer::findCardToPlaySans(const Whist3Player &player) const {
  const GameDescription &gameDesc        = player.getGameDesc();
  const CardHand        &hand            = player.getHand();
  const CardsOnTable    &played          = player.getPlayed();
  const CardStatistic   &statistic       = player.getCardStatistic();
  const int              totalTrickCount = player.getTrickCount().getTotalTrickCount();

  int index;
  switch(played.getCount()) {
  case 0:
    { index = findTopCard(player);
      if(index >= 0) {
        return index;
      } else if(totalTrickCount < 9) {
        return hand.findLowestAceHigest();
      } else {
        return hand.findHigestAceHigest();
      }
    }
    break;

  case 1:
    { Card card0 = played.getCard0();
      if(hand.isRenonce(CARDSUIT(card0))) {
        return hand.findLowestAceHigest();
      } else { // not renonce
        int index = hand.findLowestHigherThanAceHisgest(card0);
        return index >= 0 ? index : hand.findLowestInSuitAceHigest(CARDSUIT(card0));
      }
    }
    break;
  case 2:
    { Card card0 = played.getCard0();
      Card card1 = played.getCard1();
      if(hand.isRenonce(CARDSUIT(card0))) {
        return hand.findLowestAceHigest();
      } else { // not renonce
        if(CARDSUIT(card1) != CARDSUIT(card0)) {
          index = hand.findLowestHigherThanAceHisgest(card0);
        } else if(CARDVALUEAH(card1) > CARDVALUEAH(card0)) {
          index = hand.findLowestHigherThanAceHisgest(card1);
        } else {
          index = hand.findLowestHigherThanAceHisgest(card0);
        }
        return index >= 0 ? index : hand.findLowestInSuitAceHigest(CARDSUIT(card0));
      }
    }
    break;
  }
  throwException(_T("%s dropped to the end. played.getCount()=%d"), __TFUNCTION__, played.getCount());
  return 0;
}

UINT Whist3EPlayer::findCardToPlaySpar(const Whist3Player &player) const {
  const GameDescription &gameDesc        = player.getGameDesc();
  const CardHand        &hand            = player.getHand();
  const CardsOnTable    &played          = player.getPlayed();
  const CardStatistic   &statistic       = player.getCardStatistic();
  const int              totalTrickCount = player.getTrickCount().getTotalTrickCount();

  int index;
  switch(played.getCount()) {
  case 0:
    { if(statistic.allCardsOfSuitIsGone(SPAR)) {
        index = findTopCard(player);
        if(index >= 0) {
          return index;
        }
      }
      if(totalTrickCount <= 4 || hand.getNotSuitCount(SPAR) < 2) {
        index = hand.findHigestInSuitAceHigest(SPAR);
      } else {
        index = hand.findLowestNotInSuitAceHigest(SPAR);
      }
      if(index >= 0) {
        return index;
      }
      if(totalTrickCount < 9) {
        return hand.findLowestAceHigest();
      } else {
        return hand.findHigestAceHigest();
      }
    }
    break;

  case 1:
    { Card card0 = played.getCard0();
      if(hand.isRenonce(CARDSUIT(card0))) {
        if(CARDSUIT(card0) == SPAR) {
          return hand.findLowestAceHigest();
        }
        index = hand.findLowestInSuitAceHigest(SPAR);
        return index >= 0 ? index : hand.findLowestAceHigest();
      } else { // not renonce
        index = hand.findLowestHigherThanAceHisgest(card0);
        return index >= 0 ? index : hand.findLowestInSuitAceHigest(CARDSUIT(card0));
      }
    }
    break;

  case 2:
    { Card card0 = played.getCard0();
      Card card1 = played.getCard1();
      if(hand.isRenonce(CARDSUIT(card0))) {
        if(CARDSUIT(card0) == SPAR) {
          return hand.findLowestAceHigest();
        }
        if(CARDSUIT(card1) == SPAR) {
          index = hand.findLowestHigherThanAceHisgest(card1);
          if(index >= 0) {
            return index;
          }
          index = hand.findLowestNotInSuitAceHigest(SPAR);
          return index >= 0 ? index : hand.findLowestAceHigest();
        } else {
          index = hand.findLowestInSuitAceHigest(SPAR);
          return index >= 0 ? index : hand.findLowestAceHigest();
        }
      } else { // not renonce
        if(CARDSUIT(card0) == SPAR && CARDSUIT(card1) == SPAR) {
          if(CARDVALUEAH(card1) > CARDVALUEAH(card0)) {
            index = hand.findLowestHigherThanAceHisgest(card1);
          } else {
            index = hand.findLowestHigherThanAceHisgest(card0);
          }
        } else if(CARDSUIT(card0) == SPAR && CARDSUIT(card1) != SPAR) {
          index = hand.findLowestHigherThanAceHisgest(card0);
        } else if(CARDSUIT(card0) != SPAR && CARDSUIT(card1) == SPAR) {
          index = hand.findLowestInSuitAceHigest(CARDSUIT(card0));
        } else if(CARDSUIT(card0) == CARDSUIT(card1)) {
          if(CARDVALUEAH(card1) > CARDVALUEAH(card0)) {
            index = hand.findLowestHigherThanAceHisgest(card1);
          } else {
            index = hand.findLowestHigherThanAceHisgest(card0);
          }
        } else { // card1 is renonce and not SPAR
          index = hand.findLowestHigherThanAceHisgest(card0);
        }
        return index >= 0 ? index : hand.findLowestInSuitAceHigest(CARDSUIT(card0));
      }
    }
    break;
  }
  throwException(_T("%s dropped to the end. played.getCount()=%d"), __TFUNCTION__, played.getCount());
  return 0;
}

UINT Whist3EPlayer::findCardToPlayFarve(const Whist3Player &player) const {
  const GameDescription &gameDesc        = player.getGameDesc();
  const CardHand        &hand            = player.getHand();
  const CardsOnTable    &played          = player.getPlayed();
  const CardStatistic   &statistic       = player.getCardStatistic();
  const int              totalTrickCount = player.getTrickCount().getTotalTrickCount();
  const Suit             suit            = gameDesc.getSuit();

  int index;
  switch(played.getCount()) {
  case 0:
    { if(statistic.allCardsOfSuitIsGone(suit)) {
        index = findTopCard(player);
        if(index >= 0)
          return index;
      }
      if(totalTrickCount <= 4 || hand.getNotSuitCount(suit) < 2) {
        index = hand.findHigestInSuitAceHigest(suit);
      } else {
        index = hand.findLowestNotInSuitAceHigest(suit);
      }
      if(index >= 0) {
        return index;
      }
      if(totalTrickCount < 9) {
        return hand.findLowestAceHigest();
      } else {
        return hand.findHigestAceHigest();
      }
    }
    break;
  case 1:
    { Card card0 = played.getCard0();
      if(hand.isRenonce(CARDSUIT(card0))) {
        if(CARDSUIT(card0) == suit) {
          return hand.findLowestAceHigest();
        }
        index = hand.findLowestInSuitAceHigest(suit);
        return index >= 0 ? index : hand.findLowestAceHigest();
      } else { // not renonce
        index = hand.findLowestHigherThanAceHisgest(card0);
        return index >= 0 ? index : hand.findLowestInSuitAceHigest(CARDSUIT(card0));
      }
    }
    break;

  case 2:
    { Card card0 = played.getCard0();
      Card card1 = played.getCard1();
      if(hand.isRenonce(CARDSUIT(card0))) {
        if(CARDSUIT(card0) == suit) {
          return hand.findLowestAceHigest();
        }
        if(CARDSUIT(card1) == suit) {
          index = hand.findLowestHigherThanAceHisgest(card1);
          if(index >= 0) {
            return index;
          }
          index = hand.findLowestNotInSuitAceHigest(suit);
          return index >= 0 ? index : hand.findLowestAceHigest();
        } else {
          index = hand.findLowestInSuitAceHigest(suit);
          return index >= 0 ? index : hand.findLowestAceHigest();
        }
      } else { // not renonce
        if(CARDSUIT(card0) == suit && CARDSUIT(card1) == suit) {
          if(CARDVALUEAH(card1) > CARDVALUEAH(card0)) {
            index = hand.findLowestHigherThanAceHisgest(card1);
          } else {
            index = hand.findLowestHigherThanAceHisgest(card0);
          }
        } else if(CARDSUIT(card0) == suit && CARDSUIT(card1) != suit) {
          index = hand.findLowestHigherThanAceHisgest(card0);
        } else if(CARDSUIT(card0) != suit && CARDSUIT(card1) == suit) {
          index = hand.findLowestInSuitAceHigest(CARDSUIT(card0));
        } else if(CARDSUIT(card0) == CARDSUIT(card1)) {
          if(CARDVALUEAH(card1) > CARDVALUEAH(card0)) {
            index = hand.findLowestHigherThanAceHisgest(card1);
          } else {
            index = hand.findLowestHigherThanAceHisgest(card0);
          }
        } else { // card1 is renonce and not suit
          index = hand.findLowestHigherThanAceHisgest(card0);
        }
        return index >= 0 ? index : hand.findLowestInSuitAceHigest(CARDSUIT(card0));
      }
    }
    break;
  }
  throwException(_T("%s dropped to the end. played.getCount()=%d"), __TFUNCTION__, played.getCount());
  return 0;
}

