#pragma once

// Define best moves in King-Bishop-Bishop-Bishop/King endgame
class EndGameKBBBK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men3Equal keyDefinition;

  EndGameKBBBK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop-Bishop/King-Bishop endgame
class EndGameKBBKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKBBKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop-Bishop/King-Knight endgame
class EndGameKBBKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKBBKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop-Bishop/King-Rook endgame
class EndGameKBBKR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKBBKR() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop-Bishop-Knight/King endgame
class EndGameKBBNK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKBBNK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop-Knight/King-Bishop endgame
class EndGameKBNKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKBNKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop-Knight/King-Knight endgame
class EndGameKBNKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKBNKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop-Knight/King-Rook endgame
class EndGameKBNKR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKBNKR() : EndGameTablebase(keyDefinition) {
  }
};


#if defined(ENDGAME_NOKING)

class EndGameKeyDefinitionKBNNKNoKing : public EndGameKeyDefinition5Men2Equal {
public:
  EndGameKeyDefinitionKBNNKNoKing() : EndGameKeyDefinition5Men2Equal(WHITEBISHOP, WHITEKNIGHT) {
  }

#if defined(TABLEBASE_BUILDER)
  void insertInitialPositions(EndGameTablebase &tablebase) const {
    tablebase.addInitPosition(WHITEPLAYER, D4, F3, A1, B8, A7);
    tablebase.addInitPosition(WHITEPLAYER, D4, F3, A2, B8, A7);
  }
#endif
};

// Define best moves in King-Bishop-Bishop-Knight/King endgame, with no white king moves
class EndGameKBNNK : public EndGameTablebase {
#if defined(TABLEBASE_BUILDER)
protected:
  bool isUsableMove(const Move &m) const {
    return m.m_piece->getKey() != WHITEKING;
  }

  bool isUsableBackMove(const Move &m) const {
    return m.m_piece->getKey() != WHITEKING;
  }
#endif
public:
  static const EndGameKeyDefinitionKBNNKNoKing keyDefinition;

  EndGameKBNNK() : EndGameTablebase(keyDefinition) {
  }
};

#else

// Define best moves in King-Bishop-Bishop-Knight/King endgame
class EndGameKBNNK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKBNNK() : EndGameTablebase(keyDefinition) {
  }
};

#endif

// Define best moves in King-Knight-Knight/King-Bishop endgame
class EndGameKNNKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKNNKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Knight-Knight/King-Knight endgame
class EndGameKNNKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKNNKN() : EndGameTablebase(keyDefinition) {
  }
};


#if defined(ENDGAME_NOKING)

class EndGameKeyDefinitionKNNNKNoKing : public EndGameKeyDefinition5Men3Equal {
public:
  EndGameKeyDefinitionKNNNKNoKing() : EndGameKeyDefinition5Men3Equal(WHITEKNIGHT) {
  }

#if defined(TABLEBASE_BUILDER)
  void insertInitialPositions(EndGameTablebase &tablebase) const {
    tablebase.addInitPosition(WHITEPLAYER, D4, F3, A8, B8, A7);
  }
#endif
};

class EndGameKNNNK : public EndGameTablebase {
#if defined(TABLEBASE_BUILDER)
protected:
  bool isUsableMove(const Move &m) const {
    return m.m_piece->getKey() != WHITEKING;
  }

  bool isUsableBackMove(const Move &m) const {
    return m.m_piece->getKey() != WHITEKING;
  }
#endif
public:
  static const EndGameKeyDefinitionKNNNKNoKing keyDefinition;

  EndGameKNNNK() : EndGameTablebase(keyDefinition) {
  }
};

#else

// Define best moves in King-Knight-Knight-Knight/King endgame
class EndGameKNNNK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men3Equal keyDefinition;

  EndGameKNNNK() : EndGameTablebase(keyDefinition) {
  }
};

#endif

