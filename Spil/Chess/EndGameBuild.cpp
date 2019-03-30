#include "stdafx.h"
#include <io.h>
#include <ByteFile.h>
#include <CompressFilter.h>
#include "EndGameTablebase.h"

#ifdef TABLEBASE_BUILDER

void EndGameTablebase::load(const String &fileName, ByteCounter *byteCounter) {
  try {
    verbose(_T("\nLoading %s-tablebase from %s..."), getName().cstr(), fileName.cstr());
    load(DecompressFilter(CountedByteInputStream(byteCounter?*byteCounter:(ByteCounter&)StreamProgress(fileName), ByteInputFile(fileName))));
  } catch(WrongVersionException e) {
    throw;
  } catch(Exception e) {
    clear();
    throwException(_T("%s:%s"), getName().cstr(), e.what());
  }
}

void EndGameTablebase::load(ByteInputStream &s) {
  clear();
  m_info.load(s);
  m_positionIndex.load(s);
}

String EndGameTablebase::loadPacked() {
  if(m_packedIndex != NULL) {
    return EMPTYSTRING;
  }
  const String fileName = getTbFileName(ALLTABLEBASE);
  loadPacked(fileName);
  return fileName;
}

void EndGameTablebase::loadPacked(const String &fileName) {
  try {
    verbose(_T("\nLoading (packed) %s-tablebase from %s..."), getName().cstr(), fileName.cstr());
    loadPacked(DecompressFilter(CountedByteInputStream(StreamProgress(fileName), ByteInputFile(fileName))));
  } catch(WrongVersionException e) {
    throw;
  } catch(Exception e) {
    clear();
    throwException(_T("%s:%s"), getName().cstr(), e.what());
  }
}

void EndGameTablebase::loadPacked(ByteInputStream &s) {
  clear();
  m_info.load(s);
  if(!m_info.isConsistent()) {
    throwException(_T("Not checked for consistency"));
  }
  m_packedIndex = new PackedIndexedMap(m_keydef, EndGameKeyDefinition::isMetricDTM(), m_info.m_maxPlies.getMax()); TRACE_NEW(m_packedIndex);
  m_packedIndex->load(s);
}

TablebaseInfo EndGameTablebase::getInfo() const {
  if(isLoaded()) {
    return m_info;
  }
  TablebaseInfo result;
  if(tbFileExist(ALLTABLEBASE)) {
    result.load(DecompressFilter(ByteInputFile(getTbFileName(ALLTABLEBASE))));
  } else {
    result.load(DecompressFilter(ByteInputFile(getTbFileName(COMPRESSEDTABLEBASE))));
  }
  return result;
}


TCHAR *EndGameTablebase::toStr(TCHAR *dst, const EndGameEntry &entry, bool ply) const {
  TCHAR keyStr[200], resultStr[56];
  _stprintf(dst, _T("%s:%s"), m_keydef.createKeyString(keyStr, entry.getKey(), false), entry.getValue().toStr(resultStr, ply));
  return dst;
}

String EndGameTablebase::toString(const EndGameEntry &entry, bool ply) const {
  TCHAR result[256];
  return toStr(result, entry, ply);
}

bool EndGameTablebase::needBuild(bool recover) const {
  return getFirstBuildStep(recover) != BUILD_DONE;
}

void EndGameTablebase::metricError(const TCHAR *method) { // static
  throwException(_T("%s:Metric must be DTC"), method);
}

BuildStep EndGameTablebase::getFirstBuildStep(bool recover) const {
  if(!recover) {
    return INSERT_INITIALPOSITIONS;
  } else if(!tbFileExist(ALLFORWARDPOSITIONS)) {
    return INSERT_INITIALPOSITIONS;
  } else if(!tbFileExist(ALLRETROPOSITIONS) || (getTbFileTime(ALLFORWARDPOSITIONS) > getTbFileTime(ALLRETROPOSITIONS))) {
    return RECOVER_FROM_FORWARDPOSITIONS;
  } else if(!tbFileExist(ALLTABLEBASE) || (getTbFileTime(ALLRETROPOSITIONS) > getTbFileTime(ALLTABLEBASE))) {
    return RECOVER_FROM_RETROPOSITIONS;
  } else if(!getInfo().isConsistent()) {
    return RECOVER_FROM_ALLPOSITIONS;
  } else if(!tbFileExist(COMPRESSEDTABLEBASE) || (getTbFileTime(ALLTABLEBASE) > getTbFileTime(COMPRESSEDTABLEBASE))) {
    return RECOVER_FROM_FIXED_POSITIONS;
  } else {
    return BUILD_DONE;
  }
}

void EndGameTablebase::buildDTM() {
  try {
    for(int i = 0; i < 2; i++) {
      EndGameKeyDefinition::setMetric(DEPTH_TO_MATE);
      try {
        build(true);
        unload();
        break;
      } catch(MissingPositionException e) {
        verbose(_T("%s\n"), e.what());
        UNLINK(getTbFileName(ALLRETROPOSITIONS));
        unload();
        EndGameKeyDefinition::setMetric(DEPTH_TO_CONVERSION);
        build(true);
        unload();
        EndGameKeyDefinition::setMetric(DEPTH_TO_MATE);
      }
    }
  } catch(...) {
    unload();
    throw;
  }
}

void EndGameTablebase::build(bool recover) {
  BuildStep buildStep = getFirstBuildStep(recover);

  if(buildStep == BUILD_DONE) {
    return;
  }

  clear();
  verbose(_T("%s %s tablebase with index capacity=%s. Metric=%s\n")
         ,recover ? _T("Recover build") : _T("Build")
         ,getName().cstr()
         ,format1000(m_keydef.getIndexSize()).cstr()
         ,EndGameKeyDefinition::getMetricName().cstr()
         );

  TimeUsagePrinter timeUsage(this, _T("Total time"));

  doBuild(buildStep);

  verbose(_T("%s%-16s%s\n\n")
         ,TablebaseInfo::getColumnHeaders(TBIFORMAT_PRINT_COLUMNS1, _T("Final tablebase:"),EMPTYSTRING, true).cstr()
         ,getName().cstr()
         ,getInfo().toString(TBIFORMAT_PRINT_COLUMNS1).cstr());
}

void EndGameTablebase::doBuild(BuildStep buildStep) {
  try {
    m_buildOk = true;
    m_workGame.setEndGameKeyDefinition(m_keydef);
    m_generatingPositions = false;
    startVerboseTrigger();

    while(buildStep != BUILD_DONE) {
      switch(buildStep) {
      case INSERT_INITIALPOSITIONS      :
        { const INT64 startCount = m_info.m_totalPositions;
          m_keydef.insertInitialPositions(*this);
          const INT64 initCount   = m_info.m_totalPositions - startCount;
          insertManualPositions();
          const INT64 manualCount = m_info.m_totalPositions - initCount - startCount;

          verbose(_T("%s initial and %s manual positions added\n"), format1000(initCount).cstr(), format1000(manualCount).cstr());

          checkBuildOk();
          checkInitialSetupFlags();
          buildStep = GENERATE_FORWARDPOSITIONS;
        }
        break;

      case GENERATE_FORWARDPOSITIONS    :
        m_generatingPositions = true;
//        startLogging();
        generateAllForwardPositions();
        buildStep = GENERATE_RETROPOSITIONS;
        break;

      case GENERATE_RETROPOSITIONS      :
        generateAllRetroPositions();
        buildStep = ANALYZERETRO;
        break;

      case RECOVER_FROM_FORWARDPOSITIONS:
        { loadAllForwardPositions();
          const INT64 startCount = m_info.m_totalPositions;
          insertManualPositions();
          const INT64 manualCount = m_info.m_totalPositions - startCount;

          verbose(_T("%s manual positions added\n"), format1000(manualCount).cstr());

          checkBuildOk();
          buildStep = GENERATE_FORWARDPOSITIONS;
        }
        break;

      case RECOVER_FROM_RETROPOSITIONS  :
        loadAllRetroPositions();
        buildStep = ANALYZERETRO;
        break;

      case ANALYZERETRO                 :
        m_generatingPositions = false;
        checkAllSubKeysFound();
        if(EndGameKeyDefinition::isMetricDTM()) {
          findDTM();
        } else {
          unravelWinnerPositions();
        }
        m_info.m_buildTime = _time32(NULL);
        save();
        checkAllSubKeysFound();
        buildStep = FIXUP_POSITIONS;
        break;

      case RECOVER_FROM_ALLPOSITIONS:
        load();
        buildStep = FIXUP_POSITIONS;
        break;

      case FIXUP_POSITIONS              :
        if(fixupPositions()) {
          save();
          buildStep = COMPRESS;
        } else {
          buildStep = BUILD_DONE;
        }
        break;

      case RECOVER_FROM_FIXED_POSITIONS :
        if(getInfo().isConsistent()) {
          load();
          buildStep = COMPRESS;
        } else {
          buildStep = BUILD_DONE;
        }
        break;

      case COMPRESS                     :
        compress();
        buildStep = BUILD_DONE; // May not use index anymore, as
                                // index-data are destroyed in compress()
        break;

      case BUILD_DONE                   :
        break;

      default:
        throwException(_T("%s:Invalid buildStep:%d"), __TFUNCTION__, buildStep);
      }
    }

    stopLogging();
    stopVerboseTrigger();
    unloadSubTablebases();
  } catch(...) {
    stopLogging();
    stopVerboseTrigger();
    unloadSubTablebases();
    verbose(_T("CurrentPosition:[%s]\n"), toString(m_workGame).cstr());
    throw;
  }
}

void EndGameTablebase::checkBuildOk() {
  if(!m_buildOk) {
    throwException(_T("%s:Build stopped"), getName().cstr());
  }
}

void EndGameTablebase::addInitPosition(Player playerInTurn,...) {
  EndGameKey key;
  key.setPlayerInTurn(playerInTurn);
  va_list argptr;
  va_start(argptr, playerInTurn);
  for(int pIndex = 0; pIndex < m_keydef.getPieceCount(); pIndex++) {
    const int pos = va_arg(argptr, int);
    VALIDATEPOS(pos);
    key.setPosition(pIndex, pos);
  }
  va_end(argptr);
  addInitPosition(key, false);
}

