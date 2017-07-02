#pragma once

#include <MyAssert.h>
#include <Tokenizer.h>
#include "FieldSet.h"
#include "FieldNames.h"

typedef enum {
  WHITEPLAYER
 ,BLACKPLAYER
} Player;

#define NEXTPLAYER(p) ((int&)p)++
#define forEachPlayer(p)  for(Player p = WHITEPLAYER; p <= BLACKPLAYER; NEXTPLAYER(p))

typedef enum {
  WHITEFIELD
 ,BLACKFIELD
} FieldColor;

// Don't modify the order of the elements in this type
typedef enum {
  NoPiece
 ,King
 ,Queen
 ,Rook
 ,Bishop
 ,Knight
 ,Pawn
} PieceType;

// Don't change these without updating MoveTable::isDiagonalMove, and MoveTable::uncoversKing Matrix defined in Move.cpp
// and kingDirToPinnedState defined in BackMoveGenerator
typedef enum {
  MD_NONE // Also used for knight-moves. The knight is the only piece that doesn't move in any of the other directions. Should be the first (=0)
 ,MD_LEFT
 ,MD_RIGHT
 ,MD_DOWN
 ,MD_UP
 ,MD_DOWNDIAG1
 ,MD_UPDIAG1
 ,MD_DOWNDIAG2
 ,MD_UPDIAG2
} MoveDirection;

typedef enum {
  NOMOVE
 ,NORMALMOVE
 ,PROMOTION
 ,ENPASSANT
 ,SHORTCASTLING
 ,LONGCASTLING
} MoveType;

typedef enum { // if modified => check MoveBase::getMaxStringLength
  MOVE_SHORTFORMAT
 ,MOVE_LONGFORMAT
 ,MOVE_FILEFORMAT
 ,MOVE_UCIFORMAT
 ,MOVE_DEBUGFORMAT
} MoveStringFormat;

typedef enum {
  NORESULT
 ,WHITE_CHECKMATE
 ,BLACK_CHECKMATE
 ,STALEMATE
#ifndef TABLEBASE_BUILDER
 ,POSITION_REPEATED_3_TIMES
 ,NO_CAPTURE_OR_PAWNMOVE
#endif
 ,DRAW
} GameResult;

#define GAMERESULTTOWINNER(gs) (((gs)==WHITE_CHECKMATE)?BLACKPLAYER:((gs)==BLACK_CHECKMATE)?WHITEPLAYER:(Player)-1)

typedef enum {
  DEPTH_TO_CONVERSION
 ,DEPTH_TO_MATE
} TablebaseMetric;


typedef enum {
  NOANNOTATION        // empty
 ,BAD_MOVE            // ?
 ,VERYBAD_MOVE        // ??
 ,GOOD_MOVE           // !
 ,EXCELLENT_MOVE      // !!
 ,INTERESTING_MOVE    // !?
 ,DOUBIOUS_MOVE       // ?!
} MoveAnnotation;

typedef enum {
  NORMAL_POSITION
 ,DRAW_POSITION
 ,TABLEBASE_POSITION
} PositionType;

typedef BYTE  CastleState;
typedef BYTE  BishopFlags;
typedef BYTE  AttackAttribute;
typedef BYTE  PinnedState;
typedef BYTE  PieceKey;
typedef BYTE  KingAttackState;
typedef const int     *PositionArray;
typedef BYTE  SymmetricTransformation;

// Dont modify these, without making the corresponding changes in GameKey::castleScore
#define SHORTCASTLE_ALLOWED          0x01
#define LONGCASTLE_ALLOWED           0x02
#define SHORTCASTLE_DONE             0x04
#define LONGCASTLE_DONE              0x08

#define WHITEFIELD_BISHOP            0x01
#define BLACKFIELD_BISHOP            0x02
#define BISHOPPAIR                   (WHITEFIELD_BISHOP | BLACKFIELD_BISHOP)

#define GETENEMY(player) ((Player)((player)^1))

#define ATTACKS_PARALLEL             0x01
#define ATTACKS_UPPERDIAGONAL        0x02
#define ATTACKS_LOWERDIAGONAL        0x04
#define ATTACKS_LONGDISTANCE         0x08
#define ATTACKS_DIAGONAL             (ATTACKS_UPPERDIAGONAL | ATTACKS_LOWERDIAGONAL)

#define NOT_PINNED                      0
#define PINNED_TO_ROW                   1
#define PINNED_TO_COL                   2
#define PINNED_TO_DIAG1                 3
#define PINNED_TO_DIAG2                 4

#define KING_NOT_ATTACKED               0
#define KING_LD_ATTACKED_FROM_ROW    0x01
#define KING_LD_ATTACKED_FROM_COL    0x02
#define KING_LD_ATTACKED_FROM_DIAG1  0x04
#define KING_LD_ATTACKED_FROM_DIAG2  0x08
#define KING_SD_ATTACKED             0x10
#define KING_MANY_ATTACKS            0x20 // Impossible in normal play, but can occur after edit the position, and skip the validation!
                                          // Should be handled as a doublecheck, ie. the only reply is a king-move.
                                          // This is automaticly done in Game::getMoveGenerator. The default case in the switch(m_kingAttackState)
                                          // takes care of this situation, which handles any (legal) combination of the other flags too. fx.
                                          // KING_LD_ATTACKED_FROM_ROW | KING_LD_ATTACKED_FROM_DIAG1, which can occur in normal play as a doublecheck!

#define TRANSFORM_SWAPPLAYERS               1
#define TRANSFORM_ROTATE180                 2
#define TRANSFORM_ROTATERIGHT               3
#define TRANSFORM_ROTATELEFT                4
#define TRANSFORM_MIRRORROW                 5
#define TRANSFORM_MIRRORCOL                 6
#define TRANSFORM_MIRRORDIAG1               7
#define TRANSFORM_MIRRORDIAG2               8

typedef enum {
  LOWERLEFT_SQUARE   // Square bounded by {A1,D1,D4,A4}
 ,LOWERRIGHT_SQUARE  // Square bounded by {E1,H1,H4,E4}
 ,UPPERLEFT_SQUARE   // Square bounded by {A5,D5,D8,A8}
 ,UPPERRIGHT_SQUARE  // Square bounded by {E5,H5,H8,E8}
} QuarterSquare;

#define MAKE_POSITION(row,col)           (((row)<<3)|(col))

#define GETROW(pos)                      ((int)((pos)>>3))
#define GETCOL(pos)                      ((int)((pos)&7))

#define _GET_DIAGONAL1(  row, col)       (7 + (row) - (col))
#define _GET_DIAGONAL2(  row, col)       ((row) + (col))
#define _GETDIAGONAL1(   pos)            _GET_DIAGONAL1(GETROW(pos), GETCOL(pos))
#define _GETDIAGONAL2(   pos)            _GET_DIAGONAL2(GETROW(pos), GETCOL(pos))

#define IS_INLOWERHALF(  pos)            (GETROW(pos) <= GETROW(D4))
#define IS_INUPPERHALF(  pos)            (GETROW(pos) >  GETROW(D4))
#define IS_QUEENSIDE(    pos)            (GETCOL(pos) <= GETCOL(D4))
#define IS_KINGSIDE(     pos)            (GETCOL(pos) >  GETCOL(D4))

#define IS_SAMESIDE(     pos1, pos2)     (IS_QUEENSIDE(pos1) == IS_QUEENSIDE(pos2))

#define _GET_SQUARE(pos)                 (IS_INLOWERHALF(pos) ? (IS_QUEENSIDE(pos) ? LOWERLEFT_SQUARE : LOWERRIGHT_SQUARE) \
                                                              : (IS_QUEENSIDE(pos) ? UPPERLEFT_SQUARE : UPPERRIGHT_SQUARE))

#define _GET_FIELDCOLOR(pos)             (((GETROW(pos) + GETCOL(pos)) % 2) ? WHITEFIELD : BLACKFIELD)

#define _IS_INNERROW(   r)               ((0 < (r)) && ((r) < 7))
#define _IS_INNERCOL(   c)               ((0 < (c)) && ((c) < 7))
#define _IS_INNERFIELD( pos)             ( _IS_INNERROW(GETROW(pos)) &&  _IS_INNERCOL(GETCOL(pos)))
#define _IS_CORNERFIELD(pos)             (!_IS_INNERROW(GETROW(pos)) && !_IS_INNERCOL(GETCOL(pos)))

// The last row for pawn moving forward
#define GETPROMOTEROW(player)            ((player==WHITEPLAYER)?7:0)
#define GETPAWNSTARTROW(player)          ((player==WHITEPLAYER)?1:6)

#define INSIDEBOARD(row, col)            (((UINT)(row) < 8) && ((UINT)(col) < 8))

#define LD_ATTACKS(piece, attr)          ((piece->m_attackAttribute & ((attr)|ATTACKS_LONGDISTANCE)) > ATTACKS_LONGDISTANCE)

#define LONGDISTANCE_ATTACKS( player, piece, attr) ((piece->getPlayer() == player) && LD_ATTACKS(piece, attr))

#define EMPTYPIECEKEY                    ((PieceKey)0)
#define MAKE_PIECEKEY(player, pieceType) ((PieceKey)(((BYTE)(player)<<3) + (BYTE)(pieceType)))
#define GET_PLAYER_FROMKEY(pieceKey)     ((Player)(((pieceKey)>>3)&1))
#define GET_TYPE_FROMKEY(pieceKey)       ((PieceType)((pieceKey)&0x7))

