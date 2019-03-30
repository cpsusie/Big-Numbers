#pragma once

#include <CompactHashSet.h>
#include <Map.h>
#include <CountedByteStream.h>
#include <Timer.h>
#include "EndGameKeyDefinition.h"

#ifdef NEWCOMPRESSION
#pragma message("Compiling with NEW COMPRESSION")
#include <BigEndianStream.h>
#else
#pragma message("Compiling with OLD COMPRESSION")
#endif

typedef enum {
  EG_UNDEFINED
 ,EG_DRAW
 ,EG_WHITEWIN   // dont swap these. EG_WHITEWIN, EG_BLACKWIN must be the last. See IndexedMapEntryIteratorWinnerEntries
 ,EG_BLACKWIN
} EndGamePositionStatus;

inline EndGamePositionStatus transformPositionStatus(EndGamePositionStatus status, bool swapPlayers) {
  return (swapPlayers && (status >= EG_WHITEWIN)) ? (EndGamePositionStatus)(5-status) : status;
}

const TCHAR *positionStatusToString(EndGamePositionStatus state);

#define STATUSTOWINNER(status)     (((status) == EG_WHITEWIN) ? WHITEPLAYER : BLACKPLAYER)
#define WINNERTOSTATUS(winner)     (((winner) == WHITEPLAYER) ? EG_WHITEWIN : EG_BLACKWIN)
#define PLIESTOMOVES(plies)        (((plies)+1)/2)
// assume this is a winnerposition for playerinturn or opponent.
// pitWin is true if this is a winnerposition for playerInTurn, or else false
#define MOVESTOPLIES(movesToEnd,pitWin) (2*(movesToEnd)+((pitWin)?0:1))
//#define MOVESTOPLIES(moves,pitWin) ((moves)*2-((pitWin)?1:0))

#define BOOL2MASK(b1, b2)     (((b1)?1:0) | ((b2)?2:0))
#define BOOL3MASK(b1, b2, b3) (BOOL2MASK(b1, b2) | ((b3)?4:0))

#define FBOOL2MASK(f, x, y   ) BOOL2MASK(f(x), f(y))
#define FBOOL3MASK(f, x, y, z) BOOL3MASK(f(x), f(y), f(z))

class EndGameResult {     // bit 0-8:pliesToEnd, bit 9-10:positionStatus, bit 11:exist, bit 12:visited, bit 13:changed, bit 14:marked
private:                  // f e d c b a9     876543210
  USHORT m_data;          // - M C V E Status pliesToEnd
  inline EndGameResult(USHORT data) : m_data(data) {
  }
public:
  inline EndGameResult() {
    m_data = 0;
  }
  inline EndGameResult(EndGamePositionStatus status, UINT pliesToEnd) {
    m_data = (1<<11);
    setResult(status, pliesToEnd);
  }

  EndGameResult(GameResult r);

  EndGameResult &setResult(EndGamePositionStatus status, UINT pliesToEnd);
  EndGameResult &changePliesToEnd(UINT pliesToEnd);

  static EndGameResult getPrunedWinnerResult() { // special code to indicate winner result.
                                                 // which is pruned from compressed table
                                                 // Does not occur in any tables
                                                 // Dont use getStatus/getPliesToEnd on this
    return EndGameResult(-1);
  }
  inline bool isPrunedWinnerResult() const {
    return m_data == (USHORT)-1;
  }

  inline UINT getPliesToEnd() const {
    return (m_data & 0x1ff);
  }

  inline UINT getMovesToEnd() const {
    return PLIESTOMOVES(getPliesToEnd());
  }

  inline EndGamePositionStatus getStatus() const {
    return (EndGamePositionStatus)((m_data>>9) & 0x3);
  }

  inline bool exists() const {
    return (m_data & (1<<11)) != 0;
  }

  inline void setUndefined() {
    assert(exists());
    m_data &= ~0x07ff;
  }

  inline bool isDefined() const {
    return getStatus() != EG_UNDEFINED;
  }

  inline bool isWinner() const {
    return getStatus() >= EG_WHITEWIN;
  }

  inline bool isStalemate() const {
    return (getStatus() == EG_DRAW) && (getPliesToEnd() == 0);
  }