void EndGameTablebase::addInitPosition(EndGameKey key, bool allowTransform) {
  try {
    for(int pIndex = 0; pIndex < m_keydef.getPieceCount(); pIndex++) {
      const int pos = key.getPosition(pIndex);
      if((m_keydef.getPieceType(pIndex) == Pawn) && !isValidPawnPosition(pos)) {
        throwInvalidArgumentException(__TFUNCTION__, _T("%s is not a valid position for a pawn."), getFieldName(pos));
      }
    }

    const SymmetricTransformation st = m_keydef.getSymTransformation(key);

    if(st != TRANSFORM_NONE) {
      if(!allowTransform) {
        throwException(_T("Initial position[%s] needs symmetric transformation [%s] to be valid.")
                      ,toString(key, true).cstr()
                      ,getSymmetricTransformationToString(st).cstr());
      } else {
        key = m_keydef.getTransformedKey(key, st);
      }
    }

    if(contains(key)) {
      return; // verbose(_T("Initial position[%s] already exists. Ignored\n"), toString(key, true).cstr());
    } else {
      m_workGame = key.getGameKey(m_keydef);
      try {
        m_workGame.validateBoard(true);
      } catch(Exception e) {
        throwException(_T("Initial position[%s]:%s"), toString(key, true).cstr(), e.what());
      }
      addPosition(key, false);

      for(int pIndex = 0; pIndex < m_keydef.getPieceCount(); pIndex++) {
        const int pos = key.getPosition(pIndex);
        switch(m_keydef.getPieceType(pIndex)) {
        case Pawn:
          { const Player pawnOwner = m_keydef.getPieceOwner(pIndex);
            if(GETROW(pos) == GETPAWNSTARTROW(pawnOwner)) {
              m_pawnInitialField[pIndex][GETCOL(pos)] = true;
            }
          }
          break;
        case Bishop:
          m_bishopInitialField[pIndex][getFieldColor(pos)] = true;
          break;
        }
      }
    }
  } catch(Exception e) {
    verbose(_T("%s:%s\n"), getName().cstr(), e.what());
    m_buildOk = false;
  }
}

void EndGameTablebase::resetInitialSetupFlags() {
  for(int pIndex = 0; pIndex < MAX_ENDGAME_PIECECOUNT; pIndex++) {
    for(int c = 0; c < ARRAYSIZE(m_bishopInitialField[0]); c++) {
      m_bishopInitialField[pIndex][c] = false;
    }
    for(int c = 0; c < ARRAYSIZE(m_pawnInitialField[0]); c++) {
      m_pawnInitialField[pIndex][c] = false;
    }
  }
}

void EndGameTablebase::checkInitialSetupFlags() {
  int pawnCount = 0;
  bool manyEqualPawnsWarningDone = false;

  for(int pIndex = 0; pIndex < m_keydef.getPieceCount(); pIndex++) {
    const PieceKey pieceKey = m_keydef.getPieceKey(pIndex);
    const Player   player   = GET_PLAYER_FROMKEY(pieceKey);
    switch(GET_TYPE_FROMKEY(pieceKey)) {
    case Bishop:
      switch(m_keydef.getPieceCount(player, Bishop)) {
      case 1:
        { const bool w = m_bishopInitialField[pIndex][WHITEFIELD];
          const bool b = m_bishopInitialField[pIndex][BLACKFIELD];
          if(!w || !b ) {
            throwException(_T("%s:Piece[%d] is a bishop but is not present on both fieldcolors i initial position (w=%s, b=%s)")
                          ,getName().cstr(), pIndex, boolToStr(w), boolToStr(b));
          }
        }
        break;
      case 2:

#define  _BISHOPCOMBI2_EXIST(fc1, fc2)   (m_bishopInitialField[bi1][fc1] && m_bishopInitialField[bi2][fc2])
#define  BISHOPCOMBI2SAMECOLOR_EXIST(fc)  _BISHOPCOMBI2_EXIST(fc, fc)
#define  BISHOPCOMBI2DIFFCOLOR_EXIST()   (_BISHOPCOMBI2_EXIST(WHITEFIELD, BLACKFIELD) || _BISHOPCOMBI2_EXIST(BLACKFIELD, WHITEFIELD))


        { const int  bi1 = m_keydef.findKeyIndexByCount(pieceKey, 1);
          const int  bi2 = m_keydef.findKeyIndexByCount(pieceKey, 2);
          const bool ww  = BISHOPCOMBI2SAMECOLOR_EXIST(WHITEFIELD);
          const bool bb  = BISHOPCOMBI2SAMECOLOR_EXIST(BLACKFIELD);
          const bool wb  = BISHOPCOMBI2DIFFCOLOR_EXIST();
          if(!ww || !bb || !wb) {
            throwException(_T("%s:Not all fieldcolor-combinations for 2 bishops are present in initial positions (ww=%s, bb=%s, wb=%s)")
                          ,getName().cstr(), boolToStr(ww), boolToStr(bb), boolToStr(wb));
          }
        }
        break;
      case 3:

#define _BISHOPCOMBI3_EXIST(fc1, fc2, fc3)    (_BISHOPCOMBI2_EXIST(fc1, fc2) && m_bishopInitialField[bi3][fc3])
#define BISHOPCOMBI3SAMECOLOR_EXIST(fc)       _BISHOPCOMBI3_EXIST(fc, fc, fc)
#define BISHOPCOMBI3DIFFCOLOR_EXIST(fc)       (_BISHOPCOMBI3_EXIST(fc, fc, 1-fc) || _BISHOPCOMBI3_EXIST(fc, 1-fc, fc) || _BISHOPCOMBI3_EXIST(1-fc, fc, fc))

        { const int  bi1 = m_keydef.findKeyIndexByCount(pieceKey, 1);
          const int  bi2 = m_keydef.findKeyIndexByCount(pieceKey, 2);
          const int  bi3 = m_keydef.findKeyIndexByCount(pieceKey, 3);
          const bool www = BISHOPCOMBI3SAMECOLOR_EXIST(WHITEFIELD);
          const bool bbb = BISHOPCOMBI3SAMECOLOR_EXIST(BLACKFIELD);
          const bool wwb = BISHOPCOMBI3DIFFCOLOR_EXIST(WHITEFIELD);
          const bool bbw = BISHOPCOMBI3DIFFCOLOR_EXIST(BLACKFIELD);
          if(!www || !bbb || !wwb || !bbw) {
            throwException(_T("%s:Not all fieldcolor-combinations for 3 bishops are present in initial positions (www=%s, bbb=%s, wwb=%s, bbw=%s)")
                          ,getName().cstr(), boolToStr(www), boolToStr(bbb), boolToStr(wwb), boolToStr(bbw));
          }
        }
        break;
      default: throwException(_T("%s:Too many bishops (=%d)"), getName().cstr(), m_keydef.getPieceCount(player, Bishop));
      }
      break;
    case Pawn:
      { switch(m_keydef.getPieceCount(player, Pawn)) {
        case 1:
          { const int colCount = (pawnCount++ == 0) ? 4 : 8;
            for(int col = 0; col < colCount; col++) {
              if(!m_pawnInitialField[pIndex][col]) {
                throwException(_T("%s:Piece[%d] is a %s pawn, but has not an initial position on field %s")
                              ,getName().cstr()
                              ,pIndex
                              ,getPlayerNameEnglish(player)
                              ,getFieldName(MAKE_POSITION(GETPAWNSTARTROW(player), col))
                              );
              }
            }
          }
          break;
        default:
          if(!manyEqualPawnsWarningDone) {
            verbose(_T("Warning:It is not checked, that all column-combinations of initial positions of %d %s pawns are inserted")
                   ,m_keydef.getPieceCount(player, Pawn)
                   ,getPlayerNameEnglish(player)
                   );
            manyEqualPawnsWarningDone = true;
          }
          break;
        }
      }
    }
  }
}

#define INLOOP_VERBOSE( m) if(m_verboseTriggered) { m('\r'); }
#define ENDLOOP_VERBOSE(m) m('\n')

#define GENERATEFORWARD_VERBOSE(eosCh)                                    \
{ verbose(_T("It:%3d(%s). Position:%*s/%-*s (%6.2lf%%). %s%c")            \
         ,iteration                                                       \
         ,it.getProgressStr().cstr()                                      \
         ,POSITIONWIDTH, format1000(m_positionsAnalyzed).cstr()           \
         ,POSITIONWIDTH, format1000(m_info.m_totalPositions).cstr()       \
         ,PERCENT(m_positionsAnalyzed, m_info.m_totalPositions)           \
         ,m_info.toString(TBIFORMAT_PRINT_TERMINALS).cstr()               \
         ,eosCh                                                           \
         );                                                               \
}

void EndGameTablebase::generateAllForwardPositions() {
  if(!EndGameKeyDefinition::isMetricDTC()) {
    metricError(__TFUNCTION__);
  }
  verbose(_T("\nGenerating all forward positions...\n"));

  bool changed;

  m_workGame.setGenerateFictivePawnCaptures(true);
  TimeUsagePrinter timeUsage(this);

  int iteration       = 0;
  m_positionsAnalyzed = 0;
  do {
    changed = false;
    iteration++;
    EndGameEntryIterator it = m_positionIndex.getIteratorUndefinedUnvisitedEntries();
    while(it.hasNext()) {
      EndGameEntry &entry = it.next();

      entry.getValue().setVisited();

      m_workGame = entry.getKey().getGameKey(m_keydef);
      changed |= addSuccessors(entry.getValue(), m_workGame, false);
      m_positionsAnalyzed++;

      INLOOP_VERBOSE(GENERATEFORWARD_VERBOSE)
    }

    ENDLOOP_VERBOSE(GENERATEFORWARD_VERBOSE)

  } while(changed);

  m_workGame.setGenerateFictivePawnCaptures(false);

  verbose(_T("All forward positions found\n"));

  saveAllForwardPositions();
}

