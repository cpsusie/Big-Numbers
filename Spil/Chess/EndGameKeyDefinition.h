#pragma once

#include <PropertyChangeListener.h>
#include "EndGameKey.h"

class PieceTypeWithIndex {
private:
  unsigned char m_data;
  void init(PieceType pieceType, int index);
public:
  PieceTypeWithIndex() {
    init(NoPiece, -1);
  }
  PieceTypeWithIndex(PieceType pieceType, int index) {
    init(pieceType, index);
  }
  inline PieceType getPieceType() const {
    return (PieceType)(m_data & 0xf);
  }
  inline int getIndex() const {
    return (int)((m_data >> 4) & 0xf);
  }
};

#define MAX_PIECEKEY_VALUE     MAKE_PIECEKEY(BLACKPLAYER, Pawn)

#define PAWN1_POSCOUNT 24
#define PAWN_POSCOUNT  48
#define PAWN2_POSCOUNT 47
#define PAWN3_POSCOUNT 46
#define PAWN4_POSCOUNT 45

class EndGameTablebase;

typedef enum {
  ENDGAME_METRIC
 ,ENDGAME_PATH
} EndGameProperties;

class EndGameGlobalProperties : public PropertyContainer {
private:
  TablebaseMetric m_metric;
  String          m_dbPath;
public:
  EndGameGlobalProperties();
  void setMetric(TablebaseMetric metric);
  inline TablebaseMetric getMetric() const {
    return m_metric;
  }
  bool setDbPath(const String &path);

  inline const String &getDbPath() {
    return m_dbPath;
  }
};

typedef UINT64 EndGamePosIndex;

#ifdef TABLEBASE_BUILDER
class KeyDefinitionSelfCheckInfo {
public:
  EndGamePosIndex m_minIndex, m_maxIndex;
  UINT64          m_checkKeyCount, m_duplicateCount;
  bool            m_testRunning;

  void reset();
  EndGamePosIndex getDistinctKeys() const {
    return m_checkKeyCount - m_duplicateCount;
  }
  EndGamePosIndex getRangeLength() const {
    return m_maxIndex - m_minIndex + 1;
  }
};
class SelfCheckStatusPrinter;

#endif

class EndGameKeyDefinition {
private:
  static const TCHAR            *s_metricName[2];
  static EndGameGlobalProperties s_globalProperties;

  PieceTypeWithIndex        m_orderedPieceType[2][MAX_ENDGAME_PIECECOUNT-1];          // Ordered by type, to make endGameKey.toString put King first, then queen, rook, etc.
  unsigned char             m_pieceCount;                                             // 4 lowend bits = #white pieces, 4 highend bits = #black pieces
  const unsigned char       m_totalPieceCount;

protected:

  PieceKey                  m_pieceKey[MAX_ENDGAME_PIECECOUNT];

#ifdef TABLEBASE_BUILDER
  mutable PositionSignature m_positionSignature;
  mutable String            m_shortName;
  mutable String            m_longName;

  void   selfCheckInit()    const;
  UINT64 selfCheckSummary(const SelfCheckStatusPrinter &statusPrinter) const;
  UINT64 checkSymmetries()  const;
  void pause() const;
#endif

  PositionSignature  createPositionSignature() const;

  static void invalidSquareError(EndGameKey key);
  static void sym8DecisionSwitchError(int line);
  static void pawnSymSwitchError(int line);
  void        pieceTypeError(int index, const String &msg) const;
  void        invalidPieceCountError(const TCHAR *method) const;
  void        impossibleEncodingError(EndGameKey key) const;
  void        checkIsPawn(UINT index, bool expected) const;
  void        checkSameOwner(UINT index1, UINT index2, bool expected) const;
  void init(PieceKey pk2, ...);

  TCHAR *createWhiteKeyString(TCHAR *dst, EndGameKey key) const;
  TCHAR *createBlackKeyString(TCHAR *dst, EndGameKey key) const;

