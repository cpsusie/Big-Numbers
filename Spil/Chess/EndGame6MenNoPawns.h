#pragma once

class EndGameKQRKBN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition6Men keyDefinition;

  EndGameKQRKBN() : EndGameTablebase(keyDefinition) {
  }
};