#define GENERATE_RETRO_VERBOSE(eosCh)                               \
{ verbose(_T("It:%d.%d. Position:%*s/%-*s  %s. %s%c")               \
         ,mainIteration                                             \
         ,innerIteration                                            \
         ,POSITIONWIDTH, format1000(positionCount).cstr()           \
         ,POSITIONWIDTH, format1000(m_info.m_totalPositions).cstr() \
         ,it.getProgressStr().cstr()                                \
         ,m_info.toString(TBIFORMAT_PRINT_TERMINALS).cstr()         \
         ,eosCh                                                     \
         );                                                         \
}

void EndGameTablebase::generateAllRetroPositions() {
  if(!EndGameKeyDefinition::isMetricDTC()) {
    metricError(__TFUNCTION__);
  }
  verbose(_T("\nSearch for positions by moving backwards...\n"));

  bool changed;
  TimeUsagePrinter timeUsage(this);

  m_positionIndex.clearAllVisited();

  m_workGame.setGenerateFictivePawnCaptures(true);
  int mainIteration = 0;
  do {
    mainIteration++;
    int innerIteration = 0;
    verbose(_T("Search for backward positions...\n"));
    const INT64 positionCountAtStart = m_info.m_totalPositions;
    do {
      innerIteration++;
      changed = false;
      EndGameEntryIterator it            = m_positionIndex.getIteratorUnvisitedEntries();
      UINT64               total         = it.getCount();
      UINT64               positionCount = 0;
      while(it.hasNext()) {
        EndGameEntry &entry = it.next();
        positionCount++;

        entry.getValue().setVisited();

        const INT64 positionBefore = m_info.m_totalPositions;

        m_workGame = entry.getKey().getGameKey(m_keydef);

        Move           bm;
        MoveGenerator &bmg = m_workGame.getBackMoveGenerator();
        m_workGame.pushState();
        for(bool moreBackMoves = bmg.firstMove(bm); moreBackMoves; m_workGame.unTryBackMove(), moreBackMoves = bmg.nextMove(bm)) {
          m_workGame.tryBackMove(bm);
          if(!isUsableBackMove(bm)) continue;
          const EndGameKey rawKey     = m_keydef.getEndGameKey(m_workGame.getKey());
          const EndGameKey predKey    = transformEndGameKey(rawKey, m_keydef.getSymTransformation(rawKey));
          EndGameResult   &predResult = m_positionIndex.get(predKey);
          if(!predResult.exists() || (predKey != rawKey)) {
            addPosition(predKey, predResult, false);
            changed = true;
            addSuccessors(predResult, m_workGame, true);
          }
        }
        m_workGame.popState();

        total += m_info.m_totalPositions - positionBefore;

        INLOOP_VERBOSE(GENERATE_RETRO_VERBOSE)
      }
      ENDLOOP_VERBOSE(GENERATE_RETRO_VERBOSE)
    } while(changed);

    // changed == false;
    const INT64 backwardPositionCount = m_info.m_totalPositions - positionCountAtStart;
    if(backwardPositionCount > 0) {
      verbose(_T("Solving %s found backward positions\n"), format1000(backwardPositionCount).cstr());
    }
    bool changed2;
    do {
      innerIteration++;
      changed2     = false;
      EndGameEntryIterator it            = m_positionIndex.getIteratorMarkedEntries();
      UINT64               total         = it.getCount();
      UINT64               positionCount = 0;
      while(it.hasNext()) {
        EndGameEntry &entry = it.next();
        positionCount++;

        entry.getValue().clearMark();

        m_workGame = entry.getKey().getGameKey(m_keydef);

        const INT64 positionBefore = m_info.m_totalPositions;
        changed2 |= addSuccessors(entry.getValue(), m_workGame, true);
        total += m_info.m_totalPositions - positionBefore;

        INLOOP_VERBOSE(GENERATE_RETRO_VERBOSE)
      }
      ENDLOOP_VERBOSE(GENERATE_RETRO_VERBOSE)
      changed |= changed2;
    } while(changed2);
  } while(changed);

  m_workGame.setGenerateFictivePawnCaptures(false);

  verbose(_T("All retro positions found\n"));

  saveAllRetroPositions();
}

bool EndGameTablebase::addSuccessors(EndGameResult &result, Game &game, bool markNew) {
  const Player playerInTurn = game.getPlayerInTurn();

  bool changed           = false;
  bool anyMovesFound     = false;
  bool winningMoveFound  = false;
  bool undefinedFound    = false;
  bool looserMoveFound   = false;

  Move           m;
  MoveGenerator &mg = game.getMoveGenerator();
  game.pushState();
  for(bool more = mg.firstMove(m); more; more = mg.nextMove(m)) {
    anyMovesFound = true;

    if(!isUsableMove(m)) {
      continue;
    }

    game.tryMove(m);
    const EndGamePositionStatus status = isTerminalMove(game, m);
    switch(status) {
    case EG_WHITEWIN :
    case EG_BLACKWIN :
      if(playerInTurn == STATUSTOWINNER(status)) {
        if(!result.isDefined() && !winningMoveFound) {
          setAsTerminalPosition(result, status, 1);
          winningMoveFound = true;
          changed = true;
        }
      } else {
        looserMoveFound = true;
      }
      break;

    case EG_DRAW     :
      undefinedFound = true;
      break;

    case EG_UNDEFINED: // ie. position is NOT a terminal move
      undefinedFound = true;
      if(addPosition(transformGameKey(game.getKey()), markNew)) {
        changed = true;
      }
      break;

    default     :
      statusError(status, game, m);
      break;
    }
    game.unTryMove();
  }
  game.popState();

  if(!result.isDefined()) {
    if(!anyMovesFound) {
      switch(game.findGameResult()) {
      case WHITE_CHECKMATE:
        setAsCheckMatePosition(result, EG_BLACKWIN);
        changed = true;
        break;

      case BLACK_CHECKMATE:
        setAsCheckMatePosition(result, EG_WHITEWIN);
        changed = true;
        break;

      case STALEMATE      :
        setAsStaleMatePosition(result);
        changed = true;
        break;

      default             :
        gameResultError(game);
        break;
      }
    } else if(looserMoveFound && !undefinedFound) {
      setAsTerminalPosition(result, (playerInTurn == WHITEPLAYER) ? EG_BLACKWIN : EG_WHITEWIN, 1);
      changed = true;
    }
  }
  return changed;
}

#define FIND_DTM_VERBOSE(eosCh)                                               \
{ verbose(_T("FindDTM fix terminals %s %s Modified:%s%c")                     \
         ,it.getProgressStr().cstr()                                          \
         ,m_info.toString(TBIFORMAT_PRINT_TERMINALS).cstr()                   \
         ,changeCount.toString('/').cstr()                                    \
         ,eosCh                                                               \
         );                                                                   \
}

void EndGameTablebase::findDTM() {
  verbose(_T("Find Depth To checkMate (DTM)...\n"));

  { TimeUsagePrinter timeUsage(this);

    PositionCount64 changeCount;
    EndGameEntryIterator it = m_positionIndex.getIteratorAllWinnerEntries();
    while(it.hasNext()) {
      EndGameEntry &entry = it.next();

      switch(entry.getValue().getPliesToEnd()) {
      case 0: // checkmate. Dont touch
        break;
      case 1:
        { m_workGame = entry.getKey().getGameKey(m_keydef);
          const EndGameResult result = getBestResult(m_workGame, false);
          switch(result.getStatus()) {
          case EG_UNDEFINED:
          case EG_DRAW     :
            statusError(result.getStatus(), entry.getKey());
            break;
          case EG_WHITEWIN :
          case EG_BLACKWIN :
            if(result.getPliesToEnd() > 1) {
              changeCount.incr(STATUSTOWINNER(result.getStatus()));
              changePliesToEnd(entry.getValue(), result);
            }
            break;
          }
        }
        break;
      default:
        throwException(_T("%s:Unexpected pliesToEnd:%s. Expected pliesToEnd is 0 or 1")
                      ,__TFUNCTION__
                      , toString(entry, true).cstr());
      }
      INLOOP_VERBOSE(FIND_DTM_VERBOSE)
    }
    ENDLOOP_VERBOSE(FIND_DTM_VERBOSE)
  }

  const int minPliesToEnd = m_info.m_maxPlies.getMax();
  m_info.m_maxPlies = findMaxPlies();
  unravelWinnerPositions(minPliesToEnd);
}

#define UNRAVEL_VERBOSE(eosCh)                                  \
{ verbose(_T("%s:%3d %s %s New win:%s%c")                       \
         ,EndGameKeyDefinition::getMetricName().cstr()          \
         ,pliesToEnd                                            \
         ,it.getProgressStr().cstr()                            \
         ,m_info.toString(TBIFORMAT_PRINT_NONTERMINALS).cstr()  \
         ,winnerCount.toString('/').cstr()                      \
         ,eosCh                                                 \
         );                                                     \
}

void EndGameTablebase::unravelWinnerPositions(int minPliesToEnd) {
  verbose(_T("Find Depth To Mate/Conversion (DTM/DTC)...\n"));

  TimeUsagePrinter timeUsage(this);

  m_positionIndex.clearHelpInfo();

  bool changed;
  UINT pliesToEnd = 0;

  do {
    changed = false;

    PositionCount64 winnerCount;
    EndGameEntryIterator it = m_positionIndex.getIteratorWinnerEntries(pliesToEnd);
    while(it.hasNext()) {
      EndGameEntry &entry = it.next();

      changed |= analyzeRetro(entry, winnerCount, pliesToEnd);

      INLOOP_VERBOSE(UNRAVEL_VERBOSE)
    }

    ENDLOOP_VERBOSE(UNRAVEL_VERBOSE)
    pliesToEnd++;
  } while(changed || ((int)pliesToEnd <= max((int)m_info.m_maxPlies.getMax(), minPliesToEnd)));

//  verbose(_T("%27s%s\n"), EMPTYSTRING, m_info.toString(TBIFORMAT_PRINT_NONTERMINALS).cstr());
}