  EndGameKeyDefinition(PieceKey pk2);
  EndGameKeyDefinition(PieceKey pk2, PieceKey pk3);
  EndGameKeyDefinition(PieceKey pk2, PieceKey pk3, PieceKey pk4);
  EndGameKeyDefinition(PieceKey pk2, PieceKey pk3, PieceKey pk4, PieceKey pk5);

  static SymmetricTransformation getSym8Transformation3Men(           EndGameKey key);
  static SymmetricTransformation getSym8Transformation4Men(           EndGameKey key);
  static SymmetricTransformation getSym8Transformation5Men(           EndGameKey key);
  static SymmetricTransformation getSym8Transformation6Men(           EndGameKey key);
  static SymmetricTransformation getSym8Transformation4Men2Equal(     EndGameKey key);
  static SymmetricTransformation getSym8Transformation5Men2Equal(     EndGameKey key);
  static SymmetricTransformation getSym8Transformation6Men2Equal(     EndGameKey key);
  static SymmetricTransformation getSym8Transformation5Men3Equal(     EndGameKey key);
  static SymmetricTransformation getSym8Transformation6Men3Equal(     EndGameKey key);
  static SymmetricTransformation getPawnSymTransformation(            EndGameKey key);
  static SymmetricTransformation get4Men2EqualPawnsSymTransformation( EndGameKey key);
  static SymmetricTransformation get5Men2EqualPawnsSymTransformation( EndGameKey key);
  static SymmetricTransformation get5Men3EqualPawnsSymTransformation( EndGameKey key);

#ifdef TABLEBASE_BUILDER
  mutable KeyDefinitionSelfCheckInfo m_selfCheckInfo;
  mutable double                     m_checkStartTime;
  mutable BitSet                    *m_usedIndex;

typedef void (EndGameKeyDefinition::*PositionScanner)(EndGameKeyWithOccupiedPositions &key, int pIndex, bool allPreviousOnDiag) const;

  virtual void         selfCheck(EndGameKeyWithOccupiedPositions &key) const;
  virtual String       getCodecName() const;
  void                 sym8PositionScanner(EndGameKeyWithOccupiedPositions &key, int pIndex, bool allPreviousOnDiag, PositionScanner nextScanner = NULL) const;
  void                 checkForBothPlayers(EndGameKey key) const;
  void                 checkKey(EndGameKey key) const;
#endif

public:
  static const char s_whiteKingPosToIndex[64];
  static const char s_whiteKingIndexToPos[10];
  static const char s_subDiagPosToIndex[  64];
  static const char s_subDiagIndexToPos[  36];
  static const char s_offDiagPosToIndex[  64];
  static const char s_offDiagIndexToPos[  56];

  static const char s_pawnPosToIndex[     64];
  static const char s_pawnIndexToPos[PAWN_POSCOUNT];

  static EndGamePosIndex          encodeKKSym8(               EndGameKey  key);
  static void                     decodeKKSym8(               EndGameKey &key, EndGamePosIndex index);

  static EndGamePosIndex          encodeKKWithPawn(           EndGameKey  key);
  static void                     decodeKKWithPawn(           EndGameKey &key, EndGamePosIndex index);

  static EndGamePosIndex          encodeLeftWKKWithPawn(      EndGameKey  key);
  static void                     decodeLeftWKKWithPawn(      EndGameKey &key, EndGamePosIndex index);

  virtual EndGamePosIndex         keyToIndex(                 EndGameKey   key  ) const = 0;
  virtual EndGameKey              indexToKey(            EndGamePosIndex   index) const = 0;
  virtual EndGamePosIndex         getIndexSize()                                  const = 0;
  virtual SymmetricTransformation getSymTransformation(       EndGameKey   key  ) const = 0;

  SymmetricTransformation         getPlayTransformation(const Game        &game ) const;

  inline int getPieceCount() const {
    return m_totalPieceCount;
  }

  inline int getPieceCount(Player player) const {
    return (m_pieceCount >> (4*player)) & 0xf;
  }