  inline bool isTerminalDraw() const {
    return (getStatus() == EG_DRAW) && (getPliesToEnd() == 1);
  }

  inline void setExist() {
    m_data |= (1<<11);
  }

  inline void clearExist() {
    m_data = 0;
  }

  inline bool isVisited() const {
    return (m_data & (1<<12)) != 0;
  }

  inline void setVisited() {
    m_data |= (1<<12);
  }

  inline void clearVisited() {
    m_data &= ~(1<<12);
  }

  inline EndGameResult &setChanged() {
    m_data |= (1<<13);
    return *this;
  }

  inline void clearChanged() {
    m_data &= ~(1<<13);
  }

  inline bool isChanged() const {
    return (m_data & (1<<13)) != 0;
  }

  inline void setMark() {
    m_data |= (1<<14);
  }

  inline void clearMark() {
    m_data &= ~(1<<14);
  }

  inline bool isMarked() const {
    return (m_data & (1<<14)) != 0;
  }

  inline void clearHelpInfo() { // clear Visited, Changed, and Marked
    m_data &= ~(7<<12);
  }

  inline bool hasHelpInfo() const {
    return (m_data & (7 << 12)) ? true : false;
  }

  inline bool operator==(const EndGameResult r) const {
    return (m_data & 0x7ff) == (r.m_data & 0x7ff);
  }

  inline bool operator!=(const EndGameResult r) const {
    return (m_data & 0x7ff) != (r.m_data & 0x7ff);
  }

  inline EndGameResult transform(bool swapPlayers) const {
    return swapPlayers ? EndGameResult(transformPositionStatus(getStatus(), true), getPliesToEnd()) : *this;
  }

  TCHAR *toStr(TCHAR *dst, bool ply = false) const;
  TCHAR *toStr(TCHAR *dst, Player playerInTurn, bool ply = false) const;

  String toString(bool ply = false) const;
  String toString(Player playerInTurn, bool ply = false) const;

  friend inline Packer &operator<<(Packer &p, const EndGameResult &egr) {
    p << egr.m_data;
    return p;
  }
  friend inline Packer &operator>>(Packer &p, EndGameResult &egr) {
    p >> egr.m_data;
    return p;
  }
};

#define EGR_STALEMATE                     EndGameResult(EG_DRAW     , 0)
#define EGR_TERMINALDRAW                  EndGameResult(EG_DRAW     , 1)
#define EGR_DRAW                          EndGameResult(EG_DRAW     , 2)
#define EGR_WHITEISMATE                   EndGameResult(EG_BLACKWIN , 0)
#define EGR_BLACKISMATE                   EndGameResult(EG_WHITEWIN , 0)
#define EGR_WINNER(pit,winner,movesToEnd) EndGameResult(WINNERTOSTATUS(winner), MOVESTOPLIES(movesToEnd,(pit)==(winner)))

typedef Entry<EndGameKey, EndGameResult> EndGameEntry;

class MoveWithResult : public MoveBase {
private:
  EndGameResult m_result;
public:
  inline MoveWithResult() {
  }
  inline MoveWithResult(const MoveBase &move, const EndGameResult &result) : MoveBase(move), m_result(result) {
  }
  inline EndGameResult getResult() const {
    return m_result;
  }
  inline void setResult(const EndGameResult &result) {
    m_result = result;
  }
  inline MoveWithResult transform(const Game &game, bool swapPlayers) const {
    return swapPlayers ? MoveWithResult(__super::transform(game, true), getResult().transform(true)) : *this;
  }
  inline String toString() const {
    return format(_T("%8s - %s"), MoveBase::toString().cstr(), m_result.toString().cstr());
  }
  friend inline Packer &operator<<(Packer &p, const MoveWithResult &m) {
    return p << (MoveBase&)m << m.m_result;
  }
  friend inline Packer &operator>>(Packer &p, MoveWithResult &m) {
    return p >> (MoveBase&)m >> m.m_result;
  }
};

