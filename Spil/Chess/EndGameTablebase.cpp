#include "stdafx.h"
#include <io.h>
#include <ByteFile.h>
#include <CompressFilter.h>
#include "EndGameTablebase.h"

EndGameTablebase::EndGameTablebase(const EndGameKeyDefinition &keydef)
: EndGameSubTablebase(keydef)
, m_positionIndex(keydef)
#ifdef TABLEBASE_BUILDER
, m_logTimer(LOG_TIMER)
, m_verboseTimer(VERBOSE_TIMER)
#endif
{

  m_useRemoteSubTablebase = false;
  m_loadRefCount          = 0;
#ifdef TABLEBASE_BUILDER
  m_packedIndex           = NULL;
  resetInitialSetupFlags();
  m_buildOk               = true;
  m_allKeysFound          = true;
  m_generatingPositions   = false;
  m_verboseTriggered      = false;
  m_logFile               = NULL;
  m_infoLength            = 0;
  m_positionsAnalyzed     = 0;
#endif
}

EndGameTablebase::~EndGameTablebase() {
}

#ifndef TABLEBASE_BUILDER
Semaphore EndGameTablebase::s_loadGate;
#define BEGIN_LOADUNLOAD_CRITICAL_SECTION s_loadGate.wait()
#define END_LOADUNLOAD_CRITICAL_SECTION   s_loadGate.signal()
#else
#define BEGIN_LOADUNLOAD_CRITICAL_SECTION
#define END_LOADUNLOAD_CRITICAL_SECTION
#endif

String EndGameTablebase::load(ByteCounter *byteCounter) {
  BEGIN_LOADUNLOAD_CRITICAL_SECTION;
  m_loadRefCount++;
  if(isLoaded()) {
    END_LOADUNLOAD_CRITICAL_SECTION;
    return EMPTYSTRING;
  }
  String result;
#ifdef TABLEBASE_BUILDER
  if(!exist(ALLTABLEBASE)) {
    build(true);
  } else {
    try {
      const String fileName = getFileName(ALLTABLEBASE);
      load(fileName, byteCounter);
      return fileName;
    } catch(Exception e) {
      verbose(_T("\nError:%s\n"), e.what());
      build();
    }
  }
  load(result = getFileName(ALLTABLEBASE), byteCounter);
#else
  if(needDecompress()) {
    decompress(byteCounter);
  }
  result = getFileName(DECOMPRESSEDTABLEBASE);
  verbose(_T("Loading %s-tablebase from %s...\n"), getName().cstr(), result.cstr());
  load(ByteInputFile(result));
#endif
  END_LOADUNLOAD_CRITICAL_SECTION;
  return result;
}

void EndGameTablebase::unload() {
  BEGIN_LOADUNLOAD_CRITICAL_SECTION;
  if(--m_loadRefCount > 0)  {
    END_LOADUNLOAD_CRITICAL_SECTION;
    return;
  }
  clear();
  END_LOADUNLOAD_CRITICAL_SECTION;
  unloadSubTablebases();
}

#ifndef TABLEBASE_BUILDER

TablebaseInfo EndGameTablebase::getInfo() const {
  TablebaseInfo result;
  if(exist(COMPRESSEDTABLEBASE)) {
    result.load(DecompressFilter(ByteInputFile(getFileName(COMPRESSEDTABLEBASE))));
  }
  return result;
}

void EndGameTablebase::decompress(ByteCounter *byteCounter) const {
  const String fileName = getFileName(COMPRESSEDTABLEBASE);
  decompress(DecompressFilter(CountedByteInputStream(byteCounter?*byteCounter:(ByteCounter&)StreamProgress(fileName), ByteInputFile(fileName))));
}

#ifndef NEWCOMPRESSION

void EndGameTablebase::decompress(ByteInputStream &s) const {
  TablebaseInfo info;
  info.load(s);
  m_positionIndex.decompress(s, info);
}

#else // NEWCOMPRESSION