bool EndGameTablebase::analyzeRetro(const EndGameEntry &entry, PositionCount64 &winnerCount, UINT pliesToEnd) {
  bool changed = false;

  const EndGameResult        &result = entry.getValue();
  const EndGamePositionStatus status = result.getStatus();

  assert(status >= EG_WHITEWIN);

  const Player winner = STATUSTOWINNER(status);

  m_workGame = entry.getKey().getGameKey(m_keydef);
  Move           bm;
  MoveGenerator &bmg = m_workGame.getBackMoveGenerator();
  m_workGame.pushState();
  for(bool moreBackMoves = bmg.firstMove(bm); moreBackMoves; m_workGame.unTryBackMove(), moreBackMoves = bmg.nextMove(bm)) {
    m_workGame.tryBackMove(bm);
    if(!isUsableBackMove(bm)) continue;

    const EndGameKey predKey    = transformGameKey(m_workGame.getKey());
    EndGameResult   &predResult = m_positionIndex.get(predKey);

    if(!predResult.exists()) {
      missingPositionError(_T("analyzeRetro"), predKey, predResult);
    } else if(predResult.isDefined()) {
      if(predResult.isWinner() && (predResult.getPliesToEnd() > pliesToEnd + 1) && (predKey.getPlayerInTurn() == winner)) {
        predResult.setUndefined();
        m_info.m_undefinedPositions++;
        m_info.m_terminalWinPositions.decr(winner);
      } else {
        continue;
      }
    }

    if(predKey.getPlayerInTurn() == winner) {
      setPositionResult(predResult, result.getStatus(), result.getPliesToEnd()+1);
      winnerCount.incr(winner);
      changed = true;
    } else {
      const EndGameResult bestResult = getBestResult(m_workGame, true);

      switch(bestResult.getStatus()) {
      case EG_UNDEFINED:
      case EG_DRAW     :
        break;

      case EG_WHITEWIN:
        if(bestResult.getPliesToEnd() == pliesToEnd+1) {
          setPositionResult(predResult, bestResult);
          winnerCount.incr(WHITEPLAYER);
          changed = true;
        }
        break;
      case EG_BLACKWIN:
        if(bestResult.getPliesToEnd() == pliesToEnd+1) {
          setPositionResult(predResult, bestResult);
          winnerCount.incr(BLACKPLAYER);
          changed = true;
        }
        break;
      default:
        statusError(bestResult.getStatus(), m_keydef.getEndGameKey(m_workGame.getKey()));
        break;
      }
    } // else
  } // for all bakcmoves
  m_workGame.popState();
  return changed;
}

void EndGameTablebase::fixupBackup(bool force) {
  time_t now;
  time(&now);
  if(force || (now - getTbFileTime(ALLTABLEBASE) > 3600)) { // save every hour
    save();
  }
}

#define TBISTATE_RETROLOOPDONE (TBISTATE_WINNERRETROFIXED    | TBISTATE_WINNERFORWARD2FIXED)
#define TBISTATE_PLIESFIXED    (TBISTATE_WINNERFORWARD1FIXED | TBISTATE_RETROLOOPDONE)

bool EndGameTablebase::fixupPositions() {
  UINT64 changedPositions = 0;

  TimeUsagePrinter timeUsage(this, _T("Fixup-positions time"));

  for(int it = 0; it < 10; it++) {
    const unsigned char startState = m_info.m_stateFlags;

    if((m_info.m_stateFlags & TBISTATE_NONWINNERFIXED) == 0) {
      fixupNonWinnerPositions(changedPositions);
      m_info.m_stateFlags |= TBISTATE_NONWINNERFIXED;
      fixupBackup();
    }
    if((m_info.m_stateFlags & TBISTATE_PLIESFIXED) != TBISTATE_PLIESFIXED) {
      fixupPlies(changedPositions);
    }

    if((changedPositions > 0) || (startState != 0)) {
      m_positionIndex.clearHelpInfo();
      m_info.m_maxPlies = findMaxPlies();
      fixupBackup();
    }
    if(((changedPositions == 0) && (startState == 0)) || checkConsistency(CHECK_HEADER | CHECK_POSITIONS | CHECK_RETURN_ON_ERROR)) {
      m_info.m_stateFlags |= TBISTATE_CONSISTENT;
      m_info.m_consistencyCheckedTime = _time32(NULL);
      return true;
    }
    m_info.m_stateFlags = 0; // reset all flags and repeat
  }
  return true;
}

#define _FIXUP_NONWINNER_VERBOSE(loop, eosCh)                                     \
{ verbose(_T("Fix non-win positions%d:%3d    %s %s Modified:%11s (%s)%c")         \
         ,loop                                                                    \
         ,iteration                                                               \
         ,it.getProgressStr().cstr()                                              \
         ,m_info.toString(TBIFORMAT_PRINT_UNDEFANDWINS).cstr()                    \
         ,format1000(changedPositions).cstr()                                     \
         ,loopChangeCount.toString('/').cstr()                                    \
         ,eosCh                                                                   \
         );                                                                       \
}

#define FIXUP_NONWINNER_VERBOSE0(eosCh) _FIXUP_NONWINNER_VERBOSE(0, eosCh)
#define FIXUP_NONWINNER_VERBOSE1(eosCh) _FIXUP_NONWINNER_VERBOSE(1, eosCh)

// Scan for all draw/undefined positions to find any missed winner-positions
void EndGameTablebase::fixupNonWinnerPositions(UINT64 &changedPositions) {
  int iteration = 1;

StartLoop0:
  PositionCount64 loopChangeCount;

  double startTime      = getThreadTime();
  INT64  startUndefined = m_info.m_undefinedPositions;

  if(startUndefined == 0) {
    return;
  }

  m_positionIndex.clearHelpInfo();
  loopChangeCount.clear();
  EndGameEntryIterator it = m_positionIndex.getIteratorAllNonWinnerEntries();
  while(it.hasNext()) { // loop0
    EndGameEntry &entry = it.next();
    m_workGame = entry.getKey().getGameKey(m_keydef);
    const EndGameResult result = getBestResult(m_workGame, false);
    switch(result.getStatus()) {
    case EG_UNDEFINED:
    case EG_DRAW     :
      break;
    case EG_WHITEWIN :
    case EG_BLACKWIN :
      { changedPositions++;
        loopChangeCount.incr(STATUSTOWINNER(result.getStatus()));
        setPositionResult(entry.getValue(), result).setChanged();
      }
      break;
    }

    INLOOP_VERBOSE(FIXUP_NONWINNER_VERBOSE0)
  }
  ENDLOOP_VERBOSE(FIXUP_NONWINNER_VERBOSE0)

  double usedTimeLoop0          = getThreadTime() - startTime;
  double timePerPosition        = usedTimeLoop0 / startUndefined;
  double estimatedTimeNextLoop0 = timePerPosition * m_info.m_undefinedPositions;
  bool   loop0StillPossible     = true;

  double startTimeLoop1 = getThreadTime();

  for(UINT64 startChangeCount = changedPositions; m_info.m_undefinedPositions > 0; startChangeCount = changedPositions) { // loop1
    loopChangeCount.clear();
    iteration++;
    for(EndGameEntryIterator it = m_positionIndex.getIteratorChangedUnvisitedEntries(); it.hasNext() && (m_info.m_undefinedPositions > 0);) {
      EndGameEntry &entry = it.next();

      changedPositions += fixupRetroStatus(entry, loopChangeCount);
      entry.getValue().setVisited();

      if(m_verboseTriggered) {
        double percent;
        if(loop0StillPossible && ((percent = it.getPercentDone()) > 0.15)) {
          const double usedTimeLoop1      = getThreadTime() - startTimeLoop1;
          const double q                  = percent / 100;
          const double estimatedTimeLoop1 = usedTimeLoop1 / q * (1-q);
          if(estimatedTimeLoop1 < estimatedTimeNextLoop0) {
            loop0StillPossible = false;
          } else {
            ENDLOOP_VERBOSE(FIXUP_NONWINNER_VERBOSE1)
            goto StartLoop0;
          }
        }
        FIXUP_NONWINNER_VERBOSE1('\r')
      }
    }
    ENDLOOP_VERBOSE(FIXUP_NONWINNER_VERBOSE1)

    if(changedPositions == startChangeCount) {
      break;
    }
    loop0StillPossible = false;
  }
}

#define FIXUP_FORWARD_VERBOSE(eosCh)                                              \
{ verbose(_T("Fix plies forward    :%3d     %s %s Modified:%11s (%s)%c")          \
         ,iteration                                                               \
         ,it.getProgressStr().cstr()                                              \
         ,getDottedLine()                                                         \
         ,format1000(changedPositions).cstr()                                     \
         ,loopChangeCount.toString('/').cstr()                                    \
         ,eosCh                                                                   \
         );                                                                       \
}

#define FIXUP_RETRO_VERBOSE(eosCh)                                                \
{ verbose(_T("Fix plies retro      :%3d.%-3d %s %s Modified:%11s (%s)%c")         \
         ,iteration ,retroIteration                                               \
         ,it.getProgressStr().cstr()                                              \
         ,getDottedLine()                                                         \
         ,format1000(changedPositions).cstr()                                     \
         ,loopChangeCount.toString('/').cstr()                                    \
         ,eosCh                                                                   \
         );                                                                       \
}

#define FIXUP_MARKPREDECESSOR_VERBOSE(eosCh)                                      \
{ verbose(_T("Mark predecessors %s%c"), it.getProgressStr().cstr(), eosCh);       \
}

