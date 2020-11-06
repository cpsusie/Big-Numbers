#pragma once

#include <Tcp.h>
#include <Date.h>
#include <CompactHashMap.h>
#include <CompactArray.h>
#include <Timer.h>
#include "IndexedMap.h"

class EndGameSubTablebase {
protected:
  const EndGameKeyDefinition &m_keydef;

public:
  inline EndGameSubTablebase(const EndGameKeyDefinition &keydef) : m_keydef(keydef) {
  }

  virtual String load(ByteCounter *byteCounter = nullptr) = 0;
  virtual void   unload()         = 0;
  virtual bool   isLoaded() const = 0;
  virtual bool   isRemote() const = 0;

  virtual EndGamePositionStatus getPositionStatus(const Game &game, bool swapPlayers) const = 0;
  virtual EndGameResult         getPositionResult(const Game &game, bool swapPlayers) const = 0;
  virtual MoveResultArray      &getAllMoves(      const Game &game, bool swapPlayers, MoveResultArray &a) const = 0; // return a
#if defined(TABLEBASE_BUILDER)
  virtual String loadPacked() = 0;
  virtual bool allKeysFound() const = 0;
#endif

  inline const EndGameKeyDefinition &getKeyDefinition() const {
    return m_keydef;
  }

  inline String getPositionTypeString() const {
    return m_keydef.toString(true);
  }

  inline String getName() const {
    return m_keydef.getName();
  }
};

typedef enum {
  REQUEST_SETGLOBALS
 ,REQUEST_LOAD
 ,REQUEST_LOADPACKED
 ,REQUEST_POSITIONSTATUS
 ,REQUEST_POSITIONRESULT
 ,REQUEST_ALLKEYSFOUND
 ,REQUEST_ALLMOVES
 ,REQUEST_QUIT
} RemoteTablebaseRequest;

typedef enum {
  LOAD_OK
 ,LOAD_FAILED
} RemoteTablebaseReply;

class RemoteEndGameSubTablebase : public EndGameSubTablebase {
private:
  // Clientside (mainprogram)
  static SOCKET s_listenSocket;
  SOCKET        m_socket;
  bool          m_loaded;
  void          sendRequest(RemoteTablebaseRequest requestCode);

  // Serverside (instances of MakeEndGame started as subtablebases by maininstance of MakeEndGame
  static RemoteTablebaseRequest getRequestCode(SOCKET s);
  String load(bool packed);
public:
  RemoteEndGameSubTablebase(const EndGameKeyDefinition &keydef);
  String load(ByteCounter *byteCounter);
  void unload();
  inline bool isLoaded() const {
    return m_loaded;
  }
  inline bool isRemote() const {
    return true;
  }
  EndGamePositionStatus getPositionStatus(const Game &game, bool swapPlayers) const;
  EndGameResult         getPositionResult(const Game &game, bool swapPlayers) const;
  MoveResultArray      &getAllMoves(      const Game &game, bool swapPlayers, MoveResultArray &a) const; // return a

#if defined(TABLEBASE_BUILDER)
  String loadPacked();
  bool allKeysFound() const;
#endif

  static void remoteService(TCHAR **argv); // assume argv points to "-s" argument. Only 1 instance for each process
};

class SubTablebaseKey {
private:
  Player      m_movingPlayer;
  MoveType    m_moveType;
  PieceKey    m_capturedPieceKey;
  PieceType   m_promoteTo;
public:
  SubTablebaseKey();
  SubTablebaseKey(const Move &m);
  ULONG hashCode() const;
  bool operator==(const SubTablebaseKey &key) const;
};

class SubTablebasePositionInfo {
private:
  EndGameSubTablebase  *m_tablebase; // if nullptr result is draw
  bool                  m_swapPlayers;

public:
  SubTablebasePositionInfo();
  SubTablebasePositionInfo(const Game &game);
  EndGamePositionStatus getPositionStatus(const Game &game) const;
  EndGameResult         getPositionResult(const Game &game) const;
  MoveResultArray      &getAllMoves(      const Game &game, MoveResultArray &a) const; // return a
  void unload();
  inline bool isRemote() const {
    return m_tablebase ? m_tablebase->isRemote() : false;
  }
#if defined(TABLEBASE_BUILDER)
  bool allKeysFound() const;
#endif
};