  int        getPieceCount(Player player, PieceType type) const;
  int        getPieceCount(PieceType type) const;
  int        findKeyIndexByCount(PieceKey pieceKey, int n) const;

  inline PieceKey   getPieceKey(        UINT i) const {
#ifdef _DEBUG
    if(i >= m_totalPieceCount) {
      throwInvalidArgumentException(__TFUNCTION__, _T("index %d out of range. totalPieceCount=%d"), i, m_totalPieceCount);
    }
#endif
    return m_pieceKey[i];
  }

  PieceType  getPieceType(       UINT i) const {
    return GET_TYPE_FROMKEY(getPieceKey(i));
  }
  Player     getPieceOwner(      UINT i) const {
    return GET_PLAYER_FROMKEY(getPieceKey(i));
  }

  virtual EndGameKey getEndGameKey(      const GameKey    &gameKey) const = 0;
  EndGameKey         getTransformedKey(  EndGameKey key, SymmetricTransformation st) const;
  EndGameKey         getNormalizedKey(   EndGameKey key, EndGamePosIndex *index = NULL) const; // will return keys' index in index if != NULL
  void               validateKey(        EndGameKey key, const TCHAR *msg) const;

#ifdef TABLEBASE_BUILDER
  const PositionSignature  &getPositionSignature()   const;
  virtual void             insertInitialPositions(EndGameTablebase &tablebase) const;
  const String            &toString(bool longNames = false) const;
  void                     listLongestUnusedSequence(BitSet &s, intptr_t sequenceMinSize = 1) const;

  void                     doSelfCheck(bool checkSym) const;
  const KeyDefinitionSelfCheckInfo &getSelfCheckInfo() const {
    return m_selfCheckInfo;
  }
  virtual bool keysEqual(EndGameKey key1, EndGameKey key2) const {
    return key1 == key2;
  }

  virtual bool isDupletsAllowed() const = 0;
  virtual EndGamePosIndex         keyToIndexNew(EndGameKey      key  ) const;
  virtual EndGameKey              indexToKeyNew(EndGamePosIndex index) const;
  bool                            hasConversion() const;

#else
  PositionSignature        getPositionSignature() const;
  String                   toString(bool longNames = false) const;
#endif

  String getName() const {
    return toString(false);
  }

  TCHAR *createKeyString(TCHAR *dst, EndGameKey key, bool initFormat) const;
  String createInitKeyString(EndGameKey key) const;

  static String getFileMetricSuffix();

  static String getDbFileName(const String &fileName);
#ifdef TABLEBASE_BUILDER
  String getTablebaseFileName() const;
#else
  String getDecompressedFileName() const;
#endif
  String getCompressedFileName() const;

  static void addPropertyChangeListener(   PropertyChangeListener *listener);
  static void removePropertyChangeListener(PropertyChangeListener *listener);

  static bool setDbPath(const String &path) {
    return s_globalProperties.setDbPath(path);
  }

  static const String &getDbPath() {
    return s_globalProperties.getDbPath();
  }
  static void setMetric(TablebaseMetric m) {
    s_globalProperties.setMetric(m);
  }
  static inline TablebaseMetric getMetric() {
    return s_globalProperties.getMetric();
  }
  static inline bool isMetricDTM() {
    return getMetric() == DEPTH_TO_MATE;
  }
  static inline bool isMetricDTC() {
    return getMetric() == DEPTH_TO_CONVERSION;
  }
  static const TCHAR *getMetricName(TablebaseMetric m) {
    return s_metricName[m];
  }
  static const TCHAR *getMetricName() {
    return getMetricName(getMetric());
  }
};

#ifdef TABLEBASE_BUILDER

#define DECLARE_SELFCHECK                                                                 \
  void selfCheck(EndGameKeyWithOccupiedPositions &key) const;                             \
  String getCodecName() const

#define DECLARE_KEYSEQUAL bool keysEqual(EndGameKey key1, EndGameKey key2) const

#define DUMP_MACRO(m) _tprintf(_T("%-50s:%14s\n"), _T(#m), format1000(m).cstr())