#define WHITEKING   MAKE_PIECEKEY(WHITEPLAYER, King  )
#define WHITEQUEEN  MAKE_PIECEKEY(WHITEPLAYER, Queen )
#define WHITEROOK   MAKE_PIECEKEY(WHITEPLAYER, Rook  )
#define WHITEBISHOP MAKE_PIECEKEY(WHITEPLAYER, Bishop)
#define WHITEKNIGHT MAKE_PIECEKEY(WHITEPLAYER, Knight)
#define WHITEPAWN   MAKE_PIECEKEY(WHITEPLAYER, Pawn  )

#define BLACKKING   MAKE_PIECEKEY(BLACKPLAYER, King  )
#define BLACKQUEEN  MAKE_PIECEKEY(BLACKPLAYER, Queen )
#define BLACKROOK   MAKE_PIECEKEY(BLACKPLAYER, Rook  )
#define BLACKBISHOP MAKE_PIECEKEY(BLACKPLAYER, Bishop)
#define BLACKKNIGHT MAKE_PIECEKEY(BLACKPLAYER, Knight)
#define BLACKPAWN   MAKE_PIECEKEY(BLACKPLAYER, Pawn  )

int pieceTypeCmp(const PieceType &p1, const PieceType &p2);

#define IS_KING(player, piece)            (piece->m_pieceType == King && piece->m_player == player)

#define getFieldColor(position) Game::s_fieldInfo[position].m_fieldColor

// Number of pieces for 1 player of the given PieceType in the startconfiguration
int            getStartPieceCount(                PieceType               pieceType    );

// Language specific name of piece of the given PieceType. First letter uppercase.
// plur=true gives pluralis form of the name
const TCHAR   *getPieceTypeName(                  PieceType               pieceType, bool plur = false);

// Language specific short name of the given PieceType
const TCHAR   *getPieceTypeShortName(             PieceType               pieceType    );
const TCHAR   *getPieceTypeNameEnglish(           PieceType               pieceType, bool plur = false);
 // English short name of the given PieceType. King="K", Queen="Q", Rook="R" etc.
const TCHAR   *getPieceTypeShortNameEnglish(      PieceType               type         );

void           reloadStrings();
// Language specific name of the given Player.       First letter uppercase.
const TCHAR   *getPlayerName(                     Player                  player        );
// Language specific short name of the given Player. Uppercase
const TCHAR   *getPlayerShortName(                Player                  player        );
// WHITEPLAYER="white", BLACKPLAYER="black".         Lowercase
const TCHAR   *getPlayerNameEnglish(              Player                  player        );
// WHITEPLAYER="W", BLACKPLAYER="B".                 Uppercase
const TCHAR   *getPlayerShortNameEnglish(         Player                  player        );
// ex. white king, black pawn, empty...
String         getPieceNameEnglish(               PieceKey                key           );
// a1,a2,..,b1,b2,...,h1,h8
const TCHAR   *getFieldName(                      int                     position      );
// = getFieldName(MAKE_POSITION(row,col))
const TCHAR   *getFieldName(                      int row, int col                      );
// 'a'..'h'
TCHAR          getColumnName(                     int                     position      );
// '1'..'8'
TCHAR          getRowName(                        int                     position      );
// {"white","black"}
const TCHAR   *getFieldColorName(                 FieldColor              color         );
// = getFieldColorName(getFieldColor(position))
const TCHAR   *getFieldColorName(                 int                     position      );
// Converts the bits in bishopFlags to String, "[W][B]". For debug
String         getBishopFlagsToString(            BishopFlags             bishopFlags   );
// "0-0" or "0-0-0". Assume type is SHORTCASTLING or LONGCASTLING
String         getCastleString(                   MoveType                type          );
// Returns a string representing ps. For debug
String         getPinnedStateToString(            PinnedState             ps            );
// Returns a string representing moveType. For debug
String         getMoveTypeName(                   MoveType                moveType      );
MoveDirection  getMoveDirection(                  int from, int to);
// Returns a string representing MoveDirection. For debug
String         getMoveDirectionName(              MoveDirection           direction     );
// Returns a string specifying the fields in the array. For debug
String         getPositionArrayToString(          PositionArray           positions     );
// Converts the bits in attackState to String. For debug
String         getKingAttackStateToString(        KingAttackState         attackState   );
// Returns a string representing the MoveAnnotation. {"","?","??","!","!!","?!","!?"}
const TCHAR   *getAnnotationToString(             MoveAnnotation          annotation    );
String         getGameResultToString(             GameResult              gameResult    );
MoveAnnotation parseAnnotation(                   const TCHAR            *str           );
String         getSymmetricTransformationToString(SymmetricTransformation st            );
// Returns true if position = [0..63]
bool           isValidPosition(                  int                      position      );
// = isValidLine(row) && isValidLine(col)
bool           isValidPosition(                  int row, int col                       );
// Return true if position can be occupied by a pawn
bool           isValidPawnPosition(              int                      position      );
// Returns true if line = [0..7]
bool           isValidLine(                      int line                               );
// throws Exception if not valid.
void           validatePosition(                 const TCHAR *function, int pos         );
// throws Exception if not valid.
void           validatePosition(                 const TCHAR *function, int row, int col);
int            decodePosition(                   const String &s                        );
// assume str at least 2 characters
int            decodePosition(                   const TCHAR  *str                      );
MoveDirection  getOppositeDirection(             MoveDirection direction                );
String         findShortestKnightRoute(          UINT from, UINT to);

#define VALIDATEPOS(pos)          validatePosition(__TFUNCTION__, pos)
#define VALIDATEPOSITION(row,col) validatePosition(__TFUNCTION__, row,col)

#define MMSS_TO_SECONDS(m, s)      (((m) * 60) + (s))
#define HHMMSS_TO_SECONDS(h, m, s) ((h) * 3600 + MMSS_TO_SECONDS(m, s))
#define GET_HOURS(s)               ((int)(( s)/3600))
#define GET_MINUTES(s)             (((int)((s)/60))%60)
#define GET_SECONDS(s)             (( s)%60)

#ifdef TABLEBASE_BUILDER
const FieldSet &getFieldsBetween(                int pos1, int pos2                    );
#else
FieldSet        getFieldsBetween(                int pos1, int pos2                    );
#endif

// throws an Exception
void           invalidPlayerError(               const TCHAR *method, Player player   );
#define INVALIDPLAYERERROR(player) invalidPlayerError(__TFUNCTION__, player)

void           setCurrentLanguage(LANGID langID);
// Return a filename in the resourcedirectory
String         getResourceFileName(              const String &fileName                );
// Return String c:\temp\fileName
String         getTempFileName(                  const String &fileName                );
// Return a temporary filename in c:\temp
String         createTempFileName();

class VerboseReceiver {
public:
  virtual void vprintf(            const TCHAR *format, va_list argptr);
  virtual void vupdateMessageField(const TCHAR *format, va_list argptr);
  virtual void clear();
};

extern VerboseReceiver *VERBOSENULL;

void throwUserException(int id, ...);
// log to stderr and logfile if opened with setVerboseLogging
void verbose(            const TCHAR *format, ...);
void updateMessageField( const TCHAR *format, ...);
// same as verbose, Takes a va_list as argument.            Default writes to stderr
void vverbose(           const TCHAR *format, va_list argptr);
// same as updateMessageField. Takes a va_list as argument. Default writes to stderr
void vupdateMessageField(const TCHAR *format, va_list argptr);
void clearVerbose();

void redirectVerbose(VerboseReceiver *receiver);
// log only to logfile if open
void log(const TCHAR *format, ...);
void vlog(const TCHAR *format, va_list argptr);
// If on is true, everything logged by verbose or log will go to logfile
// which has an automatic generated, unique name
// (template "c:\temp\chess<timestamp>.log")
// If on is false, an open logfil will be closed
void setVerboseLogging(bool on);

class Direction {
public:
  const MoveDirection   m_direction, m_oppositeDirection;
  PositionArray         m_fields;

#ifdef _DEBUG
  const TCHAR           *m_debugStr;
  Direction(MoveDirection direction, PositionArray fields, const TCHAR *debugStr)
  : m_direction(direction)
  , m_oppositeDirection(getOppositeDirection(direction))
  , m_fields(fields)
  , m_debugStr(debugStr)
  {
  }

#else
  Direction(MoveDirection direction, PositionArray fields)
  : m_direction(direction)
  , m_oppositeDirection(getOppositeDirection(direction))
  , m_fields(fields)
  {
  }
#endif
};

class DirectionArray {
public:
  const int             m_count;
  const Direction      *m_directions;
  DirectionArray(int count, const Direction *directions)
  : m_count(count)
  , m_directions(directions)
  {
  }
};

class DoubleLine {
public:
  const bool            m_isBlockable;
  PositionArray         m_lower;       // Against 1st row/A-Column
  PositionArray         m_upper;       // Against 8th row/H-Column
  const AttackAttribute m_attr;
  const int             m_pos;
  const int             m_index;
#ifdef _DEBUG
  const String          m_debugStr;
  static String makeDebugString(int pos, PositionArray lower, PositionArray upper);
#endif
  DoubleLine(bool isBlockable, PositionArray lower, PositionArray upper, AttackAttribute attr, int pos, int index)
    : m_isBlockable(isBlockable)
    , m_lower(lower)
    , m_upper(upper)
    , m_attr(attr)
    , m_pos(pos)
    , m_index(index)
#ifdef _DEBUG
    , m_debugStr(makeDebugString(pos, lower, upper))
#endif
  {
  }
  String toString() const;
};