class MoveResultArray : public CompactArray<MoveWithResult> {
private:
  Player m_playerInTurn;
public:
  inline MoveResultArray() : m_playerInTurn((Player)(-1)) {
  }
  MoveResultArray &clear(Player playerInTurn, intptr_t capacity = 0) {
    __super::clear(capacity);
    m_playerInTurn = playerInTurn;
    return *this;
  }
  inline Player getPlayerInTurn() const {
    return m_playerInTurn;
  }
  inline MoveWithResult selectShortestWinnerMove() const {
    return (*this)[findShortestWinnerMoves().select()];
  }

  inline MoveWithResult selectLongestLoosingMove(int defendStrength) const { // defendStrength = [0..100]
    return (*this)[findLongestLoosingMoves(defendStrength).select()];
  }

  inline MoveWithResult selectDrawMove() const {
    return (*this)[findDrawMoves().select()];
  }

  CompactIntArray findShortestWinnerMoves()                   const;
  CompactIntArray findLongestLoosingMoves(int defendStrength) const; // defendStrength = [0..100]
  CompactIntArray findDrawMoves()                             const;
  MoveWithResult  selectBestMove(int defendStrength)          const; // defendStrength = [0..100]
  BYTE            getCountWithStatus(EndGamePositionStatus st) const;
  MoveResultArray &sort();

  friend Packer &operator<<(Packer &p, const MoveResultArray &a);
  friend Packer &operator>>(Packer &p,       MoveResultArray &a);

  String toString(const Game &game, MoveStringFormat mf, bool depthInPlies);
};

class ReplyCount {
private:
  BYTE m_drawReplies;  // how many of possible moves from opponent will remain in draw zone
  BYTE m_looseReplies; // how many of possible moves from opponent will he loose
public:
  inline ReplyCount() : m_drawReplies(0), m_looseReplies(0) {
  }
  inline ReplyCount(const MoveResultArray &replyArray)
    : m_drawReplies( replyArray.getCountWithStatus(EG_DRAW))
    , m_looseReplies(replyArray.getCountWithStatus((replyArray.getPlayerInTurn()==WHITEPLAYER)?EG_BLACKWIN:EG_WHITEWIN))
  {
#ifdef _DEBUG
    if(m_drawReplies + m_looseReplies != replyArray.size()) {
      throwInvalidArgumentException(__TFUNCTION__
                                   ,_T("replyArray.size=%zu, drawReplies:%u, looseReplies:%u")
                                   ,replyArray.size(), m_drawReplies, m_looseReplies);
    }
#endif _DEBUG
  }
  inline void clear() {
    m_drawReplies = m_looseReplies = 0;
  }
  inline BYTE getDrawReplyCount() const {
    return m_drawReplies;
  }
  inline BYTE getLooseReplyCount() const {
    return m_looseReplies;
  }
  inline BYTE getTotalReplyCount() const {
    return m_drawReplies + m_looseReplies;
  }
  inline double getDrawReplyPct() const {
    return PERCENT(m_drawReplies, getTotalReplyCount());
  }
  inline double getLooseReplyPct() const {
    return PERCENT(m_looseReplies, getTotalReplyCount());
  }
  String toString() const {
    return format(_T(" Replies:(draw:%2d(%5.2lf%%), loose:%2d(%5.2lf%%)")
                 ,m_drawReplies , getDrawReplyPct()
                 ,m_looseReplies, getLooseReplyPct());
  }
};

