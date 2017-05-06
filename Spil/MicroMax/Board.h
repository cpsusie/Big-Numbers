#pragma once

#include <Date.h>
#include <MyAssert.h>
#include "OccupationMap.h"

#define MASK88       0x88
#define S            0x80
#define I            8000

#define MINSCORE       -I
#define MAXSCORE        I

#define WPAWN           1
#define BPAWN           2
#define KNIGHT          3
#define KING            4
#define BISHOP          5
#define ROOK            6
#define QUEEN           7

#define ISPAWN(      ptype)  ((ptype)< KNIGHT)
#define ISKING(      ptype)  ((ptype)==KING  )
#define ISPIECE(     ptype)  ((ptype)>=KNIGHT)
#define ISCRAWLER(   ptype)  ((ptype)< BISHOP)
#define ISSLIDER(    ptype)  ((ptype)>=BISHOP)
#define SEARCHCENTER(ptype)  ((ptype)< ROOK  )

#define MAPSIZE (1<<24)

#define EMPTY           0
#define WHITE        0x08
#define BLACK        0x10
#define HASMOVED     0x20
#define COLORBITS           (WHITE|BLACK)

#define MAKE_PIECEKEY(player, pieceType) ((char)((player)|(pieceType)))

#define WHITEKING   MAKE_PIECEKEY(WHITE, KING   )
#define WHITEQUEEN  MAKE_PIECEKEY(WHITE, QUEEN  )
#define WHITEROOK   MAKE_PIECEKEY(WHITE, ROOK   )
#define WHITEBISHOP MAKE_PIECEKEY(WHITE, BISHOP )
#define WHITEKNIGHT MAKE_PIECEKEY(WHITE, KNIGHT )
#define WHITEPAWN   MAKE_PIECEKEY(WHITE, WPAWN  )
#define BLACKKING   MAKE_PIECEKEY(BLACK, KING   )
#define BLACKQUEEN  MAKE_PIECEKEY(BLACK, QUEEN  )
#define BLACKROOK   MAKE_PIECEKEY(BLACK, ROOK   )
#define BLACKBISHOP MAKE_PIECEKEY(BLACK, BISHOP )
#define BLACKKNIGHT MAKE_PIECEKEY(BLACK, KNIGHT )
#define BLACKPAWN   MAKE_PIECEKEY(BLACK, BPAWN  )

#define MAKEPOS(row,col)   (((row)<<4)+(col))
#define GETROW(pos)         ((pos)>>4)
#define GETCOL(pos)         ((pos)& 7)

#define ISLEGALCOL( ch )    (((ch) >= 'a') && ((ch) <= 'h'))
#define ISLEGALROW( ch )    (((ch) >= '1') && ((ch) <= '8'))
#define ISLEGALFIELD(str)   (ISLEGALCOL((str)[0]) && ISLEGALROW((str)[1]))
#define DECODEFIELD( str)   MAKEPOS((str)[1]-'1',(str)[0]-'a')
#define GETCOLNAME(pos)     ('a'+GETCOL(pos))
#define GETROWNAME(pos)     ('1'+GETROW(pos))

#define GETPROMOTEROW(side) (((side)==WHITE)?7:0)

#define A1 MAKEPOS(0, 0)
#define E1 MAKEPOS(0, 4)
#define H1 MAKEPOS(0, 7)
#define A8 MAKEPOS(7, 0)
#define E8 MAKEPOS(7, 4)
#define H8 MAKEPOS(7, 7)

#define STOP_IMMEDIATELY     0x01
#define STOP_WHENMOVEFOUND   0x02

typedef enum {
  GR_NORESULT
 ,GR_WHITECHECKMATE
 ,GR_BLACKCHECKMATE
 ,GR_STALEMATE
 ,GR_REPETITION
 ,GR_FIFTYMOVES
} GameResult;

#ifdef _DEBUG
const TCHAR *getSideStr(int side);          // white,black
const TCHAR *getPieceName( char pieceType); // king,queen,rook,...
String       getPieceName1(char piece);     // white king, white queen...black king,black queen...
#endif