class FieldInfo {
public:
  const DoubleLine      m_rowLine;
  const DoubleLine      m_colLine;
  const DoubleLine      m_diag1Line;
  const DoubleLine      m_diag2Line;
  PositionArray         m_kingAttacks;                 // Fields attacked by king
  PositionArray         m_knightAttacks;               // Fields attacked by knight
  PositionArray         m_whitePawnAttacks;            // Fields attacked by white pawn
  PositionArray         m_blackPawnAttacks;            // Fields attacked by black pawn
  PositionArray         m_attackingWhitePawnPositions; // Possible positions for an attacking white pawn
  PositionArray         m_attackingBlackPawnPositions; // Possible positions for an attacking black pawn
  const int             m_pos;
  const int             m_row, m_col, m_diag1, m_diag2;
  const bool            m_innerRow, m_innerCol, m_innerField, m_cornerField;
  const FieldColor      m_fieldColor;
  const QuarterSquare   m_square;
  const TCHAR           *m_name;
#ifdef _DEBUG
  const String          m_debugStr;
  static String makeDebugString(int           pos
                               ,const TCHAR   *name
                               ,PositionArray kingAttacks
                               ,PositionArray knightAttacks
                               ,PositionArray whitePawnAttacks
                               ,PositionArray blackPawnAttacks
                               ,PositionArray attackingWhitePawnPositions
                               ,PositionArray attackingBlackPawnPositions);
#endif

// Each PositionArray in the argumentlist to the constructor can be NULL, if no elements in the given direction.
  FieldInfo(int           pos
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
           );
  String toString() const;
};

class PlayerSignature {
private:
  union {
    struct {
      UINT m_kingCount   : 1;
      UINT m_queenCount  : 4;
      UINT m_rookCount   : 4;
      UINT m_bishopCount : 4;
      UINT m_knightCount : 4;
      UINT m_pawnCount   : 4;
    } m_pieceCount;
    UINT m_hashCode;
  };
  PlayerSignature() : m_hashCode(0) {
  }
  void add(PieceType pt);
  friend class PlayerState;
  friend class PositionSignature;
public:
  inline bool operator==(const PlayerSignature &ps) const {
    return m_hashCode == ps.m_hashCode;
  }
  inline bool operator!=(const PlayerSignature &ps) const {
    return m_hashCode != ps.m_hashCode;
  }
  inline bool isEmpty() const {
    return m_hashCode == 0;
  }
  int getPieceCount() const;
  int getPieceCount(PieceType pt) const;
  inline ULONG hashCode() const {
    return m_hashCode;
  }
  String toString(bool longNames) const;

  static TCHAR getPieceShortName(PieceType type);
};

//#define TEST_HASHFACTOR

class PositionSignature {
private:
  PlayerSignature m_playerSignature[2];
  static void invalidArgument(const String &str);
public:
  inline void add(PieceKey pk) {
    m_playerSignature[GET_PLAYER_FROMKEY(pk)].add(GET_TYPE_FROMKEY(pk));
  }
  inline PositionSignature() {}
  PositionSignature(const String &str);
  inline PositionSignature(PlayerSignature ws, PlayerSignature bs) {
    m_playerSignature[WHITEPLAYER] = ws;
    m_playerSignature[BLACKPLAYER] = bs;
  }

  inline PositionSignature swapPlayers() const {
    return PositionSignature(m_playerSignature[BLACKPLAYER],m_playerSignature[WHITEPLAYER]);
  }

  inline bool operator==(const PositionSignature &ps) const {
    return (m_playerSignature[WHITEPLAYER] == ps.m_playerSignature[WHITEPLAYER])
        && (m_playerSignature[BLACKPLAYER] == ps.m_playerSignature[BLACKPLAYER]);
  }
  inline bool operator!=(const PositionSignature &ps) const {
    return (m_playerSignature[WHITEPLAYER] != ps.m_playerSignature[WHITEPLAYER])
        || (m_playerSignature[BLACKPLAYER] != ps.m_playerSignature[BLACKPLAYER]);
  }
  bool match(const PositionSignature &ps, bool &swap) const;

  inline bool isEmpty() const {
    return m_playerSignature[0].isEmpty() || m_playerSignature[1].isEmpty();
  }
  inline int getPieceCount(Player player) const {
    return m_playerSignature[player].getPieceCount();
  }
  inline PlayerSignature getPlayerSignature(Player player) const {
    return m_playerSignature[player];
  }
#ifdef TEST_HASHFACTOR
  static int hashFactor;
  long hashCode() const;
#else
  inline ULONG hashCode() const {
    return (m_playerSignature[WHITEPLAYER].hashCode() * 2716389)
          ^ m_playerSignature[BLACKPLAYER].hashCode();
  }
#endif

  String toString(bool longNames = false) const;
};

class Game;
class GameKey;
class EndGameKeyDefinition;
class Move;

#define DECLARE_COMMON_FRIENDS                     \
  friend class MoveGeneratorNoCheck;               \
  friend class MoveGeneratorCheck;                 \
  friend class MoveGeneratorLDCheck;               \
  friend class MoveGeneratorSDCheck;               \
  friend class MoveGeneratorDoubleCheck;           \
  friend class BackMoveGenerator;                  \
  friend class BackMoveGeneratorNoCheck;           \
  friend class BackMoveGeneratorLDCheck;           \
  friend class BackMoveGeneratorSDCheck;           \
  friend class BackMoveGeneratorLDDoubleCheck;     \
  friend class BackMoveGeneratorSDDoubleCheck;

typedef void (Game::*GameUpdateFunction)(const Move &m);

#define BISHOPPAIRBONUS 20

class MoveTable {
public:
  static const DirectionArray  kingMoves[64];
  static const DirectionArray  queenMoves[64];
  static const DirectionArray  rookMoves[64];
  static const DirectionArray  bishopMoves[64];
  static const DirectionArray  knightMoves[64];
  static const DirectionArray  whitePawnMoves[64];
  static const DirectionArray  blackPawnMoves[64];
  static const bool            uncoversKing[5][9];              // Indexed by PinnedState, MoveDirection
  static const bool            isDiagonalMove[9];               // Indexed by MoveDirection;
  static const int            *knightPositionScore[2];
  static const int            *pawnPositionScore[2];
  static const char            pawnColumnScore[256];            // Indexed by PlayerState.m_pawnMask
};

#define IS_DIAGONALMOVE(direction) MoveTable::isDiagonalMove[direction]

class Piece {
private:
  friend class Game;
  friend class PlayerStateToPush;
  friend class PlayerState;
  friend class EndGameResult;
  friend class EnPassantMove;

  DECLARE_COMMON_FRIENDS

  static const DirectionArray *getMoveTable(       PieceKey key);
  static AttackAttribute       getAttackAttribute( PieceKey key);
  static UINT                  getMaterialValue(   PieceType pieceType);

  const UINT                   m_index;
  PlayerState                 &m_playerState, &m_enemyState;

// Set by setType (initialization or promotion)
  const DirectionArray        *m_moveTable;
  AttackAttribute              m_attackAttribute;
  PieceKey                     m_pieceKey;
  GameUpdateFunction           m_doMove, m_updateWhenCaptured;

#ifndef TABLEBASE_BUILDER
  UINT                         m_materialValue;
  BishopFlags                  m_bishopFlag; // Only used for bishops (={WHITEFIELD_BISHOP,BLACKFIELD_BISHOP} depending on the fieldcolor)
  void initBishopMask(int pos);
#else
  GameUpdateFunction           m_doBackMove;
#endif

  // Set when moved or captured
  UINT                         m_position;
  bool                         m_onBoard;
  // Reference to m_playerState.m_pinnedState[m_index]
  PinnedState                 &m_pinnedState;

// Set by makeLinks
  Piece                       *m_next;

  int getPromoteField() const;
public:
  Piece(PieceType pieceType, PlayerState &playerState, PlayerState &enemyState, int index, int row, int col);
  void setType(PieceType pieceType);
  void setType(PieceType pieceType, int pos);
  inline PieceKey getKey() const {
    return m_pieceKey;
  }
  inline PieceType getType() const {
    return GET_TYPE_FROMKEY(m_pieceKey);
  }
  inline Player getPlayer() const {
    return GET_PLAYER_FROMKEY(m_pieceKey);
  }
  inline Player getEnemy() const {
    return GETENEMY(getPlayer());
  }
  inline int getPosition() const {
    return m_position;
  }
  inline bool isOnBoard() const {
    return m_onBoard;
  }
  inline PinnedState getPinnedState() const {
    return m_pinnedState;
  }
  bool isPromotedPawn() const;
  inline const PlayerState &getState() const {
    return m_playerState;
  }
  inline const Piece *getNext() const {
    return m_next;
  }

  String getName()              const;
  const TCHAR *getEnglishName()  const;
  String getShortName()         const;
  String toString()             const;
  String toStringEnglish()      const;
};

// No constructors here, for speed
class MoveBase {
private:
  String toSimpleFormat(bool uciFormat) const;
protected:
  static const String s_nomoveString;
  inline String toStringFileFormat()   const {
    return toSimpleFormat(false);
  }
  inline String toStringUCIFormat() const {
    return toSimpleFormat(true);
  }
public:
  MoveType       m_type;
  int            m_from;
  int            m_to;
  int            m_dirIndex;
  int            m_moveIndex;
  int            m_promoteIndex; // Pawns only. Index into Game::s_legalPromotions. [0..3]
  MoveDirection  m_direction;
  MoveAnnotation m_annotation;

  inline MoveType getType() const {
    return m_type;
  }
  inline bool isMove() const {
    return m_type != NOMOVE;
  }
  inline bool isCastling() const {
    return (m_type == SHORTCASTLING) || (m_type == LONGCASTLING);
  }
  inline int getFrom() const {
    return m_from;
  }
  inline int getTo() const {
    return m_to;
  }
  inline int getCaptureEPSquare() const {
    return m_to^8;
  }
  inline MoveDirection getDirection() {
    return m_direction;
  }
  inline MoveAnnotation getAnnotation() const {
    return m_annotation;
  }
  inline bool hasAnnotation() const {
    return m_annotation != NOANNOTATION;
  }
  inline void setAnnotation(MoveAnnotation annotation) {
    m_annotation = annotation;
  }
  inline void clearAnnotation() {
    m_annotation = NOANNOTATION;
  }
  bool isGoodMove() const;
  PieceType getPromoteTo() const;
  // Return *this
  MoveBase &setNoMove();

#ifdef TABLEBASE_BUILDER
  MoveBase &swapFromTo();
#endif