class MoveWithResult2 : public MoveWithResult {
private:
  ReplyCount m_replyCount;

#ifdef _DEBUG
  void checkIsConsistent(const TCHAR *method) const {
    if(m_replyCount.getTotalReplyCount() == 0) {
      if(getResult().isWinner() || getResult().isStalemate() || getResult().isTerminalDraw()) {
        return;
      }
    } else if(m_replyCount.getDrawReplyCount() != 0) {
      if(!getResult().isWinner()) {
        return;
      }
    }
    throwException(_T("%s:Inconsistency:(moveResult=%s, m_replyCount:%s")
                  ,method
                  ,__super::toString().cstr()
                  ,m_replyCount.toString().cstr()
                  );
  }
#define CHECKISCONSISTENT() checkIsConsistent(__TFUNCTION__)
#else
#define CHECKISCONSISTENT()
#endif // _DEBUG
public:
  inline MoveWithResult2() {
  }
  inline MoveWithResult2(const MoveWithResult &m, const ReplyCount &replyCount)
    : MoveWithResult(m)
    , m_replyCount(replyCount)
  {
    CHECKISCONSISTENT();
  }
  inline MoveWithResult2(const MoveBase &move, const EndGameResult &result)
    : MoveWithResult(move, result)
  {
    CHECKISCONSISTENT();
  }
  inline MoveWithResult2(const MoveBase &move, const EndGameResult &result, const MoveResultArray &replyArray)
    : MoveWithResult(move, result)
    , m_replyCount(replyArray)
  {
    CHECKISCONSISTENT();
  }
  inline ReplyCount getReplyCount() const {
    return m_replyCount;
  }
  inline MoveWithResult2 transform(const Game &game, bool swapPlayers) const {
    return swapPlayers ? MoveWithResult2(__super::transform(game, true), getReplyCount()) : *this;
  }
  inline String replyToString() const {
    return (getResult().isWinner() || getResult().isStalemate()) ?  EMPTYSTRING : m_replyCount.toString();
  }
  String toString() const {
    return __super::toString() + replyToString();
  }
  String resultToString(Player playerInTurn, bool ply) const {
    return getResult().toString(playerInTurn, ply) + replyToString();
  }

#ifdef _DEBUG
#undef CHECKISCONSISTENT
#endif
};

class MoveResult2Array : public CompactArray<MoveWithResult2> {
private:
  Player m_playerInTurn;
  operator MoveResultArray() const;
public:
  inline MoveResult2Array() : m_playerInTurn((Player)(-1)) {
  }
  void clear(Player playerInTurn, intptr_t capacity = 0) {
    __super::clear(capacity);
    m_playerInTurn = playerInTurn;
  }
  inline Player getPlayerInTurn() const {
    return m_playerInTurn;
  }
  inline MoveWithResult selectShortestWinnerMove() const {
    return ((MoveResultArray)(*this)).selectShortestWinnerMove();
  }
  inline MoveWithResult selectLongestLoosingMove(int defendStrength) const { // defendStrength = [0..100]
    return ((MoveResultArray)(*this)).selectLongestLoosingMove(defendStrength);
  }
  CompactIntArray findBestDrawMoves() const;

  inline CompactIntArray findShortestWinnerMoves()            const {
    return ((MoveResultArray)(*this)).findShortestWinnerMoves();
  }
  inline CompactIntArray findLongestLoosingMoves(int defendStrength) const { // defendStrength = [0..100]
    return ((MoveResultArray)(*this)).findLongestLoosingMoves(defendStrength);
  }

  MoveWithResult  selectBestMove(int defendStrength) const; // defendStrength = [0..100]

  MoveResult2Array &sort();
  String toString(const Game &game, MoveStringFormat mf, bool depthInPlies);
};


class WrongVersionException : public Exception {
public:
  WrongVersionException(const TCHAR *msg) : Exception(msg) {
  }
};

class MissingPositionException : public Exception {
public:
  MissingPositionException(const TCHAR *msg) : Exception(msg) {
  }
};

// Dont change layout of this class, as it is part of header-information
template<class T> class TwoCountersTemplate {
public:
  T m_count[2]; // one for each player

  TwoCountersTemplate(T wc, T bc) {
    m_count[0] = wc; m_count[1] = bc;
  }
  TwoCountersTemplate() {
    clear();
  }
  void clear() {
    m_count[0] = m_count[1] = 0;
  }
  bool isEmpty() const {
    return (m_count[0] | m_count[1]) == 0;
  }
  T getTotal() const {
    return m_count[0] + m_count[1];
  }
  T getMax() const {
    return max(m_count[0], m_count[1]);
  }
  TwoCountersTemplate<T> operator+(const TwoCountersTemplate<T> &tc) const {
    return TwoCountersTemplate(m_count[0] + tc.m_count[0], m_count[1] + tc.m_count[1]);
  }
  TwoCountersTemplate<T> &operator+=(const TwoCountersTemplate<T> &tc) {
    m_count[0] += tc.m_count[0]; m_count[1] += tc.m_count[1];
    return *this;
  }
  String toString(TCHAR separator = _T(' '), int width = 11) const {
    return format(_T("%*s%c%*s"), width, format1000(m_count[0]).cstr(), separator, width, format1000(m_count[1]).cstr());
  }
  String toStringWithTotal(TCHAR separator = _T('+'), int width = 11) const {
    return format(_T("%*s = "), width, format1000(getTotal()).cstr()) + toString(separator, width);
  }
  inline void incr(Player player) {
    m_count[player]++;
  }
  inline void decr(Player player) {
    m_count[player]--;
  }
  bool operator==(const TwoCountersTemplate<T> &rhs) const {
    return m_count[0] == rhs.m_count[0] && m_count[1] == rhs.m_count[1];
  }
  bool operator!=(const TwoCountersTemplate<T> &rhs) const {
    return !(*this == rhs);
  }
};

