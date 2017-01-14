#include "stdafx.h"
#include <Exception.h>
#include "GameTypes.h"

void CardHand::clear() { 
  for(int i = 0; i < 13; i++) {
    m_card[i] = -1;
  }
  m_count = 0;
}

static int suitSortOrder[] = { 1,0,2,3 }; // ruder, klør, hjerter, spar

static int cardCmpAceHigest(const void *c1, const void *c2) { // ace is highest
  Card card1 = *(Card*)c1;
  Card card2 = *(Card*)c2;
  int suit1 = CARDSUIT(card1);
  int suit2 = CARDSUIT(card2);

  if(suit1 != suit2) {
    return suitSortOrder[suit1] - suitSortOrder[suit2];
  }
  return CARDVALUEAH(card1) - CARDVALUEAH(card2);
}

static int cardCmpSol(const void *c1, const void *c2) { // ace is lowest
  Card card1 = *(Card*)c1;
  Card card2 = *(Card*)c2;
  int suit1 = CARDSUIT(card1);
  int suit2 = CARDSUIT(card2);

  if(suit1 != suit2) {
    return suitSortOrder[suit1] - suitSortOrder[suit2];
  }
  return CARDVALUE(card1) - CARDVALUE(card2);
}

void CardHand::sort(bool sortAceHigest) {
  qsort(m_card,13,sizeof(m_card[0]), sortAceHigest ? cardCmpAceHigest : cardCmpSol);
}

void CardHand::removeCards(UINT count) {
  DEFINEMETHODNAME;
  if(count == 0)
    return;

  if(count > m_count) {
    throwException(_T("%s:Cannot remove %lu cards. m_count=%lu"), method, count, m_count);
  }

  int i;
  for(i = 0; i < (int)m_count - (int)count; i++)  {
    m_card[i] = m_card[i+count];
  }
  for(;i < 13; i++) {
    m_card[i] = -1;
  }
  m_count -= count;
}

void CardHand::removeCard(UINT index) {
  DEFINEMETHODNAME;
  if(index >= 13) {
    throwException(_T("%s:Illegal index=%d. Must be [0..12]"), method, index);
  }
  if(m_card[index] < 0) {
    throwException(_T("%s:Card[%lu] is already removed"), method, index);
  }
  m_card[index] = -1;
  m_count--;
}

void CardHand::addCard(Card card) {
  DEFINEMETHODNAME;
  if(m_count >= 13) {
    throwException(_T("%s:Cannot add more cards. m_count=%lu"), method, m_count);
  }
  if(card < 0 || card > 51) {
    throwException(_T("%s:Illegal card=%d. Must be in [0..51]"), method, card);
  }
  m_card[m_count++] = card;
}

void CardHand::substituteCardsInSet(const CardIndexSet &set, const CardHand &kat) {
  DEFINEMETHODNAME;
  if(set.size() != kat.getCount()) {
    throwException(_T("%s:set.size()=%d. kat.count()=%d. Must be equal"), method, set.size(), kat.getCount());
  }
  int j = 0;
  for(Iterator<int> it = set.getIterator(); it.hasNext();) {
    const Card newCard = kat.m_card[j++];

    if(newCard < 0 || newCard > 51) {
      throwException(_T("%s:Illegal card=%d. Must be in [0..51]"), method, newCard);
    }

    m_card[it.next()] = newCard;
  }
}

int CardHand::findIndex(Card card) const {
  for(int i = 0; i < 13; i++) {
    if(m_card[i] == card) {
      return i;
    }
  }
  return -1;
}

bool CardHand::contains(Card card) const {
  return findIndex(card) >= 0;
}

double CardHand::getCardAverage() const {
  int sum   = 0;
  int count = 0;
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0) {
      sum += CARDVALUE(m_card[i]);
      count++;
    }
  }
  return count ? (double)sum/count : 0;
}

double CardHand::getCardAverageAceHigest() const {
  int sum   = 0;
  int count = 0;
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0) {
      sum += CARDVALUEAH(m_card[i]);
      count++;
    }
  }
  return count ? (double)sum/count : 0;
}

int CardHand::getCountCardsLessThan(int value) const {
  int count = 0;
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0 && CARDVALUE(m_card[i]) < value) {
      count++;
    }
  }
  return count;
}

int CardHand::getCountCardsGreaterThan(int value) const {
  int count = 0;
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0 && CARDVALUEAH(m_card[i]) > value)
      count++;
  }
  return count;
}

bool CardHand::isRenonce(Suit suit) const {
  return getSuitCount(suit) == 0;
}

int CardHand::getSuitCount(Suit suit) const {
  int count = 0;
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0 && CARDSUIT(c) == suit)
      count++;
  }
  return count;
}

int CardHand::getNotSuitCount(Suit suit) const {
  int count = 0;
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0 && CARDSUIT(c) != suit)
      count++;
  }
  return count;
}

