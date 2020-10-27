#include "stdafx.h"

#define DEFAULT_STACK_CAPACITY 80

Game::Game() {
  allocateMemory(DEFAULT_STACK_CAPACITY);
  newGame();
}

Game &Game::newGame(const String &name) {
  bool wasSetup = isSetupMode();
  if(!wasSetup) {
    beginSetup();
  }

  setupStartPosition();
#if !defined(TABLEBASE_BUILDER)
  setMaxPositionRepeat();
  setMaxPlyCountWithoutCaptureOrPawnMove();
#endif
  if(name.length() == 0) {
    setName(getDefaultName());
  } else {
    setName(name);
  }

  endSetup();
  if(wasSetup) {
    beginSetup();
  }
  return *this;
}

Game::Game(const Game &src) {
  allocateMemory(src.m_stackCapacity);
  copyFrom(src);
}

Game::Game(const GameKey &src) {
  m_name = getDefaultName();
  allocateMemory(DEFAULT_STACK_CAPACITY);

  beginSetup();

  clearBoard();
  setPlayerInTurn(src.getPlayerInTurn());
  for(int pos = 0; pos < ARRAYSIZE(src.m_pieceKey); pos++) {
    const PieceKey pk = src.m_pieceKey[pos];
    if(pk != EMPTYPIECEKEY) {
      setPieceAtPosition(pk, pos);
    }
  }
  endSetup(&src);
}

Game &Game::operator=(const GameKey &src) {
  if(&src == &m_gameKey) {
    return *this;
  }
//  String signature = src.getPositionSignature().toString();

  beginSetup();

  memset(m_board,0, sizeof(m_board));
  forEachPlayer(p) {
    PlayerState &state = m_playerState[p];
    for(int pieceIndex = 0; pieceIndex < ARRAYSIZE(state.m_pieces);) {
      state.m_pieces[pieceIndex++]->m_onBoard = false;
    }
  }
  m_gameKey.d.m_playerInTurn = src.d.m_playerInTurn;
  for(int pos = 0; pos < ARRAYSIZE(src.m_pieceKey); pos++) {
    const PieceKey pk = src.m_pieceKey[pos];
    if(pk != EMPTYPIECEKEY) {
      setPieceAtPosition(pk, pos);
    }
  }
  resetGameHistory();
  return endSetup(&src);
}

Game::~Game() {
  deallocateMemory();
}

void Game::allocateMemory(int stackCapacity) {
  GameKey::validateSize(); // should be called somewhere. but only once

  allocateStack(stackCapacity);
  m_setupMode   = false;

#if defined(TABLEBASE_BUILDER)
  m_keydef                      = nullptr;
  m_swapPlayers                 = false;
  m_generateFictivePawnCaptures = false;
#else
  setMaxPositionRepeat();
  setMaxPlyCountWithoutCaptureOrPawnMove();
#endif
  allocateMoveGenerators();

  m_playerState[WHITEPLAYER].allocatePieceTable(WHITEPLAYER, m_playerState[BLACKPLAYER], &m_gameKey.d.m_castleState[WHITEPLAYER]);
  m_playerState[BLACKPLAYER].allocatePieceTable(BLACKPLAYER, m_playerState[WHITEPLAYER], &m_gameKey.d.m_castleState[BLACKPLAYER]);
}

void Game::deallocateMemory() {
  deallocateMoveGenerators();
  deallocateStack();
}

Game &Game::operator=(const Game &src) {
  if(this == &src) {
    return *this;
  }
  deallocateStack();
  allocateStack(src.m_stackCapacity);
  return copyFrom(src);
}