// Define best moves in King-Queen-Bishop-Bishop/King endgame
class EndGameKQBBK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKQBBK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Bishop/King-Bishop endgame
class EndGameKQBKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQBKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Bishop/King-Knight endgame
class EndGameKQBKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQBKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Bishop/King-Queen endgame
class EndGameKQBKQ : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQBKQ() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Bishop/King-Rook endgame
class EndGameKQBKR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQBKR() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Bishop-Knight/King endgame
class EndGameKQBNK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQBNK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen/King-Bishop-Bishop endgame
class EndGameKQKBB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKQKBB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen/King-Bishop-Knight endgame
class EndGameKQKBN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQKBN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen/King-Knight-Knight endgame
class EndGameKQKNN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKQKNN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen/King-Rook-Bishop endgame
class EndGameKQKRB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQKRB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen/King-Rook-Knight endgame
class EndGameKQKRN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQKRN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen/King-Rook-Rook endgame
class EndGameKQKRR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKQKRR() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Knight/King-Bishop endgame
class EndGameKQNKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQNKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Knight/King-Knight endgame
class EndGameKQNKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQNKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Knight/King-Queen endgame
class EndGameKQNKQ : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQNKQ() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Knight/King-Rook endgame
class EndGameKQNKR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQNKR() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Knight-Knight/King endgame
class EndGameKQNNK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKQNNK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Queen-Bishop/King endgame
class EndGameKQQBK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKQQBK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Queen/King-Bishop endgame
class EndGameKQQKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKQQKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Queen/King-Knight endgame
class EndGameKQQKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKQQKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Queen/King-Queen endgame
class EndGameKQQKQ : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKQQKQ() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Queen/King-Rook endgame
class EndGameKQQKR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKQQKR() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Queen-Knight/King endgame
class EndGameKQQNK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKQQNK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Queen-Queen/King endgame
class EndGameKQQQK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men3Equal keyDefinition;

  EndGameKQQQK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Queen-Rook/King endgame
class EndGameKQQRK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKQQRK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Rook-Bishop/King endgame
class EndGameKQRBK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQRBK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Rook/King-Bishop endgame
class EndGameKQRKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQRKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Rook/King-Knight endgame
class EndGameKQRKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQRKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Rook/King-Queen endgame
class EndGameKQRKQ : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQRKQ() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Rook/King-Rook endgame
class EndGameKQRKR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQRKR() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Rook-Knight/King endgame
class EndGameKQRNK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKQRNK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Rook-Rook/King endgame
class EndGameKQRRK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKQRRK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Bishop-Bishop/King endgame
class EndGameKRBBK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKRBBK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Bishop/King-Bishop endgame
class EndGameKRBKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKRBKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Bishop/King-Knight endgame
class EndGameKRBKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKRBKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Bishop/King-Rook endgame
class EndGameKRBKR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKRBKR() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Bishop-Knight/King endgame
class EndGameKRBNK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKRBNK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook/King-Knight-Knight endgame
class EndGameKRKNN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKRKNN::EndGameKRKNN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Knight/King-Bishop endgame
class EndGameKRNKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKRNKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Knight/King-Knight endgame
class EndGameKRNKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKRNKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Knight/King-Rook endgame
class EndGameKRNKR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men keyDefinition;

  EndGameKRNKR() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Knight-Knight/King endgame
class EndGameKRNNK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKRNNK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Rook-Bishop/King endgame
class EndGameKRRBK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKRRBK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Rook/King-Bishop endgame
class EndGameKRRKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKRRKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Rook/King-Knight endgame
class EndGameKRRKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKRRKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Rook/King-Rook endgame
class EndGameKRRKR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKRRKR() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Rook-Knight/King endgame
class EndGameKRRNK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men2Equal keyDefinition;

  EndGameKRRNK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Rook-Rook/King endgame
class EndGameKRRRK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition5Men3Equal keyDefinition;

  EndGameKRRRK() : EndGameTablebase(keyDefinition) {
  }
};