int CardHand::getSuitGreaterThanAceHigestCount(Suit suit, int value) const {
  int count = 0;
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0 && CARDSUIT(c) == suit && CARDVALUEAH(c) > value)
      count++;
  }
  return count;
}

double CardHand::getCardSuitAverage(Suit suit) const {
  int count = 0;
  int sum   = 0;
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0 && CARDSUIT(c) == suit) {
      sum += CARDVALUEAH(c);
      count++;
    }
  }
  return count ? sum/count : 0;
}

Suit CardHand::getMaxSuit(int &count, double &bavg) const { // find the best suit
  int    a[4];
  double avg[4];
  int i;
  Suit result;

  for(i = 0; i < 4; i++) {
    a[i] = 0;
  }
  for(i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0) {
      a[CARDSUIT(c)]++;
    }
  }
  for(i = 0; i < 4; i++) {
    avg[i] = getCardSuitAverage((Suit)i);
  }
  for(i = 1, result = (Suit)0; i < 4; i++) {
    if(i == SPAR) {
      continue; // spar does not count
    }
    if(a[i] > a[result] || (a[i] == a[result] && avg[i] > avg[result])) {
      result = (Suit)i;
    }
  }

  count = a[result];
  bavg  = avg[result];

  return result;
}

int CardHand::findLowest() const { // find the index of the lowest card in the hand, ace lowest
  int result = -1;
  int lv     = 15;
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0 && CARDVALUE(c) < lv) {
      lv = CARDVALUE(c);
      result = i;
    }
  }
  return result;  
}

int CardHand::findHigest() const { // find the index of the higest card in the hand, ace lowest
  int result = -1;
  int hv     = 0;
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0 && CARDVALUE(c) > hv) {
      hv = CARDVALUE(c);
      result = i;
    }
  }
  return result;  
}

int CardHand::findLowestAceHigest() const { // find the index of lowest card in the hand, ace higest
  int result = -1;
  int lv     = 15;
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0 && CARDVALUEAH(c) < lv) {
      lv = CARDVALUEAH(c);
      result = i;
    }
  }
  return result;  
}

int CardHand::findHigestAceHigest() const { // find the index of the higest card in the hand, ace higest
  int result = -1;
  int hv     = 0;
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0 && CARDVALUEAH(c) > hv) {
      hv = CARDVALUEAH(c);
      result = i;
    }
  }
  return result;
}

int CardHand::findLowestInSuit(Suit suit) const {     // find the index of the lowest card in the hand
  int result = -1;                                    // with the specified suit 
  int lv     = 15;                                    // returns -1 if none found
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0 && CARDSUIT(c) == suit && CARDVALUE(c) < lv) {
      lv = CARDVALUE(c);
      result = i;
    }
  }
  return result;  
}

int CardHand::findHigestInSuit(Suit suit) const {     // find the index of the higest card in the hand
  int result = -1;                                    // with the specified suit 
  int hv     = 0;                                     // returns -1 if none found
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0 && CARDSUIT(c) == suit && CARDVALUE(c) > hv) {
      hv = CARDVALUE(c);
      result = i;
    }
  }
  return result;  
}

int CardHand::findLowestInSuitAceHigest(Suit suit) const {     // find the index of lowest card in the hand
  int result = -1;                                      // with the specified suit, ace higest 
  int lv     = 15;                                      // returns -1 if none found
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0 && CARDSUIT(c) == suit && CARDVALUEAH(c) < lv) {
      lv = CARDVALUEAH(c);
      result = i;
    }
  }
  return result;  
}

int CardHand::findHigestInSuitAceHigest(Suit suit) const {     // find the index of higest card in the hand
  int result = -1;                                      // with the specified suit, ace higest
  int hv     = 0;                                       // returns -1 if none found
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0 && CARDSUIT(c) == suit && CARDVALUEAH(c) > hv) {
      hv = CARDVALUEAH(c);
      result = i;
    }
  }
  return result;  
}

int CardHand::findHigestLowerThan(Card card) const {
  int result = -1;
  int hv = 0;
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0 && CARDSUIT(c) == CARDSUIT(card) 
              && CARDVALUE(c) < CARDVALUE(card)
              && CARDVALUE(c) > hv) {
      hv = CARDVALUE(c);
      result = i;
    }
  }
  return result;
}

int CardHand::findLowestHigherThanAceHisgest(Card card) const {
  int result = -1;
  int lv = 15;
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0 && CARDSUIT(c) == CARDSUIT(card) 
              && CARDVALUEAH(c) > CARDVALUEAH(card)
              && CARDVALUEAH(c) < lv) {
      lv = CARDVALUEAH(c);
      result = i;
    }
  }
  return result;
}

int CardHand::findLowestNotInSuitAceHigest(Suit suit) const {
  int result = -1;
  int lv = 15;
  for(int i = 0; i < 13; i++) {
    const Card c = m_card[i];
    if(c >= 0 && CARDSUIT(c) != suit && CARDVALUEAH(c) < lv) {
      lv = CARDVALUEAH(c);
      result = i;
    }
  }
  return result;
}

