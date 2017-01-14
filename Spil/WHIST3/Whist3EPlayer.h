#pragma once

#include "Whist3Player.h"

class Whist3EPlayer : public Whist3PlayerCallback {
private:
  int m_myId;

  int             findTopCard(          const Whist3Player &player) const;
  UINT            findCardToPlaySol(    const Whist3Player &player) const;
  UINT            findCardToPlaySans(   const Whist3Player &player) const;
  UINT            findCardToPlaySpar(   const Whist3Player &player) const;
  UINT            findCardToPlayFarve(  const Whist3Player &player) const;
public:
  void            init(                 const Whist3Player &player);
  GameDescription getDecidedGame(       const Whist3Player &player);
  CardIndexSet    getCardsToSubstitute( const Whist3Player &player);
  UINT            getCardToPlay(        const Whist3Player &player);
};

void startComputerPlayer(const String &name);