  String toString() const; // FILEFORMAT
  static int getMaxStringLength(MoveStringFormat mf);
  // compare only from,to,promoteIndex, not annotations
  bool operator==(const MoveBase &m) const;
  bool operator!=(const MoveBase &m) const; // do
};

typedef CompactArray<MoveBase> MoveBaseArray;

class Move : public MoveBase { // No constructors here, for speed
private:
  String toStringLongFormat()  const;
  String toStringDebugFormat() const;
public:
  Piece *m_piece;
  Piece *m_capturedPiece;
  Move &setNoMove();                                            // Return *this
  // Do not accept SHORTFORMAT
  String toString(MoveStringFormat mf = MOVE_LONGFORMAT) const;
  inline PieceType getPieceType() const {
    return m_piece ? m_piece->getType() : NoPiece;
  }
  inline PieceType getCapturedPieceType() const {
    return m_capturedPiece ? m_capturedPiece->getType() : NoPiece;
  }
#ifdef TABLEBASE_BUILDER
  Move makeBackMove() const;
#endif
};

class EnPassantMove {
public:
  const Move      &m_move;
  const int        m_capturedPosition;
  const FieldInfo &m_fromInfo;
  const FieldInfo &m_toInfo;
  const FieldInfo &m_capturedInfo;
  EnPassantMove(const Move &m);
};

class PrintableMove : public MoveBase { // Contains no pointers
private:
  Player          m_player;
  PieceType       m_pieceType;
  PieceType       m_capturedPieceType;
  String          m_uniqueString;
  KingAttackState m_kingAttackState;
  bool            m_checkMate;

  void  findUniqueString(   const Game &game);
  void  findKingAttackState(      Game &game, const Move &m);
  const TCHAR *getCheckString() const;
  String toStringShortFormat() const;
  String toStringLongFormat()  const;
  String toStringDebugFormat() const;

public:
  PrintableMove() {
    setNoMove();
  }
  PrintableMove(const Game &game, const MoveBase &m);
//  PrintableMove(const Game &game, const String   &uciString);
  PrintableMove &setNoMove();
  String toString(MoveStringFormat mf = MOVE_SHORTFORMAT) const;
  Player getPlayer() const {
    return m_player;
  }
  bool operator==(const PrintableMove &m) const; // compares all fields
  bool operator!=(const PrintableMove &m) const {
    return !(*this == m);
  }
};

class GameHistory : public Array<PrintableMove> {
public:
  String toString(MoveStringFormat mf = MOVE_SHORTFORMAT, int width = -1) const;
};

typedef struct {
  // 1 bit for each of the 8 directions is required, to indicate if the field is attacked by Queen, Rook or Bishop,
  // Then this structure requires 4 + 8*1 = 12 bits, which can easily be contained in an unsigned long (32 bits).
  // Then the check for zero attacks is quite easy. See class FieldAttacks.
  UINT           m_sdAttacks      : 4;  // (S)hort (D)istance attacks. ie from King, Knight or Pawn.
                                        // A bizare situation with 8 knights, 2 pawns and the king attacking the same field
                                        // gives a maximum of 11 short distance attacks on one field.
                                        // With 4 bits we can count to 15
  UINT           m_fromLeft       : 1;  // Queen, Rook
  UINT           m_fromRight      : 1;  // Queen, Rook
  UINT           m_fromBelove     : 1;  // Queen, Rook
  UINT           m_fromAbove      : 1;  // Queen, Rook
  UINT           m_fromLowerDiag1 : 1;  // Queen, Bishop
  UINT           m_fromUpperDiag1 : 1;  // Queen, Bishop
  UINT           m_fromLowerDiag2 : 1;  // Queen, Bishop
  UINT           m_fromUpperDiag2 : 1;  // Queen, Bishop
  MoveDirection  m_kingDirection  : 5;  // Is king directly left for, right for,... the given field
} AttackInfo;

typedef struct {
  UINT           m_sdAttacked     : 4;  // Same bits as AttackInfo.m_sdAttacks
  UINT           m_rowAttacked    : 2;  // Same bits as AttackInfo.m_fromLeft and AttackInfo.m_fromRight. if != 0 one of these is != 0
  UINT           m_colAttacked    : 2;  // Same bits as AttackInfo.m_fromBelove and AttackInfo.m_fromAbove.
  UINT           m_diag1Attacked  : 2;  // Same bits as AttackInfo.m_fromLowerDiag1 and AttackInfo.m_fromUpperDiag1
  UINT           m_diag2Attacked  : 2;  // Same bits as AttackInfo.m_fromLowerDiag2 and AttackInfo.m_fromUpperDiag2
  MoveDirection  m_kingDirection  : 5;
} AttackDirectionInfo1;

typedef struct {
  UINT           m_sdAttacked     : 4;  // Same bits as AttackDirectionInfo1.m_sdAttacks
  UINT           m_rcAttacked     : 4;  // Same bits as AttackDirectionInfo1.m_rowAttacked   and AttackDirectionInfo1.m_colAttacked
  UINT           m_diagAttacked   : 4;  // Same bits as AttackDirectionInfo1.m_diag1Attacked and AttackDirectionInfo1.m_diag2Attacked
  MoveDirection  m_kingDirection  : 5;
} AttackDirectionInfo2;

typedef struct {
  UINT           m_sdAttacked     : 4;  // Same bits as AttackInfo.m_sdAttacks
  UINT           m_ldAttacked     : 8;  // Same bits as all LD-attacking bits. if != 0 one of these is != 0
  MoveDirection  m_kingDirection  : 5;
} AttackTypeInfo;

class FieldAttacks {
public:
  union {
    AttackInfo           m_attackInfo;
    AttackDirectionInfo1 m_attackDirectionInfo1;
    AttackDirectionInfo2 m_attackDirectionInfo2;
    AttackTypeInfo       m_attackTypeInfo;
    ULONG                m_isAttacked : 12;     // Dont interpret this as a count of attacking pieces. Only zero check is valid
  };
  bool isLDAttackedFromDir(MoveDirection direction) const;
  inline bool isRCAttacked() const {
    return m_attackDirectionInfo2.m_rcAttacked != 0;
  }
  inline bool isDiagAttacked() const {
    return m_attackDirectionInfo2.m_diagAttacked != 0;
  }
};

class GameKey {
private:
  static GameKey s_startupPosition;
  CastleState findCastleState(Player player) const;

  // Change black <-> white pieces, and mirror row numbers.
  GameKey swapPlayers()   const;
  // Rotate positions for every piece 180 degrees around the middle
  GameKey rotate180()     const;
  // Rotate positions for every piece 90 degrees clockwise around middle
  GameKey rotateRight()   const;
  // Rotate positions for every piece 90 degrees counterclockwise around middle
  GameKey rotateLeft()    const;
  // Mirror rows for ervery piece   (new row = 7 - oldrow)
  GameKey mirrorRows()    const;
  // Mirror columns for every piece (new column = 7 - oldcolumn).
  GameKey mirrorColumns() const;
  // Mirror positions for every piece in A1-H8-diagonal. Be careful if pawns on the board
  GameKey mirrorDiag1()   const;
  // Mirror positions for every piece in H1-A8-diagonal. Be careful if pawns on the board
  GameKey mirrorDiag2()   const;
  // Return piece positions in FEN-format
  TCHAR *getFENBoardString( TCHAR *dst) const;
  TCHAR *getFENCastleString(TCHAR *dst) const;
  TCHAR *getFENEpString(    TCHAR *dst) const;
public:
  // Indexed by m_castleState
  static const char    s_castleScore[16];
  static void validateSize();

  union {
    struct {
      char          m_playerInTurn;
      CastleState   m_castleState[2];                         // Any combination of SHORTCASTLE_ALLOWED,LONGCASTLE_ALLOWED,SHORTCASTLE_DONE,LONGCASTLE_DONE (=[0..15])
      char          m_EPSquare;                               // Position of a pawn, if it's just made a 2-step move. else -1
    } d;
    int m_u;
  };
  PieceKey      m_pieceKey[64];
                                                              // Pointed to by PlayerState.m_castleState
  void clear(Player playerInTurn);
  void clearCastleState();
  inline void clearEPSquare() {
    d.m_EPSquare = -1;
  }

  void initCastleState();

  inline Player getPlayerInTurn() const {
    return (Player)d.m_playerInTurn;
  }
  inline int getEPSquare() const {
    return d.m_EPSquare;
  }

  ULONG hashCode() const;

  // Return key obtained by doing the specified transformations on this
  GameKey transform(       SymmetricTransformation st) const;
  static int transform(    int pos, SymmetricTransformation st);
  PositionSignature getPositionSignature() const;

  String toString() const;
  // Forsyth-Edwards Notation
  String toFENString(int plyCountWithoutCaptureOrPawnMove=0, int moveNumber=1) const;
  static const GameKey &getStartUpPosition();
};

#define PLAYERINTURN m_gameKey.getPlayerInTurn()
#define CURRENTENEMY (Player)(((int)PLAYERINTURN)^1)

bool operator==(const GameKey &key1, const GameKey &key2);
bool operator!=(const GameKey &key1, const GameKey &key2);

#ifndef TABLEBASE_BUILDER

typedef int (PlayerState::*ScoreEvaluator)() const;

class GameKeyHashElement : public GameKey {
private:
  friend class GameKeyHashStack;
  UINT                m_count;
  GameKeyHashElement *m_next, **m_prev;
  GameKeyHashElement() {
    m_count = 1;
  }
};

