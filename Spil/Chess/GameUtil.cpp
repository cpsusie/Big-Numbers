#include "stdafx.h"
#include <io.h>
#include <Console.h>
#include <Date.h>
#include <Language.h>

bool Game::mustSelectPromotion(int from, int to) const {
  const Piece *piece = getPieceAtPosition(from);
  if(piece == NULL || piece->getPlayer() != PLAYERINTURN) {
    throwException(_T("Internal error:mustSelectPromotion(%s,%s):Piece=%s")
                  ,getFieldName(from),getFieldName(to)
                  ,piece?getPlayerName(piece->getPlayer()).cstr():_T("NULL"));
  }
  return (piece->getType() == Pawn) && (GETROW(to) == GETPROMOTEROW(piece->getPlayer()));
}

ExecutableMove Game::generateMove(int from, int to, PieceType promoteTo, MoveAnnotation annotation) const {
  const Piece *piece = getPieceAtPosition(from);
  if(piece == NULL || piece->getPlayer() != PLAYERINTURN) {
    throwException(_T("%s-%s is not a legal move in the current position")
                  ,getFieldName(from)
                  ,getFieldName(to));
  }
  Move move;
  MoveGenerator &mg = getMoveGenerator();
  for(bool more = mg.firstMove(move); more; more = mg.nextMove(move)) {
    if(move.m_piece == piece && move.m_to == to
      && (move.m_type != PROMOTION || legalPromotions[move.m_promoteIndex] == promoteTo)) {
      move.setAnnotation(annotation);
      return ExecutableMove(*this, move);
    }
  }
  throwException(_T("%s%s-%s not a legal move in position[%s]")
                ,piece->getShortName().cstr()
                ,getFieldName(from),getFieldName(to)
                ,m_gameKey.toString().cstr()
                );
  return ExecutableMove();
}

static bool moveMatchAnyFormat(const ExecutableMove &m, const String &s) {
  return (m.toString(MOVE_SHORTFORMAT  ).equalsIgnoreCase(s)
      ||  m.toString(MOVE_LONGFORMAT   ).equalsIgnoreCase(s)
      ||  m.toString(MOVE_FILEFORMAT   ).equalsIgnoreCase(s)
      ||  m.toString(MOVE_UCIFORMAT    ).equalsIgnoreCase(s)
      ||  m.toString(MOVE_DEBUGFORMAT  ).equalsIgnoreCase(s)
         );
}

ExecutableMove Game::generateMove(const String &s, MoveStringFormat mf) const {
  if(mf == -1) { // unknown format. try all
    String tmp = trim(s);
    const intptr_t qMark  = tmp.find('?');
    const intptr_t exMark = tmp.find('!');
    MoveAnnotation annotation = NOANNOTATION;
    if(qMark > 0 || exMark > 0) {
      const intptr_t annotationPos = (qMark < 0) ? exMark : (exMark < 0) ? qMark : min(qMark,exMark);
      annotation = parseAnnotation(tmp.cstr() + annotationPos);
      tmp = left(tmp, annotationPos);
    }
    Move move;
    move.setAnnotation(NOANNOTATION);
    MoveGenerator &mg = getMoveGenerator();
    for(bool more = mg.firstMove(move); more; more = mg.nextMove(move)) {
      const ExecutableMove em(*this, move);
      if(moveMatchAnyFormat(em, tmp)) {
        move.setAnnotation(annotation);
        return ExecutableMove(*this, move);
      }
    }
    const ExecutableMove nullMove;
    if(moveMatchAnyFormat(nullMove, tmp)) {
      return nullMove;
    }
  } else {
    switch(mf) {
    case MOVE_SHORTFORMAT  :
    case MOVE_LONGFORMAT   :
    case MOVE_FILEFORMAT   :
    case MOVE_DEBUGFORMAT  :
      { String tmp = trim(s);
        const intptr_t qMark  = tmp.find('?');
        const intptr_t exMark = tmp.find('!');
        MoveAnnotation annotation = NOANNOTATION;
        if(qMark > 0 || exMark > 0) {
          const intptr_t annotationPos = (qMark < 0) ? exMark : (exMark < 0) ? qMark : min(qMark,exMark);
          annotation = parseAnnotation(tmp.cstr() + annotationPos);
          tmp = left(tmp, annotationPos);
        }
        Move move;
        move.setAnnotation(NOANNOTATION);
        MoveGenerator &mg = getMoveGenerator();
        for(bool more = mg.firstMove(move); more; more = mg.nextMove(move)) {
          ExecutableMove em(*this, move);
          if(em.toString(mf).equalsIgnoreCase(tmp)) {
            move.setAnnotation(annotation);
            return ExecutableMove(*this, move);
          }
        }
        const ExecutableMove nullMove;
        if(nullMove.toString(mf).equalsIgnoreCase(tmp)) {
          return nullMove;
        }
      }
      break;
    case MOVE_UCIFORMAT :
      { Move move;
        move.setAnnotation(NOANNOTATION);
        MoveGenerator &mg = getMoveGenerator();
        for(bool more = mg.firstMove(move); more; more = mg.nextMove(move)) {
          if(move.toString(mf).equalsIgnoreCase(s)) {
            return ExecutableMove(*this, move);
          }
        }
        const ExecutableMove nullMove;
        if(nullMove.toString(mf).equalsIgnoreCase(s)) {
          return nullMove;
        }
      }
      break;
    default                :
      throwInvalidArgumentException(__TFUNCTION__, _T("Invalid format:%d"), mf);
    }
  }
  throwException(_T("%s is not a legal move"), s.cstr());
  return ExecutableMove();
}