void EndGameTablebase::decompress(ByteInputStream &s) const {
  BigEndianInputStream bes(s);
  TablebaseInfo info;
  info.load(bes);
  m_positionIndex.decompress(bes, info);
}
#endif // NEWCOMPRESSION

void EndGameTablebase::load(ByteInputStream &s) {
  m_positionIndex.load();
}

#endif // TABLEBASE_BUILDER

void EndGameTablebase::clear() {
#ifdef TABLEBASE_BUILDER
  m_info.clear();
  SAFEDELETE(m_packedIndex);
#endif // TABLEBASE_BUILDER
  m_positionIndex.clear();
}

void EndGameTablebase::unloadSubTablebases() {
  for(Iterator<Entry<SubTablebaseKey, SubTablebasePositionInfo> > it = m_subTablebaseMap.getEntryIterator(); it.hasNext();) {
    it.next().getValue().unload();
  }
  m_subTablebaseMap.clear();
  m_useRemoteSubTablebase = false;
}

static MoveBase transformMove(const Game &game, const MoveBase &m, bool swapPlayers) {
  if(!m.isMove() || !swapPlayers) {
    return m;
  }
  const int from = GameKey::transform(m.m_from, TRANSFORM_SWAPPLAYERS);
  const int to   = GameKey::transform(m.m_to  , TRANSFORM_SWAPPLAYERS);
  return game.generateMove(from, to, m.getPromoteTo());
}

static EndGamePositionStatus transformPositionStatus(EndGamePositionStatus status, bool swapPlayers) {
  if(!swapPlayers) {
    return status;
  } else {
    switch(status) {
    case EG_UNDEFINED:
    case EG_DRAW     : return status;
    case EG_WHITEWIN : return EG_BLACKWIN;
    case EG_BLACKWIN : return EG_WHITEWIN;
    default          : throwInvalidArgumentException(__TFUNCTION__, _T("status=%d"), status);
    }
  }
  return EG_UNDEFINED;
}

static EndGameResult transformEndGameResult(EndGameResult egr, bool swapPlayers) {
  return swapPlayers ? EndGameResult(transformPositionStatus(egr.getStatus(), true), egr.getPliesToEnd()) : egr;
}

static MoveWithResult transformMove(const Game &game, const MoveWithResult &m, bool swapPlayers) {
  return swapPlayers ? MoveWithResult(transformMove(game, (const MoveBase&)m, true), transformEndGameResult(m.getResult(), true)) : m;
}

static MoveWithResult2 transformMove(const Game &game, const MoveWithResult2 &m, bool swapPlayers) {
  return swapPlayers ? MoveWithResult2(transformMove(game, (MoveWithResult&)m, swapPlayers), m.getReplyCount()) : m;
}

PrintableMove EndGameTablebase::findBestMove(const Game &game, MoveResultArray &allMoves, int defendStrength) const {
  return getAllMoves(game, allMoves).isEmpty()
       ? PrintableMove()
       : PrintableMove(game, allMoves.selectBestMove(defendStrength));
}

EndGamePositionStatus EndGameTablebase::isTerminalMove(const Game &game, const Move &m, UINT *pliesToEnd, MoveResultArray *allMoves) const {
  if((m.m_capturedPiece == NULL) && (m.m_type != PROMOTION)) {
    return EG_UNDEFINED;
  } else {
    const SubTablebaseKey key(m);
    SubTablebasePositionInfo *sps = m_subTablebaseMap.get(key);
    if(sps == NULL) {
      m_subTablebaseMap.put(key, SubTablebasePositionInfo(game));
      sps = m_subTablebaseMap.get(key);
      if(sps->isRemote()) {
        m_useRemoteSubTablebase = true;
      }
    }
    EndGamePositionStatus status;
    switch(EndGameKeyDefinition::getMetric()) {
    case DEPTH_TO_CONVERSION:
      switch(status = sps->getPositionStatus(game)) {
      case EG_WHITEWIN:
      case EG_BLACKWIN:
        if(pliesToEnd) *pliesToEnd = 1;
        return status;
      case EG_DRAW    :
        if(pliesToEnd) *pliesToEnd = 1;
        if(allMoves  )  sps->getAllMoves(game, *allMoves);
        return status;
      }
      break;
    case DEPTH_TO_MATE      :
      { const EndGameResult egr = sps->getPositionResult(game);
        status = egr.getStatus();
        switch(status) {
        case EG_WHITEWIN:
        case EG_BLACKWIN:
          if(pliesToEnd) *pliesToEnd = egr.getPliesToEnd();
          return status;
        case EG_DRAW    :
          if(pliesToEnd) *pliesToEnd = egr.getPliesToEnd();
          if(allMoves  )  sps->getAllMoves(game, *allMoves);
          return status;
        }
      }
      break;
    default:
      throwException(_T("%s:Unknown metric:%d"), __TFUNCTION__, EndGameKeyDefinition::getMetric());
    }
    throwException(_T("%s:Unexpected terminal status for key[%s], move:[%s]:%d")
                  ,__TFUNCTION__
                  ,transformGameKey(game.getKey()).toString(m_keydef).cstr()
                  ,m.toString().cstr()
                  ,status
                  );
    return EG_UNDEFINED;
  }
}