#define DUMP_RANGETABLE(table)                                                            \
{ _tprintf(_T("%s\n"), _T(#table));                                                       \
  const UINT _size = ARRAYSIZE(table);                                                    \
  for(UINT i = 0; i < _size; i++) {                                                       \
    _tprintf(_T("%2d:%14s (%11s)\n"), i                                                   \
            ,format1000(table[i]).cstr()                                                  \
            ,(i<_size-1)?format1000(table[i+1]-table[i]).cstr():_T(""));                  \
  }                                                                                       \
}

#else

#define DECLARE_SELFCHECK
#define DECLARE_KEYSEQUAL
#define DUMP_MACRO(m)
#define DUMP_RANGETABLE(table)

#endif

extern "C" {
  int findTableRange(const EndGamePosIndex *rangeTable, UINT size, EndGamePosIndex index);
  int findRange2Equal(EndGamePosIndex f, EndGamePosIndex index);
};

inline BYTE getBitCount(UINT n, BYTE maxBits=8) {
  BYTE count = 0;
  for(UINT m = 0; (count <= 32) && (m < n); count++, m = (m << 1) | 1);
  if(count > maxBits) {
    throwException(_T("%s:n=%lu. Needs %d bits to encode. Max=%d"), __TFUNCTION__, n, count, maxBits);
  }
  return count;
}

class EndGameKeyDefinitionDupletsNotAllowed : public EndGameKeyDefinition {
private:
  char m_pieceKeyIndexMap[MAX_PIECEKEY_VALUE+1]; // Indexed by [pieceKey]
  void initIndexMap();
protected:
  EndGameKeyDefinitionDupletsNotAllowed(PieceKey pk2);
  EndGameKeyDefinitionDupletsNotAllowed(PieceKey pk2, PieceKey pk3);
  EndGameKeyDefinitionDupletsNotAllowed(PieceKey pk2, PieceKey pk3, PieceKey pk4);
  EndGameKeyDefinitionDupletsNotAllowed(PieceKey pk2, PieceKey pk3, PieceKey pk4, PieceKey pk5);
public:
  EndGameKey    getEndGameKey(const GameKey     &gameKey) const;
  bool          isDupletsAllowed() const {
    return false;
  }
};

#define MAX_DUPLETCOUNT 3

class EndGameKeyDefinitionDupletsAllowed : public EndGameKeyDefinition {
private:
  char m_pieceKeyIndexMap[MAX_PIECEKEY_VALUE+1][MAX_DUPLETCOUNT]; // Indexed by pieceKey,count[pieceKey]
  void initIndexMap();
protected:
  EndGameKeyDefinitionDupletsAllowed(PieceKey pk23);
  EndGameKeyDefinitionDupletsAllowed(PieceKey pk2, PieceKey pk34);
  EndGameKeyDefinitionDupletsAllowed(PieceKey pk2, PieceKey pk3, PieceKey pk45);
#ifdef TABLEBASE_BUILDER
  static bool keysMatch23Equal( EndGameKey k1, EndGameKey k2);
  static bool keysMatch34Equal( EndGameKey k1, EndGameKey k2);
  static bool keysMatch45Equal( EndGameKey k1, EndGameKey k2);
  static bool keysMatch234Equal(EndGameKey k1, EndGameKey k2);
  static bool keysMatch345Equal(EndGameKey k1, EndGameKey k2);
#endif
public:
  EndGameKey getEndGameKey(const GameKey &gameKey) const;

#ifdef TABLEBASE_BUILDER
  bool isDupletsAllowed() const {
    return true;
  }
#endif
};

class EndGameKeyDefinition3Men : public EndGameKeyDefinitionDupletsNotAllowed {
public:
  EndGameKeyDefinition3Men(PieceKey pk2);

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 56112;
  }
  SymmetricTransformation getSymTransformation(EndGameKey key) const;
};

class EndGameKeyDefinition4Men : public EndGameKeyDefinitionDupletsNotAllowed {
public:
  EndGameKeyDefinition4Men(PieceKey pk2, PieceKey pk3);

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 3415776;
  }
  SymmetricTransformation getSymTransformation(EndGameKey key) const;
};

