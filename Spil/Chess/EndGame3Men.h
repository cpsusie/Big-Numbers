#pragma once

// Define best moves in King-Rook/King endgame
class EndGameKRK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition3Men keyDefinition;

  EndGameKRK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen/King endgame
class EndGameKQK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition3Men keyDefinition;

  EndGameKQK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Pawn/King endgame
class EndGameKPK : public EndGameTablebase {
#if defined(TABLEBASE_BUILDER)
protected:
  bool isUsableMove(const Move &m) const;
#endif

public:
  static const EndGameKeyDefinition3Men1Pawn keyDefinition;

  EndGameKPK() : EndGameTablebase(keyDefinition) {
  }
};