// Fix up all plies to end, that is NO MORE winner-positions exist
void EndGameTablebase::fixupPlies(UINT64 &changedPositions) {
  UINT iteration = 1;

  if((m_info.m_stateFlags & TBISTATE_WINNERFORWARD1FIXED) == 0) {
    m_positionIndex.clearHelpInfo();

    fixupForwardPlies(m_positionIndex.getIteratorAllWinnerEntries(), iteration, changedPositions);
    // All positions which have been changed from draw/undefined -> white/black win are now marked as changed

    m_positionIndex.markAllChanged();

    m_info.m_stateFlags |= TBISTATE_WINNERFORWARD1FIXED;
    fixupBackup();
  }

  if((m_info.m_stateFlags & TBISTATE_RETROLOOPDONE) != TBISTATE_RETROLOOPDONE) {
    for(iteration++;;iteration++) {
      if((m_info.m_stateFlags & TBISTATE_WINNERRETROFIXED) == 0) {
        for(UINT64 retroStartChangeCount = changedPositions, retroIteration = 1;; retroStartChangeCount = changedPositions, retroIteration++) {
          PositionCount64 loopChangeCount;
          m_positionIndex.clearAllChanged();
          // Invariant:No changed. loop through marked positions
          // fix all plies to predecessors to the marked positions
          EndGameEntryIterator it = m_positionIndex.getIteratorMarkedEntries();
          while(it.hasNext()) {
            EndGameEntry &entry = it.next();

            changedPositions += fixupRetroPlies(entry, loopChangeCount); // Changed positions are set to changed and visited

            INLOOP_VERBOSE(FIXUP_RETRO_VERBOSE)
          }
          ENDLOOP_VERBOSE(FIXUP_RETRO_VERBOSE)

          // Invariant: All modified positions are now marked as changed and visisted
          m_positionIndex.clearAllMarked().markAllChanged(); // Dont touch visit-bit. Changes are accumulated as visited

          fixupBackup();

          if(changedPositions == retroStartChangeCount) {
            break;
          }
        }
        m_info.m_stateFlags |=  TBISTATE_WINNERRETROFIXED;
        m_info.m_stateFlags &= ~TBISTATE_WINNERFORWARD2FIXED;
        fixupBackup();
      }

      // Invariant:All modified positions in the loop above are now marked as visited
      if((m_info.m_stateFlags & TBISTATE_WINNERFORWARD2FIXED) == 0) {
        EndGameEntryIterator it = m_positionIndex.getIteratorVisitedEntries();
        while(it.hasNext()) {
          const EndGameEntry &entry = it.next();
          markPredecessors(entry, true);
          INLOOP_VERBOSE(FIXUP_MARKPREDECESSOR_VERBOSE)
        }
        ENDLOOP_VERBOSE(FIXUP_MARKPREDECESSOR_VERBOSE)

        m_positionIndex.clearAllVisited();

        fixupForwardPlies(m_positionIndex.getIteratorMarkedEntries(), iteration, changedPositions);

        m_positionIndex.clearAllMarked().markAllChanged();

        if(!m_positionIndex.getIteratorMarkedEntries().hasNext()) {
          break;
        }
      }
      m_info.m_stateFlags &= ~TBISTATE_WINNERRETROFIXED;
      m_info.m_stateFlags |=  TBISTATE_WINNERFORWARD2FIXED;
      fixupBackup();
    }
    m_info.m_stateFlags |= TBISTATE_RETROLOOPDONE;
  }
}

void EndGameTablebase::verboseHelpInfo(const String &label) {
  verbose(_T("%-70s: #marked:%11s, #visited:%11s, #changed:%11s\n")
         ,label.cstr()
         ,format1000(m_positionIndex.getIteratorMarkedEntries().getCount()).cstr()
         ,format1000(m_positionIndex.getIteratorVisitedEntries().getCount()).cstr()
         ,format1000(m_positionIndex.getIteratorChangedEntries().getCount()).cstr()
         );
}

void EndGameTablebase::fixupForwardPlies(EndGameEntryIterator &it, int iteration, UINT64 &changedPositions) {
  PositionCount64 loopChangeCount;

  verbose(_T("%-70s\r"),EMPTYSTRING);
  verbose(_T("Positions to check:%s\n"), format1000(it.getCount()).cstr());

  while(it.hasNext()) {
    EndGameEntry &entry = it.next();

    m_workGame = entry.getKey().getGameKey(m_keydef);
    const EndGameResult bestResult = getBestResult(m_workGame, false);
    EndGameResult      &dbResult   = entry.getValue();
    if(bestResult.getPliesToEnd() != dbResult.getPliesToEnd()) {
//      log(_T("ChangeF [%s] -> (%s)\n"), toString(entry,true).cstr(), bestResult.toString(true).cstr());
      changePliesToEnd(dbResult, bestResult.getStatus(), bestResult.getPliesToEnd()).setChanged();
      loopChangeCount.incr(STATUSTOWINNER(bestResult.getStatus()));
      changedPositions++;
    }
    INLOOP_VERBOSE(FIXUP_FORWARD_VERBOSE)
  }
  ENDLOOP_VERBOSE(FIXUP_FORWARD_VERBOSE)
}

UINT64 EndGameTablebase::fixupRetroPlies(const EndGameEntry &entry, PositionCount64 &changeCount) {
  const UINT64 startChangeCount = changeCount.getTotal();

  const EndGameResult        &result     = entry.getValue();
  const EndGamePositionStatus status     = result.getStatus();
  const int                   pliesToEnd = result.getPliesToEnd() + 1;

  assert(status >= EG_WHITEWIN);

  const Player winner = STATUSTOWINNER(status);

  m_workGame = entry.getKey().getGameKey(m_keydef);
  Move           bm;
  MoveGenerator &bmg = m_workGame.getBackMoveGenerator();
  m_workGame.pushState();
  for(bool moreBackMoves = bmg.firstMove(bm); moreBackMoves; m_workGame.unTryBackMove(), moreBackMoves = bmg.nextMove(bm)) {
    m_workGame.tryBackMove(bm);

    if(!isUsableBackMove(bm)) continue;
    const EndGameKey predKey    = transformGameKey(m_workGame.getKey());
    EndGameResult   &predResult = m_positionIndex.get(predKey);

    if(!predResult.exists()) {
      missingPositionError(_T("fixupRetroPlies"), predKey, predResult);
    }

    switch(predResult.getStatus()) {
    case EG_UNDEFINED:
    case EG_DRAW     :
      assert(getBestResult(m_workGame, false).getStatus() <= EG_DRAW);
      break;

    case EG_WHITEWIN:
    case EG_BLACKWIN:
      { const EndGameResult         bestResult = getBestResult(m_workGame, false);
        const EndGamePositionStatus bestStatus = bestResult.getStatus();
        const int                   pliesToEnd = bestResult.getPliesToEnd();
        if(pliesToEnd != predResult.getPliesToEnd()) {
/*
          log(_T("ChangeR [%s:%s] -> (%s)\n")
             ,toString(predKey).cstr()
             ,predResult.toString(true).cstr()
             ,bestResult.toString(true).cstr());
*/
          if(!changePliesToEnd(predResult, bestStatus, pliesToEnd).setChanged().isVisited()) {
            predResult.setVisited();
            changeCount.incr(STATUSTOWINNER(bestStatus));
          }
        }
      }
      break;
    default:
      statusError(predResult.getStatus(), m_keydef.getEndGameKey(m_workGame.getKey()));
      break;
    } // switch
  } // for all bakcmoves
  m_workGame.popState();
  return changeCount.getTotal() - startChangeCount;
}

UINT64 EndGameTablebase::fixupRetroStatus(const EndGameEntry &entry, PositionCount64 &changeCount) {
  const UINT64 startChangeCount = changeCount.getTotal();

  const EndGameResult        &result = entry.getValue();
  const EndGamePositionStatus status = result.getStatus();

  assert(status >= EG_WHITEWIN);

  const Player winner = STATUSTOWINNER(status);

  m_workGame = entry.getKey().getGameKey(m_keydef);
  Move           bm;
  MoveGenerator &bmg = m_workGame.getBackMoveGenerator();
  m_workGame.pushState();
  for(bool moreBackMoves = bmg.firstMove(bm); moreBackMoves; m_workGame.unTryBackMove(), moreBackMoves = bmg.nextMove(bm)) {
    m_workGame.tryBackMove(bm);

    if(!isUsableBackMove(bm)) continue;
    const EndGameKey predKey    = transformGameKey(m_workGame.getKey());
    EndGameResult   &predResult = m_positionIndex.get(predKey);

    if(!predResult.exists()) {
      missingPositionError(_T("fixupRetroStatus"), predKey, predResult);
    }

    const EndGameResult bestResult = getBestResult(m_workGame, false);
    switch(bestResult.getStatus()) {
    case EG_UNDEFINED:
    case EG_DRAW     :
      assert(predResult.getStatus() <= EG_DRAW);
      break;
    case EG_WHITEWIN :
    case EG_BLACKWIN :
      if(predResult.getStatus() <= EG_DRAW) {
        changeCount.incr(STATUSTOWINNER(bestResult.getStatus()));
        setPositionResult(predResult, bestResult).setChanged();
      } else {
        assert(predResult.getStatus() == getBestResult(m_workGame, false).getStatus());
      }
      break;
    }
  } // for all bakcmoves
  m_workGame.popState();
  return changeCount.getTotal() - startChangeCount;
}

void EndGameTablebase::markPredecessors(const EndGameEntry &entry, bool onlyWinnerPositions) {
  m_workGame = entry.getKey().getGameKey(m_keydef);
  markPredecessors(onlyWinnerPositions);
}

void EndGameTablebase::markPredecessors(bool onlyWinnerPositions) {
  Move           bm;
  MoveGenerator &bmg = m_workGame.getBackMoveGenerator();
  m_workGame.pushState();
  for(bool more = bmg.firstMove(bm); more; m_workGame.unTryBackMove(), more = bmg.nextMove(bm)) {
    m_workGame.tryBackMove(bm);
    if(!isUsableBackMove(bm)) continue;
    EndGameResult &pred = m_positionIndex.get(transformGameKey(m_workGame.getKey()));
    if(!onlyWinnerPositions || pred.getStatus() >= EG_WHITEWIN) {
      pred.setMark();
    }
  }
  m_workGame.popState();
}

