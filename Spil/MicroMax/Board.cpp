#include "stdafx.h"
#include <BitSet.h>
#include <Random.h>

#pragma warning(disable:4554)

BoardConfig        Board::s_bc;
BoardConfig        Board::s_savedBoard;
int                Board::s_zobristIndex[1024];                                  // hash-index translation table
int                Board::s_zobristKey[  1024];                                  // hash-key   translation table
HashElement       *Board::s_hashMap     = NULL;                                  // hash table, MAPSIZE entries
int                Board::s_maxDepth    = 30;                                    // Maximum depth of the search
Move               Board::s_bestMove;
Move               Board::s_usableMove;
SearchStatistic    Board::s_searchStatistic;
bool               Board::s_moveFound;
int                Board::s_stopCode;
bool               Board::s_gameOver;
BoardConfig        Board::s_boardHistory[1024];
Move               Board::s_moveHistory[1024];
int                Board::s_historySize = 0;

//#define TEST_TABLETRANSLATE

#ifdef TEST_TABLETRANSLATE

class TableSet : public BitSet {
  const String m_name;
public:
  TableSet(const TCHAR *name, UINT size) : BitSet(size), m_name(name) {
  }
  ~TableSet();
};

TableSet::~TableSet() {
  FILE *f = FOPEN(_T("c:\\temp\\Unused") + m_name + _T(".txt"),_T("w"));
  const size_t n = getCapacity();
  for(size_t i = 0; i < n; i++) {
    if(!contains(i)) {
      _ftprintf(f, _T("%zd\n"), i);
    }
  }
  fclose(f);
}

int Board::getZobristIndex(int t) { // static
  static TableSet set(_T("ZobristIndex"), ARRAYSIZE(s_zobristIndex));
  if(t < 0 || t >= ARRAYSIZE(s_zobristIndex)) {
    throwInvalidArgumentException(__TFUNCTION__, _T("t=%d"), t);
  }
  set.add(t);
  return s_zobristIndex[t];
}

int Board::getZobristKey(int t) { // static
  static TableSet set(_T("ZobristKey"), ARRAYSIZE(s_zobristKey));
  if(t < 0 || t >= ARRAYSIZE(s_zobristKey)) {
    throwInvalidArgumentException(__TFUNCTION__, _T("t=%d"), t);
  }
  set.add(t);
  return s_zobristKey[t];
}

#define TRANSLATE(table, n) getZobrist##table(n)
#else 
#define TRANSLATE(table, n) s_zobrist##table[n]
#endif

#define TR(table, f, p) TRANSLATE(table,(f)|((p)&8)|(S*((p)&7)))
#define SQUARE(pos)     s_bc.m_field[pos]

#define HASHCODE(table, m) (TR(table, m.d.m_to,SQUARE(m.d.m_to))-TR(table, m.d.m_from,movingPiece)-TR(table, capturedSquare,capturedPiece))
#define HASHINDEX(m)       HASHCODE(Index,m)
#define HASHKEY(m)         HASHCODE(Key  ,m)

static const char _wpSteps[]      = { 16, 15, 17, 0                     }; // WP 
static const char _bpSteps[]      = {-16,-15,-17, 0                     }; // BP 
static const char _knightSteps[]  = {-14, 14,-18, 18,-31, 31,-33, 33, 0 }; // N
static const char _kqSteps[]      = { -1,  1,-16, 16,-15, 15,-17, 17, 0 }; // K/Q
static const char _bishopSteps[]  = {-15, 15,-17, 17, 0                 }; // B  
static const char _rookSteps[]    = { -1,  1,-16, 16, 0                 }; // R

const char *Board::s_stepList[] = {
  NULL
 ,_wpSteps
 ,_bpSteps
 ,_knightSteps
 ,_kqSteps
 ,_bishopSteps
 ,_rookSteps
 ,_kqSteps
};

static const char legalPromotions[5] = { 0, QUEEN, KNIGHT, ROOK, BISHOP };

#define FIELDSCORE(m) (BoardConfig::getFieldValue(m.d.m_to)-BoardConfig::getFieldValue(m.d.m_from))