Game &Game::copyFrom(const Game &src) {
  m_name                             = src.m_name;
  m_gameKey                          = src.m_gameKey;
  m_playerState[WHITEPLAYER]         = src.m_playerState[WHITEPLAYER];
  m_playerState[BLACKPLAYER]         = src.m_playerState[BLACKPLAYER];
  m_stackSize                        = src.m_stackSize;
  m_gameKey                          = src.m_gameKey;
  m_gameResult                       = src.m_gameResult;
  m_setupMode                        = src.m_setupMode;

#if !defined(TABLEBASE_BUILDER)
  m_lastCaptureOrPawnMove            = src.m_lastCaptureOrPawnMove;
  m_maxPositionRepeat                = src.m_maxPositionRepeat;
  m_maxPliesWithoutCaptureOrPawnMove = src.m_maxPliesWithoutCaptureOrPawnMove;
  m_hashStack                        = src.m_hashStack;
#else
  m_generateFictivePawnCaptures      = src.m_generateFictivePawnCaptures;
#endif

  memcpy(m_gameStack, src.m_gameStack, sizeof(m_gameStack[0]) * m_stackSize);
  for(UINT i = 0; i < m_stackSize; i++) {
    const GameStackElement &e1 = src.m_gameStack[i];
    GameStackElement &e2 = m_gameStack[i];
    e2.m_piece         = getPiecePointer(src,e1.m_piece);
    e2.m_capturedPiece = getPiecePointer(src,e1.m_capturedPiece);
  }
  for(int i = 0; i < ARRAYSIZE(m_board); i++) {
    m_board[i] = getPiecePointer(src,src.m_board[i]);
  }
  updateLinks();
  return *this;
}

const GameKey &Game::getStartPosition() const {
  return (m_stackSize == 0) ? m_gameKey : m_gameStack[0].m_gameKey;
}

Game &Game::setGameAfterPly(const Game &src, int ply) {
  return setGameAfterPly(src.getStartPosition(), src.getHistory(), ply);
}

Game &Game::setGameAfterPly(const GameKey &startKey, const GameHistory &history, int ply) {
  *this = startKey;
  resetCapturedPieceTypes(WHITEPLAYER);
  resetCapturedPieceTypes(BLACKPLAYER);
  for(int i = 0; i <= ply; i++) {
    executeMove(history[i]);
  }
  return *this;
}

Piece *Game::getPiecePointer(const Game &src, const Piece *piece) {
  return piece ? m_playerState[piece->getPlayer()].m_pieces[piece->m_index] : nullptr;
}

GameUpdateFunction Game::getMoveFunction(PieceType pieceType) { // static
  switch(pieceType) {
  case King   : return &updateGameMoveKing;
  case Queen  : return &updateGameMoveQueen;
  case Rook   : return &updateGameMoveRook;
  case Bishop : return &updateGameMoveBishop;
  case Knight : return &updateGameMoveKnight;
  case Pawn   : return &updateGameMovePawn;
  default     : throwInvalidArgumentException(__TFUNCTION__, _T("pieceType=%d"), pieceType);
                return nullptr;
  }
}

#if defined(TABLEBASE_BUILDER)
GameUpdateFunction Game::getBackMoveFunction(PieceType pieceType) { // static
  switch(pieceType) {
  case King   : return &updateGameBackMoveKing;
  case Queen  : return &updateGameBackMoveQueen;
  case Rook   : return &updateGameBackMoveRook;
  case Bishop : return &updateGameBackMoveBishop;
  case Knight : return &updateGameBackMoveKnight;
  case Pawn   : return &updateGameBackMovePawn;
  default     : throwInvalidArgumentException(__TFUNCTION__, _T("pieceType=%d"), pieceType);
                return nullptr;
  }
}
#endif

GameUpdateFunction Game::getCaptureFunction(PieceType pieceType) { // static
  switch(pieceType) {
  case King   : return &updateGameCaptureKing;
  case Queen  : return &updateGameCaptureQueen;
  case Rook   : return &updateGameCaptureRook;
  case Bishop : return &updateGameCaptureBishop;
  case Knight : return &updateGameCaptureKnight;
  case Pawn   : return &updateGameCapturePawn;
  default     : throwInvalidArgumentException(__TFUNCTION__, _T("pieceType=%d"), pieceType);
                return nullptr;
  }
}

String Game::getDefaultName() const {
  return loadString(IDS_UNNAMED);
}

const String Game::getDisplayName() const {
  FileNameSplitter info(getFileName());
  return info.getFileName();
}