EndGameResult EndGameTablebase::getBestResult(Game &game, bool breakOnDraw) { // find best result for player in turn.
  const Player playerInTurn      = game.getPlayerInTurn();
  UINT         pliesToWin        = 999999;
  UINT         pliesToLoose      = 0;
  bool         undefinedFound    = false;
  bool         drawMoveFound     = false;
  bool         winningMoveFound  = false;
  bool         loosingMoveFound  = false;
  const Piece *skipNextPromotion = NULL;

  Move           m;
  MoveGenerator &mg   = game.getMoveGenerator();
  bool           done = false;
  game.pushState();
  for(bool more = mg.firstMove(m); more && !done; more = mg.nextMove(m)) {
    if(!isUsableMove(m)) {
      continue;
    }
    if(skipNextPromotion) {
      if(m.m_piece == skipNextPromotion) {
        continue;
      } else {
        skipNextPromotion = NULL;
      }
    }
    game.tryMove(m);
    EndGamePositionStatus status;
    UINT pliesToEnd;
    switch(status = isTerminalMove(game, m, &pliesToEnd)) {
    case EG_DRAW     :
      drawMoveFound = true;
      done = breakOnDraw;
      break;
    case EG_WHITEWIN:
    case EG_BLACKWIN:
      if(EndGameKeyDefinition::isMetricDTM()) pliesToEnd++;
      if(playerInTurn == STATUSTOWINNER(status)) {
        winningMoveFound = true;
        if(pliesToEnd < pliesToWin) {
          pliesToWin = pliesToEnd;
          if(pliesToWin == 1) {
            done = true;
          }
        }
        if(m.getType() == PROMOTION && EndGameKeyDefinition::isMetricDTC()) {
          // Dont waste time to find a better promotion. if metric is DTC. A Queen will almost always do the job.
          // For metric DTM however, we may find an underpromotion prior to queen. Unlikely, but possible.
          skipNextPromotion = m.m_piece;
        }
      } else {
        loosingMoveFound = true;
        pliesToLoose     = max(pliesToLoose, pliesToEnd);
      }
      break;

    case EG_UNDEFINED:
      { const EndGameKey  succKey    = transformGameKey(game.getKey());
        EndGameResult    &succResult = m_positionIndex.get(succKey);
        if(!succResult.exists()) {
          missingPositionError(_T("getBestResult"), succKey, succResult);
        } else {
          switch(status = succResult.getStatus()) {
          case EG_UNDEFINED:
            undefinedFound = true;
            done = breakOnDraw;
            break;

          case EG_DRAW     :
            drawMoveFound  = true;
            done = breakOnDraw;
            break;

          case EG_WHITEWIN:
          case EG_BLACKWIN:
            { if(playerInTurn == STATUSTOWINNER(status)) {
                winningMoveFound = true;
                pliesToWin       = min(pliesToWin  , succResult.getPliesToEnd()+1);
                if(pliesToWin == 1) {
                  done = true;
                }
              } else {
                loosingMoveFound = true;
                pliesToLoose     = max(pliesToLoose, succResult.getPliesToEnd()+1);
              }
            }
            break;

          default:
            statusError(succResult.getStatus(), succKey);
            break;
          }
        }
      }
      break;

    default:
      statusError(status, game, m);
      break;
    }
    game.unTryMove();
  }
  game.popState();

  if(winningMoveFound) {
    return EndGameResult((playerInTurn == WHITEPLAYER) ? EG_WHITEWIN : EG_BLACKWIN, pliesToWin);
  } else if(drawMoveFound && !m_info.m_canWin[playerInTurn]) {
    return EGR_TERMINALDRAW;
  }
  if(undefinedFound) {
    return EndGameResult();
  } else if(drawMoveFound) {
    return EGR_TERMINALDRAW;
  } else if(loosingMoveFound) {
    return EndGameResult((playerInTurn == WHITEPLAYER) ? EG_BLACKWIN : EG_WHITEWIN, pliesToLoose);
  }

  // There are no moves at all => checkmate or stalemate
  switch(game.findGameResult()) {
  case WHITE_CHECKMATE: return EGR_WHITEISMATE;
  case BLACK_CHECKMATE: return EGR_BLACKISMATE;
  case STALEMATE      : return EGR_STALEMATE;
  default             : gameResultError(game);
  }
  return EndGameResult();
}

bool EndGameTablebase::addPosition(const EndGameKey &key, bool markNew) {
  EndGameResult &egr = m_positionIndex.get(key);
  return addPosition(key, egr, markNew);
}

bool EndGameTablebase::addPosition(const EndGameKey &key, EndGameResult &result, bool markNew) {
  if(result.exists()) {
    return false;
  } else {
    result.setExist();
    m_info.m_undefinedPositions++;
    m_info.m_totalPositions++;
    if(markNew ) {
      result.setMark();
    }
    return true;
  }
}

EndGameResult &EndGameTablebase::setAsTerminalPosition(EndGameResult &dst, EndGamePositionStatus status, UINT pliesToEnd) {
  assert(pliesToEnd <= 1);

  dst.setResult(status, pliesToEnd);

  m_info.m_undefinedPositions--;
  switch(status) {
  case EG_DRAW    :
    if(pliesToEnd) {
      verbose(_T("Warning:setAsTerminalPosition(status=EG_DRAW, pliesToEnd=%d)\n"), pliesToEnd);
    } else {
      m_info.m_stalematePositions++;
    }
    break;

  case EG_WHITEWIN:
    m_info.m_canWin[WHITEPLAYER] = true;
    if(pliesToEnd) {
      m_info.m_terminalWinPositions.incr(WHITEPLAYER);
      adjustMaxPly(WHITEPLAYER, pliesToEnd);
    } else {
      m_info.m_checkMatePositions.incr(WHITEPLAYER);
    }
    break;

  case EG_BLACKWIN:
    m_info.m_canWin[BLACKPLAYER] = true;
    if(pliesToEnd) {
      m_info.m_terminalWinPositions.incr(BLACKPLAYER);
      adjustMaxPly(BLACKPLAYER, pliesToEnd);
    } else {
      m_info.m_checkMatePositions.incr(BLACKPLAYER);
    }
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("status=%d"), status);
  }
  return dst;
}

EndGameResult &EndGameTablebase::setPositionResult(EndGameResult &dst, EndGamePositionStatus status, UINT pliesToEnd) {
  m_info.m_undefinedPositions--;
  switch(status) {
  case EG_DRAW    :
    if(pliesToEnd == 0) {
      setAsStaleMatePosition(dst);
    } else {
      dst.setResult(EG_DRAW, pliesToEnd);
      m_info.m_drawPositions++;
    }
    break;
  case EG_WHITEWIN:
    dst.setResult(status, pliesToEnd);
    m_info.m_canWin[WHITEPLAYER] = true;
    m_info.m_nonTerminalWinPositions.incr(WHITEPLAYER);
    adjustMaxPly(WHITEPLAYER, pliesToEnd);
    break;
  case EG_BLACKWIN:
    dst.setResult(status, pliesToEnd);
    m_info.m_canWin[BLACKPLAYER] = true;
    m_info.m_nonTerminalWinPositions.incr(BLACKPLAYER);
    adjustMaxPly(BLACKPLAYER, pliesToEnd);
    break;
  default         :
    throwInvalidArgumentException(__TFUNCTION__, _T("status=%d"), status);
  }
  return dst;
}

EndGameResult &EndGameTablebase::changePliesToEnd(EndGameResult &dst, EndGamePositionStatus status, UINT pliesToEnd) {
  if(status != dst.getStatus()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("dst=%s, status=%d. pliesToEnd=%d"), dst.toString(true).cstr(), status, pliesToEnd);
  }
  switch(status) {
  case EG_WHITEWIN:
    dst.changePliesToEnd(pliesToEnd);
    adjustMaxPly(WHITEPLAYER, pliesToEnd);
    break;
  case EG_BLACKWIN:
    dst.changePliesToEnd(pliesToEnd);
    adjustMaxPly(BLACKPLAYER, pliesToEnd);
    break;
  default         :
    throwInvalidArgumentException(__TFUNCTION__, _T("status=%d"), status);
  }
  return dst;
}

EndGameResult &EndGameTablebase::setPositionResult(EndGameResult &dst, const EndGameResult &result) {
  return setPositionResult(dst, result.getStatus(), result.getPliesToEnd());
}

EndGameResult &EndGameTablebase::changePliesToEnd(EndGameResult &dst, const EndGameResult &result) {
  return changePliesToEnd(dst, result.getStatus(), result.getPliesToEnd());
}

void EndGameTablebase::missingPositionError(const TCHAR *method, const EndGameKey key, EndGameResult &result) {
  throw MissingPositionException(format(_T("%s:missing position:[%s]"), method, key.toString(m_keydef).cstr()).cstr());
}

void EndGameTablebase::insertManualPositions() {
  EndGameManualPositions mp(m_keydef);
  mp.load();
//  mp.save(stdout);
  for(EndGameKeyIterator it = mp.getKeyIterator(); it.hasNext();) {
    addInitPosition(it.next(), false);
  }
}

bool EndGameTablebase::isUsableMove(const Move &m) const {
  switch(m.getType()) {
  case NORMALMOVE   :
  case ENPASSANT    :
  case PROMOTION    :
    return true;

  case SHORTCASTLING:
  case LONGCASTLING :
    return false;

  default           :
    moveTypeError(m);
    return false;
  }
}

const TCHAR *EndGameTablebase::getDottedLine() {
  static String line = spaceString(getInfoLength(),_T('.'));
  return line.cstr();
}

int EndGameTablebase::getInfoLength() const {
  if(m_infoLength == 0) {
    m_infoLength = (int)m_info.toString(TBIFORMAT_PRINT_UNDEFANDWINS).length();
  }
  return m_infoLength;
}