EndGameKey EndGameTablebase::transformGameKey(const GameKey &gameKey) const {
  assert(gameKey.getPositionSignature() == m_keydef.getPositionSignature());
  const EndGameKey              egk = m_keydef.getEndGameKey(gameKey);
  const SymmetricTransformation st  = m_keydef.getSymTransformation(egk);
  return (st == 0) ? egk : m_keydef.getTransformedKey(egk, st);
}

EndGameKey EndGameTablebase::transformEndGameKey(const EndGameKey &egk, SymmetricTransformation st) const {
  return (st == 0) ? egk : m_keydef.getTransformedKey(egk, st);
}

bool EndGameTablebase::exist(TablebaseFileType fileType) const {
  return ACCESS(getFileName(fileType),0) == 0;
}

__time64_t EndGameTablebase::getFileTime(TablebaseFileType fileType) const {
  return STAT(getFileName(fileType)).st_mtime;
}

UINT64 EndGameTablebase::getFileSize(TablebaseFileType fileType) const {
  return STAT(getFileName(fileType)).st_size;
}

String EndGameTablebase::getFileName(TablebaseFileType fileType) const {
  switch(fileType) {
#ifdef TABLEBASE_BUILDER
  case ALLFORWARDPOSITIONS  : return getAllForwardPositionsFileName();    // only available to makeEndGame
  case ALLRETROPOSITIONS    : return getAllRetroPositionsFileName();      // only available to makeEndGame
  case ALLTABLEBASE         : return m_keydef.getTablebaseFileName();     // only available to makeEndGame
  case UNDEFINEDKEYSLOG     : return getUndefinedKeyLogFileName();        // only available to makeEndGame
#else
  case DECOMPRESSEDTABLEBASE: return m_keydef.getDecompressedFileName();  // only available for chess-program
#endif
  case COMPRESSEDTABLEBASE  : return m_keydef.getCompressedFileName();    // available to both makeEndGame and chess-program
  default                   : throwInvalidArgumentException(__TFUNCTION__, _T("fileType=%d"), fileType);
  }
  return EMPTYSTRING;
}

void EndGameTablebase::capturedPieceTypeError(PieceType pieceType) const { // throws an Exception
  throwException(_T("%s:Unexpected piecetype for captured piece:%d (%s)")
                ,getName().cstr()
                ,pieceType
                ,getPieceTypeNameEnglish(pieceType)
                );
}

void EndGameTablebase::moveTypeError(const Move &m) const {         // throws Exception
  throwException(_T("%s:Unexpected movetype:%d (%s)")
                ,getName().cstr()
                ,m.getType()
                ,getMoveTypeName(m.getType()).cstr()
                );
}

void EndGameTablebase::statusError(EndGamePositionStatus status, const EndGameKey &key) const {
  throwException(_T("Unknown positionstatus:%d. Position:[%s]"), status, toString(key).cstr());
}

