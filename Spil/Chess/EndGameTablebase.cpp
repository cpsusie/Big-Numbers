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

#endif

void EndGameTablebase::clear() {
#ifdef TABLEBASE_BUILDER
  m_info.clear();
  SAFEDELETE(m_packedIndex);
#endif
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
  if(!swapPlayers) {
    return egr;
  } else {
    return EndGameResult(transformPositionStatus(egr.getStatus(), swapPlayers), egr.getPliesToEnd());
  }
}

static MoveWithResult transformMove(const Game game, const MoveWithResult &m, bool swapPlayers) {
  if(!swapPlayers) {
    return m;
  } else {
    return MoveWithResult(transformMove(game, (const MoveBase&)m, swapPlayers), transformEndGameResult(m.m_result, swapPlayers));
  }
}

PrintableMove EndGameTablebase::findBestMove(const Game &game, MoveResultArray &allMoves, int defendStrength) const {
  allMoves = getAllMoves(game);
  return allMoves.isEmpty()
       ? PrintableMove()
       : PrintableMove(game, allMoves.selectBestMove(defendStrength));
}

MoveResultArray EndGameTablebase::getAllMoves(const Game &game) const {
  const SymmetricTransformation pst      = m_keydef.getPlayTransformation(game);
  const GameKey                 gameKey  = game.getKey().transform(pst);
  const MoveResultArray         rawMoves = getAllMoves(gameKey);

  if(pst == 0) {
    return rawMoves;
  } else {
    MoveResultArray result(game.getPlayerInTurn());
    for(size_t i = 0; i < rawMoves.size(); i++) {
      result.add(transformMove(game, rawMoves[i], true));
    }
    return result;
  }
}

EndGamePositionStatus EndGameTablebase::isTerminalMove(const Game &game, const Move &m, UINT *pliesToEnd) const {
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
          if(pliesToEnd) *pliesToEnd = 1;
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

#endif

MoveResultArray EndGameTablebase::getAllMoves(const GameKey &gameKey) const {
  assert(gameKey.getPositionSignature() == m_keydef.getPositionSignature());

  const Player playerInTurn = gameKey.getPlayerInTurn();

  MoveResultArray result(playerInTurn);
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
      result.add(MoveWithResult(m, EGR_TERMINALDRAW));
      break;
    case EG_WHITEWIN :
    case EG_BLACKWIN :
      { const EndGameResult succResult(status, pliesToEnd);
        result.add(MoveWithResult(m, succResult));
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
        const MoveResultArray rma = getAllMoves(game.getKey());
        const MoveWithResult  rm  = rma.selectBestMove(100);
        if(rm.m_result.getStatus() <= EG_DRAW) {
          result.add(MoveWithResult(m, EGR_DRAW));
        } else {
          result.add(MoveWithResult(m, EndGameResult(rm.m_result.getStatus(), rm.m_result.getPliesToEnd()+1)));
        }
      } else {
        const EndGameKey    succKey = transformGameKey(game.getKey());
        const EndGameResult egr     = m_positionIndex.get(succKey);
        if(!egr.isDefined()) {
          result.add(MoveWithResult(m, EGR_DRAW));
        } else {
          result.add(MoveWithResult(m, egr));
        }
      }
      break;
    default:
      statusError(status, game, m);
    }
  }
  game.popState();
  return result;
}

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
#endif

EndGamePositionStatus EndGameTablebase::getPositionStatus(const Game &game, bool swapPlayers) const { // never return EG_UNDEFINED
  const SymmetricTransformation st     = swapPlayers ? TRANSFORM_SWAPPLAYERS : 0;
  const EndGameKey              mapKey = transformGameKey(game.getKey().transform(st));
  EndGamePositionStatus         result;

#ifdef TABLEBASE_BUILDER
  if(m_packedIndex) {
    result = m_packedIndex->getPositionStatus(mapKey);
  } else
#endif
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

EndGameResult EndGameTablebase::getPositionResult(const Game &game, bool swapPlayers) const { // never return EG_UNDEFINED
  const SymmetricTransformation st     = swapPlayers ? TRANSFORM_SWAPPLAYERS : 0;
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
    throwException(_T("Tablebase <%s>:getKeyResult:Tablebase not loaded"), getName().cstr());
  }
  const EndGameResult egr = m_positionIndex.get(key);
  if(egr.exists()) {
    return egr.isDefined() ? egr : EGR_DRAW;
  } else {
    return EndGameResult(EG_UNDEFINED, 0);
  }
}