void EndGameTablebase::logPositionCount() const {
  if(m_logFile == NULL) {
    m_logFile = MKFOPEN(getTempFileName(format(_T("%sCount.log"), getName().cstr())), _T("w"));
  }

  const UINT64 posCount = m_info.m_totalPositions;
  Timestamp    now;
  if(m_generatingPositions) {
    _ftprintf(m_logFile,_T("%s %12llu %12llu %.3lf\n"), now.toString(_T("yyyyMMdd.hhmm")).cstr(),posCount,m_positionsAnalyzed, PERCENT(m_positionsAnalyzed, posCount));
  } else {
    _ftprintf(m_logFile,_T("%s %12llu %12llu %.3lf\n"), now.toString(_T("yyyyMMdd.hhmm")).cstr(),posCount,m_info.m_undefinedPositions, PERCENT(m_info.m_undefinedPositions, posCount));
  }
  fflush(m_logFile);
}

void EndGameTablebase::logUndefinedPosition(const EndGameKey &key) const {
  FILE *f = MKFOPEN(getTbFileName(UNDEFINEDKEYSLOG),_T("a"));
  _ftprintf(f,_T("    %s\n"), key.toString(m_keydef, true).cstr());
  fclose(f);
  m_allKeysFound = false;
}

void EndGameTablebase::verbose(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) const {
  va_list argptr;
  va_start(argptr, format);
  vverbose(format, argptr);
  va_end(argptr);
  m_verboseTriggered = false;
}

void EndGameTablebase::startLogging() {
  if(m_keydef.getPieceCount() > 4 && !m_logTimer.isRunning()) {
    m_logTimer.startTimer(60000, *this, true);
  }
}

void EndGameTablebase::stopLogging() {
  m_logTimer.stopTimer();
  if(m_logFile != NULL) {
    fclose(m_logFile);
    m_logFile = NULL;
  }
}

void EndGameTablebase::startVerboseTrigger(int sec) {
  if(!m_verboseTimer.isRunning()) {
    m_verboseTimer.startTimer(sec*1000, *this, true);
  }
}

void EndGameTablebase::stopVerboseTrigger() {
  m_verboseTimer.stopTimer();
}

void EndGameTablebase::handleTimeout(Timer &timer) {
  switch(timer.getId()) {
  case LOG_TIMER    :
    logPositionCount();
    break;
  case VERBOSE_TIMER:
    m_verboseTriggered = true;
    break;
  default:
    verbose(_T("unknown timerId:%d\n"), timer.getId());
    timer.stopTimer();
  }
}

void EndGameTablebase::list(FILE *f, ListFilter filter) {
  switch(filter) {
  case LIST_WINNER    :
    list(f, m_positionIndex.getIteratorAllWinnerEntries());
    break;
  case LIST_MAXVAR    :
    list(f, LIST_MAXVAR    , true, true);
    break;
  case LIST_CHECKMATES:
    list(f, LIST_CHECKMATES, true, true);
    break;
  case LIST_ALL       :
    list(f, m_positionIndex.getEntryIterator());
    break;
  case LIST_STALEMATES:
    list(f, m_positionIndex.getIteratorStalemateEntries());
    break;
  case LIST_UNDEFINED:
    list(f, m_positionIndex.getIteratorAllNonWinnerEntries());
    break;
  case LIST_PLIESTOEND:
    listPositionCount(f);
    break;
  case LIST_HASHELPINFO:
    list(f, m_positionIndex.getIteratorNonEmptyHelpInfo());
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("filter=%d"), filter);
  }
}

void EndGameTablebase::list(FILE *f, ListFilter filter, bool whiteWin, bool blackWin, int plies) {
  const bool winner[] = { whiteWin, blackWin };
  switch(filter) {
  case LIST_MAXVAR    :
    { forEachPlayer(p) {
        if(winner[p]) {
          if(m_info.m_canWin[p]) {
            list(f, m_positionIndex.getIteratorMinPliesToEnd(p, m_info.m_maxPlies.m_count[p]));
          } else {
            verbose(_T("No winnerpositions for %s\n"), getPlayerNameEnglish(p));
          }
        }
      }
    }
    break;
  case LIST_EXACTPLIES:
    { forEachPlayer(p) {
        if(winner[p]) {
          if(m_info.m_canWin[p]) {
            list(f, m_positionIndex.getIteratorExactPliesToEnd(p, plies));
          } else {
            verbose(_T("No winnerpositions for %s\n"), getPlayerNameEnglish(p));
          }
        }
      }
    }
    break;

  case LIST_CHECKMATES:
    { forEachPlayer(p) {
        if(winner[p]) {
          if(m_info.m_canWin[p]) {
            list(f, m_positionIndex.getIteratorCheckmateEntries(p));
          } else {
            verbose(_T("No winnerpositions for %s\n"), getPlayerNameEnglish(p));
          }
        }
      }
    }
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("filter=%d"), filter);
  }
}

#define LIST_VERBOSE(eosCh) verbose(_T("%.2lf%%%c"), it.getPercentDone(), eosCh);

void EndGameTablebase::list(FILE *f, EndGameEntryIterator &it) {
  if(!isatty(f)) {
    startVerboseTrigger(2);
  }
  TCHAR tmp[256];
  while(it.hasNext()) {
    _ftprintf(f, _T("%s\n"), toStr(tmp, it.next(), true));
    INLOOP_VERBOSE(LIST_VERBOSE)
  }
  stopVerboseTrigger();
}

void EndGameTablebase::listPositionCount(FILE *f) {
  const CompactArray<PositionCount64> wpTable = getWinnerPositionCountArray();
  _ftprintf(f, _T("Positions sorted by plies to end for tablebase %s (%s)\n")
           ,getName().cstr()
           ,EndGameKeyDefinition::getMetricName().cstr());
  _ftprintf(f,_T("Plies   White win   Black win\n"));
  PositionCount64 total;
  for(UINT plies = 0; plies < wpTable.size(); plies++) {
    const PositionCount64 &wp = wpTable[plies];
    total += wp;
    _ftprintf(f,_T("%5u %s\n"), plies, wp.toString().cstr());
  }
  _ftprintf(f, _T("Total %s\n"), total.toString().cstr());
}

#define CONSISTENCYCHECK_VERBOSE(eosCh)                                 \
{ static const String dotString = spaceString(40-_tcslen(msg),_T('.')); \
  verbose(_T("%s%s%s%c")                                                \
         ,msg                                                           \
         ,dotString.cstr()                                              \
         ,it.getProgressStr().cstr()                                    \
         ,eosCh                                                         \
         );                                                             \
}