Move Game::generateMove(const MoveBase &m) const {
  if(!m.isMove()) {
    return Move().setNoMove();
  } else {
    Move result;
    (MoveBase&)result      = m;
    result.m_piece         = m_board[result.m_from];
    result.m_capturedPiece = (result.m_type == ENPASSANT) ? m_board[result.m_to^8] : m_board[result.m_to];
    return result;
  }
}

bool Game::isSingleMovePosition() const {
  Move move;
  MoveGenerator &mg = getMoveGenerator();
  return mg.firstMove(move) ? !mg.nextMove(move) : false;
}

#ifdef TABLEBASE_BUILDER
Move Game::generateBackMove(int from, int to) const {
  const Piece *piece = getPieceAtPosition(to);
  if(piece == NULL || piece->getPlayer() != CURRENTENEMY) {
    throwException(_T("%s-%s is not a legal backmove in the current position")
                  ,getFieldName(from)
                  ,getFieldName(to));
  }
  Move move;
  MoveGenerator &mg = getBackMoveGenerator();
  for(bool more = mg.firstMove(move); more; more = mg.nextMove(move)) {
    if(move.m_piece == piece && move.m_from == from) {
      return move;
    }
  }
  throwException(_T("%s%s-%s not a legal backmove"),piece->getShortName().cstr(),getFieldName(from),getFieldName(to));
  return Move();
}

#endif

void Game::updateLinks() {
#ifdef TABLEBASE_BUILDER
  m_playerState[WHITEPLAYER].makeLinks();
  m_playerState[BLACKPLAYER].makeLinks();
#else
  m_whiteScore = m_playerState[WHITEPLAYER].makeLinks().getScoreFunction();
  m_blackScore = m_playerState[BLACKPLAYER].makeLinks().getScoreFunction();
#endif
}

void Game::executeMove(const MoveBase &m) {
  checkSetupMode(false);
  doMove(generateMove(m));
  updateLinks();
  setGameResult();
}

MoveBase Game::unExecuteLastMove() {
  checkSetupMode(false);

  if(m_stackSize == 0) {
    return MoveBase().setNoMove();
  }
  MoveBase m = undoMove();
  updateLinks();
  m_gameResult = NORESULT;
  return m;
}

bool Game::isDoubleCheck() const {
  switch(getKingAttackState()) {
  case KING_NOT_ATTACKED:
  case KING_LD_ATTACKED_FROM_ROW  :
  case KING_LD_ATTACKED_FROM_COL  :
  case KING_LD_ATTACKED_FROM_DIAG1:
  case KING_LD_ATTACKED_FROM_DIAG2:
  case KING_SD_ATTACKED           :
    return false;

  default:
    return true;
  }
}

int Game::getKingPosition(Player player) const {
  const PlayerState &state = m_playerState[player];
  return state.m_king->isOnBoard() ? state.m_king->getPosition() : -1;
}

const Piece *Game::getNonKingPieceOnBoardByIndex(Player player, int n) const {
  return m_playerState[player].getNonKingPieceOnBoardByIndex(n);
}

void Game::setGameResult() {
  m_gameResult = findGameResult();
}

GameResult Game::findGameResult() const {
  if(isSetupMode()) {
    if(!m_playerState[WHITEPLAYER].m_king->isOnBoard() || !m_playerState[BLACKPLAYER].m_king->isOnBoard()) {
      return NORESULT;
    }
  }
  Move m;
  if(!getMoveGenerator().firstMove(m)) {
    if(isKingInCheck()) {
      return (getPlayerInTurn() == WHITEPLAYER) ? WHITE_CHECKMATE : BLACK_CHECKMATE;
    } else {
      return STALEMATE;
    }
  }
#ifdef TABLEBASE_BUILDER
  return NORESULT;
#else
  if(getPositionRepeats() >= 3) {
    return POSITION_REPEATED_3_TIMES;
  } else if(getPlyCountWithoutCaptureOrPawnMove() > getMaxPlyCountWithoutCaptureOrPawnMove()) {
    return NO_CAPTURE_OR_PAWNMOVE;
  } else {
    return NORESULT;
  }
#endif
}

