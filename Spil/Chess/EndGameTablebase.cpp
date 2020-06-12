#include "stdafx.h"
#include <io.h>
#include <ByteFile.h>
#include <CompressFilter.h>
#include "EndGameTablebase.h"

EndGameTablebase::EndGameTablebase(const EndGameKeyDefinition &keydef)
: EndGameSubTablebase(keydef)
, m_positionIndex(keydef)
#if defined(TABLEBASE_BUILDER)
, m_logTimer(LOG_TIMER)
, m_verboseTimer(VERBOSE_TIMER)
#endif
{

  m_useRemoteSubTablebase = false;
  m_loadRefCount          = 0;
#if defined(TABLEBASE_BUILDER)
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

#if !defined(TABLEBASE_BUILDER)
FastSemaphore EndGameTablebase::s_loadGate;
#define BEGIN_LOADUNLOAD_CRITICAL_SECTION s_loadGate.wait()
#define END_LOADUNLOAD_CRITICAL_SECTION   s_loadGate.notify()
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
#if defined(TABLEBASE_BUILDER)
  if(!tbFileExist(ALLTABLEBASE)) {
    build(true);
  } else {
    try {
      const String fileName = getTbFileName(ALLTABLEBASE);
      load(fileName, byteCounter);
      return fileName;
    } catch(Exception e) {
      verbose(_T("\nError:%s\n"), e.what());
      build();
    }
  }
  load(result = getTbFileName(ALLTABLEBASE), byteCounter);
#else
  if(needDecompress()) {
    decompress(byteCounter);
  }
  result = getTbFileName(DECOMPRESSEDTABLEBASE);
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

#if !defined(TABLEBASE_BUILDER)
TablebaseInfo EndGameTablebase::getInfo() const {
  TablebaseInfo result;
  if(tbFileExist(COMPRESSEDTABLEBASE)) {
    result.load(DecompressFilter(ByteInputFile(getTbFileName(COMPRESSEDTABLEBASE))));
  }
  return result;
}

void EndGameTablebase::decompress(ByteCounter *byteCounter) const {
  const String fileName = getTbFileName(COMPRESSEDTABLEBASE);
  decompress(DecompressFilter(CountedByteInputStream(byteCounter?*byteCounter:(ByteCounter&)StreamProgress(fileName), ByteInputFile(fileName))));
}

bool EndGameTablebase::needDecompress() const {
  return !tbFileExist(DECOMPRESSEDTABLEBASE)
      || (tbFileExist(COMPRESSEDTABLEBASE) && (getTbFileTime(DECOMPRESSEDTABLEBASE) < getTbFileTime(COMPRESSEDTABLEBASE)));
}

void EndGameTablebase::load(ByteInputStream &s) {
  m_positionIndex.load();
}
#endif // TABLEBASE_BUILDER

void EndGameTablebase::clear() {
#if defined(TABLEBASE_BUILDER)
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

#if defined(TABLEBASE_BUILDER)
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

#if defined(TABLEBASE_BUILDER)
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

#if defined(TABLEBASE_BUILDER)
  if(m_packedIndex) {
    result = m_packedIndex->getPositionResult(mapKey);
  } else
#endif
    result = getKeyResult(mapKey);

  if(result.getStatus() != EG_UNDEFINED) {
    return result.transform(swapPlayers);
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