void Board::initEngine() {
  if(s_hashMap == NULL) {
    s_hashMap = new HashElement[MAPSIZE];
  }

  memset(s_zobristIndex,0, sizeof(s_zobristIndex));
  memset(s_zobristKey  ,0, sizeof(s_zobristKey  ));

  BoardConfig::initOnce();

  randomize();
  for(int i = ARRAYSIZE(s_zobristIndex); i--;) {
    s_zobristIndex[i] = randInt();
  }
  for(int i = ARRAYSIZE(s_zobristKey); i--;) {
    s_zobristKey[i] = randInt();
  }

  initGame();
}

void Board::initGame() {
  s_bc.setupStartBoard();
  initHistory();
}

void Board::setSide(int side) { // static
  if(side != s_bc.m_side) {
    s_bc.m_side = side;
    initHistory();
  }
}

void Board::initHistory() {
  s_historySize = 0;
  s_gameOver    = false;
  memset(s_boardHistory, 0, sizeof(s_boardHistory));
  memset(s_moveHistory,  0, sizeof(s_moveHistory));
  initHashMap();
}

void Board::initHashMap() {
  memset(s_hashMap, 0, MAPSIZE*sizeof(HashElement));
  s_bc.m_hashCode.clear();
}

#define REQUEST_FINDMOVE    0x01
#define REQUEST_FORCEMOVE   0x02
#define REQEUST_EXECUTEMOVE (REQUEST_FINDMOVE | REQUEST_FORCEMOVE)
#define REQUEST_CHECKMOVE   0x04
#define REQUEST_LOOKAHEAD   0x08
#define REQUEST_SEARCHNULL  0x10

bool Board::findMove() { // static 
  s_bestMove.setNoMove();
  if(s_gameOver) {
    return false;
  }
  s_savedBoard = getConfig();
  int score;
  try {
    if((score = searchMove(REQUEST_FINDMOVE, 30)) == MAXSCORE) {
      throw STOP_WHENMOVEFOUND;
    }
    setConfig(s_savedBoard);
  } catch(int) {
    setConfig(s_savedBoard);
    throw;
  }
  return false;
}

void Board::executeMove(const Move &move) {
  if(s_gameOver) {
    throwException(_T("Game is over. %s"), getGameResultString(getGameResult()).cstr());
  }
  s_boardHistory[s_historySize] = getConfig();
  s_bestMove = move;
#ifdef _DEBUG
  replyMessage(_T("executeMove(%s). ybit:%s"), s_bestMove.toString().cstr(), s_bestMove.d.m_ybit?"true":"false");
#endif

  const SearchStatistic saveStatistic = s_searchStatistic;
  s_searchStatistic.startSearch(false);
  resetStopCode();
  HashElement *link;
  const int score = negamax(getSide(), MINSCORE, MAXSCORE, s_bc.m_score, s_bc.m_EP, REQEUST_EXECUTEMOVE, 3, link);
  s_searchStatistic = saveStatistic;

  if(score != MAXSCORE) {
    throwException(_T("%s:Illegal move:<%s>\n"), getModuleFileName().cstr(), move.toString().cstr());
  }
  switchSide();
  s_moveHistory[s_historySize++] = move;
  if(getGameResult() != GR_NORESULT) {
    s_gameOver = true;
  }
}

bool Board::isDrawByRepetition() { // static
  if(s_bc.m_fiftyMoves < 3) {
    return false;
  }
  const BoardConfig *last = s_boardHistory + s_historySize - 1;
  for(const BoardConfig *bhcp = last-2, *bhcp0 = last-s_bc.m_fiftyMoves; bhcp > bhcp0; bhcp -= 2) {
    if(memcmp(s_bc.m_field, bhcp->m_field, 120) == 0) {; // only compare field-area ignoring m_side and EP
      return true;
    }
  }
  return false;
}

String Board::getUCIScoreStr(int score) { // static
  if (score >= MAXSCORE - 30) {
    return format(_T("mate %d"), MAXSCORE-score);
  } else if(score <= MINSCORE + 30) {
    return format(_T("mate %d"), MINSCORE-score);
  } else {
    return format(_T("cp %d"), score);
  }
}