bool Game::isCheckmate(Player player) const {
  switch(player) {
  case WHITEPLAYER: return findGameResult() == WHITE_CHECKMATE;
  case BLACKPLAYER: return findGameResult() == BLACK_CHECKMATE;
  default         : INVALIDPLAYERERROR(player);
                    return false;
  }
}

PositionType Game::getPositionType() const {
  const int whiteCount = getPieceCountOnBoard(WHITEPLAYER);
  const int blackCount = getPieceCountOnBoard(BLACKPLAYER);
  switch(whiteCount + blackCount) {
  case 2 : return DRAW_POSITION; // 2 kings left => draw
  case 3 : return getPositionType21();
  case 4 :
  case 5 : return TABLEBASE_POSITION;
  default: return NORMAL_POSITION;
  }
}

PositionType Game::getPositionType21() const {
  const int whiteCount = getPieceCountOnBoard(WHITEPLAYER);
  const int blackCount = getPieceCountOnBoard(BLACKPLAYER);
  if(whiteCount + blackCount != 3) {
    invalidPositionSignature(_T("21"));
  }

  const Player twoPiecePlayer = (whiteCount == 2) ? WHITEPLAYER : BLACKPLAYER;
  switch(m_playerState[twoPiecePlayer].getNonKingPieceOnBoardByIndex(0)->getType()) {
  case Bishop:
  case Knight: return DRAW_POSITION; // KBK or KNK => draw
  }
  return TABLEBASE_POSITION;
}

void Game::invalidPositionSignature(const TCHAR *functionSuffix) const {
  throwInvalidArgumentException(format(_T("getPositionType%s"), functionSuffix).cstr()
                               ,_T("Invalid position signature:%s"), getPositionSignature().toString().cstr());
}

FieldSet Game::getLegalDestinationFields(int pos) const {
  if(!isValidPosition(pos)) {
    return FieldSet();
  }
  return getLegalDestinationFields(getPieceAtPosition(pos));
}

FieldSet Game::getLegalDestinationFields(const Piece *piece) const {
  const MoveBaseArray moves = getLegalMoves(piece);
  FieldSet result;

  for(size_t i = 0; i < moves.size(); i++) {
    result.add(moves[i].m_to);
  }
  return result;
}

MoveBaseArray Game::getLegalMoves(int pos) const {
  if(!isValidPosition(pos)) {
    return MoveBaseArray();
  }
  return getLegalMoves(getPieceAtPosition(pos));
}

MoveBaseArray Game::getLegalMoves(const Piece *piece) const {
  MoveBaseArray result;
  if(piece == NULL || piece->getPlayer() != PLAYERINTURN) {
    return result;
  }

  Move move;
  MoveGenerator &mg = getMoveGenerator();
  for(bool b = mg.firstMove(move); b; b = mg.nextMove(move)) {
    if(move.m_piece == piece) {
      result.add(move);
    }
  }
  return result;
}

#ifdef TABLEBASE_BUILDER
FieldSet Game::getLegalSourceFields(int pos) const {
  if(!isValidPosition(pos)) {
    return FieldSet();
  }
  return getLegalSourceFields(getPieceAtPosition(pos));
}

FieldSet Game::getLegalSourceFields(const Piece *piece) const {
  const MoveBaseArray moves = getLegalBackMoves(piece);
  FieldSet result;

  for(UINT i = 0; i < moves.size(); i++) {
    result.add(moves[i].m_from);
  }
  return result;
}

MoveBaseArray Game::getLegalBackMoves(int pos) const {
  if(!isValidPosition(pos)) {
    return MoveBaseArray();
  }
  return getLegalBackMoves(getPieceAtPosition(pos));
}

MoveBaseArray Game::getLegalBackMoves(const Piece *piece) const {
  MoveBaseArray result;
  if(piece == NULL || piece->getPlayer() == PLAYERINTURN) {
    return result;
  }

  Move move;
  MoveGenerator &mg = getBackMoveGenerator();
  for(bool b = mg.firstMove(move); b; b = mg.nextMove(move)) {
    if(move.m_piece == piece) {
      result.add(move);
    }
  }
  return result;
}
#endif

PositionSignature Game::getPositionSignature() const {
  PositionSignature result;
  forEachPlayer(p) {
    result.m_pieceTypes[p] = m_playerState[p].getPlayerSignature();
  }
  return result;
}

void Game::resetCapturedPieceTypes(Player player) {
  m_playerState[player].resetCapturedPieceTypes();
}

Move Game::getRandomMove() const {
  MoveGenerator &mg = getMoveGenerator();
  Move m;
  m.clearAnnotation();
  Array<Move> possibleMoves;
  for(bool more = mg.firstMove(m); more; more = mg.nextMove(m)) {
    possibleMoves.add(m);
  }
  if(possibleMoves.size() == 0) {
    return m.setNoMove();
  } else {
    return possibleMoves.select();
  }
}