template<class T> int compare(const TwoCountersTemplate<T> &tc1, const TwoCountersTemplate<T> &tc2) {
  return sign((long long)tc1.getTotal() - (long long)tc2.getTotal());
};

template<class T> int compare(const TwoCountersTemplate<T> &tc1, const TwoCountersTemplate<T> &tc2, Player player) {
  return sign((long long)tc1.m_count[player] - (long long)tc2.m_count[player]);
};

template<class T> int compareMax(const TwoCountersTemplate<T> &tc1, const TwoCountersTemplate<T> &tc2) {
  return sign((long long)tc1.getMax() - (long long)tc2.getMax());
};

typedef TwoCountersTemplate<UINT>           PositionCount32;
typedef TwoCountersTemplate<unsigned short> MaxVariantCount;

class PositionCount64 : public TwoCountersTemplate<UINT64> {
public:
  PositionCount64() : TwoCountersTemplate<UINT64>() {
  }
  PositionCount64(UINT64 wc, UINT64 bc) : TwoCountersTemplate(wc, bc) {
  }
  PositionCount64(const PositionCount32 &src) : TwoCountersTemplate(src.m_count[0], src.m_count[1]) {
  }
  PositionCount64(const TwoCountersTemplate<UINT64> &src) : TwoCountersTemplate(src) {
  }
};

// Bits for TablebaseInfo.m_stateFlags
#define TBISTATE_CONSISTENT          0x01
#define TBISTATE_NONWINNERFIXED      0x02
#define TBISTATE_WINNERFORWARD1FIXED 0x04
#define TBISTATE_WINNERRETROFIXED    0x08
#define TBISTATE_WINNERFORWARD2FIXED 0x10

#define TBISTATE_MASK                0x1f

#define WHITECANWIN 0x01
#define BLACKCANWIN 0x02
#define BOTHCANWIN  (WHITECANWIN|BLACKCANWIN)

#ifdef TABLEBASE_BUILDER
typedef enum {
  TBIFORMAT_PRINT_ALL            // print 1 field a line, with newlines.
 ,TBIFORMAT_PRINT_TERMINALS      // all other formats print as columns
 ,TBIFORMAT_PRINT_NONTERMINALS
 ,TBIFORMAT_PRINT_UNDEFANDWINS
 ,TBIFORMAT_PRINT_COLUMNS1
 ,TBIFORMAT_PRINT_COLUMNS2
} TablebaseInfoStringFormat;

#define WBWIDTH(n) (2*(n)+1)

#define POSITIONWIDTH    14
#define INDEXSIZEWIDTH   14
#define UNDEFINEDWIDTH   11
#define STALEMATEWIDTH   10
#define MATEPOSWIDTH      9
#define WINNERPOSWIDTH   14
#define TERMPOSWIDTH     11
#define NTPOSWIDTH       13
#define MAXVARFLDWIDTH    3
#define MAXVARWIDTH       WBWIDTH(MAXVARFLDWIDTH)
#define VERSIONWIDTH      5

#endif // TABLEBASE_BUILDER