TCHAR      *getFieldName( TCHAR *dst, char pos);
String      getFieldName( char pos);
char        getPieceType( char letter);    // letter = P,N,B,R,Q,K
String      fieldValuesToString();
String      pawnPushBonusToString();

TCHAR      *formatTime(TCHAR *dst, int ms, bool showMS = false);
String      formatTime(int ms, bool showMS = false);

void reply(       const TCHAR *format,...);
void replyMessage(const TCHAR *format,...);

class FastMove { // no constructor
public:
  union {
    struct {
      char  m_from;
      char  m_to;
      char  m_promoteIndex;
      bool  m_ybit;
    } d;
    int m_data;
  };
  inline bool isMove() const {
    return m_data != 0;
  }
  inline void setNoMove() {
    m_data = 0;
  }
  String toString() const;
  int toStr(TCHAR *dst) const;                                                      // return length
  static String toString(     char from, char to, BYTE promoteIndex=0); 
  static int toStr(TCHAR *dst, char from, char to, BYTE promoteIndex=0);   // return length
  inline bool operator==(const FastMove &m) const {
    return m_data == m.m_data;
  }
  inline int isDiagonalMove() const {
    return (d.m_to-d.m_from)&7;
  }
};

class Move : public FastMove {
public:
  Move() { setNoMove(); }
  Move(const String &str);
  inline Move &operator=(const FastMove &m) {
    ((FastMove&)*this) = m;
    return *this;
  }
  inline bool operator==(const FastMove &m) const {
    return !((m_data^m.m_data)&0xffffff); // works only for little-endian. for bigendian use 0xffffff00
                                          // we want to compare m_from,m_to,m_promoteIndex, but not m_ybit
  }
};

class HashElement {
public:
  int            m_key;
  HashElement   *m_next;
  FastMove       m_move;
  short          m_score;
  BYTE           m_depth;
  bool           m_cutAlpha : 1;
  bool           m_cutBeta  : 1;
  String getVariantString() const;
  TCHAR *getVariantStr(TCHAR *dst) const;
  inline int getMoveStr(TCHAR *dst) const {
    return m_move.toStr(dst);
  }
};

class HashCode {
public:
  int m_index, m_key;
  void clear() {
    m_index = m_key = 0;
  }
  String toString() const {
    return format(_T("(%9lu,%9lu)"), m_index, m_key);
  }
};

#ifdef USE_KNIGHTBONUS