String getMoveTypeName(MoveType type) {

#define caseMoveType(c) case c:return #c;

  switch(type) {
  caseMoveType(NOMOVE       )
  caseMoveType(NORMALMOVE   )
  caseMoveType(PROMOTION    )
  caseMoveType(ENPASSANT    )
  caseMoveType(SHORTCASTLING)
  caseMoveType(LONGCASTLING )
  default:
    return format(_T("getMoveTypeName::Unknown moveType:%d"),type);
  }
}

MoveDirection getOppositeDirection(MoveDirection direction) {
  switch(direction) {
  case MD_NONE      : return MD_NONE;
  case MD_LEFT      : return MD_RIGHT;
  case MD_RIGHT     : return MD_LEFT;
  case MD_DOWN      : return MD_UP;
  case MD_UP        : return MD_DOWN;
  case MD_DOWNDIAG1 : return MD_UPDIAG1;
  case MD_UPDIAG1   : return MD_DOWNDIAG1;
  case MD_DOWNDIAG2 : return MD_UPDIAG2;
  case MD_UPDIAG2   : return MD_DOWNDIAG2;
  default           : throwInvalidArgumentException(__TFUNCTION__, _T("direction=%d"), direction);
                      return MD_NONE;
  }
}

String getMoveDirectionName(MoveDirection direction) {

#define caseMoveDirection(c) case MD_##c: return #c;

  switch(direction) {
  caseMoveDirection(NONE);
  caseMoveDirection(LEFT);
  caseMoveDirection(RIGHT);
  caseMoveDirection(DOWN);
  caseMoveDirection(UP);
  caseMoveDirection(DOWNDIAG1);
  caseMoveDirection(UPDIAG1);
  caseMoveDirection(DOWNDIAG2);
  caseMoveDirection(UPDIAG2);
  default:
    return format(_T("getMoveDirectionName:Unknown direction:%d"), direction);
  }
}

String getSymmetricTransformationToString(SymmetricTransformation st) {

#define caseTransformation(t) case TRANSFORM_##t: return #t;

  switch(st) {
  case 0: return _T("EMPTY");
  caseTransformation(SWAPPLAYERS);
  caseTransformation(ROTATE180  );
  caseTransformation(ROTATERIGHT);
  caseTransformation(ROTATELEFT );
  caseTransformation(MIRRORROW  );
  caseTransformation(MIRRORCOL  );
  caseTransformation(MIRRORDIAG1);
  caseTransformation(MIRRORDIAG2);
  default:
    return format(_T("getSymmetricTransformationToString:Unknown symmetric transformation:%d"), st);
  }
}

#pragma warning(disable : 4073)
#pragma init_seg(lib)

static StringArray playerName;
static StringArray pieceName;
static StringArray pieceName_plur;
static StringArray pieceShortName;


static void loadStrings() {
  playerName.add(loadString(IDS_WHITEPLAYERNAME));
  playerName.add(loadString(IDS_BLACKPLAYERNAME));

  pieceName.add(_T("?"));
  pieceName.add(loadString(IDS_KINGNAME       ));
  pieceName.add(loadString(IDS_QUEENNAME      ));
  pieceName.add(loadString(IDS_ROOKNAME       ));
  pieceName.add(loadString(IDS_BISHOPNAME     ));
  pieceName.add(loadString(IDS_KNIGHTNAME     ));
  pieceName.add(loadString(IDS_PAWNNAME       ));

  pieceName_plur.add(_T("?"));
  pieceName_plur.add(loadString(IDS_KINGNAME_PLUR  ));
  pieceName_plur.add(loadString(IDS_QUEENNAME_PLUR ));
  pieceName_plur.add(loadString(IDS_ROOKNAME_PLUR  ));
  pieceName_plur.add(loadString(IDS_BISHOPNAME_PLUR));
  pieceName_plur.add(loadString(IDS_KNIGHTNAME_PLUR));
  pieceName_plur.add(loadString(IDS_PAWNNAME_PLUR  ));

  pieceShortName.add(_T("?"));
  pieceShortName.add(loadString(IDS_KINGSHORTNAME  ));
  pieceShortName.add(loadString(IDS_QUEENSHORTNAME ));
  pieceShortName.add(loadString(IDS_ROOKSHORTNAME  ));
  pieceShortName.add(loadString(IDS_BISHOPSHORTNAME));
  pieceShortName.add(loadString(IDS_KNIGHTSHORTNAME));
  pieceShortName.add(_T(""));
}

static void unloadStrings() {
  playerName.clear();
  pieceName.clear();
  pieceName_plur.clear();
  pieceShortName.clear();
}

void reloadStrings() {
  unloadStrings();
  loadStrings();
}

