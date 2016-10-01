#pragma once

// Define best moves in King-Bishop-Pawn/King endgame
class EndGameKBPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men1Pawn keyDefinition;

  EndGameKBPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Knight-Pawn/King endgame
class EndGameKNPK : public EndGameTablebase {
#ifdef TABLEBASE_BUILDER
protected:
  bool isUsableMove(const Move &m) const;
#endif

public:
  static const EndGameKeyDefinition4Men1Pawn keyDefinition;

  EndGameKNPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Pawn/King-Bishop endgame
class EndGameKPKB : public EndGameTablebase {
#ifdef TABLEBASE_BUILDER
protected:
  bool isUsableMove(const Move &m) const;
#endif

public:
  static const EndGameKeyDefinition4Men1Pawn keyDefinition;

  EndGameKPKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Pawn/King-Knight endgame
class EndGameKPKN : public EndGameTablebase {
#ifdef TABLEBASE_BUILDER
protected:
  bool isUsableMove(const Move &m) const;
#endif

public:
  static const EndGameKeyDefinition4Men1Pawn keyDefinition;

  EndGameKPKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Pawn/King-Pawn endgame
class EndGameKPKP : public EndGameTablebase {
#ifdef TABLEBASE_BUILDER
protected:
  bool isUsableMove(const Move &m) const;
#endif

public:
  static const EndGameKeyDefinition4Men2Pawns keyDefinition;

  EndGameKPKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Pawn-Pawn/King endgame
class EndGameKPPK : public EndGameTablebase {
#ifdef TABLEBASE_BUILDER
protected:
  bool isUsableMove(const Move &m) const;
#endif

public:
  static const EndGameKeyDefinition4Men2EqualPawns keyDefinition;

  EndGameKPPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen/King-Pawn endgame
class EndGameKQKP : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men1Pawn keyDefinition;

  EndGameKQKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Pawn/King endgame
class EndGameKQPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men1Pawn keyDefinition;

  EndGameKQPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen/King-Pawn endgame
class EndGameKRKP : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men1Pawn keyDefinition;

  EndGameKRKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Pawn/King endgame
class EndGameKRPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men1Pawn keyDefinition;

  EndGameKRPK() : EndGameTablebase(keyDefinition) {
  }
};