class EndGameKeyDefinition4Men2Equal : public EndGameKeyDefinitionDupletsAllowed {
private:
#ifdef TABLEBASE_BUILDER
  void scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex, bool allPreviousOnDiag) const;
#endif

public:
  EndGameKeyDefinition4Men2Equal(PieceKey pk23);

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 1708476;
  }
  SymmetricTransformation getSymTransformation(EndGameKey key) const;

  DECLARE_SELFCHECK;
  DECLARE_KEYSEQUAL;
};

class EndGameKeyDefinition5Men : public EndGameKeyDefinitionDupletsNotAllowed {
public:
  EndGameKeyDefinition5Men(PieceKey pk2, PieceKey pk3, PieceKey pk4);

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 204911280;
  }
  SymmetricTransformation getSymTransformation(EndGameKey key) const;
};

class EndGameKeyDefinition5Men2Equal : public EndGameKeyDefinitionDupletsAllowed {
private:
#ifdef TABLEBASE_BUILDER
  void scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex, bool allPreviousOnDiag) const;
#endif

public:
  EndGameKeyDefinition5Men2Equal(PieceKey pk2, PieceKey pk34);

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 102459168;
  }
  SymmetricTransformation getSymTransformation(EndGameKey key) const;

  DECLARE_SELFCHECK;
  DECLARE_KEYSEQUAL;
};

class EndGameKeyDefinition5Men3Equal : public EndGameKeyDefinitionDupletsAllowed {
private:
#ifdef TABLEBASE_BUILDER
  void scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex, bool allPreviousOnDiag) const;
#endif

public:
  EndGameKeyDefinition5Men3Equal(PieceKey pk234);

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 34155408;
  }
  SymmetricTransformation getSymTransformation(EndGameKey key) const;

  DECLARE_SELFCHECK;
  DECLARE_KEYSEQUAL;
};

class EndGameKeyDefinition6Men : public EndGameKeyDefinitionDupletsNotAllowed {
public:
  EndGameKeyDefinition6Men(PieceKey pk2, PieceKey pk3, PieceKey pk4, PieceKey pk5);

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 12089624400;
  }
  SymmetricTransformation getSymTransformation(EndGameKey key) const;
};

class EndGameKeyDefinition6Men2Equal : public EndGameKeyDefinitionDupletsAllowed {
private:
#ifdef TABLEBASE_BUILDER
  void scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex, bool allPreviousOnDiag) const;
#endif

public:
  EndGameKeyDefinition6Men2Equal(PieceKey pk2, PieceKey pk3, PieceKey pk45);

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 6044829840;
  }
  SymmetricTransformation getSymTransformation(EndGameKey key) const;

  DECLARE_SELFCHECK;
  DECLARE_KEYSEQUAL;
};

class EndGameKeyDefinition6Men3Equal : public EndGameKeyDefinitionDupletsAllowed {
private:
#ifdef TABLEBASE_BUILDER
  void scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex, bool allPreviousOnDiag) const;
#endif

public:
  EndGameKeyDefinition6Men3Equal(PieceKey pk2, PieceKey pk345);

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 2014955040;
  }
  SymmetricTransformation getSymTransformation(EndGameKey key) const;

  DECLARE_SELFCHECK;
  DECLARE_KEYSEQUAL;
};

#ifdef TABLEBASE_BUILDER
class PawnOwner {
private:
  unsigned char m_owner; // 1 bit for each pawn. 0 = WHITEPLAYER, 1 = BLACKPLAYER
public:
  PawnOwner() {
    m_owner = 0;
  }
  void setOwner(UINT index, Player owner) {
    if(owner == BLACKPLAYER) { m_owner |= (1 << index); } else { m_owner &= ~(1 << index); }
  }
  Player getOwner(UINT index) const {
    return (m_owner & (1 << index)) ? BLACKPLAYER : WHITEPLAYER;
  }
};
#endif

