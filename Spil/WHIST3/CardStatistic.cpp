#include "stdafx.h"
#include <Exception.h>
#include "GameTypes.h"

void CardStatistic::clear() {
  memset(m_cardsGone,0,sizeof(m_cardsGone));
  memset(m_playerIsRenonce,0,sizeof(m_playerIsRenonce));
}

void CardStatistic::update(const CardsOnTable &played) {
  for(int i = 0; i < 3; i++) {
    setCardGone(played.getCard(i));
  }

  int player0 = played.getPlayerId(0);
  int player1 = played.getPlayerId(1);
  int player2 = played.getPlayerId(2);
  Card card0  = played.getCard0();
  Card card1  = played.getCard1();
  Card card2  = played.getCard2();

  if(CARDSUIT(card1) != CARDSUIT(card0)) {
    m_playerIsRenonce[player1][CARDSUIT(card0)] = true;
  }
  if(CARDSUIT(card2) != CARDSUIT(card0)) {
    m_playerIsRenonce[player2][CARDSUIT(card0)] = true;
  }
}

void CardStatistic::update(const CardHand &hand, const CardIndexSet &set)  {
  for(Iterator<int> it = set.getIterator(); it.hasNext();) {
    setCardGone(hand.getCard(it.next()));
  }
}

void CardStatistic::setCardGone(Card card) {
  validateCardIsNotGone(card);
  m_cardsGone[CARDSUIT(card)][CARDVALUE(card)] = true;
}

void CardStatistic::validateCardIsNotGone(Card card) const {
  if(isCardGone(card)) {
    throwException(_T("Card %d has alredy been played or substituted"), card);
  }
}

bool CardStatistic::isCardGone(Card card) const {
  if(card < 0 || card > 51) {
    throwException(_T("%s:Invalid card=%d. Must be [0.51]"),__TFUNCTION__,card);
  }
  return m_cardsGone[CARDSUIT(card)][CARDVALUE(card)];
}

bool CardStatistic::allHigherIsGoneah(Card card) const {
  const Suit suit = CARDSUIT(card);
  const int  v    = CARDVALUEAH(card);
  for(int i = v+1; i < 14; i++) { // this works because v = 14 for ace
    if(!m_cardsGone[suit][i]) {
      return false;
    }
  }
  return true;
}

bool CardStatistic::allCardsOfSuitIsGone(Suit suit) const { // are all cards of suit gone
  for(int i = 1; i < 14; i++) {
    if(!m_cardsGone[suit][i]) {
      return false;
    }
  }
  return true;
}

bool CardStatistic::isPlayerRenonce(UINT playerId, Suit suit) const {
  if(playerId > 2) {
    throwException(_T("%s:Invalid playerId=%d. Must be [0.2]"), __TFUNCTION__, playerId);
  }
  return m_playerIsRenonce[playerId][suit];
}