class StringTableLoader {
public:
  StringTableLoader()  { loadStrings();   }
  ~StringTableLoader() { unloadStrings(); }
};


static StringTableLoader stringsLoader;

String getPlayerName(Player player) {
  switch(player) {
  case WHITEPLAYER :
  case BLACKPLAYER : return playerName[player];
  default          : INVALIDPLAYERERROR(player);
                     return _T("?");
  }
}

String getPieceTypeName(PieceType pieceType, bool plur) {
  switch(pieceType) {
  case NoPiece :
  case King    :
  case Queen   :
  case Rook    :
  case Bishop  :
  case Knight  :
  case Pawn    : return plur ? pieceName_plur[pieceType] : pieceName[pieceType];
  default      : throwInvalidArgumentException(__TFUNCTION__, _T("pieceType=%d"), pieceType);
                 return _T("?");
  }
}

String getPieceTypeShortName(PieceType pieceType) {
  switch(pieceType) {
  case NoPiece :
  case King    :
  case Queen   :
  case Rook    :
  case Bishop  :
  case Knight  :
  case Pawn    : return pieceShortName[pieceType];
  default      : throwInvalidArgumentException(__TFUNCTION__, _T("pieceType=%d"), pieceType);
                 return _T("?");
  }
}

String getPieceNameEnglish(PieceKey key) {
  if(key == EMPTYPIECEKEY) {
    return _T("empty");
  } else {
    return format(_T("%s %s"), getPlayerNameEnglish(GET_PLAYER_FROMKEY(key)), getPieceTypeNameEnglish(GET_TYPE_FROMKEY(key)));
  }
}

String getBishopFlagsToString(BishopFlags bishopFlags) {
  String result;
  if(bishopFlags & WHITEFIELD_BISHOP) result += _T("W");
  if(bishopFlags & BLACKFIELD_BISHOP) result += _T("B");
  return result;
}

String getCastleString(MoveType type) {
  return (type == SHORTCASTLING) ? _T("0-0") : _T("0-0-0");
}

String getPinnedStateToString(PinnedState ps) {
  switch(ps) {
  case NOT_PINNED         : return _T("");
  case PINNED_TO_ROW      : return _T("-");
  case PINNED_TO_COL      : return _T("|");
  case PINNED_TO_DIAG1    : return _T("/");
  case PINNED_TO_DIAG2    : return _T("\\");
  default                 : throwInvalidArgumentException(__TFUNCTION__, _T("ps=%d"), ps);
                            return _T("?");
  }
}

String GameHistory::toString(MoveStringFormat mf, int width) const {
  if(size() == 0) {
    return _T("");
  }
  StringArray strList;
  const bool firstIsBlack = ((*this)[0].getPlayer() == BLACKPLAYER);
  for(size_t i = 0; i < size(); i++) {
    strList.add((*this)[i].toString(mf));
  }
  if(width == -1) {
    width = max(width, (int)strList.maxLength() + 2);
  }

  String result;
  for(int ply = 0, moveCount = 1; ply < (int)strList.size(); moveCount++) {
    result += format(_T("%3d. %-*s"), moveCount, width, ((ply==0)&&firstIsBlack)?_T("-"):strList[ply++].cstr()); // incr ply if white move
    if(ply < (int)size()) {
      result += format(_T(", %-*s\r\n"), width, strList[ply++].cstr());
    }
  }
  return result;
}

bool Game::hasSamePartialHistory(const Game &g1, const Game &g2) { // static
  if(g1.getStartPosition() != g2.getStartPosition()) {
    return false;
  }
  int n = min(g1.getPlyCount(), g2.getPlyCount());
  for(int ply = 0; ply < n; ply++) {
    const MoveBase &m1 = g1.getMove(ply);
    const MoveBase &m2 = g2.getMove(ply);
    if(m1 != m2) {
      return false;
    }
  }
  return true;
}

String getKingAttackStateToString(KingAttackState attackState) {
  String result;
  if(attackState & KING_LD_ATTACKED_FROM_ROW     ) result =  _T("-");
  if(attackState & KING_LD_ATTACKED_FROM_COL     ) result += _T("|");
  if(attackState & KING_LD_ATTACKED_FROM_DIAG1   ) result += _T("/");
  if(attackState & KING_LD_ATTACKED_FROM_DIAG2   ) result += _T("\\");
  if(attackState & KING_SD_ATTACKED              ) result += _T("sd");
  if(attackState & KING_MANY_ATTACKS             ) result += _T("m");
  return result;
}

const TCHAR *getAnnotationToString(MoveAnnotation annotation) {
  switch(annotation) {
  case NOANNOTATION        : return _T("");
  case BAD_MOVE            : return _T("?");
  case VERYBAD_MOVE        : return _T("??");
  case GOOD_MOVE           : return _T("!");
  case EXCELLENT_MOVE      : return _T("!!");
  case INTERESTING_MOVE    : return _T("!?");
  case DOUBIOUS_MOVE       : return _T("?!");
  default                  : throwInvalidArgumentException(__TFUNCTION__, _T("annotation=%d"), annotation);
                             return _T("");
  }
}