void EndGameTablebase::statusError(EndGamePositionStatus status, const Game &game, const Move &m) const {
  throwException(_T("Unknown positionstatus:%d, Position:[%s], Move:%s"), status, toString(game).cstr(), m.toString().cstr());
}

void EndGameTablebase::gameResultError(const Game &game) { // static
  throwException(_T("Invalid gameResult for position [%s]:%d"), game.getPositionSignature().toString().cstr(), game.findGameResult());
}

#ifndef TABLEBASE_BUILDER

bool EndGameTablebase::needDecompress() const {
  return !exist(DECOMPRESSEDTABLEBASE)
      || (exist(COMPRESSEDTABLEBASE) && (getFileTime(DECOMPRESSEDTABLEBASE) < getFileTime(COMPRESSEDTABLEBASE)));
}

#endif //  TABLEBASE_BUILDER

MoveResultArray &EndGameTablebase::getAllMoves(const Game &game, MoveResultArray &a) const {
  return getAllMoves(game, m_keydef.getPlayTransformation(game) == TRANSFORM_SWAPPLAYERS, a);
}

MoveResultArray &EndGameTablebase::getAllMoves(const Game &game, bool swapPlayers, MoveResultArray &a) const {
  if(!swapPlayers) {
    return getAllMoves(game.getKey(), a);
  } else {
    const GameKey   trKey  = game.getKey().transform(TRANSFORM_SWAPPLAYERS);
    MoveResultArray trMoves;
    getAllMoves(trKey, trMoves);
    const size_t n = trMoves.size();
    a.clear(game.getPlayerInTurn(), n);
    for(size_t i = 0; i < n; i++) {
      a.add(transformMove(game, trMoves[i], true));
    }
    return a;
  }
}

MoveResultArray &EndGameTablebase::getAllMoves(const GameKey &gameKey, MoveResultArray &a) const {
  assert(gameKey.getPositionSignature() == m_keydef.getPositionSignature());

  const Player playerInTurn = gameKey.getPlayerInTurn();

  a.clear(playerInTurn);
  Game            game = gameKey;
  Move            m;
  m.clearAnnotation();
  const Piece    *skipNextPromotion = NULL;

  MoveGenerator &mg = game.getMoveGenerator();
  game.pushState();
  for(bool more = mg.firstMove(m); more; game.unTryMove(), more = mg.nextMove(m)) {
    game.tryMove(m);
    if(skipNextPromotion) {
      if(m.m_piece == skipNextPromotion) {
        continue;
      } else {
        skipNextPromotion = NULL;
      }
    }
    UINT pliesToEnd;
    const EndGamePositionStatus status = isTerminalMove(game, m, &pliesToEnd);
    switch(status) {
    case EG_DRAW     :
      a.add(MoveWithResult(m, EGR_TERMINALDRAW));
      break;
    case EG_WHITEWIN :
    case EG_BLACKWIN :
      { const EndGameResult succResult(status, pliesToEnd);
        a.add(MoveWithResult(m, succResult));
#ifndef TABLEBASE_BUILDER
        if(m.getType() == PROMOTION) {
          const EndGameResult keyResult = getKeyResult(transformGameKey(gameKey));
          if(  (succResult.getStatus()         == keyResult.getStatus())
            && (succResult.getPliesToEnd() + 1 == keyResult.getPliesToEnd())) {
            skipNextPromotion = m.m_piece; // got the best promotion, no need to load the others, same trick as getBestResult
          }
        }
#endif
      }
      break;
    case EG_UNDEFINED:
      if(game.getKey().getEPSquare() > 0) {
        MoveResultArray allReplies;
        const EndGameResult egr = getAllMoves(game.getKey(), allReplies).selectBestMove(100).getResult();
        if(!egr.isWinner()) {
          a.add(MoveWithResult(m, allReplies.isEmpty() ? EGR_STALEMATE : EGR_DRAW));
        } else {
          a.add(MoveWithResult(m, EndGameResult(egr.getStatus(), egr.getPliesToEnd()+1)));
        }
      } else {
        const EndGameKey    succKey = transformGameKey(game.getKey());
        const EndGameResult egr     = m_positionIndex.get(succKey);
        if(!egr.isDefined()) {
          a.add(MoveWithResult(m, EGR_DRAW));
        } else {
          a.add(MoveWithResult(m, egr));
        }
      }
      break;
    default:
      statusError(status, game, m);
    }
  }
  game.popState();
  return a;
}