class GameKeyHashStack {
private:
  UINT                 m_hashCapacity;
  UINT                 m_stackCapacity;
  UINT                 m_stackTop;
  GameKeyHashElement **m_hashTable;
  GameKeyHashElement **m_stack;
  GameKeyHashElement  *m_freeList;

  void init(UINT stackCapacity = 100);
  void cleanUp();
  void initFreeList();
  void deleteFreeList();
  void expandStack();
  GameKeyHashElement *fetchElement();
public:
  GameKeyHashStack();
  GameKeyHashStack(const GameKeyHashStack &src);
  GameKeyHashStack &operator=(const GameKeyHashStack &src);
  ~GameKeyHashStack();
  void clear();
  int push(const GameKey &gameKey);          // Returns the number of times this position has occurred during the game, with the same player in turn
                                             // If == 3 => draw by repetition
  void pop();
  bool isEmpty() const {
    return m_stackTop == 0;
  }
  int getRepeatCount() const;                // Return isEmpty() ? 0 : top.count
};

#endif

class AttackSetInfo {
public:
  BYTE     m_pos;
  FieldSet m_leftRow                    , m_rightRow;
  FieldSet m_lowerCol                   , m_upperCol;
  FieldSet m_lowerDiag1                 , m_upperDiag1;
  FieldSet m_lowerDiag2                 , m_upperDiag2;
  FieldSet m_kingAttacks                , m_knightAttacks;
  FieldSet m_pawnAttacks[2];
//FieldSet m_attackingWhitePawnPositions, m_attackingBlackPawnPositions;
  AttackSetInfo() {}
  AttackSetInfo(const FieldInfo &info);
  void dump() const;
  static void initArray();
  static AttackSetInfo s_bitBoards[64];
};

class PlayerStateToPush {
public:
  FieldAttacks       m_attackTable[64];       // Number of attacking pieces owned by m_player
  PinnedState        m_pinnedState[16];       // One for each piece. Each piece has a reference to the corresponding element in this array.
  int                m_checkingSDAPosition;   // Only valid for enemyState.m_kingAttackState == KING_SD_ATTACKED (Knight or Pawn)
  KingAttackState    m_kingAttackState;       // Any combination of KING_NOT_ATTACKED,KING_ATTACKED_FROM_ROW,-COLUMN,-DIAG1,-DIAG2,-KNIGHT and KING_MANY_ATTACKS

#ifndef TABLEBASE_BUILDER
  int                m_totalMaterial;         // Sum of m_materialValue of not captured pieces belonging to player
  int                m_positionalScore;       // Sum of positional scores for not captured pieces belonging to player
  BishopFlags        m_bishopFlags;           // Any combination of WHITEFIELD_BISHOP and BLACKFIELD_BISHOP
  BYTE               m_pawnMask;              // 1 bit for each pawn. bit 0=A-pawn, bit 1=B-pawn, etc.
  BYTE               m_doublePawnMask;        // Bit c is set if m_pawnCount[c] > 1
  BYTE               m_pawnCount[8];          // Indexed by column. Number of pawns in each column

  void incrementPawnCount(int col);
  void decrementPawnCount(int col);
  String pawnCountToString() const;
  String bishopFlagsToString() const;
#endif
};

class GameStackElement : public Move {
public:
  PlayerStateToPush  m_savedState[2];         // One for each player
  GameKey            m_gameKey;
#ifndef TABLEBASE_BUILDER
  UINT               m_lastCaptureOrPawnMove;
#endif
};

class PlayerState : public PlayerStateToPush {
  friend class Game;

private:
#ifndef TABLEBASE_BUILDER
  int evaluateNormalPlay()                   const;
  int evaluateNormalPlayWithoutCastleScore() const;
  int evaluateEndGame()                      const;
  int evaluateKingSeachMiddle()              const;
  int noEvaluation()                         const; // throws an Exception if called.
  int getFreePawnCount()                     const;

  ScoreEvaluator getScoreFunction() const;
  void initPawnCount();
#endif
  int getPromotionCount() const;
  void allocatePieceTable(Player player, PlayerState &enemyState, const CastleState *castleState);
  bool hasOpposition() const;

public:
  Player             m_player;
  Piece             *m_pieces[16];
  Piece             *m_first;                      // Linked list of all pieces on the board
  Piece             *m_king;                       // For direct access to the king.
  const CastleState *m_castleState;                // Pointer to m_gameKey.m_castleState[m_player] of the game we belong to
  const PlayerState *m_enemyState;

  PlayerState();
  ~PlayerState();

  PlayerState &operator=(const PlayerState &src);
  PlayerState &makeLinks();                  // Return *this
  void         clear();
  void         validatePromotionCount() const;
  bool         isPromotedPawn(const Piece *piece) const;
  CompactArray<PieceKey> getCapturedPieces() const;
  void         resetCapturedPieceTypes();
  void         validateAddPieceAtPosition( PieceType pieceType, int pos, bool validatePromotions) const; // throw Exception if not allowed
  static int   getStartPieceCount(   PieceType pieceType);
  int          getPieceCountOnBoard( PieceType pieceType) const;
  int          getPieceCount(        PieceType pieceType) const;
  int          getPieceCountOnBoard() const;
  int          getBishopCountOnBoard(FieldColor color) const;
  int          getNonPawnCountOnBoard() const;
  PlayerSignature getPlayerSignature() const;
  Piece       *findUnusedPiece(PieceType pieceType);
  const Piece *getNonKingPieceOnBoardByIndex(int n) const;                      // Returns the n'th non-king piece on board. n=[0..15]
  void         setKingAttackState();
#ifdef TABLEBASE_BUILDER
  bool         setKingAttackStateBackMove();
#endif
};

class MoveGenerator {
protected:
  const Game &m_game;
public:
  MoveGenerator(const Game &game) : m_game(game) {
  }
  virtual ~MoveGenerator() {
  }
  virtual bool firstMove(Move &m) const = 0;
  virtual bool nextMove( Move &m) const = 0;
};

class Game {
  friend class PlayerState;

  DECLARE_COMMON_FRIENDS

private:
  String                      m_name;
  GameStackElement           *m_gameStack;
  UINT                        m_stackCapacity;
  UINT                        m_stackSize;
  PlayerState                 m_playerState[2];
  Piece                      *m_board[64];
  GameKey                     m_gameKey;
  GameResult                  m_gameResult;
  MoveGenerator              *m_moveGenerator[4];
  bool                        m_setupMode;

#ifndef TABLEBASE_BUILDER
  UINT                        m_maxPliesWithoutCaptureOrPawnMove;
  UINT                        m_lastCaptureOrPawnMove;
  UINT                        m_maxPositionRepeat;
  GameKeyHashStack            m_hashStack;
  ScoreEvaluator              m_whiteScore, m_blackScore;

  void parseUCI(const String &line);
  void parseFEN(Tokenizer &tok); // tok should contain a valid FEN-string

#else
// These are only used when generating retrograd moves
  MoveGenerator              *m_backMoveGenerator[6];
  const EndGameKeyDefinition *m_keydef;
  bool                        m_swapPlayers;
  bool                        m_generateFictivePawnCaptures; // if true, move-generators will generate diagonal pawn moves without capture
                                                             // Used in generateAllPositions in EndGameTableBase.

  void updateGameBackMoveKing(           const Move &m);
  void updateGameBackMoveQueen(          const Move &m);
  void updateGameBackMoveRook(           const Move &m);
  void updateGameBackMoveBishop(         const Move &m);
  void updateGameBackMoveKnight(         const Move &m);
  void updateGameBackMovePawn(           const Move &m);
  void whitePawnWalkBackward(            const FieldInfo &toInfo, int from);
  void blackPawnWalkBackward(            const FieldInfo &toInfo, int from);
  void setKingBackMove(                  const Move &m);
  void allocateBackMoveGenerators();
  void deallocateBackMoveGenerators();
#endif

  void updateGameMoveKing(               const Move &m);
  void updateGameMoveQueen(              const Move &m);
  void updateGameMoveRook(               const Move &m);
  void updateGameMoveBishop(             const Move &m);
  void updateGameMoveKnight(             const Move &m);
  void updateGameMovePawn(               const Move &m);

  void updateGameCaptureKing(            const Move &m);
  void updateGameCaptureQueen(           const Move &m);
  void updateGameCaptureRook(            const Move &m);
  void updateGameCaptureBishop(          const Move &m);
  void updateGameCaptureKnight(          const Move &m);
  void updateGameCapturePawn(            const Move &m);

  void updateGameNormalKingMove(         const Move &m);
  void updateGameShortCastling();
  void updateGameLongCastling();

  void updateGameNormalPawnMove(         const Move &m);
  void updateGamePromotion(              const Move &m);
  void updateGameEP(                     const Move &m);

  void kingLeaveField(                   const Move &m);
  void queenLeaveField(                  const Move &m);
  void rookLeaveField(                   const Move &m);
  void bishopLeaveField(                 const Move &m);
  void knightLeaveField(                 const Move &m);
  void pawnLeaveField(                   const Move &m);
  void setNonCapturingKing(              const Move &m);
  void setNonCapturingQueen(             const Move &m);
  void setNonCapturingRook(              const Move &m);
  void setNonCapturingBishop(            const Move &m);
  void setNonCapturingKnight(            const Move &m);
  void setPawn(                          const Move &m);
  void setCapturingKing(                 const Move &m);
  void setCapturingQueen(                const Move &m);
  void setCapturingRook(                 const Move &m);
  void setCapturingBishop(               const Move &m);
  void setNonCapturingPromotedLDA(       const Move &m, PieceType type); // type = { Queen, Rook, bishop }
  void setCapturingPromotedLDA(          const Move &m, PieceType type); // type = { Queen, Rook, bishop }
  void setNonCapturingPromotedKnight(    const Move &m);
  void setCapturingPromotedKnight(       const Move &m);