bool EndGameTablebase::checkConsistency(UINT flags) {
  UINT64 inconsistentPositionCount  = 0;
  bool   subTablebasesOk            = true;
  bool   headerOk                   = true;
  bool   verboseWasStarted          = isVerboseTriggerStarted();

  if(!verboseWasStarted) {
    startVerboseTrigger();
  }

  if(flags & CHECK_POSITIONS) {
    const TCHAR         *msg = _T("Checking all non-winner positions");
    EndGameEntryIterator it  = m_positionIndex.getIteratorAllNonWinnerEntries();
    while(it.hasNext()) {
      const EndGameEntry &entry = it.next();
      m_workGame = entry.getKey().getGameKey(m_keydef);
      const EndGameResult bestResult = getBestResult(m_workGame, false);
      switch(bestResult.getStatus()) {
      case EG_UNDEFINED:
      case EG_DRAW     :
        break;
      case EG_WHITEWIN :
      case EG_BLACKWIN :
        { inconsistentPositionCount++;
          verbose(_T("\nDatabase position [%s] has calculated result [%s]\n")
                 ,toString(entry,true).cstr(), bestResult.toString(true).cstr());

          if(flags & CHECK_LIST_MOVES) {
            MoveResultArray allMoves;
            getAllMoves(m_workGame.getKey(), allMoves);
            for(UINT i = 0; i < allMoves.size(); i++) {
              const MoveWithResult &mr = allMoves[i];
              verbose(_T("%-7s - %s\n")
                     ,PrintableMove(m_workGame,mr).toString(MOVE_SHORTFORMAT).cstr()
                     ,mr.getResult().toString(true).cstr());
            }
          }
          if(flags & CHECK_RETURN_ON_ERROR) {
            goto CheckHeader;
          }
        }
        break;
      }
      INLOOP_VERBOSE(CONSISTENCYCHECK_VERBOSE)
    }
    ENDLOOP_VERBOSE(CONSISTENCYCHECK_VERBOSE)

    msg = _T("Checking all winner positions");

    for(it = m_positionIndex.getIteratorAllWinnerEntries(); it.hasNext();) {
      const EndGameEntry &entry = it.next();
      const EndGameResult dbResult = entry.getValue();
      m_workGame = entry.getKey().getGameKey(m_keydef);
      const EndGameResult bestResult = getBestResult(m_workGame, false);
      if(bestResult.getStatus() != dbResult.getStatus() || bestResult.getPliesToEnd() != dbResult.getPliesToEnd()) {
        inconsistentPositionCount++;
        verbose(_T("Database position [%s] has calculated result [%s]\n")
                ,toString(entry,true).cstr(), bestResult.toString(true).cstr());

        if(flags & CHECK_LIST_MOVES) {
          MoveResultArray allMoves;
          getAllMoves(m_workGame.getKey(), allMoves);
          for(UINT i = 0; i < allMoves.size(); i++) {
            const MoveWithResult &mr = allMoves[i];
            verbose(_T("%-7s - %s\n")
                   ,PrintableMove(m_workGame, mr).toString(MOVE_SHORTFORMAT).cstr()
                   ,mr.getResult().toString(true).cstr());
          }
        }
        if(flags & CHECK_RETURN_ON_ERROR) {
          goto CheckHeader;
        }
      }
      INLOOP_VERBOSE(CONSISTENCYCHECK_VERBOSE)
    }
    ENDLOOP_VERBOSE(CONSISTENCYCHECK_VERBOSE)

    try {
      checkAllSubKeysFound();
    } catch(Exception e) {
      subTablebasesOk = false;
      verbose(_T("%s\n"), e.what());
    }
  }

CheckHeader:
  if(flags & CHECK_HEADER) {
    const TCHAR *msg = _T("Checking header information");
    verbose(_T("%s%s"), msg, spaceString(40-_tcslen(msg),_T('.')).cstr());
    const MaxVariantCount maxPlies = findMaxPlies();
    if(m_info.m_maxPlies != maxPlies) {
      verbose(_T("\nHeader.maxVariantlength = [%s] <> calculated maxVariantlength = [%s].\n")
             ,m_info.m_maxPlies.toString('/', 3).cstr()
             ,maxPlies.toString('/', 3).cstr()
             );
      headerOk = false;
    }

    PositionCount64 checkmates(m_positionIndex.getIteratorCheckmateEntries(WHITEPLAYER).getCount()
                              ,m_positionIndex.getIteratorCheckmateEntries(BLACKPLAYER).getCount()
                              );
    if(m_info.m_checkMatePositions != checkmates) {
      verbose(_T("\nHeader.checkmate positions %s = calculated number of checkmate positions = %s\n")
             ,m_info.m_checkMatePositions.toString('/', 5).cstr()
             ,checkmates.toString('/', 5).cstr()
             );
      headerOk = false;
    }

    const UINT64 positionCount = m_positionIndex.getEntryIterator().getCount();
    if(m_info.m_totalPositions != positionCount) {
      verbose(_T("\nHeader.total position count = %lu != calculated number of positions = %I64u\n")
             ,m_info.m_totalPositions
             ,positionCount);
      headerOk = false;
    }
    const UINT64 undefinedPositionCount = m_positionIndex.getIteratorUndefinedEntries().getCount();
    if(m_info.m_undefinedPositions != undefinedPositionCount) {
      verbose(_T("\nHeader.undefined position count = %lu != calculated number undefined of positions = %I64u\n")
             ,m_info.m_undefinedPositions
             ,undefinedPositionCount);
      headerOk = false;
    }
    const UINT64 stalematePositionCount = m_positionIndex.getIteratorStalemateEntries().getCount();
    if(m_info.m_stalematePositions != stalematePositionCount) {
      verbose(_T("\nHeader.stalemate position count = %lu != calculated number of stalemate positions = %I64u\n")
             ,m_info.m_stalematePositions
             ,stalematePositionCount);
      headerOk = false;
    }
    const PositionCount64 headerWinPositions  = m_info.getWinnerPositionCount();
    const UINT64          winnerPositionCount = m_positionIndex.getIteratorAllWinnerEntries().getCount();
    if(headerWinPositions.getTotal() != winnerPositionCount) {
      verbose(_T("\nHeader.total winner positioncount = %lu != calculated number of total winner positions = %lu\n")
             ,headerWinPositions.getTotal()
             ,winnerPositionCount);
      headerOk = false;
    }
    forEachPlayer(p) {
      if((headerWinPositions.m_count[p] != 0) != m_info.m_canWin[p]) {
        verbose(_T("((\nHeader.winner positioncount(%s) == %lu) != 0) != Header.m_canWin[%s] = %s\n")
               ,getPlayerNameEnglish(p)
               ,headerWinPositions.m_count[p]
               ,getPlayerNameEnglish(p)
               ,boolToStr(m_info.m_canWin[p])
              );
        headerOk = false;
      }
      if((m_info.m_maxPlies.m_count[p] != 0) != m_info.m_canWin[p]) {
        verbose(_T("((\nHeader.Max variant(%s) == %lu plies) != 0) != Header.m_canWin[%s] = %s\n")
               ,getPlayerNameEnglish(p)
               ,m_info.m_maxPlies.m_count[p]
               ,getPlayerNameEnglish(p)
               ,boolToStr(m_info.m_canWin[p])
              );
        headerOk = false;
      }
    }
    if(headerOk) {
      verbose(_T("Header information ok\n"));
    } else {
      verbose(_T("Header information failed\n"));
    }
  }

  if(inconsistentPositionCount > 0) {
    verbose(_T("\n%s inconsistent positions found\n"), format1000(inconsistentPositionCount).cstr());
  }
  if(!verboseWasStarted) {
    stopVerboseTrigger();
  }
  return subTablebasesOk && (inconsistentPositionCount == 0) && headerOk;
}

CompactArray<PositionCount64> EndGameTablebase::getWinnerPositionCountArray() {
  const int            maxPlies = m_info.m_maxPlies.getMax();
  PositionCount64     *wpCount  = new PositionCount64[maxPlies+1]; TRACE_NEW(wpCount);
  startVerboseTrigger(1);
  for(EndGameEntryIterator it = m_positionIndex.getIteratorAllWinnerEntries(); it.hasNext();) {
    const EndGameResult &res = it.next().getValue();
    wpCount[res.getPliesToEnd()].incr(STATUSTOWINNER(res.getStatus()));
    if(m_verboseTriggered) {
      verbose(_T("%3.0lf%%\r"), it.getPercentDone());
    }
  }
  stopVerboseTrigger();
  CompactArray<PositionCount64> result;
  for(int i = 0; i <= maxPlies; i++) {
    result.add(wpCount[i]);
  }
  SAFEDELETEARRAY(wpCount);
  return result;
}

void EndGameTablebase::checkAllSubKeysFound() const {
  if(!allSubKeysFound()) {
    throwException(_T("There are undefined positions in the subtablebases. Se c:\\temp for logfiles containing undefined positions"));
  }
}

bool EndGameTablebase::allSubKeysFound() const {
  for(Iterator<Entry<SubTablebaseKey, SubTablebasePositionInfo> > it = m_subTablebaseMap.getEntryIterator(); it.hasNext();) {
    if(!it.next().getValue().allKeysFound()) {
      return false;
    }
  }
  return true;
}

String EndGameTablebase::save(bool convert) const {
  return save(getTbFileName(ALLTABLEBASE), convert);
}

String EndGameTablebase::saveAllForwardPositions(bool convert) {
  if(!convert) {
    m_info.m_buildTime = _time32(NULL);
  }
  return save(getTbFileName(ALLFORWARDPOSITIONS), convert);
}

String EndGameTablebase::loadAllForwardPositions() {
  const String fileName = getTbFileName(ALLFORWARDPOSITIONS);
  load(fileName);
  return fileName;
}

String EndGameTablebase::saveAllRetroPositions(bool convert) {
  if(!convert) {
    m_info.m_buildTime = _time32(NULL);
  }
  return save(getTbFileName(ALLRETROPOSITIONS), convert);
}

String EndGameTablebase::loadAllRetroPositions() {
  const String fileName = getTbFileName(ALLRETROPOSITIONS);
  load(fileName);
  return fileName;
}

String EndGameTablebase::save(const String &fileName, bool convert) const {
  String result;
  if(convert) {
//    if(m_info.m_consistencyCheckedTime != 0) {
//      throwException(_T("%s already has a consistencyCheckedTime"), getName().cstr());
//    }
//    if(!m_info.isConsistent()) {
//      throwException(_T("%s is not checked for consistency"), getName().cstr());
//    }
//    ((EndGameTablebase*)this)->m_info.m_consistencyCheckedTime = (__time32_t)getFileTime(ALLTABLEBASE);
    result = FileNameSplitter(fileName).setExtension(_T("new")).getFullPath();
    verbose(_T("Saving tablebase to %s..."), result.cstr());
    saveNew(CountedByteOutputStream(StreamProgress(this), CompressFilter(ByteOutputFile(result))));
  } else {
    result = fileName;
    verbose(_T("Saving tablebase to %s..."), result.cstr());
    save(CountedByteOutputStream(StreamProgress(this), CompressFilter(ByteOutputFile(result))));
  }
  return result;
}

void EndGameTablebase::saveNew(ByteOutputStream &s) const {
//  save(s);
  throwException(_T("%s:saveNew:Not supported"), getName().cstr());
//  getTablebaseInfoNew().save(s);
//  m_positionIndex.save(s);
}

void EndGameTablebase::save(ByteOutputStream &s) const {
  TablebaseInfo   tmp = m_info;
  tmp.m_indexCapacity = m_positionIndex.size();
  tmp.save(s);
  m_positionIndex.save(s);
}

String EndGameTablebase::compress(bool convert) {
  String fileName = getTbFileName(COMPRESSEDTABLEBASE);
  if(convert) {
    fileName = FileNameSplitter(fileName).setExtension(_T("new")).getFullPath();
  }
  try {
    verbose(_T("Compressing tablebase to %s..."), fileName.cstr());
    if(convert) {
      compressNew(CompressFilter(ByteOutputFile(fileName)));
    } else {
      compress(CompressFilter(ByteOutputFile(fileName)));
    }
    verbose(_T("Tablebase compressed\n"));
    return fileName;
  } catch(Exception e) {
    verbose(_T("%s\n"), e.what());
    unlink(fileName);
    throw;
  }
}

void EndGameTablebase::compress(ByteOutputStream &s) {
  m_info.m_indexCapacity = m_positionIndex.size();
#ifndef NEWCOMPRESSION
  m_info.save(s);
  m_positionIndex.saveCompressed(s, m_info);
#else // NEWCOMPRESSION
  BigEndianOutputStream bes(s);
  m_info.save(bes);
  m_positionIndex.saveCompressed(bes, m_info);
#endif // NEWCOMPRESSION
}

MaxVariantCount EndGameTablebase::findMaxPlies() const {
  int wmPlies = 0, bmPlies = 0;
  for(EndGameEntryIterator it = m_positionIndex.getIteratorAllWinnerEntries(); it.hasNext();) {
    const EndGameEntry  &e          = it.next();
    const EndGameResult  egr        = e.getValue();
    const int            pliesToEnd = egr.getPliesToEnd();
    switch(egr.getStatus()) {
    case EG_WHITEWIN:
      if(pliesToEnd > wmPlies) {
        wmPlies = pliesToEnd;
      }
      break;
    case EG_BLACKWIN:
      if(pliesToEnd > bmPlies) {
        bmPlies = pliesToEnd;
      }
      break;
    default:
      statusError(egr.getStatus(), e.getKey());
    }
  }
  return MaxVariantCount(wmPlies, bmPlies);
}

#endif // TABLEBASE_BUILDER