GameResult Board::getGameResult() { // static
  int count = 0;
// Search last 50 states with this stm for third repeat 
  for(int j = s_historySize-4; j >= 0; j-=2) {
    if(s_bc == s_boardHistory[j]) {
      if(++count == 2) {// third repeat 
        return GR_REPETITION;
      }
    }
  }

  const SearchStatistic saveStatistic = s_searchStatistic;
  const int score = searchMove(REQUEST_CHECKMOVE, 3);
  s_searchStatistic = saveStatistic;

  if(score == 0 && !getBestMove().isMove()) {
    return GR_STALEMATE;
  }
  if(score == MINSCORE+1) {
    return (getSide() == WHITE) ? GR_WHITECHECKMATE : GR_BLACKCHECKMATE;
  }
  if(isFiftyMoves()) {
    return GR_FIFTYMOVES;
  }
  return GR_NORESULT;
}

int Board::searchMove(char request, int n) {
  s_bestMove.setNoMove();
  s_searchStatistic.startSearch(true);
  resetStopCode();
  s_moveFound = false;
  HashElement *link;
  return negamax(getSide(),MINSCORE,MAXSCORE,s_bc.m_score,s_bc.m_EP,request,n,link);
}

int Board::negamax(int side, int alfa, int beta, int e, int EP, char request, int n, HashElement* &link) {                   // Recursive minimax search, side=moving side,
                                                                                                                             // (alfa,beta)=window, e=current evaluation score,
                                                                                                                             // EP = e.p. square
                                                                                                                             // n  = depth
  const int      enemy         = side^COLORBITS;
  link                         = s_hashMap + ((s_bc.m_hashCode.m_index+side*EP)&(MAPSIZE-1));                                // return score
  HashElement   &a             = *link;                                                                                      // lookup position in hash table
  int            bestScore     = a.m_score;
  HashElement   *bestLink      = a.m_next, *tmpLink = NULL;
  const HashCode savedHashCode = s_bc.m_hashCode;
  int            depth, capturedMaterial;
  FastMove       bestMove;


  alfa -= alfa <  e;                                                                                                         // adjust window: delay bonus
  beta -= beta <= e;                                      

  if((a.m_key-s_bc.m_hashCode.m_key)|(request&REQUEST_FINDMOVE)                                                              // miss: other pos. or empty
    |!(((bestScore <= alfa)|a.m_cutAlpha) && ((bestScore >= beta)|a.m_cutBeta))                                              // or window incompatible
    ) {                                                                                                                 
    depth    = 0;
    bestMove.setNoMove();                                                                                                    // start iteration from scratch
    bestMove.d.m_from = a.m_move.d.m_from;
  } else {
    depth    = a.m_depth;                                                                                                    // resume at stored depth
    bestMove = a.m_move;
  }
  const OccupationMap savedOccupationMap = s_bc.m_occupationMap;
  PlayerOccupationMap &playerMap         = s_bc.m_occupationMap.m_playerMap[side >> 4];
  PlayerOccupationMap &enemyMap          = s_bc.m_occupationMap.m_playerMap[enemy>> 4];

  while((depth++ < n)
     || (depth   < 3)                                                                                                        // min depth = 2 iterative deepening loop
     || ((request&REQUEST_FINDMOVE)&&!s_bestMove.isMove()) 
       && ((depth<=s_maxDepth) || (s_bestMove = bestMove, depth=3))                                                          // root: deepen
       ) {

    int P = ((depth>2) && (beta+MAXSCORE)) ? negamax(enemy,-beta,1-beta,-e,S,REQUEST_SEARCHNULL,depth-3,tmpLink) : MAXSCORE; // search null move
    if((bestScore = ((-P<beta)|(s_bc.m_R>35)) ? (depth-2) ? MINSCORE : e : -P) > MINSCORE) {                                 // prune if > beta  unconsidered:static eval
      bestLink = tmpLink;
    }

//    if(nodeCount % 1000 == 0) {
//      bc.validate();
//    }
    bool h = bestMove.d.m_ybit;                                                                                           // request try noncastling 1st

#define FROM move.d.m_from
#define TO   move.d.m_to

    for(PieceIterator it(playerMap, bestMove.d.m_from); it.hasNext();) {                                                  // start scan at prev. best
      FastMove           move;
      const char         fromIndex    = it.next();
      FROM    = INDEXTOPOS(fromIndex);
      const char         movingPiece  = SQUARE(FROM);
      UINT64            &origFromBits = playerMap.getBits(fromIndex);
      const UINT64       fromRemoved  = origFromBits & ~((UINT64)1<<fromIndex);

#ifdef _DEBUG
      if(!(movingPiece&side)) {
        const String fromName = getFieldName(FROM);
        if(movingPiece) {
          throwException(_T("scanning %s:There is a %s on %s"), getSideStr(side), getPieceName1(movingPiece).cstr(), fromName.cstr());
        } else {
          throwException(_T("scanning %s:No piece at %s"), getSideStr(side), fromName.cstr());
        }
      }
#endif

      char movingPieceType, step; 
      for(const char *stepp = s_stepList[movingPieceType=movingPiece&7]; step = *(stepp++);) {                            // loop over directions in stepList
A:                                                                                                                        // resume normal after best
        int F, G;
        F = G = S;                                                                                                        // (F,G)=castling.rights
        char capturedPiece, capturedPieceType;
        TO = FROM;                                                                                    
        do {                                                                                                              // traverse ray.
          if(h) {
            if(!bestMove.d.m_ybit) {                                                                                      // sneak in prev. best move
              break;
            }
            TO = bestMove.d.m_to;
            move.d.m_promoteIndex = bestMove.d.m_promoteIndex ? (bestMove.d.m_promoteIndex-1) : 0;
          } else {                                                                                                      
            if((TO += step) & MASK88) {                                                                                   // check for board edge
              break;
            }
            move.d.m_promoteIndex   = 0;
          }

          bestScore = (EP-S) & SQUARE(EP) && ((move.d.m_to-EP)<2) & ((EP-move.d.m_to)<2) ? MAXSCORE : bestScore;          // bad castling

          const char capturedSquare = TO^(ISPAWN(movingPieceType) && (TO==EP)?16:0);                                      // shift capturedSquare if e.p.

          if(((capturedPiece = SQUARE(capturedSquare))&side) || (ISPAWN(movingPieceType) && (!move.isDiagonalMove()-!capturedPiece))) { // capture own, bad pawn move
            break;
          }

          if((capturedMaterial = BoardConfig::getPieceValue(capturedPieceType=(capturedPiece&7))) < 0) {                  // value of captured piece
            bestScore = MAXSCORE;                                                                                         // if king capture
            depth = 98;
          }

          do {                                                                                                            // for all possible promotions
            s_searchStatistic.m_nodeCount++;
            if(s_stopCode) {                                                                                              // check if we are interrupted
              if(s_stopCode & STOP_IMMEDIATELY) {                                                                         // by timer or user
                throw s_stopCode;
              } else if(s_moveFound) {
                s_bestMove = s_usableMove;
                throw s_stopCode;
              }
            }

            if(bestScore >= beta && depth > 1) {                                                                          // abort on fail high
              goto C;
            }
            int material = capturedMaterial;
            int score    = depth-1 ? e : material-movingPieceType;                                                        // MVV/LVA scoring if depth=1
            if(depth-!capturedPiece > 1) {                                                                                // all captures if depth=2
              score = SEARCHCENTER(movingPieceType) ? FIELDSCORE(move) : 0;                                               // center positional points for all but queen,rook
                                                                                                                          // do move,
              if(capturedPieceType) {
                SQUARE(capturedSquare) = 0;
                enemyMap.remove(capturedSquare);
                if(SEARCHCENTER(capturedPieceType)) {
                  score += BoardConfig::getFieldValue(capturedSquare);
                  switch(capturedPieceType) {
#ifdef USE_KNIGHTBONUS
                  case KNIGHT:
                    score += bc.m_player[enemy>>4].captureKnight(capturedSquare);
                    break;
#endif
                  case WPAWN:
                  case BPAWN:
                    score += BoardConfig::getPawnPushBonus(enemy,capturedSquare);
                    break;
                  }
                }
              }
              SQUARE(FROM) = 0;
              SQUARE(TO  ) = movingPiece | HASMOVED;                                                                      // set HASMOVED bit
              char promoteTo;
              origFromBits = fromRemoved;                                                                                 // playerMap.remove(from);
              playerMap.add(TO);
              switch(movingPieceType) {
              case KING:
                if(!(G&MASK88)) {                                                                                         // castling:
                  SQUARE(G) = 0;
                  SQUARE(F) = ROOK | side;                                                                                // put rook & score.
                  playerMap.remove(G);
                  playerMap.add(F);
                  score += (F&4)?50:30;                                                                                   // 0-0=50, 0-0-0=30
                }
                if(s_bc.m_R <= 30) {                                                                                      // freeze king in mid-game
                  score -= 20;
                }
#ifdef USE_KNIGHTBONUS
                score += bc.m_player[enemy>>4].moveKing(move);
#endif
                break;

#ifdef USE_KNIGHTBONUS
              case KNIGHT:
                score += bc.m_player[side>>4].moveKnight(move);
                break;
#endif
              case WPAWN:
              case BPAWN:
                score -= 9*((((FROM-2)&MASK88)||(SQUARE(FROM-2)-movingPiece))                                             // structure, undefended squares
                           +(((FROM+2)&MASK88)||(SQUARE(FROM+2)-movingPiece))-1                                           // plus bias
                           +(SQUARE(FROM^16)==(side|KING|HASMOVED))                                                       // cling to magnetic king
                           )
                         -(s_bc.m_R>>2);                                                                                  // end-game pawn-push bonus
                if((TO+step+1) & S) {                                                                                     // promotion
#ifdef _DEBUG
                  if(move.d.m_promoteIndex >= ARRAYSIZE(legalPromotions)) {
                    throwException(_T("promoteIndex=%d"), move.d.m_promoteIndex);
                  }
#endif
                  promoteTo = legalPromotions[++move.d.m_promoteIndex];
                  material += BoardConfig::getPieceValue(promoteTo) - 202;                                                // promotion. 202 = value(pawn) + accumulated pawnpush bons = 74 + 128
                  SQUARE(TO) = promoteTo | side | HASMOVED;                                                               // convert pawn
#ifdef USE_KNIGHTBONUS
                  if(promoteTo == KNIGHT) {
                    score += bc.m_player[side>>4].promoteToKnight(TO);
                  }
#endif
                } else {
                  score += 2*(movingPiece&(TO+16)&HASMOVED);                                                              // passer bonus
                  promoteTo = 0;
                }
                break;
              }
              s_bc.m_hashCode.m_index += HASHINDEX(move);
              s_bc.m_hashCode.m_key   += HASHKEY(  move) + G;

                                                                                                                          // move done

              if((request & REQUEST_FINDMOVE) && isDrawByRepetition()) {
                score = 0;
              } else {
                score += e + material;

                const int V = max(bestScore, alfa);                                                                       // new eval & alpha
                int C = depth-1-(!h && !capturedPiece && (depth>5) && !ISPAWN(movingPieceType));                          // new depth, reduce non-capture.
                C = ((s_bc.m_R>30) | (P-MAXSCORE) | (depth<3)) || (capturedPiece && !ISKING(movingPieceType))? C:depth;   // extend 1 ply if in-check
                int s;
                do {
                  s = ((C > 2) || (score > V)) ? -negamax(enemy,-beta,-V,-score,F,REQUEST_LOOKAHEAD,C,tmpLink) : score;   // futility, recursive eval. of reply
                } while((s>alfa) & (++C<depth));
                score = s;                                                                                                // no fail:re-search unreduced
              }

              if((request&REQUEST_FINDMOVE) && s_bestMove.isMove()) {                                                     // move pending: check legal
                if((score+MAXSCORE) && (s_bestMove == move)) {                                                            // if move found
                  s_bc.m_score = -e-material;
                  s_bc.m_EP    = F;
                  a.m_depth    = 99;
                  a.m_score    = 0;                                                                                       // lock game in hash as draw
                  s_bc.m_R     += material>>7;                                                                            // total captured material
#ifdef USE_KNIGHTBONUS
                  bc.ajourKnightBonusTable();
#endif
                  if(capturedPiece || ISPAWN(movingPieceType)) {
                    s_bc.m_fiftyMoves = 0;
                  } else {
                    s_bc.m_fiftyMoves++;
                  }
                  return beta;                                                                                            // & not in check, signal
                }
                score = bestScore;                                                                                        // prevent fail-lows on king-capture replies
              }

              s_bc.m_hashCode          = savedHashCode;                                                                   // undo move
              SQUARE(FROM)             = movingPiece;
              SQUARE(TO)               = 0;
              if(capturedPiece) {
                SQUARE(capturedSquare) = capturedPiece;

#ifdef USE_KNIGHTBONUS
                switch(capturedPieceType) {
                case KNIGHT:
                  bc.m_player[enemy>>4].uncaptureKnight(capturedSquare);
                  break;
                }
#endif
              }
              switch(movingPieceType) {
              case KING:
#ifdef USE_KNIGHTBONUS
                bc.m_player[enemy>>4].unmoveKing(move);
#endif
                if(!(G&MASK88)) {
                  SQUARE(G)            = ROOK | side;
                  SQUARE(F)            = 0;
                }
                break;

#ifdef USE_KNIGHTBONUS
              case KNIGHT:
                bc.m_player[side>>4].unmoveKnight(move);
                break;
              case WPAWN:
              case BPAWN:
                if(promoteTo == KNIGHT) {
                  bc.m_player[side>>4].removeKnight(TO);
                }
                break;
#endif
              }
              s_bc.m_occupationMap = savedOccupationMap;                                                                  // move undone
            }                                                                                                             // if non-castling
            if(score > bestScore) {                                                                                       // new best, update max,best
              bestScore         = score;
              bestMove          = move;
              bestMove.d.m_ybit = (F==S);
              bestLink          = tmpLink;
            }
          } while(move.d.m_promoteIndex && (move.d.m_promoteIndex < ARRAYSIZE(legalPromotions)-1));

          if(h) {
            h = false;
            goto A;                                                                                                       // redo after doing old best
          }
          if((FROM+step-TO) | (movingPiece&HASMOVED) | !ISPAWN(movingPieceType)                                           // not 1st step, moved before, not pawn-move
            &(!ISKING(movingPieceType) | ((step+1)&0xfc)                                                                  // no lateral king move.
            || SQUARE(G = ((FROM+3)^((step>>1)&7)))-(ROOK|side)                                                           // no virgin rook in corner G,
            || SQUARE(G ^ 1) | SQUARE(G ^ 2))                                                                             // no 2 empty squares next to rook
            ) {
            if(ISCRAWLER(movingPieceType)) {
              break;
            }
          } else {
            F = TO;                                                                                                       // enable e.p. or castling
          }
        } while(!capturedPiece);                                                                                          // if not capture continue ray
      }                                                                                                                   // next direction for same piece
    }                                                                                                                     // next piece in iterator, wrap

C:
    bestScore = (bestScore+MAXSCORE) | (P==MAXSCORE) ? bestScore : 0;                                                     // check test thru NM  best loses K: (stale)mate
    if(a.m_depth < 99) {                                                                                                  // protect game history
      a.m_key          = s_bc.m_hashCode.m_key;
      a.m_score        = bestScore;
      a.m_next         = bestLink;
      a.m_depth        = depth;                                                                                           // always store in hash table
      a.m_move         = bestMove;
      a.m_cutAlpha     = bestScore>alfa;
      a.m_cutBeta      = bestScore<beta;
    }
    if(request&REQUEST_FINDMOVE) {
      s_moveFound  = true;
      s_usableMove = bestMove;
      if(isVerbose()) {
        if(depth > 2) {
          static TCHAR varStr[1000];
          reply(_T("info depth %d score %s nodes %u pv %s\n"), depth, getUCIScoreStr(bestScore).cstr(), s_searchStatistic.m_nodeCount, a.getVariantStr(varStr));
        }
      }
    }
  }
  if(request&REQUEST_CHECKMOVE) {
    s_bestMove = bestMove;
  }
  return bestScore + (bestScore<e);                                                                                       // delayed-loss bonus
}