  void undoShortCastling();
  void undoLongCastling();
  void undoEnpassant(                    const Move &m);
  void undoPromotion(                    const Move &m);

  void blockRow(                         const FieldInfo &fieldInfo);    // enter row           (not queen/rook        )
  void blockColumn(                      const FieldInfo &fieldInfo);    // enter column        (not queen/rook        )
  void blockDiag1(                       const FieldInfo &fieldInfo);    // enter diagonal      (not queen/bishop/pawn )
  void blockDiag2(                       const FieldInfo &fieldInfo);    // enter diagona2      (not queen/bishop/pawn )
  void blockDiag12(                      const FieldInfo &fieldInfo);    // block both diagonals. ie { blockDiag1(f); blockDiag2(f); }

  void unblockRow(                       const FieldInfo &fieldInfo);    // leave row           (not queen/rook        )
  void unblockColumn(                    const FieldInfo &fieldInfo);    // leave column        (not queen/rook        )
  void unblockDiag1(                     const FieldInfo &fieldInfo);    // leave diagonal1     (not queen/bishop/pawn )
  void unblockDiag2(                     const FieldInfo &fieldInfo);    // leave diagonal2     (not queen/bishop/pawn )
  void unblockDiag12(                    const FieldInfo &fieldInfo);    // unblock both diagonals. ie { unblockDiag1(f); unblockDiag2(f); }
  void unblockRowEP(                     const FieldInfo &leftInfo, const FieldInfo &rightInfo);

  void kingLeaveRow(                     const FieldInfo &fieldInfo);
  void kingLeaveColumn(                  const FieldInfo &fieldInfo);
  void kingLeaveRC(                      const FieldInfo &fieldInfo);
  void kingLeaveDiag1(                   const FieldInfo &fieldInfo);
  void kingLeaveDiag2(                   const FieldInfo &fieldInfo);
  void kingLeaveDiag12(                  const FieldInfo &fieldInfo);
  void kingEnterRow(                     const FieldInfo &fieldInfo);
  void kingEnterColumn(                  const FieldInfo &fieldInfo);
  void kingEnterRC(                      const FieldInfo &fieldInfo);
  void kingEnterDiag1(                   const FieldInfo &fieldInfo);
  void kingEnterDiag2(                   const FieldInfo &fieldInfo);
  void kingEnterDiag12(                  const FieldInfo &fieldInfo);
  void kingEnterRowCapture(              const FieldInfo &fieldInfo);
  void kingEnterColumnCapture(           const FieldInfo &fieldInfo);
  void kingEnterRCCapture(               const FieldInfo &fieldInfo);
  void kingEnterDiag1Capture(            const FieldInfo &fieldInfo);
  void kingEnterDiag2Capture(            const FieldInfo &fieldInfo);
  void kingEnterDiag12Capture(           const FieldInfo &fieldInfo);
  void setKingLeft(                      PlayerState &state, const FieldInfo &fieldInfo);
  void setKingRight(                     PlayerState &state, const FieldInfo &fieldInfo);
  void setKingDown(                      PlayerState &state, const FieldInfo &fieldInfo);
  void setKingUp(                        PlayerState &state, const FieldInfo &fieldInfo);
  void setKingDownDiag1(                 PlayerState &state, const FieldInfo &fieldInfo);
  void setKingUpDiag1(                   PlayerState &state, const FieldInfo &fieldInfo);
  void setKingDownDiag2(                 PlayerState &state, const FieldInfo &fieldInfo);
  void setKingUpDiag2(                   PlayerState &state, const FieldInfo &fieldInfo);
  void updateKingDir(                    const Move &m);
  void updateKingDirKingMove(            const Move &m);
  void updateKingDirMovingLeft(          const Move &m);
  void updateKingDirMovingRight(         const Move &m);
  void updateKingDirMovingDown(          const Move &m);
  void updateKingDirMovingUp(            const Move &m);
  void updateKingDirMovingDownDiag1(     const Move &m);
  void updateKingDirMovingUpDiag1(       const Move &m);
  void updateKingDirMovingDownDiag2(     const Move &m);
  void updateKingDirMovingUpDiag2(       const Move &m);
  void updateKingDirMovingKnight(        const Move &m);
  void updateKingDirKingMovingLeft(      const Move &m);
  void updateKingDirKingMovingRight(     const Move &m);
  void updateKingDirKingMovingDown(      const Move &m);
  void updateKingDirKingMovingUp(        const Move &m);
  void updateKingDirKingMovingDownDiag1( const Move &m);
  void updateKingDirKingMovingUpDiag1(   const Move &m);
  void updateKingDirKingMovingDownDiag2( const Move &m);
  void updateKingDirKingMovingUpDiag2(   const Move &m);
  void updateKingDirEPDownDiag1(         const EnPassantMove &m);
  void updateKingDirEPUpDiag1(           const EnPassantMove &m);
  void updateKingDirEPDownDiag2(         const EnPassantMove &m);
  void updateKingDirEPUpDiag2(           const EnPassantMove &m);

  void LDAenterRow(                      const FieldInfo &fieldInfo);           // enter row            (queen/rook    )
  void LDAenterColumn(                   const FieldInfo &fieldInfo);           // enter column         (queen/rook    )
  void LDAenterRC(                       const FieldInfo &fieldInfo);           // enter row/column. ie { LDAenterRow(f); LDAenterColumn(f); }
  void LDAenterDiag1(                    const FieldInfo &fieldInfo);           // enter diagonal       (queen/bishop  )
  void LDAenterDiag2(                    const FieldInfo &fieldInfo);           // enter diagona2       (queen/bishop  )
  void LDAenterDiag12(                   const FieldInfo &fieldInfo);           // enter both diagonals. ie { LDAenterDiag1(f); LDAenterDiag2(f); }

  void LDAleaveRow(                      const FieldInfo &fieldInfo);           // leave row            (queen/rook    )
  void LDAleaveColumn(                   const FieldInfo &fieldInfo);           // leave column         (queen/rook    )
  void LDAleaveRC(                       const FieldInfo &fieldInfo);           // leave row/column. ie { LDAleaveRow(f); LDAleaveColumn(f); }
  void LDAleaveDiag1(                    const FieldInfo &fieldInfo);           // leave diagonal       (queen/bishop  )
  void LDAleaveDiag2(                    const FieldInfo &fieldInfo);           // leave diagona2       (queen/bishop  )
  void LDAleaveDiag12(                   const FieldInfo &fieldInfo);           // leave both diagonals. ie { LDAleaveDiag1(f); LDAleaveDiag2(f); }

  void LDAenterRowCapture(               const FieldInfo &fieldInfo);           // enter row            (queen/rook    ) (capturing a piece)
  void LDAenterColumnCapture(            const FieldInfo &fieldInfo);           // enter column         (queen/rook    ) (capturing a piece)
  void LDAenterRCCapture(                const FieldInfo &fieldInfo);           // enter row/column     (queen/rook    ) (capturing a piece)
  void LDAenterDiag1Capture(             const FieldInfo &fieldInfo);           // enter diagonal1      (queen/bishop  ) (capturing a piece)
  void LDAenterDiag2Capture(             const FieldInfo &fieldInfo);           // enter diagonal2      (queen/bishop  ) (capturing a piece)
  void LDAenterDiag12Capture(            const FieldInfo &fieldInfo);           // enter both diagonals (queen/bishop  ) (capturing a piece)

  void LDAwalkLeft(                      const FieldInfo &fieldInfo, int from); // walk left            (queen/rook    )
  void LDAwalkRight(                     const FieldInfo &fieldInfo, int from); // walk right           (queen/rook    )
  void LDAwalkDown(                      const FieldInfo &fieldInfo, int from); // walk down column     (queen/rook    )
  void LDAwalkUp(                        const FieldInfo &fieldInfo, int from); // walk up   column     (queen/rook    )
  void LDAwalkDownDiag1(                 const FieldInfo &fieldInfo, int from); // walk down diagonal1  (queen/bishop  )
  void LDAwalkUpDiag1(                   const FieldInfo &fieldInfo, int from); // walk up   diagonal1  (queen/bishop  )
  void LDAwalkDownDiag2(                 const FieldInfo &fieldInfo, int from); // walk down diagonal2  (queen/bishop  )
  void LDAwalkUpDiag2(                   const FieldInfo &fieldInfo, int from); // walk up   diagonal2  (queen/bishop  )

  void LDAwalkLeftCapture(               const FieldInfo &fieldInfo, int from); // walk left            (queen/rook    ) (capturing a piece)
  void LDAwalkRightCapture(              const FieldInfo &fieldInfo, int from); // walk right           (queen/rook    ) (capturing a piece)
  void LDAwalkDownCapture(               const FieldInfo &fieldInfo, int from); // walk down column     (queen/rook    ) (capturing a piece)
  void LDAwalkUpCapture(                 const FieldInfo &fieldInfo, int from); // walk up   column     (queen/rook    ) (capturing a piece)
  void LDAwalkDownDiag1Capture(          const FieldInfo &fieldInfo, int from); // walk down diagonal1  (queen/bishop  ) (capturing a piece)
  void LDAwalkUpDiag1Capture(            const FieldInfo &fieldInfo, int from); // walk up   diagonal1  (queen/bishop  ) (capturing a piece)
  void LDAwalkDownDiag2Capture(          const FieldInfo &fieldInfo, int from); // walk down diagonal2  (queen/bishop  ) (capturing a piece)
  void LDAwalkUpDiag2Capture(            const FieldInfo &fieldInfo, int from); // walk up   diagonal2  (queen/bishop  ) (capturing a piece)