#ifndef TABLEBASE_BUILDER
PrintableMove EndGameTablebase::findBestMove(const Game &game, MoveResult2Array &allMoves, int defendStrength) const {
  return getAllMoves2(game, allMoves).isEmpty()
       ? PrintableMove()
       : PrintableMove(game, allMoves.selectBestMove(defendStrength));
}

MoveResult2Array &EndGameTablebase::getAllMoves2(const Game &game, MoveResult2Array &a) const {
  return getAllMoves2(game, m_keydef.getPlayTransformation(game) == TRANSFORM_SWAPPLAYERS,a);
}

MoveResult2Array &EndGameTablebase::getAllMoves2(const Game &game, bool swapPlayers, MoveResult2Array &a) const {
  if(!swapPlayers) {
    getAllMoves2(game.getKey(), a);
  } else {
    const GameKey    trKey  = game.getKey().transform(TRANSFORM_SWAPPLAYERS);
    MoveResult2Array trMoves;
    getAllMoves2(trKey, trMoves);
    const size_t n = trMoves.size();
    a.clear(game.getPlayerInTurn(), n);
    for(size_t i = 0; i < n; i++) {
      a.add(transformMove(game, trMoves[i], true));
    }
  }
  return a;
}

MoveResult2Array &EndGameTablebase::getAllMoves2(const GameKey &gameKey, MoveResult2Array &a) const {
  assert(gameKey.getPositionSignature() == m_keydef.getPositionSignature());

  const Player playerInTurn = gameKey.getPlayerInTurn();

  a.clear(playerInTurn);
  Game            game = gameKey;
  Move            m;
  m.clearAnnotation();
  const Piece    *skipNextPromotion = NULL;

  MoveGenerator &mg = game.getMoveGenerator();
  game.pushState();
  for(bool more = mg.firstMove(m); more; game.unTryMove(), more = mg.nextMove(m)) {
    game.tryMove(m);
    if(skipNextPromotion) {
      if(m.m_piece == skipNextPromotion) {
        continue;
      } else {
        skipNextPromotion = NULL;
      }
    }
    UINT            pliesToEnd;
    MoveResultArray allReplies;
    const EndGamePositionStatus status = isTerminalMove(game, m, &pliesToEnd, &allReplies);
    switch(status) {
    case EG_DRAW     :
      // cannot use pliesToEnd...it's always zero. check for allReplies.isEmpty() instead
      a.add(MoveWithResult2(m, allReplies.isEmpty()?EGR_STALEMATE:EGR_DRAW, allReplies));
      break;
    case EG_WHITEWIN :
    case EG_BLACKWIN :
      { const EndGameResult succResult(status, pliesToEnd);
        a.add(MoveWithResult2(m, succResult));
        if(m.getType() == PROMOTION) {
          const EndGameResult keyResult = getKeyResult(transformGameKey(gameKey));
          if(  (succResult.getStatus()         == keyResult.getStatus())
            && (succResult.getPliesToEnd() + 1 == keyResult.getPliesToEnd())) {
            skipNextPromotion = m.m_piece; // got the best promotion, no need to load the others, same trick as getBestResult
          }
        }
      }
      break;
    case EG_UNDEFINED:
      if(game.getKey().getEPSquare() > 0) {
        const EndGameResult egr = getAllMoves(game.getKey(), allReplies).selectBestMove(100).getResult();
        if(!egr.isWinner()) {
          a.add(MoveWithResult2(m, allReplies.isEmpty() ? EGR_STALEMATE : EGR_DRAW, allReplies));
        } else {
          a.add(MoveWithResult2(m, EndGameResult(egr.getStatus(), egr.getPliesToEnd()+1)));
        }
      } else {
        const EndGameKey    succKey = transformGameKey(game.getKey());
        const EndGameResult egr     = m_positionIndex.get(succKey);
        if(!egr.isWinner()) {
          getAllMoves(game.getKey(), allReplies);
          a.add(MoveWithResult2(m, allReplies.isEmpty() ? EGR_STALEMATE : EGR_DRAW, allReplies));
        } else {
          a.add(MoveWithResult2(m, egr));
        }
      }
      break;
    default:
      statusError(status, game, m);
    }
  }
  game.popState();
  return a;
}