#if defined(TABLEBASE_BUILDER)

typedef enum {
  INSERT_INITIALPOSITIONS
 ,GENERATE_FORWARDPOSITIONS
 ,RECOVER_FROM_FORWARDPOSITIONS
 ,GENERATE_RETROPOSITIONS
 ,RECOVER_FROM_RETROPOSITIONS
 ,ANALYZERETRO
 ,RECOVER_FROM_ALLPOSITIONS
 ,FIXUP_POSITIONS
 ,RECOVER_FROM_FIXED_POSITIONS
 ,SAVE
 ,COMPRESS
 ,BUILD_DONE
} BuildStep;

typedef enum {
  LIST_WINNER
 ,LIST_ALL
 ,LIST_MAXVAR
 ,LIST_EXACTPLIES
 ,LIST_CHECKMATES
 ,LIST_STALEMATES
 ,LIST_UNDEFINED
 ,LIST_PLIESTOEND
 ,LIST_HASHELPINFO
} ListFilter;

#define LOG_TIMER     1
#define VERBOSE_TIMER 2
#endif

class EndGameTablebaseList : public CompactArray<EndGameTablebase*> {
public:
  EndGameTablebaseList() : CompactArray<EndGameTablebase*>(143) {
  }
  ~EndGameTablebaseList() override {
  }
};

#if defined(TABLEBASE_BUILDER)
class EndGameTablebase : public EndGameSubTablebase, public TimeoutHandler {
#else
class EndGameTablebase : public EndGameSubTablebase {
#endif

private:
  mutable IndexedMap                                                m_positionIndex;
  mutable CompactHashMap<SubTablebaseKey, SubTablebasePositionInfo> m_subTablebaseMap;
  mutable bool                                                      m_useRemoteSubTablebase;
  int                                                               m_loadRefCount;
#if !defined(TABLEBASE_BUILDER)
  static  Semaphore                                                 s_loadGate;
#endif

  // assume gameKey.positionSignature == keydef.positionSignature
  MoveResultArray  &getAllMoves(const GameKey &gameKey, MoveResultArray &a) const;
#if !defined(TABLEBASE_BUILDER)
  // assume gameKey.positionSignature == keydef.positionSignature
  MoveResult2Array &getAllMoves(const GameKey &gameKey, MoveResult2Array &a) const;
#endif // TABLEBASE_BUILDER

  void           unloadSubTablebases();
  void           clear();

  inline EndGameKey transformGameKey(   const GameKey    &gameKey) const {
    return m_keydef.transformGameKey(gameKey);
  }

  inline EndGameKey transformEndGameKey(const EndGameKey &key, SymmetricTransformation st) const {
    return (st == 0) ? key : m_keydef.getTransformedKey(key, st);
  }

  inline String  toString(const EndGameKey &key, bool initFormat=false) const {
    return key.toString(m_keydef, initFormat);
  }
  EndGamePositionStatus isTerminalMove(const Game &game, const Move &m, UINT *pliesToEnd = nullptr, MoveResultArray *allMoves = nullptr) const;
  EndGameResult         getKeyResult(  const EndGameKey  key) const;
  // throw Exception
  void                  statusError(EndGamePositionStatus status, const EndGameKey &key) const {
    throwException(_T("Unknown positionstatus:%d. Position:[%s]"), status, toString(key).cstr());
  }
  // throw Exception
  void                  statusError(EndGamePositionStatus status, const Game &game, const Move &m) const {
    throwException(_T("Unknown positionstatus:%d, Position:[%s], Move:%s"), status, toString(game).cstr(), m.toString().cstr());
  }
  void                  missingPositionError(const TCHAR *method, const EndGameKey key, EndGameResult &result);