class EndGameKeyDefinition1Pawn : public EndGameKeyDefinitionDupletsNotAllowed {
#ifdef TABLEBASE_BUILDER
private:
  PawnOwner m_pawnOwner;
  void setPawnOwner();
#endif
protected:
  EndGameKeyDefinition1Pawn(PieceKey pk2);
  EndGameKeyDefinition1Pawn(PieceKey pk2, PieceKey pk3);
  EndGameKeyDefinition1Pawn(PieceKey pk2, PieceKey pk3, PieceKey pk4);
  SymmetricTransformation getSymTransformation(EndGameKey key) const;

#ifdef TABLEBASE_BUILDER
  Player getPawnOwner() const {
    return m_pawnOwner.getOwner(0);
  }
typedef void (EndGameKeyDefinition1Pawn::*PositionScanner1Pawn)(EndGameKeyWithOccupiedPositions &key, int pIndex) const;
  void scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex, PositionScanner1Pawn nextScanner = NULL) const;
#endif

public:

#ifdef TABLEBASE_BUILDER
  void insertInitialPositions(EndGameTablebase &tablebase) const;
#endif
  DECLARE_SELFCHECK;
};

class EndGameKeyDefinition3Men1Pawn : public EndGameKeyDefinition1Pawn {
public:
  EndGameKeyDefinition3Men1Pawn(PieceKey pk2) : EndGameKeyDefinition1Pawn(pk2) {
  }

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 173376;
  }
};

class EndGameKeyDefinition4Men1Pawn : public EndGameKeyDefinition1Pawn {
public:
  EndGameKeyDefinition4Men1Pawn(PieceKey pk2, PieceKey pk3) : EndGameKeyDefinition1Pawn(pk2, pk3) {
  }

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 10575936;
  }
};

class EndGameKeyDefinition5Men1Pawn : public EndGameKeyDefinition1Pawn {
public:
  EndGameKeyDefinition5Men1Pawn(PieceKey pk2, PieceKey pk3, PieceKey pk4);

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 634556160;
  }
};

class EndGameKeyDefinition5Men1Pawn2Equal : public EndGameKeyDefinitionDupletsAllowed {
private:
#ifdef TABLEBASE_BUILDER
  void scanPositions( EndGameKeyWithOccupiedPositions &key, int pIndex) const;
#endif

public:
  EndGameKeyDefinition5Men1Pawn2Equal(PieceKey pk2, PieceKey pk34);

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 317278080;
  }
  SymmetricTransformation getSymTransformation(EndGameKey key) const;

#ifdef TABLEBASE_BUILDER
  void insertInitialPositions(EndGameTablebase &tablebase) const;
#endif
  DECLARE_SELFCHECK;
  DECLARE_KEYSEQUAL;
};

class EndGameKeyDefinition2Pawns : public EndGameKeyDefinitionDupletsNotAllowed {
#ifdef TABLEBASE_BUILDER
private:
  PawnOwner m_pawnOwner;
  void setPawnOwners();
#endif

protected:
  EndGameKeyDefinition2Pawns(PieceKey pk2, PieceKey pk3);
  EndGameKeyDefinition2Pawns(PieceKey pk2, PieceKey pk3, PieceKey pk4);
  SymmetricTransformation getSymTransformation(EndGameKey key) const;

#ifdef TABLEBASE_BUILDER
  Player getPawnOwner(int index) const {
    return m_pawnOwner.getOwner(index);
  }
typedef void (EndGameKeyDefinition2Pawns::*PositionScanner2Pawns)(EndGameKeyWithOccupiedPositions &key, int pIndex) const;
  void scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex, PositionScanner2Pawns nextScanner = NULL) const;
#endif

public:
#ifdef TABLEBASE_BUILDER
  void insertInitialPositions(EndGameTablebase &tablebase) const;
#endif

  DECLARE_SELFCHECK;
};