class TablebaseInfo {
private:
  void checkVersion() const; // throws exception if wrong version
public:
  mutable char       m_version[20];
  UINT64             m_totalPositions;
  UINT64             m_indexCapacity;
  UINT64             m_undefinedPositions;
  UINT64             m_stalematePositions;
  UINT64             m_drawPositions;                // excl. stalemates and terminalDrawPositions;
  PositionCount64    m_checkMatePositions;           // m_checkMatePositions[WHITEPLAYER] = #positions where black is checkmate (white win) likewise black
  PositionCount64    m_terminalWinPositions;         // excl. checkmates
  PositionCount64    m_nonTerminalWinPositions;      // excl. checkmates and terminalWinPositions
  MaxVariantCount    m_maxPlies;                     // max length in plies of forced winning variant for each player. 0 if no winnerpositions
  bool               m_canWin[2];                    // Indicates if a player has any winner-positions,
                                                     // ie. checkmates or conversions to a winning position in another endgame exist
  BYTE               m_stateFlags;
  __time64_t         m_buildTime;
  __time64_t         m_consistencyCheckedTime;

  inline TablebaseInfo() {
    clear();
  }
  void clear();
  void save(ByteOutputStream      &s) const;
  void load(ByteInputStream       &s);
#ifdef NEWCOMPRESSION
  void save(BigEndianOutputStream &s) const;
  void load(BigEndianInputStream  &s);
#endif // NEWCOMPRESSION

  UINT64 getCheckMatePositions() const {
    return m_checkMatePositions.getTotal();
  }
  UINT64 getTerminalPositions() const {
    return m_terminalWinPositions.getTotal();
  }
  UINT64 getNonterminalPositions() const {
    return m_nonTerminalWinPositions.getTotal();
  }
  PositionCount64 getWinnerPositionCount() const;
  UINT64 getWinnerPositionCount(Player winner) const;
  String formatMaxVariants(bool plies=false) const;

  bool isConsistent() const {
    return (m_stateFlags & TBISTATE_CONSISTENT) != 0;
  }
  inline BYTE getCanWinFlags() const {
    return (m_canWin[WHITEPLAYER]?WHITECANWIN:0)|(m_canWin[BLACKPLAYER]?BLACKCANWIN:0);
  }
  String getVersion() const {
    return m_version;
  }
  static const char *s_programVersion;
  static String getProgramVersion() {
    return s_programVersion;
  }

#ifdef TABLEBASE_BUILDER
  // only defined for TBIFORMAT_PRINT_COLUMNS1/2
  static int getDataStringLength(TablebaseInfoStringFormat format);
  static String getColumnHeaders(TablebaseInfoStringFormat f, const String &headerLeft, const String &headerRight, bool plies);
  String toString(TablebaseInfoStringFormat f, bool plies = true) const;
#endif // TABLEBASE_BUILDER
};

class StreamProgress : public ByteCounter, public TimeoutHandler {
private:
  UINT64 m_total;
  Timer  m_timer;
public:
  StreamProgress(const EndGameTablebase *tb);
  StreamProgress(const String &fileName);
  ~StreamProgress();
  void handleTimeout(Timer &timer);
};

#ifdef TABLEBASE_BUILDER

typedef Iterator<EndGameKey>   EndGameKeyIterator;

class EndGameManualPositions {
private:
  const EndGameKeyDefinition &m_keydef;
  CompactHashSet<EndGameKey>  m_keySet;
public:
  EndGameManualPositions(const EndGameKeyDefinition &keydef);
  bool addKey(const EndGameKey &key);
  EndGameKey createKey(const String &str) const;
  void save(FILE *f);
  void load(FILE *f, int &lineCount);
  void load();
  String getName() const {
    return m_keydef.toString();
  }
  const EndGameKeyDefinition &getKeyDefinition() const {
    return m_keydef;
  }
  EndGameKeyIterator getKeyIterator();
  CompactArray<EndGameKey> readUndefinedKeysLog(const String &fileName) const;
};

class AllManualPositions {
private:
  Array<EndGameManualPositions> m_endGamePositions;
  static String getFileName();
  friend class EndGameManualPositions;
public:
  void load();
  void save();
  void clear();
  EndGameManualPositions &get(const String &name);
};

class IntervalChecker {
private:
  const TCHAR  *m_name;
  EndGamePosIndex m_minValue;
  EndGamePosIndex m_maxValue;
public:
  IntervalChecker(const TCHAR *name);
  ~IntervalChecker();
  void update(EndGamePosIndex v);
};

#endif // TABLEBASE_BUILDER