  void                  load(ByteInputStream &s);

#if defined(TABLEBASE_BUILDER)

  PackedIndexedMap                   *m_packedIndex;
  TablebaseInfo                       m_info;
  bool                                m_bishopInitialField[MAX_ENDGAME_PIECECOUNT][2];
  bool                                m_pawnInitialField[MAX_ENDGAME_PIECECOUNT][8];
  // m_bishopInitialField[i][fieldColor] is true if piece[i] = Bishop
  // and has an initial position on a field with color fieldColor
  mutable bool                        m_buildOk;
  mutable bool                        m_allKeysFound;
  mutable Game                        m_workGame;
  Timer                               m_logTimer, m_verboseTimer;
  bool                                m_generatingPositions;
  mutable bool                        m_verboseTriggered;
  mutable FILE                       *m_logFile;
  mutable int                         m_infoLength;
  UINT64                              m_positionsAnalyzed;

  bool addPosition(const EndGameKey &key, bool markNew);
  bool addPosition(const EndGameKey &key, EndGameResult &result, bool markNew);
  EndGameResult getBestResult(Game &game, bool breakOnDraw);
  inline void adjustMaxPly(Player winner, UINT plies) {
    USHORT &maxPlies = m_info.m_maxPlies.m_count[winner];
    if(plies > maxPlies) maxPlies = plies;
  }
  MaxVariantCount findMaxPlies() const;

  void   resetInitialSetupFlags();
  void   checkInitialSetupFlags();
  String saveAllForwardPositions(bool convert = false);
  String loadAllForwardPositions();
  String saveAllRetroPositions(  bool convert = false);
  String loadAllRetroPositions();
  void   load(const String    &fileName, ByteCounter *byteCounter = nullptr);
  void   loadPacked(const String &fileName);
  void   loadPacked(ByteInputStream &s);
  String save( const String     &fileName, bool convert) const;
  void   save(   ByteOutputStream &s) const;
  void   saveNew(ByteOutputStream &s) const;
  void   fixupBackup(bool force = false);

  void compress(   ByteOutputStream &s);
  void compressNew(ByteOutputStream &s);
  void startLogging();
  void stopLogging();
  void startVerboseTrigger(int sec = 10);
  void stopVerboseTrigger();
  inline bool isVerboseTriggerStarted() const {
    return m_verboseTimer.isRunning();
  }
  void verbose(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) const;
  void handleTimeout(Timer &timer);
  void logPositionCount() const;
  void logUndefinedPosition(const EndGameKey &key) const;
  static void metricError(const TCHAR *method);                                                       // throw Exception

  int  getInfoLength() const;
  const TCHAR *getDottedLine();
  inline EndGameResult &setAsStaleMatePosition(EndGameResult &result) {
    return setAsTerminalPosition(result, EG_DRAW, 0);
  }

  inline EndGameResult &setAsCheckMatePosition(EndGameResult &result, EndGamePositionStatus status) {
    assert(status >= EG_WHITEWIN);
    return setAsTerminalPosition(result, status, 0);
  }

  EndGameResult &setPositionResult(    EndGameResult &dst, const EndGameResult &result);
  EndGameResult &changePliesToEnd(     EndGameResult &dst, const EndGameResult &result);

  EndGameResult &setAsTerminalPosition(EndGameResult &dst, EndGamePositionStatus status, UINT pliesToEnd);
  EndGameResult &setPositionResult(    EndGameResult &dst, EndGamePositionStatus status, UINT pliesToEnd);
  EndGameResult &changePliesToEnd(     EndGameResult &dst, EndGamePositionStatus status, UINT pliesToEnd);