class EndGameKeyDefinition2EqualPawns : public EndGameKeyDefinitionDupletsAllowed {
#ifdef TABLEBASE_BUILDER
private:
  PawnOwner m_pawnsOwner;
  void setPawnsOwner();
#endif
protected:
  EndGameKeyDefinition2EqualPawns(PieceKey pk23);
  EndGameKeyDefinition2EqualPawns(PieceKey pk2, PieceKey pk34);
#ifdef TABLEBASE_BUILDER
  Player getPawnsOwner() const {
    return m_pawnsOwner.getOwner(0);
  }
#endif
};

class EndGameKeyDefinition4Men2Pawns: public EndGameKeyDefinition2Pawns {
public:
  EndGameKeyDefinition4Men2Pawns(PieceKey pk2, PieceKey pk3) : EndGameKeyDefinition2Pawns(pk2, pk3) {
  }

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 8148670;
  }
};

class EndGameKeyDefinition5Men2Pawns: public EndGameKeyDefinition2Pawns {
public:
  EndGameKeyDefinition5Men2Pawns(PieceKey pk2, PieceKey pk3, PieceKey pk4) : EndGameKeyDefinition2Pawns(pk2, pk3, pk4) {
  }

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 488920200;
  }
};

class EndGameKeyDefinition4Men2EqualPawns: public EndGameKeyDefinition2EqualPawns {
private:
#ifdef TABLEBASE_BUILDER
  void scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex) const;
#endif

public:
  EndGameKeyDefinition4Men2EqualPawns(PieceKey pk23);

#ifdef TABLEBASE_BUILDER
  void insertInitialPositions(EndGameTablebase &tablebase) const;
#endif

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 4074226;
  }

  SymmetricTransformation getSymTransformation(EndGameKey key) const;

  DECLARE_SELFCHECK;
  DECLARE_KEYSEQUAL;
};

class EndGameKeyDefinition5Men2EqualPawns: public EndGameKeyDefinition2EqualPawns {
private:
#ifdef TABLEBASE_BUILDER
  void scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex) const;
#endif

public:
  EndGameKeyDefinition5Men2EqualPawns(PieceKey pk2, PieceKey pk34);

#ifdef TABLEBASE_BUILDER
  void insertInitialPositions(EndGameTablebase &tablebase) const;
#endif

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 252602012;
  }

  SymmetricTransformation getSymTransformation(EndGameKey key) const;

  DECLARE_SELFCHECK;
  DECLARE_KEYSEQUAL;
};

class EndGameKeyDefinition5Men3Pawns : public EndGameKeyDefinitionDupletsAllowed {
#ifdef TABLEBASE_BUILDER
private:
  PawnOwner m_pawnOwner;
  void setPawnOwners();

  void scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex) const;
#endif

public:
  EndGameKeyDefinition5Men3Pawns(PieceKey pk2, PieceKey pk34);

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 187416810;
  }
  SymmetricTransformation getSymTransformation(EndGameKey key) const;

#ifdef TABLEBASE_BUILDER
  void insertInitialPositions(EndGameTablebase &tablebase) const;
#endif

  DECLARE_SELFCHECK;
  DECLARE_KEYSEQUAL;
};

class EndGameKeyDefinition5Men3EqualPawns : public EndGameKeyDefinitionDupletsAllowed {
#ifdef TABLEBASE_BUILDER
private:
  PawnOwner m_pawnsOwner;
  void setPawnsOwner();

  void scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex) const;
#endif

public:
  EndGameKeyDefinition5Men3EqualPawns(PieceKey pk234);

  EndGamePosIndex keyToIndex(EndGameKey      key  ) const;
  EndGameKey      indexToKey(EndGamePosIndex index) const;

  EndGamePosIndex getIndexSize() const {
    return 62473152;
  }
  SymmetricTransformation getSymTransformation(EndGameKey key) const;

#ifdef TABLEBASE_BUILDER
  void insertInitialPositions(EndGameTablebase &tablebase) const;
#endif

  DECLARE_SELFCHECK;
  DECLARE_KEYSEQUAL;
};
