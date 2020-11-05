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
  void            init(                 const Whist3Player &player) override;
  GameDescription getDecidedGame(       const Whist3Player &player) override;
  CardIndexSet    getCardsToSubstitute( const Whist3Player &player) override;
  UINT            getCardToPlay(        const Whist3Player &player) override;
  bool            deleteOnEndOfUse()    const                       override {
    return true;
  }
};

Whist3Player *startComputerPlayer(const String &name);
