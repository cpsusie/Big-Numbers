#pragma once

// Define best moves in King-Bishop-Bishop/King endgame
class EndGameKBBK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men2Equal keyDefinition;

  EndGameKBBK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop/King-Bishop endgame
class EndGameKBKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men keyDefinition;

  EndGameKBKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop/King-Knight endgame
class EndGameKBKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men keyDefinition;

  EndGameKBKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Bishop-Knight/King endgame
class EndGameKBNK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men keyDefinition;

  EndGameKBNK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Knight/King-Knight endgame
class EndGameKNKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men keyDefinition;

  EndGameKNKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Knight-Knight/King endgame
class EndGameKNNK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men2Equal keyDefinition;

  EndGameKNNK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Bishop/King endgame
class EndGameKQBK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men keyDefinition;

  EndGameKQBK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen/King-Bishop endgame
class EndGameKQKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men keyDefinition;

  EndGameKQKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen/King-Knight endgame
class EndGameKQKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men keyDefinition;

  EndGameKQKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen/King-Queen endgame
class EndGameKQKQ : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men keyDefinition;

  EndGameKQKQ() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen/King-Rook endgame
class EndGameKQKR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men keyDefinition;

  EndGameKQKR() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Knight/King endgame
class EndGameKQNK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men keyDefinition;

  EndGameKQNK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Queen/King endgame
class EndGameKQQK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men2Equal keyDefinition;

  EndGameKQQK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Queen-Rook/King endgame
class EndGameKQRK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men keyDefinition;

  EndGameKQRK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Bishop/King endgame
class EndGameKRBK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men keyDefinition;

  EndGameKRBK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook/King-Bishop endgame
class EndGameKRKB : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men keyDefinition;

  EndGameKRKB() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook/King-Knight endgame
class EndGameKRKN : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men keyDefinition;

  EndGameKRKN() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook/King-Rook endgame
class EndGameKRKR : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men keyDefinition;

  EndGameKRKR() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Knight/King endgame
class EndGameKRNK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men keyDefinition;

  EndGameKRNK() : EndGameTablebase(keyDefinition) {
  }
};

// Define best moves in King-Rook-Rook/King endgame
class EndGameKRRK : public EndGameTablebase {
public:
  static const EndGameKeyDefinition4Men2Equal keyDefinition;

  EndGameKRRK() : EndGameTablebase(keyDefinition) {
  }
};
