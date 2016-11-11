#pragma once

class EndGameKQRKBN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition6Men keyDefinition;

  EndGameKQRKBN() : EndGameTablebase(keyDefinition) {
  }
};

class EndGameKQRKRR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition6Men2Equal keyDefinition;

  EndGameKQRKRR() : EndGameTablebase(keyDefinition) {
  }
};

class EndGameKQKNNN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition6Men3Equal keyDefinition;

  EndGameKQKNNN() : EndGameTablebase(keyDefinition) {
  }
};