class PlayerConfig {
private:
  static char knightBonus[7][7];
#ifdef _DEBUG
  static int _knightBonus(int pos, int king) {
    const int score = knightBonus[abs(GETROW(pos)-GETROW(king))][abs(GETCOL(pos)-GETCOL(king))];
    char str1[4],str2[4];
    reply("knight:%s king:%s->score=%d\n", getFieldName(str1, pos), getFieldName(str2, king), score);
    return score;
  }
#define KNIGHTBONUS(pos,king) _knightBonus(pos,king)
#else
#define KNIGHTBONUS(pos,king) knightBonus[abs(GETROW(pos)-GETROW(king))][abs(GETCOL(pos)-GETCOL(king))]
#endif
  char          m_side;
  unsigned char m_enemyKingPos;
  unsigned int  m_knightCount;
  unsigned char m_knightPos[10];
  unsigned char m_knightIndex[120];
  void checkInvariant(const char *fileName, int line) const;
  int sumKnightBonus() const {
    int sum = 0;
    switch(m_knightCount) {
    case 10: sum += KNIGHTBONUS(m_knightPos[9], m_enemyKingPos);
    case  9: sum += KNIGHTBONUS(m_knightPos[8], m_enemyKingPos);
    case  8: sum += KNIGHTBONUS(m_knightPos[7], m_enemyKingPos);
    case  7: sum += KNIGHTBONUS(m_knightPos[6], m_enemyKingPos);
    case  6: sum += KNIGHTBONUS(m_knightPos[5], m_enemyKingPos);
    case  5: sum += KNIGHTBONUS(m_knightPos[4], m_enemyKingPos);
    case  4: sum += KNIGHTBONUS(m_knightPos[3], m_enemyKingPos);
    case  3: sum += KNIGHTBONUS(m_knightPos[2], m_enemyKingPos);
    case  2: sum += KNIGHTBONUS(m_knightPos[1], m_enemyKingPos);
    case  1: sum += KNIGHTBONUS(m_knightPos[0], m_enemyKingPos);
    }
    return sum;
  }
public:
  void clear(char side);
  inline void addKing(unsigned char pos) {
    assert(m_enemyKingPos == S);
    m_enemyKingPos = pos;
  }
  inline int moveKing(const FastMove m) {
    assert(m_enemyKingPos == m.d.m_from);
    const int oldSum = sumKnightBonus();
    m_enemyKingPos = m.d.m_to;
    return sumKnightBonus() - oldSum;
  }
  inline void unmoveKing(const FastMove m) {
    assert(m_enemyKingPos == m.d.m_to);
    m_enemyKingPos = m.d.m_from;
  }
  inline void addKnight(   unsigned char pos) {
    assert(m_knightCount < ARRAYSIZE(m_knightPos));
    m_knightPos[m_knightIndex[pos] = m_knightCount++] = pos;
  }
  void removeKnight(unsigned char pos);
  inline int promoteToKnight(unsigned char pos) {
    addKnight(pos);
    return KNIGHTBONUS(pos, m_enemyKingPos);
  }
  inline int captureKnight(unsigned char pos) {
    removeKnight(pos);
    return KNIGHTBONUS(pos, m_enemyKingPos);
  }
  inline void uncaptureKnight(unsigned char pos) {
    addKnight(pos);
  }
  inline int moveKnight(const FastMove m) {
    unsigned char &index = m_knightIndex[m.d.m_from];
    assert(index < m_knightCount);
    m_knightPos[m_knightIndex[m.d.m_to] = index] = m.d.m_to;
    index = S;
    return KNIGHTBONUS(m.d.m_to, m_enemyKingPos) - KNIGHTBONUS(m.d.m_from, m_enemyKingPos);
  }
  inline void unmoveKnight(const FastMove m) {
    unsigned char &index = m_knightIndex[m.d.m_to];
    assert(index < m_knightCount);
    m_knightPos[m_knightIndex[m.d.m_from] = index] = m.d.m_from;
    index = S;
  }
  static void ajourKnightBonusTable(int R);
#ifdef _DEBUG
  String toString() const;
#endif
};

#endif

class BoardConfig {
private:
  TCHAR *getFENBoardString( TCHAR *dst) const;
  TCHAR *getFENCastleString(TCHAR *dst) const;
  TCHAR *getFENEpString(    TCHAR *dst) const;
  static const int  s_pieceValue[8];
  static int        s_fieldValue[120];
  static int        s_accumulatedPawnPushBonus[2][120];
public:
  char          m_field[129];         // board: half of 16x8+dummy (index S)
  int           m_EP;                 // Ep-square. if == S e.p. not allowed
  char          m_side;               // Side to move
  char          m_R;                  // 0=all officers on board. 40=none. Q=6, R=3, B and N=2
                                      // m_R <= 30 => midgame, m_R > 30 => endgame
  HashCode      m_hashCode;
  int           m_score;
  int           m_fiftyMoves, m_startMoveNumber;
  OccupationMap m_occupationMap;

#ifdef USE_KNIGHTBONUS
  PlayerConfig  m_player[2];
#endif

  inline bool operator==(const BoardConfig &bc) const {
    return memcmp(this, &bc, offsetof(BoardConfig, m_R)) == 0;
  }

  inline bool operator!=(const BoardConfig &bc) const {
    return !(*this==bc);
  }