String getGameResultToString(GameResult gameResult) {

#define caseToStr(v) case v: return #v;

  switch(gameResult) {
  caseToStr(NORESULT                  )
  caseToStr(WHITE_CHECKMATE           )
  caseToStr(BLACK_CHECKMATE           )
  caseToStr(STALEMATE                 )
#ifndef TABLEBASE_BUILDER
  caseToStr(POSITION_REPEATED_3_TIMES )
  caseToStr(NO_CAPTURE_OR_PAWNMOVE    )
  caseToStr(DRAW                      )
#endif
  default: return format(_T("Unknown gameResult:%d"), gameResult);
  }
}

int parseInt(Tokenizer &tok) {
  int result;
  if(!tok.hasNext() || (_stscanf(tok.next().cstr(), _T("%d"), &result) != 1)) {
    throwException(_T("Expected number"));
  }
  return result;
}

bool isValidPosition(int position) {
  return (0 <= position) && (position < 64);
}

bool isValidPawnPosition(int position) {
  const int row = GETROW(position);
  return (row >= 1) && (row <= 6);
}

bool isValidPosition(int row, int col) {
  return isValidLine(row) && isValidLine(col);
}

bool isValidLine(int line) {
  return (0 <= line) && (line < 8);
}

const TCHAR *getFieldName(int row, int col) {
#ifdef _DEBUG
  VALIDATEPOSITION(row, col);
#endif
  return Game::fieldInfo[MAKE_POSITION(row, col)].m_name;
}

const TCHAR *getFieldName(int position) {
#ifdef _DEBUG
  VALIDATEPOS(position);
#endif
  return Game::fieldInfo[position].m_name;
}

void validatePosition(const TCHAR *function, int pos) {
  if(!isValidPosition(pos)) {
    throwInvalidArgumentException(function, _T("Position=%d. Must be [0..63]"), pos);
  }
}

void validatePosition(const TCHAR *function, int row, int col) {
  if(!isValidPosition(row,col)) {
    throwInvalidArgumentException(function, _T("row=%d. col=%d"), row, col);
  }
}

void invalidPlayerError(const TCHAR *method, Player player) {
  throwException(_T("%s:Invalid player:%d"), method, player);
}

char getColumnName(int position) {
  return 'a' + GETCOL(position);
}

char getRowName(int position) {
  return '1' + GETROW(position);
}

const TCHAR *getFieldColorName(FieldColor color) {
  return (color == WHITEFIELD) ? _T("white") : _T("black");
}

const TCHAR *getFieldColorName(int position) {
  return getFieldColorName(getFieldColor(position));
}

static String _getFieldName(int position) { // used by getPositionArrayToString, to avoid cyclic reference for FieldInfo-constructor
  return format(_T("%c%c"), getColumnName(position), getRowName(position));
}

String getPositionArrayToString(PositionArray positions) {
  if(positions == NULL) {
    return _T("");
  } else {
    String result;
    const TCHAR *delimiter = _T("");
    for(int count = *(positions)++; count--; delimiter=_T(",")) {
      result += format(_T("%s%s"), delimiter, _getFieldName(*(positions++)).cstr());
    }
    return result;
  }
}

String DoubleLine::toString() const {
  return format(_T("%s, index=%d, blockable=%s, lower=(%s), upper=(%s)")
               ,getFieldName(m_pos)
//               ,getLineDirectionName(m_direction).cstr()
               ,m_index
               ,boolToStr(m_isBlockable)
               ,getPositionArrayToString(m_lower).cstr()
               ,getPositionArrayToString(m_upper).cstr());
}

#ifdef _DEBUG

String DoubleLine::makeDebugString(int pos, PositionArray lower, PositionArray upper) { // static
  return format(_T("(%s)"), getFieldName(pos))
       + _T(":Lower:(") + getPositionArrayToString(lower) + _T(")")
       + _T(",Upper:(") + getPositionArrayToString(upper) + _T(")");
}

String FieldInfo::makeDebugString(int           pos
                                 ,const TCHAR   *name
                                 ,PositionArray kingAttacks
                                 ,PositionArray knightAttacks
                                 ,PositionArray whitePawnAttacks
                                 ,PositionArray blackPawnAttacks
                                 ,PositionArray attackingWhitePawnPositions
                                 ,PositionArray attackingBlackPawnPositions) { //   static
  return format(_T("\nPosition             :%s"
                   "\nKingAttacks          :%s"
                   "\nKnightAttacks        :%s"
                   "\nWhitePawnAttacks     :%s"
                   "\nBlackPawnAttacks     :%s"
                   "\nAttackingWhitePawnPos:%s"
                   "\nAttackingBlackPawnPos:%s"
                   "\n")
               ,name
               ,getPositionArrayToString(kingAttacks).cstr()
               ,getPositionArrayToString(knightAttacks).cstr()
               ,getPositionArrayToString(whitePawnAttacks).cstr()
               ,getPositionArrayToString(blackPawnAttacks).cstr()
               ,getPositionArrayToString(attackingWhitePawnPositions).cstr()
               ,getPositionArrayToString(attackingBlackPawnPositions).cstr()
               );
}