const Piece *Game::findFirstPieceInDirection(int pos, MoveDirection dir) const {
  switch(dir) {
  case MD_NONE      : return nullptr;
  case MD_LEFT      : return findFirstPieceInDirection(s_fieldInfo[pos].m_rowLine.m_lower  );
  case MD_RIGHT     : return findFirstPieceInDirection(s_fieldInfo[pos].m_rowLine.m_upper  );
  case MD_DOWN      : return findFirstPieceInDirection(s_fieldInfo[pos].m_colLine.m_lower  );
  case MD_UP        : return findFirstPieceInDirection(s_fieldInfo[pos].m_colLine.m_upper  );
  case MD_DOWNDIAG1 : return findFirstPieceInDirection(s_fieldInfo[pos].m_diag1Line.m_lower);
  case MD_UPDIAG1   : return findFirstPieceInDirection(s_fieldInfo[pos].m_diag1Line.m_upper);
  case MD_DOWNDIAG2 : return findFirstPieceInDirection(s_fieldInfo[pos].m_diag2Line.m_lower);
  case MD_UPDIAG2   : return findFirstPieceInDirection(s_fieldInfo[pos].m_diag2Line.m_upper);
  default           : throwInvalidArgumentException(__TFUNCTION__, _T("dir=%d"), dir);
                      return nullptr;
  }
}

const Piece *Game::findFirstPieceInDirection(PositionArray dir) const {
  if(dir == nullptr) {
    return nullptr;
  }
  for(int count = *(dir++); count--;) {
    const Piece *p = m_board[*(dir++)];
    if(p) {
      return p;
    }
  }
  return nullptr;
}

#if !defined(TABLEBASE_BUILDER)
int Game::evaluateScore() const {
  return (m_playerState[WHITEPLAYER].*m_whiteScore)() - (m_playerState[BLACKPLAYER].*m_blackScore)();
}
#endif

void Game::capturePiece(const Move &m) {
  Piece *piece = m.m_capturedPiece;
  (this->*(piece->m_updateWhenCaptured))(m);
  piece->m_onBoard = false;

#if !defined(TABLEBASE_BUILDER)
  piece->m_playerState.m_totalMaterial -= piece->m_materialValue;
  m_lastCaptureOrPawnMove = m_stackSize;
#endif
}

bool Game::doMove(const Move &m) {
  pushState();
  return tryMove(m);
}

Move Game::undoMove() {
  Move result = m_gameStack[m_stackSize-1];
  unTryMove();
  popState();
  return result;
}

// Assume gamestate has been saved by pushState
bool Game::tryMove(const Move &m) {
  ((Move&)(m_gameStack[m_stackSize-1])) = m;
  m_gameKey.d.m_EPSquare = -1;
  (this->*(m.m_piece->m_doMove))(m);
  m_playerState[PLAYERINTURN].m_kingAttackState = KING_NOT_ATTACKED;
  m_playerState[m_gameKey.d.m_playerInTurn ^= 1].setKingAttackState();
#if defined(TABLEBASE_BUILDER)
  return true;
#else
  return (m_hashStack.push(m_gameKey) <= (int)m_maxPositionRepeat) && (getPlyCountWithoutCaptureOrPawnMove() <= getMaxPlyCountWithoutCaptureOrPawnMove());
#endif
}

// Undo move done with tryMove and restores gamestate without popping it (should be done with popState)
void Game::unTryMove() {
  const GameStackElement &top = restoreState();

#if !defined(TABLEBASE_BUILDER)
  m_hashStack.pop();
#endif

  switch(top.m_type) {
  case NORMALMOVE   :
    { Piece *piece = top.m_piece;
      m_board[piece->m_position = top.m_from] = piece;
      if(m_board[top.m_to] = top.m_capturedPiece) {
        top.m_capturedPiece->m_onBoard = true;
      }
    }
    break;

  case PROMOTION    :
    undoPromotion(top);
    break;
  case ENPASSANT    :
    undoEnpassant(top);
    break;
  case SHORTCASTLING:
    undoShortCastling();
    break;
  case LONGCASTLING :
    undoLongCastling();
    break;
  default:
    throwException(_T("%s:Unknown movetype:%d"),__TFUNCTION__,top.m_type);
    break;
  }
}

void Game::undoShortCastling() {
  switch(PLAYERINTURN) {
  case WHITEPLAYER:
    MOVEPIECE_NOUPDATEKEY(F1, H1)
    MOVEPIECE_NOUPDATEKEY(G1, E1)
    break;
  case BLACKPLAYER:
    MOVEPIECE_NOUPDATEKEY(F8, H8)
    MOVEPIECE_NOUPDATEKEY(G8, E8)
    break;
  }
}