  BoardConfig();
  static void initOnce();
  void clear();
  void setupStartBoard();
  void setupBoard(Tokenizer &tok);
  void initOccupationMap();
#ifdef USE_KNIGHTBONUS
  void ajourKnightBonusTable() {
    PlayerConfig::ajourKnightBonusTable(m_R);
  };
#endif
  static inline int getFieldValue(char pos) {
    return s_fieldValue[pos];
  }
  static inline int getPawnPushBonus(int side, int pos) {
    return s_accumulatedPawnPushBonus[side>>4][pos];
  }
  static inline int getPieceValue(char pieceType) {
    return s_pieceValue[pieceType];
  }

  String getFENString(int historySize = 0) const;
#ifdef _DEBUG
  void validate() const;
  String toString(int historySize = 0, int computerSide=BLACK, bool detailed=true) const;
#endif
};

class SearchStatistic {
private:
  Timestamp m_searchStartTime;
public:
  UINT      m_nodeCount;
  bool      m_verbose;
  SearchStatistic();
  void startSearch(bool verbose);
  double getTimeUsed() const; // in milliseconds
};

class Board {
private:
  friend class HashElement;
  friend class TableSet;
  static const char               *s_stepList[];
  static int                       s_zobristIndex[1024];   // hash-index translation table
  static int                       s_zobristKey[  1024];   // hash-key   translation table
  static HashElement              *s_hashMap;              // hash table, MAPSIZE entries
  static BoardConfig               s_bc;                   // bc is the workingboard
  static BoardConfig               s_savedBoard;
  static int                       s_maxDepth;             // Maximum depth of the search
  static Move                      s_bestMove, s_usableMove;
  static bool                      s_moveFound;
  static int                       s_stopCode;
  static bool                      s_gameOver;
  static SearchStatistic           s_searchStatistic;
  static BoardConfig               s_boardHistory[1024];
  static Move                      s_moveHistory[1024];
  static int                       s_historySize;


  static void initEngine();
  static void initHistory();
  static void initHashMap();
  static int searchMove(char request, int n);
  static int negamax(int side, int alfa, int beta, int e, int EP, char request, int n, HashElement* &link);
  static int getZobristIndex(int t);
  static int getZobristKey(  int t);

  static inline void switchSide() {
    s_bc.m_side ^= COLORBITS;
  }
  static bool isDrawByRepetition();
  static inline bool isVerbose() {
    return s_searchStatistic.m_verbose;
  }
  static inline void resetStopCode() {
    s_stopCode = 0;
  }
  static String getUCIScoreStr(int score);
public:
  Board() {
    initEngine();
  }
  static void initGame();
  static void setup(const String &position);

  static inline int getSide() {
    return s_bc.m_side;
  }
  static void setSide(int side);
  static inline bool isGameOver() {
    return s_gameOver;
  }
  static GameResult getGameResult();

  static String getGameResultString(GameResult result);

  static bool findMove();

  static inline void stopSearch() {
    s_stopCode = STOP_IMMEDIATELY;
  }
  static inline void moveNow() {
    s_stopCode = STOP_WHENMOVEFOUND;
  }
  static inline Move getBestMove() {
    return s_bestMove;
  }

  static inline int getTimeUsed() {
    return (int)getSearchStatistic().getTimeUsed();
  }
  static inline UINT getNodeCount() {
    return getSearchStatistic().m_nodeCount;
  }

  static void executeMove(const Move &move);
  static inline int getMaxDepth() {
    return s_maxDepth;
  }
  static void setMaxDepth(int n) {
    s_maxDepth = n;
  }
  static inline bool isFiftyMoves() {
    return s_bc.m_fiftyMoves >= 100;
  }
  static inline int getHistorySize() {
    return s_historySize;
  }
  static inline const BoardConfig &getConfig() {
    return s_bc;
  }
  static inline void setConfig(const BoardConfig &src) {
    s_bc = src;
  }

  static inline const SearchStatistic &getSearchStatistic() {
    return s_searchStatistic;
  }
#ifdef _DEBUG
  static void validate() {
    s_bc.validate();
  }
  static String toString(int computerSide = BLACK, bool detailed = false);
  static String getHistoryString();
  static String getBoardHistoryString(bool detailed = false);
#endif
};