#endif // TABLEBASE_BUILDER

#ifdef TABLEBASE_BUILDER
#define LOGUNDEFINEDKEY                                                       \
{ const EndGameKey normalizedKey = m_keydef.getNormalizedKey(mapKey);         \
  logUndefinedPosition(normalizedKey);                                        \
  verbose(_T("Warning:Position [%s] not found in index for tablebase <%s>\n") \
         ,toString(normalizedKey).cstr()                                      \
         ,getName().cstr()                                                    \
         );                                                                   \
}
#else
#define LOGUNDEFINEDKEY
#endif // TABLEBASE_BUILDER

EndGamePositionStatus EndGameTablebase::getPositionStatus(const Game &game, bool swapPlayers) const { // never return EG_UNDEFINED
  const SymmetricTransformation st     = swapPlayers ? TRANSFORM_SWAPPLAYERS : TRANSFORM_NONE;
  const EndGameKey              mapKey = transformGameKey(game.getKey().transform(st));
  EndGamePositionStatus         result;

#ifdef TABLEBASE_BUILDER
  if(m_packedIndex) {
    result = m_packedIndex->getPositionStatus(mapKey);
  } else
#endif // TABLEBASE_BUILDER
    result = getKeyResult(mapKey).getStatus();

  if(result != EG_UNDEFINED) {
    return transformPositionStatus(result, swapPlayers);
  }
  const GameResult gameResult = game.findGameResult();
  switch(gameResult) {
  case STALEMATE      : return EG_DRAW;
  case WHITE_CHECKMATE: return swapPlayers ? EG_WHITEWIN : EG_BLACKWIN;
  case BLACK_CHECKMATE: return swapPlayers ? EG_BLACKWIN : EG_WHITEWIN;
  }
  LOGUNDEFINEDKEY
  return EG_DRAW;
}

EndGameResult EndGameTablebase::getPositionResult(const Game &game, bool swapPlayers) const { // Never return EG_UNDEFINED
  const SymmetricTransformation st     = swapPlayers ? TRANSFORM_SWAPPLAYERS : TRANSFORM_NONE;
  const EndGameKey              mapKey = transformGameKey(game.getKey().transform(st));
  EndGameResult                 result;

#ifdef TABLEBASE_BUILDER
  if(m_packedIndex) {
    result = m_packedIndex->getPositionResult(mapKey);
  } else
#endif
    result = getKeyResult(mapKey);

  if(result.getStatus() != EG_UNDEFINED) {
    return transformEndGameResult(result, swapPlayers);
  }
  const GameResult gameResult = game.findGameResult();
  switch(gameResult) {
  case STALEMATE      : return EGR_STALEMATE;
  case WHITE_CHECKMATE: return swapPlayers ? EGR_BLACKISMATE : EGR_WHITEISMATE;
  case BLACK_CHECKMATE: return swapPlayers ? EGR_WHITEISMATE : EGR_BLACKISMATE;
  }
  LOGUNDEFINEDKEY
  return EGR_DRAW;
}

EndGameResult EndGameTablebase::getKeyResult(const EndGameKey key) const {
  if(!m_positionIndex.isAllocated()) {
    throwException(_T("Tablebase <%s>:%s:Tablebase not loaded"), getName().cstr(), __TFUNCTION__);
  }
  const EndGameResult egr = m_positionIndex.get(key);
  if(egr.exists()) {
    return egr.isDefined() ? egr : EGR_DRAW;
  } else {
    return EndGameResult(EG_UNDEFINED, 0);
  }
}