  void whitePawnWalkForward(             const FieldInfo &fieldInfo, int from);
  void blackPawnWalkForward(             const FieldInfo &fieldInfo, int from);
  void pawnWalkAndPromote(               const Move &m);
  void pawnCaptureAndPromote(            const Move &m);

  void clearLeft(                        PlayerState &state, const FieldInfo &fieldInfo);
  void clearRight(                       PlayerState &state, const FieldInfo &fieldInfo);
  void clearDown(                        PlayerState &state, const FieldInfo &fieldInfo);
  void clearUp(                          PlayerState &state, const FieldInfo &fieldInfo);
  void clearDownDiag1(                   PlayerState &state, const FieldInfo &fieldInfo);
  void clearUpDiag1(                     PlayerState &state, const FieldInfo &fieldInfo);
  void clearDownDiag2(                   PlayerState &state, const FieldInfo &fieldInfo);
  void clearUpDiag2(                     PlayerState &state, const FieldInfo &fieldInfo);

  void setLeft(                          PlayerState &state, const FieldInfo &fieldInfo);
  void setRight(                         PlayerState &state, const FieldInfo &fieldInfo);
  void setDown(                          PlayerState &state, const FieldInfo &fieldInfo);
  void setUp(                            PlayerState &state, const FieldInfo &fieldInfo);
  void setDownDiag1(                     PlayerState &state, const FieldInfo &fieldInfo);
  void setUpDiag1(                       PlayerState &state, const FieldInfo &fieldInfo);
  void setDownDiag2(                     PlayerState &state, const FieldInfo &fieldInfo);
  void setUpDiag2(                       PlayerState &state, const FieldInfo &fieldInfo);

  PinnedState getPinnedState(int pos);
  void capturePiece(const Move &m);

  bool isShortCastleAllowed() const;
  bool isLongCastleAllowed()  const;

  bool uncoversKingEP(const Piece *pawn, const int to) const; // Return true if king will be attacked, (move = En passant)
  bool hasEnemyPawnBeside(int to) const;                      // Used to check if en passant is possible. assuming a 2-step pawn move

  Game &setupStartPosition();
  void initAttackCounters( bool validate);
  void initPinnedState();
  void initGameKey();
  void initKingDirections();
  void setCastleState(Player player, CastleState state);
  void setGameResult();

  PinnedState findPinnedState(const Piece *piece) const;

        Piece *findUnusedPiece(PieceType pieceType, Player player);
  const Piece *findFirstPieceInDirection(PositionArray dir) const;
  const Piece *findFirstPieceInDirection(int pos, MoveDirection dir) const;
  const Piece *findLDAttackingPiece(Player player, int pos, bool diagonalAttack) const;

  void parsePiecePositions(       Player player, Tokenizer &tok);
  void parseHistory(              Tokenizer &tok);
  void parseMove(                 const String &str);
  int  getKnightAttackCount(       Player player, int pos) const;
  int  getPawnAttack(              Player player, int pos, int diagonal) const; // diagonal = {1,2}
  int  getKingAttack(              Player player, int pos) const;
  int  findAttackingPawnPosition(  Player player, int pos) const;
  int  findAttackingKnightPosition(Player player, int pos) const;
  const Piece *findAttackingPiece( Player player, int pos, FieldSet *result) const;
  const Piece *findPinningPiece(   const Piece *piece, int wantedDestination) const;
  FieldSet findAttackingPiecePositions( Player player, int pos) const;
  bool pieceCanGotoPosition(      const Piece *piece, int pos) const;
  bool pieceAttacksPosition(      const Piece *piece, int pos) const;
  bool pieceCanGotoAnyPosition(   const Piece *piece, const FieldSet &positions) const;
  bool pieceAttacksAnyPosition(   const Piece *piece, const FieldSet &positions) const;
  int  getKingRowAttackedFrom(    Player player) const; // Return the position from which the players king is row attacked from, -1 if not attacked
  int  getKingColAttackedFrom(    Player player) const; // Return the position from which the players king is column attacked from, -1 if not attacked
  void validateAddPieceAtPosition(PieceType pieceType, Player player, int pos, bool valiadatePromotions) const;  // throw Exception if not allowed

  Game &copyFrom(const Game &src); // return *this
  Piece *getPiecePointer(const Game &src, const Piece *piece);
  void updateLinks();
  inline void checkSetupMode(bool assumedMode) const {
    if(m_setupMode != assumedMode) {
      throwException(_T("Game is %sin setupMode."), m_setupMode ? _T("") : _T("not "));
    }
  }

  PositionType getPositionType21() const;
  void invalidPositionSignature(const TCHAR *functionSuffix) const;

  void allocateMemory(int stackSize);
  void deallocateMemory();
  void allocateMoveGenerators();
  void deallocateMoveGenerators();
  void allocateStack(int stackSize);
  void deallocateStack();
  void resizeStack();
  void resetGameHistory();

  String getDefaultName() const;
public:
  static const PieceType s_officersStartConfiguration[8]; // = { Rook, Knight, Bishop, Queen, King, Bishop, Knight, Rook }
  static const PieceType s_legalPromotions[4];            // = { Queen, Knight, Rook, Bishop }
  static const FieldInfo s_fieldInfo[64];

  Game();
  Game(const Game &src);
  Game(const GameKey &src);
  Game &operator=(const Game    &src);
  Game &operator=(const GameKey &src);
  Game &operator=(const String  &src);
  ~Game();
  Game &save(FILE *f);
  Game &load(FILE *f);
  String toString() const;
  Game &newGame(const String &name = _T(""));
  Game &clearBoard();
  void initState(bool validate=false, const GameKey *key=NULL);
  inline Game &beginSetup() {                // return *this
    checkSetupMode(false);
    m_setupMode = true;
    return *this;
  }

  Game &endSetup(const GameKey *key = NULL); // return *this
  inline bool isSetupMode() const {
    return m_setupMode;
  }
  PieceKey     setPieceAtPosition(PieceKey  key                , int pos); // returns key of the old one
  PieceKey     setPieceAtPosition(PieceType type, Player player, int pos);
  PieceKey     removePieceAtPosition(int pos, bool resetType=true);
  const Piece *getPieceAtPosition(   int pos) const;
  PieceKey     getPieceKeyAtPosition(int pos) const;
  bool         isPositionEmpty(      int pos) const;
  int          getPieceCountOnBoard(PieceType pieceType, Player player) const;
  int          getPieceCountOnBoard(Player player) const;
  int          getPieceCountOnBoard() const;
  int          getNonPawnCountOnBoard(Player player) const;
  int          getNonPawnCountOnBoard() const;

  bool isAddPieceLegal(     PieceKey  key                     , int pos, bool validatePromotions=true) const;
  bool isAddPieceLegal(     PieceType pieceType, Player player, int pos, bool validatePromotions=true) const;
  Game &validateBoard(bool intensive); // return *this
  Game &setPlayerInTurn(Player player);
  Game &setGameAfterPly(const Game    &src, int ply);
  Game &setGameAfterPly(const GameKey &startKey, const GameHistory &history, int ply);
  static bool hasSamePartialHistory(const Game &g1, const Game &g2);
  inline Player getPlayerInTurn() const {
    return m_gameKey.getPlayerInTurn();
  }

  inline const GameKey &getKey() const {
    return m_gameKey;
  }

  inline const PlayerState &getPlayerState(Player player) const {
    return m_playerState[player];
  }

  inline bool isKingInCheck() const { // Return true, if the king owned by player in turn is in check
    return m_playerState[PLAYERINTURN].m_kingAttackState != KING_NOT_ATTACKED;
  }

  inline KingAttackState getKingAttackState() const {
    return m_playerState[PLAYERINTURN].m_kingAttackState;
  }

  bool isDoubleCheck() const;
  int getKingPosition(Player player) const;
  const Piece *getNonKingPieceOnBoardByIndex(Player player, int n) const; // Return the n'th non-king piece on board, belonging to player. n=[0..15],

  MoveGenerator &getMoveGenerator() const;

  static GameUpdateFunction getMoveFunction(    PieceType pieceType);
  static GameUpdateFunction getCaptureFunction( PieceType pieceType);

  bool doMove(const Move &m);             // Push gamestate and do move m. Return the same as tryMove
  Move undoMove();                        // Undo move done with doMove and restore gamestate

  // These are used by MoveFinder, because they are faster than do-/undoMove
  bool tryMove(const Move &m);            // Assume the gamestate has been saved by pushState.
                                          // Return false if getPositionRepeats() > m_maxPositionRepeat or getMoveCountWithoutCaptureOrPawnMove() > 100
  void unTryMove();                       // Undo move done with tryMove and restores gamestate without popping it (should be done with popState)


#ifndef TABLEBASE_BUILDER
  String toUCIString() const;
  String toFENString() const;             // Current position in FEN-format
  Game &fromFENString(const String &s);

  inline int getPositionRepeats() const {
    return m_hashStack.getRepeatCount();
  }

  inline void setMaxPositionRepeat(int maxPositionRepeat=2) {
    m_maxPositionRepeat = maxPositionRepeat;
  }

  inline void setMaxPlyCountWithoutCaptureOrPawnMove(int maxPlies=100) {
    m_maxPliesWithoutCaptureOrPawnMove = maxPlies;
  }

  inline int getMaxPlyCountWithoutCaptureOrPawnMove() const {
    return m_maxPliesWithoutCaptureOrPawnMove;
  }

  inline int getLastCaptureOrPawnMove() const {
    return m_lastCaptureOrPawnMove;
  }

  inline int getPlyCountWithoutCaptureOrPawnMove() const {
    return m_stackSize - m_lastCaptureOrPawnMove;
  }

  int getMaterial(Player player) const {
    return m_playerState[player].m_totalMaterial;
  }