#endif

FieldInfo::FieldInfo(int           pos
                    ,const TCHAR   *name
                    ,PositionArray leftRow
                    ,PositionArray rightRow
                    ,PositionArray lowerCol
                    ,PositionArray upperCol
                    ,PositionArray lowerDiag1
                    ,PositionArray upperDiag1
                    ,PositionArray lowerDiag2
                    ,PositionArray upperDiag2
                    ,PositionArray kingAttacks
                    ,PositionArray knightAttacks
                    ,PositionArray whitePawnAttacks
                    ,PositionArray blackPawnAttacks
                    ,PositionArray attackingWhitePawnPositions
                    ,PositionArray attackingBlackPawnPositions
                    )
                    : m_rowLine(   _IS_INNERCOL(  GETCOL(pos)), leftRow   , rightRow  , ATTACKS_PARALLEL, pos, GETROW(pos)       )
                    , m_colLine(   _IS_INNERROW(  GETROW(pos)), lowerCol  , upperCol  , ATTACKS_PARALLEL, pos, GETCOL(pos)       )
                    , m_diag1Line( _IS_INNERFIELD(pos)        , lowerDiag1, upperDiag1, ATTACKS_DIAGONAL, pos, _GETDIAGONAL1(pos))
                    , m_diag2Line( _IS_INNERFIELD(pos)        , lowerDiag2, upperDiag2, ATTACKS_DIAGONAL, pos, _GETDIAGONAL2(pos))
                    , m_kingAttacks(     kingAttacks     )
                    , m_knightAttacks(   knightAttacks   )
                    , m_whitePawnAttacks(whitePawnAttacks)
                    , m_blackPawnAttacks(blackPawnAttacks)
                    , m_attackingWhitePawnPositions(attackingWhitePawnPositions)
                    , m_attackingBlackPawnPositions(attackingBlackPawnPositions)
                    , m_pos(pos)
                    , m_row(        GETROW(         pos))
                    , m_col(        GETCOL(         pos))
                    , m_diag1(      _GETDIAGONAL1(  pos))
                    , m_diag2(      _GETDIAGONAL2(  pos))
                    , m_innerRow(   _IS_INNERROW(   GETROW(pos)))
                    , m_innerCol(   _IS_INNERCOL(   GETCOL(pos)))
                    , m_innerField( _IS_INNERFIELD( pos))
                    , m_cornerField(_IS_CORNERFIELD(pos))
                    , m_fieldColor( _GET_FIELDCOLOR(pos))
                    , m_square(     _GET_SQUARE(    pos))
                    , m_name(name)


#ifdef _DEBUG
                    , m_debugStr(makeDebugString(pos
                                                ,name
                                                ,kingAttacks
                                                ,knightAttacks
                                                ,whitePawnAttacks
                                                ,blackPawnAttacks
                                                ,attackingWhitePawnPositions
                                                ,attackingBlackPawnPositions))
#endif
{
}

String FieldInfo::toString() const {
  return format(_T("%s:row=%d, col=%d, diag1=%2d, diag2=%2d, innerRow=%s, innerCol=%s innerField=%s\n"
                   "  %s\n"
                   "  %s\n"
                   "  %s\n"
                   "  %s\n"
                   "  king attacks                 :%s\n"
                   "  knight attacks               :%s\n"
                   "  whitepawn attacks            :%s\n"
                   "  blackpawn attacks            :%s\n"
                   "  attacking whitepawn positions:%s\n"
                   "  attacking blackpawn positions:%s\n")
              , m_name
              , m_row
              , m_col
              , m_diag1
              , m_diag2
              , boolToStr(m_innerRow)
              , boolToStr(m_innerCol)
              , boolToStr(m_innerField)
              , m_rowLine.toString().cstr()
              , m_colLine.toString().cstr()
              , m_diag1Line.toString().cstr()
              , m_diag2Line.toString().cstr()
              , getPositionArrayToString(m_kingAttacks     ).cstr()
              , getPositionArrayToString(m_knightAttacks   ).cstr()
              , getPositionArrayToString(m_whitePawnAttacks).cstr()
              , getPositionArrayToString(m_blackPawnAttacks).cstr()
              , getPositionArrayToString(m_attackingWhitePawnPositions).cstr()
              , getPositionArrayToString(m_attackingBlackPawnPositions).cstr()
              );

}