  BuildStep    getFirstBuildStep(bool recover) const;
  void         doBuild(BuildStep buildStep);
  void         checkBuildOk();
  void         generateAllForwardPositions();
  void         generateAllRetroPositions();
  bool         addSuccessors(EndGameResult &result, Game &game, bool retroPosition);
  void         findDTM();
  void         unravelWinnerPositions(int minPliesToEnd = -1);
  bool         analyzeRetro(const EndGameEntry &entry, PositionCount64 &winnerCount, UINT pliesToEnd);
  bool         fixupPositions();
  void         fixupNonWinnerPositions(UINT64 &changedPositions);
  void         fixupPlies(             UINT64 &changedPositions);

  UINT64       fixupRetroStatus(const EndGameEntry &entry, PositionCount64 &changeCount);
  void         fixupForwardPlies(EndGameEntryIterator &it, int iteration, UINT64 &changedPositions);
  UINT64       fixupRetroPlies( const EndGameEntry &entry, PositionCount64 &changeCount);
  void         markPredecessors(const EndGameEntry &entry, bool onlyWinnerPositions);
  void         markPredecessors(bool onlyWinnerPositions); // use m_workGame as startPosition
  void         verboseHelpInfo(const String &label);

  void list(FILE *f, EndGameEntryIterator &it);
  void listPositionCount(FILE *f);

  CompactArray<PositionCount64> getWinnerPositionCountArray();

  String toString(           const EndGameEntry &entry, bool ply=false) const; // same default as EndGameResult::toString
  TCHAR *toStr(  TCHAR *dst, const EndGameEntry &entry, bool ply=false) const;

#else
  void                  decompress(ByteInputStream &s) const;
#endif // TABLEBASE_BUILDER

protected:

  void                  capturedPieceTypeError(PieceType pieceType) const;       // throws Exception
  void                  moveTypeError(const Move &m) const;                      // throws Exception

#if defined(TABLEBASE_BUILDER)

  virtual bool          isUsableMove(const Move &m) const;
  virtual bool          isUsableBackMove(const Move &m) const {
    return true;
  }
  void                  checkAllSubKeysFound() const;                            // can throw Exception

#endif // TABLEBASE_BUILDER

public:
  EndGameTablebase(const EndGameKeyDefinition &keydef);
  virtual ~EndGameTablebase();

  String                load(ByteCounter *byteCounter = nullptr);
  void                  unload();
  inline int            getLoadRefCount() const {
    return m_loadRefCount;
  }
  // Never return EG_UNDEFINED. Possible return values are { EG_DRAW, EG_WHITEWIN. EG_BLACKWIN }
  EndGamePositionStatus getPositionStatus(const Game &game, bool swapPlayers) const;
  // Never return EG_UNDEFINED i status. possible status for return value same as getPositionStatus()
  // Values also encode { EGR_STALEMATE, EGR_BLACKISMATE, EGR_WHITEISMATE, EGR_DRAW
  EndGameResult         getPositionResult(const Game &game, bool swapPlayers) const;

  // assume game.positionSignature.match(keydef.positionSignature); ArrayType = MoveResultArray,MoveResult2Array
  template<class ArrayType> ArrayType &getAllMoves(const Game &game, ArrayType &a) const {
    return getAllMovesTemplate(game, m_keydef.getPlayTransformation(game) == TRANSFORM_SWAPPLAYERS, a);
  }
  template<class ArrayType> ArrayType &getAllMovesTemplate(const Game &game, bool swapPlayers, ArrayType &a) const {
    if(!swapPlayers) {
      getAllMoves(game.getKey(), a);
    } else {
      const GameKey trKey  = game.getKey().transform(TRANSFORM_SWAPPLAYERS);
      ArrayType     trMoves;
      getAllMoves(trKey, trMoves);
      const size_t n = trMoves.size();
      a.clear(game.getPlayerInTurn(), n);
      for(size_t i = 0; i < n; i++) {
        a.add(trMoves[i].transform(game, true));
      }
    }
    return a;
  }