  int getPositionalScore(Player player) const {
    return m_playerState[player].m_positionalScore;
  }

  inline BishopFlags getBishopFlags(Player player) const {
    return m_playerState[player].m_bishopFlags;
  }

  inline String getPawnCountToString(Player player) const {
    return m_playerState[player].pawnCountToString();
  }

  int evaluateScore() const;

#else

  static GameUpdateFunction getBackMoveFunction(PieceType pieceType);
  void           setEndGameKeyDefinition(const EndGameKeyDefinition &keydef);
  void           setEndGameKeyDefinition();
  void           setGenerateFictivePawnCaptures(bool on) {
    m_generateFictivePawnCaptures = on;
  }
  MoveGenerator &getBackMoveGenerator() const;
  void           doBackMove(const Move &m);
  Move           undoBackMove();
  void           tryBackMove(const Move &m);       // Move m must be generated by MoveGenerator fetched by getBackMoveGenerator
  void           unTryBackMove();
  FieldSet       getLegalSourceFields(int pos) const;
  FieldSet       getLegalSourceFields(const Piece *piece) const;
  MoveBaseArray  getLegalBackMoves(int pos) const;
  MoveBaseArray  getLegalBackMoves(const Piece *piece) const;
  Move           generateBackMove(int from, int to) const;

#endif

  void pushState();                                // push gamestate
  inline void popState() {                         // pops the state pushed by pushState()
    assert(m_stackSize > 0);
    m_stackSize--;
  }

  const GameStackElement &restoreState();          // restore gamestate pushed by pushState() without popping it.

  GameHistory     getHistory() const;
  MoveBase        getLastMove() const;
        Move     &getMove(UINT ply);       // ply must be in range [0..m_stackSize-1]
  const Move     &getMove(UINT ply) const; // ply must be in range [0..m_stackSize-1]
  int isStackEmpty() const {
    return m_stackSize == 0;
  }

  static int getWalkDistance(int pos1, int pos2); // Return the number of moves it takes to move king from pos1 to pos2.

  void setName(const String &name) {
    m_name = name;
  }

  const String getDisplayName() const;

  const String &getFileName() const {
    return m_name;
  }

  bool hasDefaultName() const {
    return m_name == getDefaultName();
  }

  FieldSet       getLegalDestinationFields(int pos) const;
  FieldSet       getLegalDestinationFields(const Piece *piece) const;
  MoveBaseArray  getLegalMoves(int pos) const;
  MoveBaseArray  getLegalMoves(const Piece *piece) const;
  FieldSet       getMatingPositions() const; // return the positions, where the pieces that participate in checkmate are located.
  void           parse(const String &s);
  void           executeMove(const MoveBase &m);
  MoveBase       unExecuteLastMove();
  bool           mustSelectPromotion(int from, int to) const;
  static int     getPromoteIndex(PieceType pt);
  Move           generateMove(const MoveBase &m) const;
  PrintableMove  generateMove(int from, int to, PieceType promoteTo = NoPiece, MoveAnnotation annotation = NOANNOTATION) const;
  PrintableMove  generateMove(const String &s, MoveStringFormat mf = (MoveStringFormat)-1) const;

  Move getRandomMove() const;

  inline bool isGameOver() const {
    return (m_gameResult != NORESULT) || (getPositionType() == DRAW_POSITION);
  }

  bool isCheckmate(Player player) const;
  inline bool isCheckmate() const {
    return isCheckmate(PLAYERINTURN);
  };

  bool isSingleMovePosition() const; // return true, if there is only one possible move

  inline GameResult getGameResult() const { // Only valid after executeMove/unExecuteLastMove
    return m_gameResult;
  }

  GameResult findGameResult() const;

  inline CastleState getCastleState(Player player) const {
    return m_gameKey.d.m_castleState[player];
  }

  inline int getCheckingSDAPosition(Player player) const {
    return m_playerState[player].m_checkingSDAPosition;
  }

  PositionType           getPositionType() const;
  PositionSignature      getPositionSignature() const;
  void                   resetCapturedPieceTypes(Player player);

  String getMoveString(const MoveBase &m, MoveStringFormat mf=MOVE_LONGFORMAT) const;

  static inline String getFieldInfoAsString(int pos) {
    return s_fieldInfo[pos].toString();
  }

  inline const GameStackElement *getGameStack() const {
    return m_gameStack;
  }

  inline int getPlyCount() const {
    return m_stackSize;
  }

  const GameKey &getStartPosition() const;
  inline bool startWithWhite() const {
    return getStartPosition().d.m_playerInTurn == WHITEPLAYER;
  }
  inline bool startWithBlack() const {
    return getStartPosition().d.m_playerInTurn == BLACKPLAYER;
  }
};

String secondsToString(double msec, bool showMilliSeconds=true);

#define POSADJACENT(wkPos,bkPos) (Game::getWalkDistance(wkPos,bkPos) <= 1)

#define GETDIAG1(         pos)     Game::s_fieldInfo[pos].m_diag1
#define GETDIAG2(         pos)     Game::s_fieldInfo[pos].m_diag2
#define IS_INNERROW(      pos)     Game::s_fieldInfo[pos].m_innerRow
#define IS_INNERCOL(      pos)     Game::s_fieldInfo[pos].m_innerCol
#define IS_INNERFIELD(    pos)     Game::s_fieldInfo[pos].m_innerField
#define IS_CORNERFIELD(   pos)     Game::s_fieldInfo[pos].m_cornerField
#define IS_EDGEFIELD(     pos)     (!IS_INNERFIELD(pos))
#define GETSQUARE(pos)             Game::s_fieldInfo[pos].m_square

#define IS_ONMAINDIAG1(   pos)     (((pos) % 9) == 0)
#define IS_ONMAINDIAG2(   pos)     (GETDIAG2(pos) ==  _GETDIAGONAL2(H1))
#define IS_OFFMAINDIAG1(  pos)     (((pos) % 9) != 0)
#define IS_OFFMAINDIAG2(  pos)     (GETDIAG2(pos) !=  _GETDIAGONAL2(H1))
#define IS_BELOWMAINDIAG1(pos)     (GETDIAG1(pos) <   _GETDIAGONAL1(A1))
#define IS_BELOWMAINDIAG2(pos)     (GETDIAG2(pos) <   _GETDIAGONAL2(H1))
#define IS_ABOVEMAINDIAG1(pos)     (GETDIAG1(pos) >   _GETDIAGONAL1(A1))
#define IS_ABOVEMAINDIAG2(pos)     (GETDIAG2(pos) >   _GETDIAGONAL2(H1))

#define IS_SAMESIDEMAINDIAG1(pos1,pos2) (IS_BELOWMAINDIAG1(pos1) == IS_BELOWMAINDIAG1(pos2))

#define SWAPPLAYER(pieceKey)       MAKE_PIECEKEY(GETENEMY(GET_PLAYER_FROMKEY(pieceKey)),GET_TYPE_FROMKEY(pieceKey))
#define MIRRORROW(        pos)     MAKE_POSITION(7-GETROW(pos),  GETCOL(pos))
#define MIRRORCOLUMN(     pos)     MAKE_POSITION(  GETROW(pos),7-GETCOL(pos))
#define MIRRORDIAG1(      pos)     MAKE_POSITION(  GETCOL(pos),  GETROW(pos))
#define MIRRORDIAG2(      pos)     MAKE_POSITION(7-GETCOL(pos),7-GETROW(pos))
#define ROTATELEFT(       pos)     MAKE_POSITION(  GETCOL(pos),7-GETROW(pos))
#define ROTATERIGHT(      pos)     MAKE_POSITION(7-GETCOL(pos),  GETROW(pos))
#define ROTATE180(        pos)     MAKE_POSITION(7-GETROW(pos),7-GETCOL(pos))

#define IS_KINGINCHECK(player) (m_playerState[player].m_kingAttackState != 0)

#define UPDATE_SDA(player, positions, delta)                        \
{ FieldAttacks  *attackTable = m_playerState[player].m_attackTable; \
  PositionArray  pa          = positions;                           \
  for(int count = *(pa++); count--;) {                              \
    attackTable[*(pa++)].m_attackInfo.m_sdAttacks += delta;         \
  }                                                                 \
}

#define UPDATE_KINGATTACKS(  player, fieldInfo, delta) UPDATE_SDA(player      , fieldInfo.m_kingAttacks     , delta)
#define UPDATE_KNIGHTATTACKS(player, fieldInfo, delta) UPDATE_SDA(player      , fieldInfo.m_knightAttacks   , delta)
#define UPDATE_WHITEPAWNATTACKS(     fieldInfo, delta) UPDATE_SDA(WHITEPLAYER , fieldInfo.m_whitePawnAttacks, delta)
#define UPDATE_BLACKPAWNATTACKS(     fieldInfo, delta) UPDATE_SDA(BLACKPLAYER , fieldInfo.m_blackPawnAttacks, delta)

#define SET_EMPTYFIELD(pos)                           \
{ m_board[pos]                   = NULL;              \
  m_gameKey.m_pieceKey[pos]      = EMPTYPIECEKEY;     \
}

#define SET_PIECE(pos, piece)                         \
{ m_board[piece->m_position=pos] = piece;             \
  m_gameKey.m_pieceKey[pos]      = piece->m_pieceKey; \
}

#define MOVEPIECE_NOUPDATEKEY(from, to)               \
{ Piece *piece                   = m_board[from];     \
  m_board[from]                  = NULL;              \
  m_board[piece->m_position=to]  = piece;             \
}

#define MOVEPIECE_UPDATEKEY(from, to)                 \
{ Piece *piece                   = m_board[from];     \
  SET_PIECE(to,piece);                                \
  SET_EMPTYFIELD(from);                               \
}
