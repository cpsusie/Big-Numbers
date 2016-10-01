#pragma once

// Define best moves in King-Bishop-Bishop/King-Pawn endgame
class EndGameKBBKP : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn2Equal keyDefinition;

  EndGameKBBKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop-Bishop-Pawn/King endgame
class EndGameKBBPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn2Equal keyDefinition;

  EndGameKBBPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop-Knight/King-Pawn endgame
class EndGameKBNKP : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKBNKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop-Knight-Pawn/King endgame
class EndGameKBNPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKBNPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop-Pawn/King-Bishop endgame
class EndGameKBPKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKBPKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop-Pawn/King-Knight endgame
class EndGameKBPKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKBPKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop-Pawn/King-Pawn endgame
class EndGameKBPKP : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Pawns keyDefinition;

  EndGameKBPKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop-Pawn/King-Queen endgame
class EndGameKBPKQ : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKBPKQ() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop-Pawn/King-Rook endgame
class EndGameKBPKR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKBPKR() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop-Pawn-Pawn/King endgame
class EndGameKBPPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2EqualPawns keyDefinition;

  EndGameKBPPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Knight-Knight/King-Pawn endgame
class EndGameKNNKP : public EndGameTablebase {
#ifdef TABLEBASE_BUILDER
protected:
  bool isUsableMove(const Move &m) const;
#endif

public:
  static const EndGameKeyDefinition5Men1Pawn2Equal keyDefinition;

  EndGameKNNKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Knight-Knight-Pawn/King endgame
class EndGameKNNPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn2Equal keyDefinition;

  EndGameKNNPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Knight-Pawn/King-Bishop endgame
class EndGameKNPKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKNPKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Knight-Pawn/King-Knight endgame
class EndGameKNPKN : public EndGameTablebase {
#ifdef TABLEBASE_BUILDER
protected:
  bool isUsableMove(const Move &m) const;
#endif

public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKNPKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Knight-Pawn/King-Pawn endgame
class EndGameKNPKP : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Pawns keyDefinition;

  EndGameKNPKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Knight-Pawn/King-Queen endgame
class EndGameKNPKQ : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKNPKQ() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Knight-Pawn/King-Rook endgame
class EndGameKNPKR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKNPKR() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Knight-Pawn-Pawn/King endgame
class EndGameKNPPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2EqualPawns keyDefinition;

  EndGameKNPPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Pawn-Pawn/King-Bishop endgame
class EndGameKPPKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2EqualPawns keyDefinition;

  EndGameKPPKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Pawn-Pawn/King-Knight endgame
class EndGameKPPKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2EqualPawns keyDefinition;

  EndGameKPPKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Pawn-Pawn/King-Pawn endgame
class EndGameKPPKP : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men3Pawns keyDefinition;

  EndGameKPPKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Pawn-Pawn/King-Queen endgame
class EndGameKPPKQ : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2EqualPawns keyDefinition;

  EndGameKPPKQ() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Pawn-Pawn/King-Rook endgame
class EndGameKPPKR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2EqualPawns keyDefinition;

  EndGameKPPKR() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Pawn-Pawn-Pawn/King endgame
class EndGameKPPPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men3EqualPawns keyDefinition;

  EndGameKPPPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Bishop/King-Pawn endgame
class EndGameKQBKP : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKQBKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Bishop-Pawn/King endgame
class EndGameKQBPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKQBPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Knight/King-Pawn endgame
class EndGameKQNKP : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKQNKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Knight-Pawn/King endgame
class EndGameKQNPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKQNPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Pawn/King-Bishop endgame
class EndGameKQPKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKQPKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Pawn/King-Knight endgame
class EndGameKQPKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKQPKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in Queen-Knight-Pawn/King-Pawn endgame
class EndGameKQPKP : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Pawns keyDefinition;

  EndGameKQPKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Pawn/King-Queen endgame
class EndGameKQPKQ : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKQPKQ() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Pawn/King-Rook endgame
class EndGameKQPKR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKQPKR() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Pawn-Pawn/King endgame
class EndGameKQPPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2EqualPawns keyDefinition;

  EndGameKQPPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Queen/King-Pawn endgame
class EndGameKQQKP : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn2Equal keyDefinition;

  EndGameKQQKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Queen-Pawn/King endgame
class EndGameKQQPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn2Equal keyDefinition;

  EndGameKQQPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Rook/King-Pawn endgame
class EndGameKQRKP : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKQRKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Rook-Pawn/King endgame
class EndGameKQRPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKQRPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Bishop/King-Pawn endgame
class EndGameKRBKP : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKRBKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Bishop-Pawn/King endgame
class EndGameKRBPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKRBPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Knight/King-Pawn endgame
class EndGameKRNKP : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKRNKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Knight-Pawn/King endgame
class EndGameKRNPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKRNPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Pawn/King-Bishop endgame
class EndGameKRPKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKRPKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Pawn/King-Knight endgame
class EndGameKRPKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKRPKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in Rook-Knight-Pawn/King-Pawn endgame
class EndGameKRPKP : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Pawns keyDefinition;

  EndGameKRPKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Pawn/King-Queen endgame
class EndGameKRPKQ : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKRPKQ() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Pawn/King-Rook endgame
class EndGameKRPKR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn keyDefinition;

  EndGameKRPKR() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Pawn-Pawn/King endgame
class EndGameKRPPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2EqualPawns keyDefinition;

  EndGameKRPPK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Rook/King-Pawn endgame
class EndGameKRRKP : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn2Equal keyDefinition;

  EndGameKRRKP() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Rook-Pawn/King endgame
class EndGameKRRPK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men1Pawn2Equal keyDefinition;

  EndGameKRRPK() : EndGameTablebase(keyDefinition) {
  }
};