  MoveResultArray &getAllMoves(const Game &game, bool swapPlayers, MoveResultArray &a) const {
    return getAllMovesTemplate(game, swapPlayers, a);
  }
  // defendStrength = [0..100]
  template<class ArrayType> PrintableMove findBestMove(const Game &game, ArrayType &a, int defendStrength) const {
    return getAllMoves(game, a).isEmpty()
         ? PrintableMove()
         : PrintableMove(game, a.selectBestMove(defendStrength));
  }

#if !defined(TABLEBASE_BUILDER)
#if defined(NEWCOMPRESSION)
  friend class GameResultMap;
#endif
#endif //  TABLEBASE_BUILDER

  inline String         getTbFileName(TablebaseFileType fileType) const {
    return m_keydef.getTbFileName(fileType);
  }
  inline bool           tbFileExist(      TablebaseFileType fileType) const {
    return ACCESS(getTbFileName(fileType),0) == 0;
  }
  inline __time64_t     getTbFileTime(TablebaseFileType fileType) const {
    return STAT(getTbFileName(fileType)).st_mtime;
  }
  inline UINT64         getTbFileSize(TablebaseFileType fileType) const {
    return STAT64(getTbFileName(fileType)).st_size;
  }

  inline bool isLoaded() const {
    return m_positionIndex.isAllocated();
  }

  inline bool contains(const EndGameKey &key) const {
    return m_positionIndex.isAllocated() && m_positionIndex.get(key).exists();
  }

  inline bool isRemote() const {
    return false;
  }

  // throws Exception
  static void gameResultError(const Game &game) {
    throwException(_T("Invalid gameResult for position [%s]:%d"), game.getPositionSignature().toString().cstr(), game.findGameResult());
  }

  inline String toString(const Game &game) const {
    return m_keydef.getEndGameKey(game.getKey()).toString(m_keydef);
  }

  static const EndGameTablebaseList &getRegisteredEndGameTablebases();
  static       EndGameTablebaseList  getExistingEndGameTablebases();
  static       EndGameTablebase     &getInstanceByName(const String &name);
  static       EndGameTablebase     *getInstanceBySignature(const PositionSignature &signature, bool &swap);
  inline const IndexedMap &getIndex() const {
    return m_positionIndex;
  }

  TablebaseInfo         getInfo() const;

#if !defined(TABLEBASE_BUILDER)
  static void           decompressAll();
  void                  decompress(ByteCounter *byteCounter = nullptr) const;
  bool                  needDecompress() const;
  static void           listNewSizes();
  static void           dumpRangeTables();
#else
  void                  build(bool recover=false);
  void                  buildDTM();
  bool                  needBuild(bool recover) const;
  String                loadPacked();
  String                save(    bool convert=false) const;
  String                compress(bool convert=false);
  void                  convert();
  void                  list(FILE *f, ListFilter filter);
  void                  list(FILE *f, ListFilter filter, bool whiteWin, bool blackWin, int plies = -1);
  bool                  checkConsistency(UINT flags);
  bool                  allSubKeysFound() const;
  void                  addInitPosition(Player playerInTurn, ...);
  void                  addInitPosition(EndGameKey key, bool allowTransform);
  bool allKeysFound() const {
    return m_allKeysFound;
  }

  void insertManualPositions();

#endif // TABLEBASE_BUILDER

  friend class TimeUsagePrinter;
};

#if defined(TABLEBASE_BUILDER)

class TimeUsagePrinter {
private:
  const Timestamp   m_startTime;
  const String      m_label;
  EndGameTablebase &m_tablebase;
public:
  TimeUsagePrinter(EndGameTablebase *tablebase, const String &label = _T("Used time")) : m_tablebase(*tablebase), m_label(label) {
  }
  ~TimeUsagePrinter() {
    m_tablebase.verbose(_T("%s:%s\n"), m_label.cstr(), secondsToString(Timestamp::diff(m_startTime, Timestamp(), TMILLISECOND), false).cstr());
  }
};


#define CHECK_HEADER          0x01
#define CHECK_POSITIONS       0x02
#define CHECK_RETURN_ON_ERROR 0x04
#define CHECK_LIST_MOVES      0x08

#endif // TABLEBASE_BUILDER