void setCurrentLanguage(LANGID langID) {
  Language::setLanguageForThread(langID);
  reloadStrings();
}

String getResourceFileName(const String &fileName) {
  FileNameSplitter fInfo(__FILE__);
  fInfo.setDir(FileNameSplitter::getChildName(fInfo.getDir(),_T("res")));
  return fInfo.setExtension(_T("")).setFileName(fileName).getAbsolutePath();
}

String createTempFileName() {
  String fileName = getTempFileName(_T("cXXXXXX"));
  _tmktemp(fileName.cstr());
  return fileName;
}

String getTempFileName(const String &fileName) {
  return FileNameSplitter::getChildName(_T("c:\\temp"), fileName);
}

void throwUserException(int id, ...) {
  const String s = loadString(id);
  va_list argptr;
  va_start(argptr, id);
  const String msg = vformat(s.cstr(), argptr);
  va_end(argptr);
  throwException(msg);
}

static FILE *verboseLog = NULL;

void setVerboseLogging(bool on) {
  if(on) {
    if(verboseLog == NULL) {
      String fileName = getTempFileName(format(_T("chess%s.log"), Timestamp().toString(_T("yyyyMMddhhmm")).cstr()));
      for(;;) {
        if(ACCESS(fileName, 0) < 0) {
          break;
        }
        fileName = getTempFileName(format(_T("chess%s.log"), Timestamp().toString(_T("yyyyMMddhhmmss")).cstr()));
      }
      verboseLog = MKFOPEN(fileName, _T("w"));
    }
  } else {
    if(verboseLog != NULL) {
      fclose(verboseLog);
      verboseLog = NULL;
    }
  }
}

void verbose(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);
  vverbose(format, argptr);
  va_end(argptr);
}

void updateMessageField(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vupdateMessageField(format, argptr);
  va_end(argptr);
}

void VerboseReceiver::vprintf(const TCHAR *format, va_list argptr) {
  String msg = vformat(format, argptr);
  const intptr_t bsIndex = msg.find('\b');
  if(bsIndex >= 0) {
    msg.cstr()[bsIndex] = '\0';
    const COORD oldPos = Console::getCursorPos(STD_ERROR_HANDLE);
    _ftprintf(stderr, _T("%s"), msg.cstr());
    fflush(stderr);
    Console::setCursorPos(oldPos, STD_ERROR_HANDLE);
    return;
  }
  _ftprintf(stderr, _T("%s"), msg.cstr());
  fflush(stderr);

  if((verboseLog != NULL) && (msg.find('\r') < 0)) {
    _ftprintf(verboseLog, _T("%s"), msg.cstr());
    fflush(verboseLog);
  }
}

class ConsoleTitleUpdater {
private:
  String m_origTitle;
public:
  ConsoleTitleUpdater();
  ~ConsoleTitleUpdater();
  void setTitle(const String &s);
};

ConsoleTitleUpdater::ConsoleTitleUpdater() {
  m_origTitle = Console::getTitle();
}

ConsoleTitleUpdater::~ConsoleTitleUpdater() {
  if(m_origTitle.length() != 0) {
    Console::setTitle(m_origTitle);
  }
}

void ConsoleTitleUpdater::setTitle(const String &s) {
  Console::setTitle(m_origTitle + s);
}

void VerboseReceiver::vupdateMessageField(const TCHAR *format, va_list argptr) {
  static ConsoleTitleUpdater ctu;
  ctu.setTitle(vformat(format, argptr));
}

void VerboseReceiver::clear() {
  // empty
}

static VerboseReceiver defaultVerboseReceiver, *currentVerboseReceiver = &defaultVerboseReceiver;

class VerboseSilent : public VerboseReceiver {
public:
  void vprintf(            const TCHAR *format, va_list argptr) {
  }
  void vupdateMessageField(const TCHAR *format, va_list argptr) {
  }
  void clear() {
  }
};

static VerboseSilent verboseNullDevice;
VerboseReceiver *VERBOSENULL = &verboseNullDevice;

void vverbose(const TCHAR *format, va_list argptr) {
  currentVerboseReceiver->vprintf(format, argptr);
}

void vupdateMessageField(const TCHAR *format, va_list argptr) {
  currentVerboseReceiver->vupdateMessageField(format, argptr);
}

void clearVerbose() {
  currentVerboseReceiver->clear();
}

void redirectVerbose(VerboseReceiver *receiver) {
  currentVerboseReceiver = receiver ? receiver : &defaultVerboseReceiver;
}

void log(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vlog(format, argptr);
  va_end(argptr);
}

void vlog(const TCHAR *format, va_list argptr) {
  if(verboseLog == NULL) {
    return;
  }
  const String msg = vformat(format, argptr);
  if(msg.find('\r') < 0) {
    _ftprintf(verboseLog, _T("%s"), msg.cstr());
    fflush(verboseLog);
  }
}