void Game::undoLongCastling() {
  switch(PLAYERINTURN) {
  case WHITEPLAYER:
    MOVEPIECE_NOUPDATEKEY(D1, A1)
    MOVEPIECE_NOUPDATEKEY(C1, E1)
    break;
  case BLACKPLAYER:
    MOVEPIECE_NOUPDATEKEY(D8, A8)
    MOVEPIECE_NOUPDATEKEY(C8, E8)
    break;
  }
}

void Game::undoEnpassant(const Move &m) {
  m_board[m.m_piece->m_position = m.m_from] = m.m_piece;
  m_board[m.m_to]                           = nullptr;
  m_board[m.m_capturedPiece->m_position]    = m.m_capturedPiece;
  m.m_capturedPiece->m_onBoard              = true;
}

void Game::undoPromotion(const Move &m) {
  m_board[m.m_piece->m_position = m.m_from] = m.m_piece;
  m.m_piece->setType(Pawn);

  if(m_board[m.m_to] = m.m_capturedPiece) {
    m.m_capturedPiece->m_onBoard = true;
  }
}

#if defined(TABLEBASE_BUILDER)
void Game::doBackMove(const Move &m) {
  pushState();
  tryBackMove(m);
}

Move Game::undoBackMove() {
  Move result = m_gameStack[m_stackSize-1];
  unTryBackMove();
  popState();
  return result;
}

void Game::tryBackMove(const Move &m) {
  ((Move&)(m_gameStack[m_stackSize-1])) = m;
  m_gameKey.d.m_EPSquare      = -1;
  m_gameKey.d.m_playerInTurn ^=  1;
  (this->*(m.m_piece->m_doBackMove))(m);
  m_playerState[CURRENTENEMY].m_kingAttackState = KING_NOT_ATTACKED;
  if(m_playerState[PLAYERINTURN].setKingAttackStateBackMove()) {
    PlayerState &state = m_playerState[CURRENTENEMY];
    const int kingPos = m_playerState[PLAYERINTURN].m_king->m_position;
    if((state.m_checkingSDAPosition = findAttackingPawnPosition(CURRENTENEMY, kingPos)) < 0) {
      state.m_checkingSDAPosition = findAttackingKnightPosition(CURRENTENEMY, kingPos);
    }
  }
  m_gameResult = NORESULT;
}

// Undo backward move done with tryBackMove and restores gamestate without popping it (should be done with popState)
void Game::unTryBackMove() {
  const GameStackElement &top = restoreState();
  Piece *piece = top.m_piece;
  m_board[piece->m_position = top.m_to] = piece;
  m_board[top.m_from] = nullptr;
}
#endif

void Game::pushState() {              // push state NOT including move
  if(m_stackSize == m_stackCapacity) {
    resizeStack();
  }
  GameStackElement &top = m_gameStack[m_stackSize++];
  top.m_savedState[WHITEPLAYER] = m_playerState[WHITEPLAYER];
  top.m_savedState[BLACKPLAYER] = m_playerState[BLACKPLAYER];
  top.m_gameKey                 = m_gameKey;
#if !defined(TABLEBASE_BUILDER)
  top.m_lastCaptureOrPawnMove   = m_lastCaptureOrPawnMove;
#endif
}

const GameStackElement &Game::restoreState() {
  const GameStackElement &top   = m_gameStack[m_stackSize-1];
  ((PlayerStateToPush&)m_playerState[WHITEPLAYER]) = top.m_savedState[WHITEPLAYER];
  ((PlayerStateToPush&)m_playerState[BLACKPLAYER]) = top.m_savedState[BLACKPLAYER];
  m_gameKey                     = top.m_gameKey;
#if !defined(TABLEBASE_BUILDER)
  m_lastCaptureOrPawnMove       = top.m_lastCaptureOrPawnMove;
#endif
  return top;
}

void Game::allocateStack(int stackCapacity) {
  m_stackCapacity = stackCapacity;
  m_gameStack     = new GameStackElement[m_stackCapacity]; TRACE_NEW(m_gameStack);
}

void Game::deallocateStack() {
  SAFEDELETEARRAY(m_gameStack);
  m_stackCapacity = 0;
}

void Game::resizeStack() { // doubles the size of the stack
  const int         capacity = m_stackCapacity;
  GameStackElement *stack    = m_gameStack;

  allocateStack(2*m_stackCapacity);
  memcpy(m_gameStack, stack, sizeof(m_gameStack[0]) * capacity);

  SAFEDELETEARRAY(stack);
}

