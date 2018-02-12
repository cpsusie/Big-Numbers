#include "stdafx.h"
#include <PriorityQueue.h>
#include "EndGameKeyDefinition.h"
#include "EndGameKeyCodec.h"

void PieceTypeWithIndex::init(PieceType pieceType, int index) {
  assert(index < 15);
  m_data = (index << 4) | (int)pieceType;
};

class PieceTypeWithIndexComparator : public Comparator<PieceTypeWithIndex> {
public:
  int compare(const PieceTypeWithIndex &t1, const PieceTypeWithIndex &t2) {
    const int c = (int)t1.getPieceType() - t2.getPieceType();
    return c ? c : (t1.getIndex() - t2.getIndex());
  }

  AbstractComparator *clone() const {
    return new PieceTypeWithIndexComparator();
  }
};

EndGameGlobalProperties::EndGameGlobalProperties() {
#ifndef NEWCOMPRESSION
  m_dbPath = _T("c:\\temp\\ChessEndGames");
#else // NEWCOMPRESSION
  m_dbPath = _T("c:\\temp\\CG1");
#endif // NEWCOMPRESSION
  m_metric = DEPTH_TO_CONVERSION;
}

bool EndGameGlobalProperties::setDbPath(const String &path) {
  FileNameSplitter newInfo(path);
  FileNameSplitter oldInfo(m_dbPath);
  const bool ret = newInfo.getFullPath() != oldInfo.getFullPath();
  setProperty(ENDGAME_PATH, m_dbPath, path);
  return ret;
}

void EndGameGlobalProperties::setMetric(TablebaseMetric m) {
  setProperty(ENDGAME_METRIC, m_metric, m);
}

EndGameGlobalProperties EndGameKeyDefinition::s_globalProperties;

const TCHAR *EndGameKeyDefinition::s_metricName[] = {
  _T("DTC")
 ,_T("DTM")
};

String EndGameKeyDefinition::getDbFileName(const String &fileName) { // static
  return FileNameSplitter::getChildName(getDbPath(), fileName);
}

String EndGameKeyDefinition::getFileMetricSuffix() { // static
  return format(_T("%s.dat"), getMetricName());
}

void EndGameKeyDefinition::addPropertyChangeListener(PropertyChangeListener *listener) { // static
  s_globalProperties.addPropertyChangeListener(listener);
}

void EndGameKeyDefinition::removePropertyChangeListener(PropertyChangeListener *listener) { // static
  s_globalProperties.removePropertyChangeListener(listener);
}

#ifdef TABLEBASE_BUILDER
String EndGameKeyDefinition::getTablebaseFileName() const {
  return getDbFileName(getName() + _T("EndGame") + getFileMetricSuffix());
}

#else

String EndGameKeyDefinition::getDecompressedFileName() const {
  FileNameSplitter info(getDbFileName(getName() + _T("Decompressed") + getFileMetricSuffix()));
  return info.setDir(FileNameSplitter::getChildName(info.getDir(), _T("Decompressed"))).getFullPath();
}
#endif

String EndGameKeyDefinition::getCompressedFileName() const {
  return getDbFileName(getName() + _T("Compressed") + getFileMetricSuffix());
}



#ifdef IS32BIT
int findRange2Equal(EndGamePosIndex f, EndGamePosIndex index) {
  const double _8 = 8.0;
  unsigned short cwSave, ctrlFlags;

  int result;
//  return n = (sqrt(1 + (double)index*8/f) - 1) / 2; invers of index = f*n*(n+1)/2 = sum(i=1..n) { f * i }
  __asm {
    fnstcw  cwSave               // Save control word
    mov ax, cwSave
    or  ax, 0x0c00               // Set   bit 10,11 to truncate result
    mov ctrlFlags, ax
    fldcw ctrlFlags
    fild index
    fld _8
    fmul
    fild f
    fdiv
    fld1
    fadd
    fsqrt
    fld1
    fsub
    fistp result
    fldcw cwSave
  }
  return result >> 1;
}

#endif

int findTableRange(const EndGamePosIndex *rangeTable, UINT size, EndGamePosIndex index) {
  int l = 1, r = size;
  while(l < r) {
    const UINT m = (l+r)>>1;
    if(rangeTable[m] <= index) {
      l = m+1;
    } else {
      r = m;
    }
  }
  return r-1;
}

#ifdef __NEVER__

// this is an alternative way to calculate the offset in the tables generated
// by INIT_RANGETABLE3EQUAL. But it's much slower than tablelookup, at least
// in C-code. Maybe some assembler-routine would do it faster. But who cares

static int findRange3Equal(EndGamePosIndex index, EndGamePosIndex m) {
  m /= 2;
  if(index < m) return 0;
  const double c = 3.0 * index/m;
  const double u = root(c - sqrt(c*c-1.0/27), 3);
  int          r = (int)((3*u*(u-1) + 1)/3/u)+1;
  while (GET_RANGESTART3EQUAL(m, r) > index) {
    r--;
  }
  return r;
}

static void test2Equal() {
  EndGamePosIndex testTable[60];
  EndGamePosIndex maxadr = 30000;
  INIT_RANGETABLE3EQUAL(testTable,maxadr)
  DUMP_RANGETABLE(testTable)
  EndGamePosIndex firstIndex = 0;
  EndGamePosIndex lastIndex  = LASTVALUE(testTable);
goto StartTimeMeasure;
  bool ok = true;
  for(EndGamePosIndex i    = firstIndex; i <= lastIndex; i++) {
    const int rOld = findTableRange(testTable, ARRAYSIZE(testTable), i);
    const int rNew = findRange3Equal(i, maxadr);
    if(rNew != rOld) {
      _tprintf(_T("findTableRange(%6llu):%4d, findRange3Equal(%6llu, %4llu):%4d\n")
              , i, rOld, i, maxadr, rNew);
      const int rNew   = findRange3Equal(i, maxadr);
      ok = false;
    }
    if(i % 5000000 == 0) {
      _tprintf(_T("%.2lf%%\r"), PERCENT(i,lastIndex));
    }
  }
  _tprintf(ok?_T("All ok!\n") : _T("Errors found\n"));

  double startTime = getProcessTime();
  for(EndGamePosIndex i = firstIndex; i <= lastIndex; i++) {
    const int rOld = findTableRange(testTable, ARRAYSIZE(testTable), i);
    if(i % 5000000 == 0) {
      _tprintf(_T("%11s : %3d ... %.2lf%% \r"), format1000(i).cstr(), rOld, PERCENT(i,lastIndex));
    }
  }
  const double findTableRangeTime = getProcessTime() - startTime;
  _tprintf(_T("Time(findTableRange):%.4lf sec\n"), findTableRangeTime/1000000);

  startTime = getProcessTime();
  for(EndGamePosIndex i = firstIndex; i <= lastIndex; i++) {
    const int rNew = findRange3Equal(i, maxadr);
    if(i % 5000000 == 0) {
      _tprintf(_T("%11s : %3d ... %.2lf%% \r"), format1000(i).cstr(), rNew, PERCENT(i,lastIndex));
    }
  }
  const double findRange3EqualTime = getProcessTime() - startTime;
  _tprintf(_T("Time(findRange3Equal):%.4lf sec\n"), findRange3EqualTime/1000000);
}

#endif // __NEVER__

EndGameKeyDefinition::EndGameKeyDefinition(PieceKey pk2) : m_totalPieceCount(3) {
  init(pk2);
}

EndGameKeyDefinition::EndGameKeyDefinition(PieceKey pk2, PieceKey pk3) : m_totalPieceCount(4) {
  init(pk2, pk3);
}

EndGameKeyDefinition::EndGameKeyDefinition(PieceKey pk2, PieceKey pk3, PieceKey pk4) : m_totalPieceCount(5) {
  init(pk2, pk3, pk4);
}

EndGameKeyDefinition::EndGameKeyDefinition(PieceKey pk2, PieceKey pk3, PieceKey pk4, PieceKey pk5) : m_totalPieceCount(6) {
  init(pk2, pk3, pk4, pk5);
}

void EndGameKeyDefinition::init(PieceKey pk2, ...) {
  assert((m_totalPieceCount >= 3) && (m_totalPieceCount <= ARRAYSIZE(m_pieceKey)));

#ifdef TABLEBASE_BUILDER
  m_usedIndex = NULL;
#endif

  int i = 0;
  m_pieceKey[i++] = WHITEKING;
  m_pieceKey[i++] = BLACKKING;
  m_pieceKey[i++] = pk2;

  va_list argptr;
  va_start(argptr, pk2);
  while(i < m_totalPieceCount) {
    m_pieceKey[i++] = va_arg(argptr, PieceKey);
  }
  va_end(argptr);

  while(i < ARRAYSIZE(m_pieceKey)) {
    m_pieceKey[i++] = EMPTYPIECEKEY;
  }
  UINT pieceCount[2];
  int  kingIndex[2];
  pieceCount[WHITEPLAYER] = pieceCount[BLACKPLAYER] =  0;
  kingIndex[ WHITEPLAYER] = kingIndex[ BLACKPLAYER] = -1;

  for(i = 0; i < m_totalPieceCount; i++) {
    const PieceKey  pk     = m_pieceKey[i];
    const Player    player = GET_PLAYER_FROMKEY(pk);
    const PieceType type   = getPieceType(i);
    UINT           &pCount = pieceCount[player];

    m_orderedPieceType[player][pCount] = PieceTypeWithIndex(type,i);
    if(type == King) {
      assert(kingIndex[player] == -1);
      kingIndex[player] = i;
    }
    pCount++;
  }
  assert(kingIndex[WHITEPLAYER] == 0);
  assert(kingIndex[BLACKPLAYER] == 1);

  m_pieceCount = (pieceCount[BLACKPLAYER] << 4) | pieceCount[WHITEPLAYER];

  quickSort(m_orderedPieceType[WHITEPLAYER], getPieceCount(WHITEPLAYER), sizeof(PieceTypeWithIndex), PieceTypeWithIndexComparator());
  quickSort(m_orderedPieceType[BLACKPLAYER], getPieceCount(BLACKPLAYER), sizeof(PieceTypeWithIndex), PieceTypeWithIndexComparator());

//#define PRINT_INFO
#ifdef PRINT_INFO
  _tprintf(_T("EndGameKeyDefinition for %s endgame\n"), toString().cstr());
  _tprintf(_T("Total pieceCount:%d. "), m_totalPieceCount);
  _tprintf(_T("White pieceCount:%d. Black pieceCount:%d.\n"), getPieceCount(WHITEPLAYER), getPieceCount(BLACKPLAYER));

  for(i = 0; i < m_totalPieceCount; i++) {
    const PieceKey  pk     = m_pieceKey[i];
    const Player    player = GET_PLAYER_FROMKEY(pk);
    const PieceType type   = GET_TYPE_FROMKEY(pk);
    _tprintf(_T("pk[%d]: %s %-9s\n")
          ,i
          ,getPlayerNameEnglish(player)
          ,getPieceTypeNameEnglish(type)
          );
  }

#define GETORDEREDPIECENAME( player,i)  (((i) < getPieceCount(player)) ? getPieceTypeNameEnglish(m_orderedPieceType[player][i].getPieceType()) : _T("None"))
#define GETORDEREDPIECEINDEX(player,i)  (((i) < getPieceCount(player)) ? m_orderedPieceType[player][i].getIndex() : -1)


  _tprintf(_T("\n%s\nOrdered PieceType\n   white       black\n"), spaceString(25,'_').cstr());
  for(i = 0; i < ARRAYSIZE(m_orderedPieceType[0]); i++) {
    _tprintf(_T("%2d:%-6s %2d   %-6s %2d\n")
          ,i
          ,GETORDEREDPIECENAME(WHITEPLAYER,i), GETORDEREDPIECEINDEX(WHITEPLAYER,i)
          ,GETORDEREDPIECENAME(BLACKPLAYER,i), GETORDEREDPIECEINDEX(BLACKPLAYER,i)
          );
  }

  _tprintf(_T("%s\n"), spaceString(25,'_').cstr());
#endif

}

int EndGameKeyDefinition::findKeyIndexByCount(PieceKey pieceKey, int n) const {
  int count = 0;
  for(int i = 0; i < m_totalPieceCount; i++) {
    if(m_pieceKey[i] == pieceKey) {
      count++;
      if(count == n) {
        return i;
      }
    }
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("pieceKey=%s %s, n=%d")
                               ,getPlayerNameEnglish(GET_PLAYER_FROMKEY(pieceKey))
                               ,getPieceTypeNameEnglish(GET_TYPE_FROMKEY(pieceKey))
                               ,n);
  return -1;
}

int EndGameKeyDefinition::getPieceCount(Player player, PieceType type) const {
  const PieceKey pk = MAKE_PIECEKEY(player,type);
  int count = 0;
  for(UINT i = m_totalPieceCount; i--;) {
    if(m_pieceKey[i] == pk) {
      count++;
    }
  }
  return count;
}

int EndGameKeyDefinition::getPieceCount(PieceType type) const {
  int count = 0;
  for(UINT i = m_totalPieceCount; i--;) {
    if(getPieceType(i) == type) {
      count++;
    }
  }
  return count;
}

void EndGameKeyDefinition::pieceTypeError(int index, const String &msg) const {
  throwException(_T("%s:PieceType[%d]=%s. %s")
                ,toString().cstr()
                ,index
                ,getPieceTypeNameEnglish(getPieceType(index))
                ,msg.cstr()
                );
}

void EndGameKeyDefinition::impossibleEncodingError(EndGameKey key) const {
  throwInvalidArgumentException(_T("keyToIndex"), _T("%s:No encoding possible. Key=[%s]"), toString().cstr(), key.toString(*this).cstr());
}

void EndGameKeyDefinition::checkIsPawn(UINT index, bool expected) const {
  if((getPieceType(index) == Pawn) != expected) {
    pieceTypeError(index, expected ? _T("Must be pawn") : _T("Cannot be pawn"));
  }
}

void EndGameKeyDefinition::checkSameOwner(UINT index1, UINT index2, bool expected) const {
  if((getPieceOwner(index1) == getPieceOwner(index2)) != expected) {
    pieceTypeError(index1, format(expected ? _T("Different owner of piece %d,%d") : _T("Owner must be different for piece %d,%d"), index1, index2));
  }
}

EndGameKey EndGameKeyDefinition::getTransformedKey(EndGameKey key, SymmetricTransformation st) const {
  assert(st != TRANSFORM_SWAPPLAYERS);
  if(st == 0) {
    return key;
  }
  switch(m_totalPieceCount) {
  case 6 : key.setPosition(5,GameKey::transform(key.getPosition(5), st)); // continue case
  case 5 : key.setPosition(4,GameKey::transform(key.getPosition(4), st)); // continue case
  case 4 : key.setPosition(3,GameKey::transform(key.getPosition(3), st)); // continue case
  case 3 : key.setPosition(2,GameKey::transform(key.getPosition(2), st));
           key.setPosition(1,GameKey::transform(key.getPosition(1), st));
           key.setPosition(0,GameKey::transform(key.getPosition(0), st));
           return key;
  default: invalidPieceCountError(__TFUNCTION__);
  }
  return key;
}

EndGameKey EndGameKeyDefinition::getNormalizedKey(EndGameKey key, EndGamePosIndex *index) const {
  static const TCHAR *msg = _T("Invalid EndGameKey:getNormalizedKey(%s):%s");
  const EndGamePosIndex ki = keyToIndex(key);
  if(index) *index = ki;
  const EndGameKey result = indexToKey(ki);
  validateKey(result, msg);
  return result;
}

void EndGameKeyDefinition::validateKey(EndGameKey key, const TCHAR *msg) const {
  const UINT wkPos = key.getWhiteKingPosition();
  const UINT bkPos = key.getBlackKingPosition();
  if(POSADJACENT(wkPos, bkPos)) {
    throwException(msg, key.toString(*this).cstr(), key.toString(*this).cstr());
  }
  for(UINT i = 2; i < m_totalPieceCount; i++) {
    const UINT posi = key.getPosition(i);
    switch(i) {
    case 5:
      if(posi == key.getPosition(4)) {
        throwException(msg, key.toString(*this).cstr(), key.toString(*this).cstr());
      }
      // continue case
    case 4:
      if(posi == key.getPosition(3)) {
        throwException(msg, key.toString(*this).cstr(), key.toString(*this).cstr());
      }
      // continue case
    case 3:
      if(posi == key.getPosition(2)) {
        throwException(msg, key.toString(*this).cstr(), key.toString(*this).cstr());
      }
      // continue case
    case 2:
      if((posi == wkPos) || (posi == bkPos)) {
        throwException(msg, key.toString(*this).cstr(), key.toString(*this).cstr());
      }
      break;
    default:
      invalidPieceCountError(__TFUNCTION__);
    }
  }
}

TCHAR *EndGameKeyDefinition::createKeyString(TCHAR *dst, EndGameKey key, bool initFormat) const {
  if(initFormat) {
    return _tcscpy(dst, createInitKeyString(key).cstr());
  }

  TCHAR blackStr[100];
  return _tcscat(createWhiteKeyString(dst, key), createBlackKeyString(blackStr, key));
}

static const TCHAR *shortNames[]    = { EMPTYSTRING,_T("K"),_T("Q"),_T("R"),_T("B"),_T("N"),EMPTYSTRING };
static const TCHAR *playerShortName = _T("WB");
#define GETFIELDNAME(pos) Game::s_fieldInfo[pos].m_name

TCHAR *EndGameKeyDefinition::createWhiteKeyString(TCHAR *dst, EndGameKey key) const {
  switch(getPieceCount(WHITEPLAYER)) {
  case 1:
    _stprintf(dst, _T("%c White:K%s ")
               , playerShortName[key.getPlayerInTurn()]
               , GETFIELDNAME(   key.getWhiteKingPosition()));
    return dst;
  case 2:
    _stprintf(dst, _T("%c White:K%s %s%s ")
               , playerShortName[key.getPlayerInTurn()]
               , GETFIELDNAME(   key.getWhiteKingPosition())
               , shortNames[                     m_orderedPieceType[WHITEPLAYER][1].getPieceType()]
               , GETFIELDNAME(   key.getPosition(m_orderedPieceType[WHITEPLAYER][1].getIndex())));
    return dst;
  case 3:
    _stprintf(dst, _T("%c White:K%s %s%s %s%s ")
               , playerShortName[key.getPlayerInTurn()]
               , GETFIELDNAME(   key.getWhiteKingPosition())
               , shortNames[                     m_orderedPieceType[WHITEPLAYER][1].getPieceType()]
               , GETFIELDNAME(   key.getPosition(m_orderedPieceType[WHITEPLAYER][1].getIndex()))
               , shortNames[                     m_orderedPieceType[WHITEPLAYER][2].getPieceType()]
               , GETFIELDNAME(   key.getPosition(m_orderedPieceType[WHITEPLAYER][2].getIndex())));
    return dst;
  case 4:
    _stprintf(dst, _T("%c White:K%s %s%s %s%s %s%s ")
               , playerShortName[key.getPlayerInTurn()]
               , GETFIELDNAME(   key.getWhiteKingPosition())
               , shortNames[                     m_orderedPieceType[WHITEPLAYER][1].getPieceType()]
               , GETFIELDNAME(   key.getPosition(m_orderedPieceType[WHITEPLAYER][1].getIndex()))
               , shortNames[                     m_orderedPieceType[WHITEPLAYER][2].getPieceType()]
               , GETFIELDNAME(   key.getPosition(m_orderedPieceType[WHITEPLAYER][2].getIndex()))
               , shortNames[                     m_orderedPieceType[WHITEPLAYER][3].getPieceType()]
               , GETFIELDNAME(   key.getPosition(m_orderedPieceType[WHITEPLAYER][3].getIndex())));
    return dst;
  default: throwException(_T("%s:Invalid number of white pieces (=%d)")
                         ,__TFUNCTION__
                         ,getPieceCount(WHITEPLAYER));
           return _T("????");
  }
}

TCHAR *EndGameKeyDefinition::createBlackKeyString(TCHAR *dst, EndGameKey key) const {
  switch(getPieceCount(BLACKPLAYER)) {
  case 1:
    _stprintf(dst, _T("Black:K%s")
               , GETFIELDNAME(   key.getBlackKingPosition()));
    return dst;
  case 2:
    _stprintf(dst, _T("Black:K%s %s%s")
               , GETFIELDNAME(   key.getBlackKingPosition())
               , shortNames[                     m_orderedPieceType[BLACKPLAYER][1].getPieceType()]
               , GETFIELDNAME(   key.getPosition(m_orderedPieceType[BLACKPLAYER][1].getIndex())));
    return dst;
  case 3:
    _stprintf(dst, _T("Black:K%s %s%s %s%s")
               , GETFIELDNAME(   key.getBlackKingPosition())
               , shortNames[                     m_orderedPieceType[BLACKPLAYER][1].getPieceType()]
               , GETFIELDNAME(   key.getPosition(m_orderedPieceType[BLACKPLAYER][1].getIndex()))
               , shortNames[                     m_orderedPieceType[BLACKPLAYER][2].getPieceType()]
               , GETFIELDNAME(   key.getPosition(m_orderedPieceType[BLACKPLAYER][2].getIndex())));
    return dst;
  case 4:
    _stprintf(dst, _T("Black:K%s %s%s %s%s %s%s")
               , GETFIELDNAME(   key.getBlackKingPosition())
               , shortNames[                     m_orderedPieceType[BLACKPLAYER][1].getPieceType()]
               , GETFIELDNAME(   key.getPosition(m_orderedPieceType[BLACKPLAYER][1].getIndex()))
               , shortNames[                     m_orderedPieceType[BLACKPLAYER][2].getPieceType()]
               , GETFIELDNAME(   key.getPosition(m_orderedPieceType[BLACKPLAYER][2].getIndex()))
               , shortNames[                     m_orderedPieceType[BLACKPLAYER][3].getPieceType()]
               , GETFIELDNAME(   key.getPosition(m_orderedPieceType[BLACKPLAYER][3].getIndex())));
    return dst;
  default: throwException(_T("%s:Invalid number of black pieces (=%d)")
                         ,__TFUNCTION__
                         ,getPieceCount(BLACKPLAYER));
           return _T("????");
  }
}

String EndGameKeyDefinition::createInitKeyString(EndGameKey key) const {
  String result = key.getPlayerInTurn()==WHITEPLAYER ? _T("WHITEPLAYER") : _T("BLACKPLAYER");
  for(int i = 0; i < m_totalPieceCount; i++) {
    result += _T(",");
    result += getFieldName(key.getPosition(i));
  }
  return toUpperCase(result);
}

#ifdef TABLEBASE_BUILDER

EndGamePosIndex EndGameKeyDefinition::keyToIndexNew(EndGameKey key) const { // should be overridden
  throwException(_T("%s:No conversion defined"), __TFUNCTION__);
  return 0;
}

EndGameKey EndGameKeyDefinition::indexToKeyNew(EndGamePosIndex index) const { // should be overridden
  throwException(_T("%s:No conversion defined"), __TFUNCTION__);
  return EndGameKey();
}

bool EndGameKeyDefinition::hasConversion() const {
  try {
    indexToKeyNew(0);
    return true;
  } catch(Exception e) {
    return false;
  }
}

const String &EndGameKeyDefinition::toString(bool longNames) const {
  if(longNames) {
    if(m_longName.length() == 0) {
      m_longName = getPositionSignature().toString(true);
    }
    return m_longName;
  } else {
    if(m_shortName.length() == 0) {
      m_shortName = getPositionSignature().toString(false);
    }
    return m_shortName;
  }
}

const PositionSignature &EndGameKeyDefinition::getPositionSignature() const {
  if(m_positionSignature.isEmpty()) {
    m_positionSignature = createPositionSignature();
  }
  return m_positionSignature;
}

#else

String EndGameKeyDefinition::toString(bool longNames) const {
  return getPositionSignature().toString(longNames);
}

PositionSignature EndGameKeyDefinition::getPositionSignature()  const {
  return createPositionSignature();
}

#endif

PositionSignature EndGameKeyDefinition::createPositionSignature() const {
  PositionSignature result;
  for(int i = 0; i < m_totalPieceCount; i++) {
    result.add(m_pieceKey[i]);
  }
  return result;
}

SymmetricTransformation EndGameKeyDefinition::getPlayTransformation(const Game &game) const {
  const PositionSignature gps = game.getPositionSignature();
  const PositionSignature kps = getPositionSignature();
  if(gps == kps) {
    return 0;
  } else if(kps.swapPlayers() == gps) {
    return TRANSFORM_SWAPPLAYERS;
  }
  throwInvalidArgumentException(__TFUNCTION__
                               ,_T("EndGameKeyDefinition has signature %s. Game has %s")
                               ,kps.toString().cstr()
                               ,gps.toString().cstr()
                               );
  return 0;
}

void EndGameKeyDefinition::invalidSquareError(EndGameKey key) { // static
  throwException(_T("Position %s not contained in any square")
                ,getFieldName(key.getWhiteKingPosition()));
}

void EndGameKeyDefinition::sym8DecisionSwitchError(int line) { // static
  throwException(_T("Line %d:SYM8DECISIONSWITCH dropped to the end"), line);
}

void EndGameKeyDefinition::pawnSymSwitchError(int line) { // static
  throwException(_T("Line %d:DECIDEPAWNSYMTRANSFORM2EQUALPAWNS dropped to the end"), line);
}

void EndGameKeyDefinition::invalidPieceCountError(const TCHAR *method) const {
  throwException(_T("%s:Invalid number of pieces (=%u) must be [3..%d]"), method, m_totalPieceCount, MAX_ENDGAME_PIECECOUNT);
}

#define SYM8DECISIONSWITCH(decideStatement)                                                                   \
{ switch(GETSQUARE(key.getWhiteKingPosition())) {                                                             \
  case LOWERLEFT_SQUARE :                                                                                     \
    decideStatement(      IS_OFFMAINDIAG1, IS_ABOVEMAINDIAG1, TRANSFORM_MIRRORDIAG1, 0                    )   \
    break;                                                                                                    \
  case LOWERRIGHT_SQUARE:                                                                                     \
    decideStatement(      IS_OFFMAINDIAG2, IS_ABOVEMAINDIAG2, TRANSFORM_ROTATERIGHT, TRANSFORM_MIRRORCOL  )   \
    break;                                                                                                    \
  case UPPERLEFT_SQUARE :                                                                                     \
    decideStatement(      IS_OFFMAINDIAG2, IS_ABOVEMAINDIAG2, TRANSFORM_MIRRORROW  , TRANSFORM_ROTATELEFT )   \
    break;                                                                                                    \
  case UPPERRIGHT_SQUARE:                                                                                     \
    decideStatement(      IS_OFFMAINDIAG1, IS_ABOVEMAINDIAG1, TRANSFORM_ROTATE180  , TRANSFORM_MIRRORDIAG2)   \
    break;                                                                                                    \
  default:                                                                                                    \
    invalidSquareError(key);                                                                                  \
  }                                                                                                           \
  sym8DecisionSwitchError(__LINE__);                                                                          \
  return 0;                                                                                                   \
}

#define DECIDESYM8TRANSFORM3MEN(f1Offdiag, f2Offdiag, trTrue, trFalse)                                        \
{ if(f1Offdiag(key.getWhiteKingPosition())) {                                                                 \
    return f2Offdiag(key.getWhiteKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getBlackKingPosition())) {                                                          \
    return f2Offdiag(key.getBlackKingPosition())  ? trTrue : trFalse;                                         \
  } else {                                                                                                    \
    return f2Offdiag(key.getPosition(2))          ? trTrue : trFalse;                                         \
  }                                                                                                           \
}

// Creates a Symmetric Transformation, that moves W.King to lower left triangle (bounded by a1,d1,d4)
// and if this is on diagonal a1-h8, moves B.King below this diagonal, ie to the triangle bounded by (a1,h1,h8).
// If B.king is on main diagonal1 too, then flip board so piece2 will be below this diagonal.
SymmetricTransformation EndGameKeyDefinition::getSym8Transformation3Men(EndGameKey key) { // static
  SYM8DECISIONSWITCH(DECIDESYM8TRANSFORM3MEN)
}

#define DECIDESYM8TRANSFORM4MEN(f1Offdiag, f2Offdiag, trTrue, trFalse)                                        \
{ if(f1Offdiag(key.getWhiteKingPosition())) {                                                                 \
    return f2Offdiag(key.getWhiteKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getBlackKingPosition())) {                                                          \
    return f2Offdiag(key.getBlackKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition(2))) {                                                                  \
    return f2Offdiag(key.getPosition(2))          ? trTrue : trFalse;                                         \
  } else {                                                                                                    \
    return f2Offdiag(key.getPosition(3))          ? trTrue : trFalse;                                         \
  }                                                                                                           \
}

// Creates a Symmetric Transformation, that moves W.King to lower left triangle (bounded by a1,d1,d4)
// and if this is on diagonal a1-h8, moves B.King below this diagonal, ie to the triangle bounded by (a1,h1,h8).
// If B.king is on main diagonal too, then flip board so piece2 will be below this diagonal.
// if piece2 is on main diagonal too, flip board so piece3 will be below the diagonal
SymmetricTransformation EndGameKeyDefinition::getSym8Transformation4Men(EndGameKey key) { // static
  SYM8DECISIONSWITCH(DECIDESYM8TRANSFORM4MEN)
}

#define DECIDESYM8TRANSFORM5MEN(f1Offdiag, f2Offdiag, trTrue, trFalse)                                        \
{ if(f1Offdiag(key.getWhiteKingPosition())) {                                                                 \
    return f2Offdiag(key.getWhiteKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getBlackKingPosition())) {                                                          \
    return f2Offdiag(key.getBlackKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition(2))) {                                                                  \
    return f2Offdiag(key.getPosition(2))          ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition(3))) {                                                                  \
    return f2Offdiag(key.getPosition(3))          ? trTrue : trFalse;                                         \
  } else {                                                                                                    \
    return f2Offdiag(key.getPosition(4))          ? trTrue : trFalse;                                         \
  }                                                                                                           \
}

// See comment on EndGameKeyDefinitionBase3Men and -4Men
SymmetricTransformation EndGameKeyDefinition::getSym8Transformation5Men(EndGameKey key) { // static
  SYM8DECISIONSWITCH(DECIDESYM8TRANSFORM5MEN)
}

#define DECIDESYM8TRANSFORM6MEN(f1Offdiag, f2Offdiag, trTrue, trFalse)                                        \
{ if(f1Offdiag(key.getWhiteKingPosition())) {                                                                 \
    return f2Offdiag(key.getWhiteKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getBlackKingPosition())) {                                                          \
    return f2Offdiag(key.getBlackKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition(2))) {                                                                  \
    return f2Offdiag(key.getPosition(2))          ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition(3))) {                                                                  \
    return f2Offdiag(key.getPosition(3))          ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition(4))) {                                                                  \
    return f2Offdiag(key.getPosition(4))          ? trTrue : trFalse;                                         \
  } else {                                                                                                    \
    return f2Offdiag(key.getPosition(5))          ? trTrue : trFalse;                                         \
  }                                                                                                           \
}

// See comment on EndGameKeyDefinitionBase3Men, -4- and -5Men
SymmetricTransformation EndGameKeyDefinition::getSym8Transformation6Men(EndGameKey key) { // static
  SYM8DECISIONSWITCH(DECIDESYM8TRANSFORM6MEN)
}

#define DECIDESYM8TRANSFORM2EQUAL_OPPOSITESIDE(trTrue, trFalse, i, j)                                         \
{ const int pi = s_offDiagPosToIndex[GameKey::transform(key.getPosition(i), trTrue)];                         \
  const int pj = s_offDiagPosToIndex[GameKey::transform(key.getPosition(j), trTrue)];                         \
  if(pi >= 28) {                                                                                              \
    return (pi - 28 > pj) ? trTrue : trFalse;                                                                 \
  } else {                                                                                                    \
    return (pj - 28 > pi) ? trTrue : trFalse;                                                                 \
  }                                                                                                           \
}

#define DECIDESYM8TRANSFORM2EQUAL_OFFDIAG(diagSide, trTrue, trFalse, i, j)                                    \
{ if(diagSide(key.getPosition(i)) == diagSide(key.getPosition(j))) {                                          \
    return diagSide(key.getPosition(i)) ? trTrue : trFalse;                                                   \
  } else {                                                                                                    \
    DECIDESYM8TRANSFORM2EQUAL_OPPOSITESIDE(trTrue, trFalse, i, j);                                            \
  }                                                                                                           \
}

#define DECIDESYM8TRANSFORM4MEN2EQUAL(f1Offdiag, f2Offdiag, trTrue, trFalse)                                  \
{ if(f1Offdiag(key.getWhiteKingPosition())) {                                                                 \
    return f2Offdiag(key.getWhiteKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getBlackKingPosition())) {                                                          \
    return f2Offdiag(key.getBlackKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition(2)) && f1Offdiag(key.getPosition(3))) {                                 \
    DECIDESYM8TRANSFORM2EQUAL_OFFDIAG(f2Offdiag, trTrue, trFalse, 2, 3);                                      \
  } else if(f1Offdiag(key.getPosition(2))) {                                                                  \
    return f2Offdiag(key.getPosition(2))          ? trTrue : trFalse;                                         \
  } else { /* f1Offdiag(pos3) */                                                                              \
    return f2Offdiag(key.getPosition(3))          ? trTrue : trFalse;                                         \
  }                                                                                                           \
}

SymmetricTransformation EndGameKeyDefinition::getSym8Transformation4Men2Equal(EndGameKey key) { // static
  SYM8DECISIONSWITCH(DECIDESYM8TRANSFORM4MEN2EQUAL)
}

#define _DECIDESYM8TRANSFORM5MEN2EQUAL(f1Offdiag, f2Offdiag, trTrue, trFalse, solo, eq1, eq2)                 \
{ if(f1Offdiag(key.getWhiteKingPosition())) {                                                                 \
    return f2Offdiag(key.getWhiteKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getBlackKingPosition())) {                                                          \
    return f2Offdiag(key.getBlackKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition(solo))) {                                                               \
    return f2Offdiag(key.getPosition(solo))     ? trTrue : trFalse;                                           \
  } else if(f1Offdiag(key.getPosition(eq1)) && f1Offdiag(key.getPosition(eq2))) {                             \
    DECIDESYM8TRANSFORM2EQUAL_OFFDIAG(f2Offdiag, trTrue, trFalse, eq1, eq2);                                  \
  } else if(f1Offdiag(key.getPosition(eq1))) {                                                                \
    return f2Offdiag(key.getPosition(eq1))      ? trTrue : trFalse;                                           \
  } else { /* f1Offdiag(pos[eq2]) */                                                                          \
    return f2Offdiag(key.getPosition(eq2))      ? trTrue : trFalse;                                           \
  }                                                                                                           \
}


#define DECIDESYM8TRANSFORM5MEN2EQUAL(f1Offdiag, f2Offdiag, trTrue, trFalse)                                  \
  _DECIDESYM8TRANSFORM5MEN2EQUAL(f1Offdiag, f2Offdiag, trTrue, trFalse, 2, 3, 4)

SymmetricTransformation EndGameKeyDefinition::getSym8Transformation5Men2Equal(EndGameKey key) { // static
  SYM8DECISIONSWITCH(DECIDESYM8TRANSFORM5MEN2EQUAL)
}

#define _DECIDESYM8TRANSFORM6MEN2EQUAL(f1Offdiag, f2Offdiag, trTrue, trFalse, solo1, solo2, eq1, eq2)         \
{ if(f1Offdiag(key.getWhiteKingPosition())) {                                                                 \
    return f2Offdiag(key.getWhiteKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getBlackKingPosition())) {                                                          \
    return f2Offdiag(key.getBlackKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition(solo1))) {                                                              \
    return f2Offdiag(key.getPosition(solo1))     ? trTrue : trFalse;                                          \
  } else if(f1Offdiag(key.getPosition(solo2))) {                                                              \
    return f2Offdiag(key.getPosition(solo2))     ? trTrue : trFalse;                                          \
  } else if(f1Offdiag(key.getPosition(eq1)) && f1Offdiag(key.getPosition(eq2))) {                             \
    DECIDESYM8TRANSFORM2EQUAL_OFFDIAG(f2Offdiag, trTrue, trFalse, eq1, eq2);                                  \
  } else if(f1Offdiag(key.getPosition(eq1))) {                                                                \
    return f2Offdiag(key.getPosition(eq1))      ? trTrue : trFalse;                                           \
  } else { /* f1Offdiag(pos[eq2]) */                                                                          \
    return f2Offdiag(key.getPosition(eq2))      ? trTrue : trFalse;                                           \
  }                                                                                                           \
}


#define DECIDESYM8TRANSFORM6MEN2EQUAL(f1Offdiag, f2Offdiag, trTrue, trFalse)                                  \
  _DECIDESYM8TRANSFORM6MEN2EQUAL(f1Offdiag, f2Offdiag, trTrue, trFalse, 2, 3, 4, 5)

SymmetricTransformation EndGameKeyDefinition::getSym8Transformation6Men2Equal(EndGameKey key) { // static
  SYM8DECISIONSWITCH(DECIDESYM8TRANSFORM6MEN2EQUAL)
}


#define DECIDESYM8TRANSFORM6MEN2PAIRS(f1Offdiag, f2Offdiag, trTrue, trFalse)                                  \
{ if(f1Offdiag(key.getWhiteKingPosition())) {                                                                 \
    return f2Offdiag(key.getWhiteKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getBlackKingPosition())) {                                                          \
    return f2Offdiag(key.getBlackKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition(2)) && f1Offdiag(key.getPosition(3))) {                                 \
    DECIDESYM8TRANSFORM2EQUAL_OFFDIAG(f2Offdiag, trTrue, trFalse, 2, 3);                                      \
  } else if(f1Offdiag(key.getPosition(2))) {                                                                  \
    return f2Offdiag(key.getPosition(2))          ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition(3))) {                                                                  \
    return f2Offdiag(key.getPosition(3))          ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition(4)) && f1Offdiag(key.getPosition(5))) {                                 \
    DECIDESYM8TRANSFORM2EQUAL_OFFDIAG(f2Offdiag, trTrue, trFalse, 4, 5);                                      \
  } else if(f1Offdiag(key.getPosition(4))) {                                                                  \
    return f2Offdiag(key.getPosition(4))          ? trTrue : trFalse;                                         \
  } else {                                                                                                    \
    return f2Offdiag(key.getPosition(5))          ? trTrue : trFalse;                                         \
  }                                                                                                           \
}


SymmetricTransformation EndGameKeyDefinition::getSym8Transformation6Men2Pairs(EndGameKey key) { // static
  SYM8DECISIONSWITCH(DECIDESYM8TRANSFORM6MEN2PAIRS)
}

#ifdef _DEBUG

static SymmetricTransformation decideSym8Transform3EqualFlipi(EndGameKey key, SymmetricTransformation trTrue, SymmetricTransformation trFalse, int i, int j, int k) {
  const int pi = EndGameKeyDefinition::s_offDiagPosToIndex[GameKey::transform(key.getPosition(i), trTrue )];
  const int pj = EndGameKeyDefinition::s_offDiagPosToIndex[GameKey::transform(key.getPosition(j), trFalse)];
  const int pk = EndGameKeyDefinition::s_offDiagPosToIndex[GameKey::transform(key.getPosition(k), trFalse)];
  return (pi > max(pj, pk)) ? trTrue : trFalse;
}

static SymmetricTransformation decideSym8Transform3EqualFlipij(EndGameKey key, SymmetricTransformation trTrue, SymmetricTransformation trFalse, int i, int j, int k) {
  const int pi = EndGameKeyDefinition::s_offDiagPosToIndex[GameKey::transform(key.getPosition(i), trTrue )];
  const int pj = EndGameKeyDefinition::s_offDiagPosToIndex[GameKey::transform(key.getPosition(j), trTrue )];
  const int pk = EndGameKeyDefinition::s_offDiagPosToIndex[GameKey::transform(key.getPosition(k), trFalse)];
  return (max(pi, pj) >= pk) ? trTrue : trFalse;
}

#define DECIDESYM8TRANSFORM3EQUAL_FLIPi( trTrue, trFalse, i, j, k) return decideSym8Transform3EqualFlipi( key, trTrue, trFalse, i, j, k)
#define DECIDESYM8TRANSFORM3EQUAL_FLIPij(trTrue, trFalse, i, j, k) return decideSym8Transform3EqualFlipij(key, trTrue, trFalse, i, j, k)

#else

// Assume key.position(i) needs flip below diagonal.
// return flip(pi) > max(pj,pk) ? trTrue : trFalse
#define DECIDESYM8TRANSFORM3EQUAL_FLIPi(trTrue, trFalse, i, j, k)                                             \
{ const int pi = s_offDiagPosToIndex[GameKey::transform(key.getPosition(i), trTrue )];                        \
  const int pj = s_offDiagPosToIndex[GameKey::transform(key.getPosition(j), trFalse)];                        \
  const int pk = s_offDiagPosToIndex[GameKey::transform(key.getPosition(k), trFalse)];                        \
  return (pi > max(pj, pk)) ? trTrue : trFalse;                                                               \
}

// Assume key.position(i) and position(j) need flip below diagonal.
// return max(flip(pi),flip(pj)) >= pk ? trTrue : trFalse
#define DECIDESYM8TRANSFORM3EQUAL_FLIPij(trTrue, trFalse, i, j, k)                                            \
{ const int pi = s_offDiagPosToIndex[GameKey::transform(key.getPosition(i), trTrue )];                        \
  const int pj = s_offDiagPosToIndex[GameKey::transform(key.getPosition(j), trTrue )];                        \
  const int pk = s_offDiagPosToIndex[GameKey::transform(key.getPosition(k), trFalse)];                        \
  return (max(pi, pj) >= pk) ? trTrue : trFalse;                                                              \
}

#endif

// assume all prev positions are on diag, that is kings,p2,p3,...p(i-1)
#define DECIDESYM8TRANSFORM3EQUAL(f1Offdiag, f2Offdiag, trTrue, trFalse, i, j, k)                           \
{ switch(KEYBOOL3MASK(key, f1Offdiag, i, j, k)) {                                                           \
  case 0: return trFalse;                                                       /* i,j,k on    diag */      \
  case 1: return f2Offdiag(key.getPosition(i)  ) ? trTrue : trFalse;            /*   j,k on    diag */      \
  case 2: return f2Offdiag(key.getPosition(j)  ) ? trTrue : trFalse;            /* i,  k on    diag */      \
  case 3: DECIDESYM8TRANSFORM2EQUAL_OFFDIAG(f2Offdiag, trTrue, trFalse, i, j);  /*     k on    diag */      \
  case 4: return f2Offdiag(key.getPosition(k)  ) ? trTrue : trFalse;            /* i,j   on    diag */      \
  case 5: DECIDESYM8TRANSFORM2EQUAL_OFFDIAG(f2Offdiag, trTrue, trFalse, i, k);  /*   j   on    diag */      \
  case 6: DECIDESYM8TRANSFORM2EQUAL_OFFDIAG(f2Offdiag, trTrue, trFalse, j, k);  /* i     on    diag */      \
  case 7: switch(KEYBOOL3MASK(key, f2Offdiag, i, j, k)) {                       /* none  on    diag */      \
          case 0: return trFalse;                                               /* i,j,k below diag */      \
          case 1: DECIDESYM8TRANSFORM3EQUAL_FLIPi( trTrue, trFalse, i, j, k);   /*   j,k below diag */      \
          case 2: DECIDESYM8TRANSFORM3EQUAL_FLIPi( trTrue, trFalse, j, i, k);   /* i,  k below diag */      \
          case 3: DECIDESYM8TRANSFORM3EQUAL_FLIPij(trTrue, trFalse, i, j, k);   /*     k below diag */      \
          case 4: DECIDESYM8TRANSFORM3EQUAL_FLIPi( trTrue, trFalse, k, i, j);   /* i,j   below diag */      \
          case 5: DECIDESYM8TRANSFORM3EQUAL_FLIPij(trTrue, trFalse, i, k, j);   /*   j   below diag */      \
          case 6: DECIDESYM8TRANSFORM3EQUAL_FLIPij(trTrue, trFalse, j, k, i);   /* i     below diag */      \
          case 7: return trTrue;                                                /* none  below diag */      \
          }                                                                                                 \
  }                                                                                                         \
}

#define DECIDESYM8TRANSFORM5MEN3EQUAL(f1Offdiag, f2Offdiag, trTrue, trFalse)                                      \
{ if(f1Offdiag(key.getWhiteKingPosition())) {                                                                 \
    return f2Offdiag(key.getWhiteKingPosition())   ? trTrue : trFalse;            /* wk off diag      */      \
  } else if(f1Offdiag(key.getBlackKingPosition())) {                                                          \
    return f2Offdiag(key.getBlackKingPosition())   ? trTrue : trFalse;            /* wk on diag       */      \
  } else {                                                                        /* kings on    diag */      \
    DECIDESYM8TRANSFORM3EQUAL(f1Offdiag, f2Offdiag, trTrue, trFalse, 2, 3, 4)                                 \
  }                                                                                                           \
}

SymmetricTransformation EndGameKeyDefinition::getSym8Transformation5Men3Equal(EndGameKey key) { // static
  SYM8DECISIONSWITCH(DECIDESYM8TRANSFORM5MEN3EQUAL)
}

#define DECIDESYM8TRANSFORM6MEN3EQUAL(f1Offdiag, f2Offdiag, trTrue, trFalse)                                  \
{ if(f1Offdiag(key.getWhiteKingPosition())) {                                                                 \
    return f2Offdiag(key.getWhiteKingPosition())   ? trTrue : trFalse;            /* wk off diag      */      \
  } else if(f1Offdiag(key.getBlackKingPosition())) {                                                          \
    return f2Offdiag(key.getBlackKingPosition())   ? trTrue : trFalse;            /* wk on diag       */      \
  } else if(f1Offdiag(key.getPosition(2))) {                                      /* kings on    diag */      \
    return f2Offdiag(key.getPosition(2))           ? trTrue : trFalse;            /* wk on diag       */      \
  } else {                                                                        /* kings,p2 on    diag */   \
    DECIDESYM8TRANSFORM3EQUAL(f1Offdiag, f2Offdiag, trTrue, trFalse, 3, 4, 5)                                 \
  }                                                                                                           \
}

SymmetricTransformation EndGameKeyDefinition::getSym8Transformation6Men3Equal(EndGameKey key) { // static
  SYM8DECISIONSWITCH(DECIDESYM8TRANSFORM6MEN3EQUAL)
}

SymmetricTransformation EndGameKeyDefinition::getPawnSymTransformation(EndGameKey key) { // static
  return IS_QUEENSIDE(key.getPosition(2)) ? 0 : TRANSFORM_MIRRORCOL;
}

#define DECIDEPAWNSYMTRANSFORM2EQUALPAWNS(i,j)                                                                \
{ switch(KEYBOOL2MASK(key, IS_QUEENSIDE, i, j)) {                                                             \
  case 0: return TRANSFORM_MIRRORCOL;                                                                         \
  case 1:                                                                                                     \
    { const UINT pi = s_pawnPosToIndex[key.getPosition(i)];                                                   \
      const UINT pj = s_pawnPosToIndex[MIRRORCOLUMN(key.getPosition(j))];                                     \
      if(pi != pj) {                                                                                          \
        return (pi < pj) ? 0 : TRANSFORM_MIRRORCOL;                                                           \
      } else {                                                                                                \
        return IS_QUEENSIDE(key.getWhiteKingPosition()) ? 0 : TRANSFORM_MIRRORCOL;                            \
      }                                                                                                       \
    }                                                                                                         \
  case 2:                                                                                                     \
    { const UINT pi = s_pawnPosToIndex[MIRRORCOLUMN(key.getPosition(i))];                                     \
      const UINT pj = s_pawnPosToIndex[key.getPosition(j)];                                                   \
      if(pi != pj) {                                                                                          \
        return (pj < pi) ? 0 : TRANSFORM_MIRRORCOL;                                                           \
      } else {                                                                                                \
        return IS_QUEENSIDE(key.getWhiteKingPosition()) ? 0 : TRANSFORM_MIRRORCOL;                            \
      }                                                                                                       \
    }                                                                                                         \
  case 3: return 0;                                                                                           \
  default: pawnSymSwitchError(__LINE__);                                                                      \
  }                                                                                                           \
  return 0;                                                                                                   \
}

SymmetricTransformation EndGameKeyDefinition::get4Men2EqualPawnsSymTransformation(EndGameKey key) { // static
  DECIDEPAWNSYMTRANSFORM2EQUALPAWNS(2, 3)
}

SymmetricTransformation EndGameKeyDefinition::get5Men2EqualPawnsSymTransformation(EndGameKey key) { // static
  DECIDEPAWNSYMTRANSFORM2EQUALPAWNS(3, 4)
}

#ifdef _DEBUG

static SymmetricTransformation decidePawnTransform3EqualPawnsFlipi(EndGameKey key, int i, int j, int k) {
  const int pi = EndGameKeyDefinition::s_pawnPosToIndex[MIRRORCOLUMN(key.getPosition(i))];
  const int pj = EndGameKeyDefinition::s_pawnPosToIndex[key.getPosition(j)];
  const int pk = EndGameKeyDefinition::s_pawnPosToIndex[key.getPosition(k)];
  return (pi > max(pj, pk)) ? TRANSFORM_MIRRORCOL : 0;
}

static SymmetricTransformation decidePawnTransform3EqualPawnsFlipij(EndGameKey key, int i, int j, int k) {
  const int pi = EndGameKeyDefinition::s_pawnPosToIndex[MIRRORCOLUMN(key.getPosition(i))];
  const int pj = EndGameKeyDefinition::s_pawnPosToIndex[MIRRORCOLUMN(key.getPosition(j))];
  const int pk = EndGameKeyDefinition::s_pawnPosToIndex[key.getPosition(k)];
  return (max(pi, pj) >= pk) ? TRANSFORM_MIRRORCOL : 0;
}

#define DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPi( i, j, k) return decidePawnTransform3EqualPawnsFlipi( key, i, j, k)
#define DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPij(i, j, k) return decidePawnTransform3EqualPawnsFlipij(key, i, j, k)

#else // !_DEBUG

/* Assume p##i is on kingside. return flip(pi) > max(pj,pk) ? MIRRORCOL : 0 */
#define DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPi(i, j, k)                                                      \
{ const int pi = s_pawnPosToIndex[MIRRORCOLUMN(key.getPosition(i))];                                          \
  const int pj = s_pawnPosToIndex[key.getPosition(j)];                                                        \
  const int pk = s_pawnPosToIndex[key.getPosition(k)];                                                        \
  return (pi > max(pj, pk)) ? TRANSFORM_MIRRORCOL : 0;                                                        \
}

/* Assume p##i and p##j is on kingside. return (max(flip(pi), flip(pj)) >= pk) ? MIRRORCOL : 0 */
#define DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPij(i, j, k)                                                     \
{ const int pi = s_pawnPosToIndex[MIRRORCOLUMN(key.getPosition(i))];                                          \
  const int pj = s_pawnPosToIndex[MIRRORCOLUMN(key.getPosition(j))];                                          \
  const int pk = s_pawnPosToIndex[key.getPosition(k)];                                                        \
  return (max(pi, pj) >= pk) ? TRANSFORM_MIRRORCOL : 0;                                                       \
}

#endif // _DEBUG

#ifdef _DEBUG

EndGamePosIndex _addPit(EndGamePosIndex addr, Player p) {
  addr <<= 1;
  addr |= p;
  return addr;
}

void _setPit(EndGameKey &key, EndGamePosIndex &addr) {
  const Player p = (Player)(addr&1);
  key.setPlayerInTurn(p);
  addr >>= 1;
}

EndGamePosIndex _addPosIndex(EndGamePosIndex addr, UINT count, UINT index) {
  addr *= count;
  addr += index;
  return addr;
}

void _setPosIndex(EndGameKey &key, EndGamePosIndex &addr, UINT count, UINT pIndex) {
  const UINT pos = addr % count;
  key.setPosition(pIndex, pos);
  addr /= count;
}

void _setPosBelowDiag(EndGameKey &key, EndGamePosIndex &addr, UINT pIndex) {
  const UINT subDiagIndex = addr % 28;
  const UINT pos          = EndGameKeyDefinition::s_subDiagIndexToPos[subDiagIndex];
  key.setPosition(pIndex, pos);
  addr /= 28;
}

EndGamePosIndex _add2Equal(EndGamePosIndex addr, EndGamePosIndex maxAddr, UINT lp, UINT hp) {
  maxAddr /= 2;
  if((lp >= hp) || (addr >= maxAddr)) {
    throwException(_T("%s:addr=%u, maxAddr=%u, (lp,hp)=(%u,%u). Assume (addr < maxAddr) && (lp < hp)")
                  ,__TFUNCTION__
                  ,addr, maxAddr, lp, hp);
  }
  EndGamePosIndex a = addr * hp;
  a += lp;
  const EndGamePosIndex rs = GET_RANGESTART2EQUAL(maxAddr,hp-1);
  return a + rs;
}

EndGamePosIndex _add2EqualAllowEqualLH(EndGamePosIndex addr, EndGamePosIndex maxAddr, UINT lp, UINT hp) {
  maxAddr /= 2;
  if((lp > hp) || (addr >= maxAddr)) {
    throwException(_T("%s:addr=%llu, maxAddr=%llu, (lp,hp)=(%u,%u). Assume (addr < maxAddr) && (lp <= hp)")
                  ,__TFUNCTION__
                  ,addr, maxAddr, lp, hp);
  }
  EndGamePosIndex a = addr * (hp+1);
  a += lp;
  const EndGamePosIndex rs = GET_RANGESTART2EQUAL(maxAddr, hp);
  return a + rs;
}

void _set2Pos2Equal(EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r  = findRange2Equal(maxAddr, addr);
  const EndGamePosIndex rs = GET_RANGESTART2EQUAL(maxAddr, r);
  addr -= rs;
  r++;
  key.setPosition(hpIndex, r);
  const UINT lpPos = addr % r;
  key.setPosition(lpIndex, lpPos);
  addr /= r;
}

void _set2OffDiagPosNoFlip(EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r  = findRange2Equal(maxAddr, addr);
  const EndGamePosIndex rs = GET_RANGESTART2EQUAL(maxAddr, r);
  addr -= rs;
  r++;
  const UINT hpPos      = EndGameKeyDefinition::s_offDiagIndexToPos[r];
  const UINT lpSubIndex = addr % r;
  const UINT lpPos      = EndGameKeyDefinition::s_offDiagIndexToPos[lpSubIndex];
  key.setPosition(hpIndex, hpPos);
  key.setPosition(lpIndex, lpPos);
  addr /= r;
}

void _set2OffDiagPosFlipi(EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r  = findRange2Equal(maxAddr, addr);
  const EndGamePosIndex rs = GET_RANGESTART2EQUAL(maxAddr, r);
  addr -= rs;
  const UINT hpPos = EndGameKeyDefinition::s_offDiagIndexToPos[r];
  key.setPosition(hpIndex, hpPos);
  r++;
  const UINT lpSupIndex = addr % r + 28;
  const UINT lpPos      = EndGameKeyDefinition::s_offDiagIndexToPos[lpSupIndex];
  key.setPosition(lpIndex, lpPos);
  addr /= r;
}

void _set2OffDiagPosFlipj(EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r  = findRange2Equal(maxAddr, addr);
  const EndGamePosIndex rs = GET_RANGESTART2EQUAL(maxAddr, r);
  addr -= rs;
  const UINT hpSupIndex = r+28;
  const UINT hpPos      = EndGameKeyDefinition::s_offDiagIndexToPos[hpSupIndex];
  key.setPosition(hpIndex, hpPos);
  r++;
  const UINT lpSubIndex = addr % r;
  const UINT lpPos      = EndGameKeyDefinition::s_offDiagIndexToPos[lpSubIndex];
  key.setPosition(lpIndex, lpPos);
  addr /= r;
}

void _set2OffDiagPosFlipij(EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r  = findRange2Equal(maxAddr, addr);
  const EndGamePosIndex rs = GET_RANGESTART2EQUAL(maxAddr, r);
  addr -= rs;
  r++;
  const UINT hpSupIndex = r+28;
  const UINT hpPos      = EndGameKeyDefinition::s_offDiagIndexToPos[hpSupIndex];
  key.setPosition(hpIndex, hpPos);
  const UINT lpSupIndex = addr % r + 28;
  const UINT lpPos      = EndGameKeyDefinition::s_offDiagIndexToPos[lpSupIndex];
  key.setPosition(lpIndex, lpPos);
  addr /= r;
}

EndGamePosIndex _add3Equal(EndGamePosIndex addr, EndGamePosIndex maxAddr, UINT lp, UINT mp, UINT hp) {
  if((lp >= mp) || (mp >= hp) || (addr >= maxAddr/2)) {
    throwException(_T("%s:addr=%llu, maxAddr=%llu, (lp,mp,hp)=(%u,%u,%u). Assume (addr < maxAddr/2) &&  lp < mp < hp")
                  ,__TFUNCTION__
                  ,addr, maxAddr, lp, mp, hp);
  }
  EndGamePosIndex a  = ADD2EQUAL(addr, maxAddr, lp, mp);
  EndGamePosIndex rs = GET_RANGESTART3EQUAL(maxAddr/2, hp-2);
  return a + rs;
}

EndGamePosIndex _add3EqualAllowEqualLM(EndGamePosIndex addr, EndGamePosIndex maxAddr, UINT lp, UINT mp, UINT hp) {
  if((lp > mp) || (mp >= hp) || (addr >= maxAddr/2)) {
    throwException(_T("%s:addr=%llu, maxAddr=%llu, (lp,mp,hp)=(%u,%u,%u). Assume (addr < maxAddr/2) && lp <= mp < hp")
                  ,__TFUNCTION__
                  ,addr, maxAddr, lp, mp, hp);
  }
  EndGamePosIndex a = ADD2EQUALALLOWEQUALLH(addr, maxAddr, lp, mp);
  EndGamePosIndex r = GET_RANGESTART3EQUAL(maxAddr/2, hp-1);
  return a + r;
}

EndGamePosIndex _add3EqualAllowEqualHM(EndGamePosIndex addr, EndGamePosIndex maxAddr, UINT lp, UINT mp, UINT hp) {
  if((lp >= mp) || (mp > hp) || (addr >= maxAddr/2)) {
    throwException(_T("%s:addr=%llu, maxAddr=%llu, (lp,mp,hp)=(%u,%u,%u). Assume (addr < maxAddr/2) && lp < mp <= hp")
                  ,__TFUNCTION__
                  ,addr, maxAddr, lp, mp, hp);
  }
  EndGamePosIndex a = ADD2EQUAL(addr, maxAddr, lp, mp);
  EndGamePosIndex r = GET_RANGESTART3EQUAL(maxAddr/2, hp-1);
  return a + r;
}

void _set3Pos3Equal(EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex *table, int tableSize, EndGamePosIndex maxAddr, int lpIndex, int mpIndex, int hpIndex) {
  int r = findTableRange(table, tableSize, addr);
  key.setPosition(hpIndex, r+2);
  const EndGamePosIndex rs1 = GET_RANGESTART3EQUAL(maxAddr/2, r);
  addr -= rs1;
  SET2POS2EQUAL(key, addr, maxAddr, lpIndex, mpIndex);
}

void _set3OffDiagPosNoFlip(EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex *table, int tableSize, EndGamePosIndex maxAddr, int lpIndex, int mpIndex, int hpIndex) {
  int r = findTableRange(table, tableSize, addr);
  const UINT hpPos = EndGameKeyDefinition::s_offDiagIndexToPos[r+2];
  key.setPosition(hpIndex, hpPos);
  const EndGamePosIndex rs = GET_RANGESTART3EQUAL(maxAddr/2, r);
  addr -= rs;
  SET2OFFDIAGPOSNOFLIP(key, addr, maxAddr, lpIndex, mpIndex);
}

void _set3OffDiagPosFlipi(EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex *table, int tableSize, EndGamePosIndex maxAddr, int lpIndex, int mpIndex, int hpIndex) {
  int r = findTableRange(table, tableSize, addr);
  key.setPosition(hpIndex, EndGameKeyDefinition::s_offDiagIndexToPos[r+1]);
  const EndGamePosIndex rs1 = GET_RANGESTART3EQUAL(maxAddr/2, r);
  addr -= rs1;
  SET2OFFDIAGPOSFLIPi(key, addr, maxAddr, lpIndex, mpIndex);
}

void _set3OffDiagPosFlipj(EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex *table, int tableSize, EndGamePosIndex maxAddr, int lpIndex, int mpIndex, int hpIndex) {
  maxAddr /= 2;
  int r = findTableRange(table, tableSize, addr);
  const UINT hpPos = EndGameKeyDefinition::s_offDiagIndexToPos[r+1];
  key.setPosition(hpIndex, hpPos);
  const EndGamePosIndex rs1 = GET_RANGESTART3EQUAL(maxAddr, r);
  addr -= rs1;
  r  = findRange2Equal(maxAddr, addr);
  const EndGamePosIndex rs2 = GET_RANGESTART2EQUAL(maxAddr, r);
  addr -= rs2;
  r++;
  const UINT mpSupIndex = r+28;
  const UINT mpPos      = EndGameKeyDefinition::s_offDiagIndexToPos[mpSupIndex];
  const UINT lpSubIndex = addr % r;
  const UINT lpPos      = EndGameKeyDefinition::s_offDiagIndexToPos[lpSubIndex];
  key.setPosition(mpIndex, mpPos);
  key.setPosition(lpIndex, lpPos);
  addr /= r;
}

void _set3OffDiagPosFlipij(EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex *table, int tableSize, EndGamePosIndex maxAddr, int lpIndex, int mpIndex, int hpIndex) {
  int r   = findTableRange(table, tableSize, addr);
  key.setPosition(hpIndex, EndGameKeyDefinition::s_offDiagIndexToPos[r+2]);
  const EndGamePosIndex rs1 = GET_RANGESTART3EQUAL(maxAddr/2, r);
  addr -= rs1;
  SET2OFFDIAGPOSFLIPij(key, addr, maxAddr, lpIndex, mpIndex);
}

// --------------------------------------- Pawns -----------------------------------------------

void _set2EqualPawnsNoFlip(EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r = findRange2Equal(maxAddr, addr);
  addr -= GET_RANGESTART2EQUAL(maxAddr, r);
  r++;
  key.setPosition(hpIndex, EndGameKeyDefinition::s_pawnIndexToPos[r]);
  key.setPosition(lpIndex, EndGameKeyDefinition::s_pawnIndexToPos[(addr) % r]);
  addr /= r;
}

void _set2EqualPawnsFlipi( EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r = findRange2Equal(maxAddr, addr);
  addr -= GET_RANGESTART2EQUAL(maxAddr, r);
  key.setPosition(hpIndex, EndGameKeyDefinition::s_pawnIndexToPos[r]);
  r++;
  const int lpPos = EndGameKeyDefinition::s_pawnIndexToPos[(addr) % r];
  key.setPosition(lpIndex, MIRRORCOLUMN(lpPos));
  addr /= r;
}

void _set2EqualPawnsFlipj( EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r = findRange2Equal(maxAddr, addr);
  addr -= GET_RANGESTART2EQUAL(maxAddr, r);
  r++;
  key.setPosition(hpIndex, MIRRORCOLUMN(EndGameKeyDefinition::s_pawnIndexToPos[r]));
  key.setPosition(lpIndex, EndGameKeyDefinition::s_pawnIndexToPos[(addr) % r]);
  addr /= r;
}

void _set2EqualPawnsFlipij(EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex maxAddr, int lpIndex, int hpIndex) {
  maxAddr /= 2;
  int r = findRange2Equal(maxAddr, addr);
  addr -= GET_RANGESTART2EQUAL(maxAddr, r);
  r++;
  int pos = EndGameKeyDefinition::s_pawnIndexToPos[r];
  key.setPosition(hpIndex, MIRRORCOLUMN(pos));
  pos = EndGameKeyDefinition::s_pawnIndexToPos[(addr) % r];
  key.setPosition(lpIndex, MIRRORCOLUMN(pos));
  addr /= r;
}


void _set3EqualPawnsNoFlip(EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex *table, int tableSize, EndGamePosIndex maxAddr, int lpIndex, int mpIndex, int hpIndex) {
  int r = findTableRange(table, tableSize, addr);
  const UINT hpPos = EndGameKeyDefinition::s_pawnIndexToPos[r+2];
  key.setPosition(hpIndex, hpPos);
  addr -= GET_RANGESTART3EQUAL(maxAddr/2, r);
  SET2EQUALPAWNSNOFLIP(key, addr, maxAddr, lpIndex, mpIndex);
}

void _set3EqualPawnsFlipi(EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex *table, int tableSize, EndGamePosIndex maxAddr, int lpIndex, int mpIndex, int hpIndex) {
  int r = findTableRange(table, tableSize, addr);
  const UINT hpPos = EndGameKeyDefinition::s_pawnIndexToPos[r+1];
  key.setPosition(hpIndex, hpPos);
  addr -= GET_RANGESTART3EQUAL(maxAddr/2, r);
  SET2EQUALPAWNSFLIPi(key, addr, maxAddr, lpIndex, mpIndex);
}

void _set3EqualPawnsFlipj(EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex *table, int tableSize, EndGamePosIndex maxAddr, int lpIndex, int mpIndex, int hpIndex) {
  maxAddr /= 2;
  int r = findTableRange(table, tableSize, addr);
  const UINT hpPos = EndGameKeyDefinition::s_pawnIndexToPos[r+1];
  key.setPosition(hpIndex, hpPos);
  addr -= GET_RANGESTART3EQUAL(maxAddr, r);
  r = findRange2Equal(maxAddr, addr);
  addr -= GET_RANGESTART2EQUAL(maxAddr, r);
  r++;
  UINT mpPos = EndGameKeyDefinition::s_pawnIndexToPos[r];
  mpPos = MIRRORCOLUMN(mpPos);
  key.setPosition(mpIndex, mpPos);

  const UINT lpSubIndex = addr % r;
  const UINT lpPos      = EndGameKeyDefinition::s_pawnIndexToPos[lpSubIndex];
  key.setPosition(lpIndex, lpPos);
  addr /= r;
}

void _set3EqualPawnsFlipij(EndGameKey &key, EndGamePosIndex &addr, EndGamePosIndex *table, int tableSize, EndGamePosIndex maxAddr, int lpIndex, int mpIndex, int hpIndex) {
  int r = findTableRange(table, tableSize, addr);
  key.setPosition(hpIndex, EndGameKeyDefinition::s_pawnIndexToPos[r+2]);
  addr -= GET_RANGESTART3EQUAL(maxAddr/2, r);
  SET2EQUALPAWNSFLIPij(key, addr, maxAddr, lpIndex, mpIndex);
}

#endif //_DEBUG

SymmetricTransformation EndGameKeyDefinition::get5Men3EqualPawnsSymTransformation(EndGameKey key) { // static
  switch(KEYBOOL3MASK(key, IS_KINGSIDE, 2, 3, 4)) { // similar to DECIDESYM8TRANSFORM5MEN3EQUAL, case 7:kings on diag, 2,3,4 off diag
  case 0 : return 0;                                                              /* 2,3,4 queenside */
  case 1 : DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPi( 2, 3, 4);                     /*   3,4 queenside */
  case 2 : DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPi( 3, 2, 4);                     /* 2,  4 queenside */
  case 3 : DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPij(2, 3, 4);                     /*     4 queenside */
  case 4 : DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPi( 4, 2, 3);                     /* 2,3   queenside */
  case 5 : DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPij(2, 4, 3);                     /*   3   queenside */
  case 6 : DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPij(3, 4, 2);                     /* 2     queenside */
  case 7 : return TRANSFORM_MIRRORCOL;                                            /* none  queenside */
  }
  throwException(_T("%s:Unexpected mask:%d. Valid are [0..7]")
                ,__TFUNCTION__
                ,KEYBOOL3MASK(key, IS_KINGSIDE, 2, 3, 4));
  return 0;
}

#ifdef TABLEBASE_BUILDER

void EndGameKeyDefinition::insertInitialPositions(EndGameTablebase &tablebase) const {
  const int wkPos = A1;
  const int bkPos = C1;

  switch(m_totalPieceCount) {
  case 3:
    tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, F2);
    tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, F3);
    break;
  case 4:
    { for(UINT p3 = G2; p3 <= G3; p3+=8)
        for(UINT p2 = F2; p2 <= F3; p2+=8)
          tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, p2, p3);
    }
    break;
  case 5:
    { for(UINT p4 = H2; p4 <= H3; p4+=8)
        for(UINT p3 = G2; p3 <= G3; p3+=8)
          for(UINT p2 = F2; p2 <= F3; p2+=8)
            tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, p2, p3, p4);
    }
    break;
  case 6:
    { for(UINT p5 = H4; p5 <= H5; p5+=8)
        for(UINT p4 = H2; p4 <= H3; p4+=8)
          for(UINT p3 = G2; p3 <= G3; p3+=8)
            for(UINT p2 = F2; p2 <= F3; p2+=8)
              tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, p2, p3, p4, p5);
    }
    break;
  default:
    invalidPieceCountError(__TFUNCTION__);
  }
}

String EndGameKeyDefinition::getCodecName() const {
  return format(_T("%dMen"), m_totalPieceCount);
}

#include <Timer.h>

void KeyDefinitionSelfCheckInfo::reset() {
  m_minIndex       = -1;
  m_maxIndex       =  0;
  m_checkKeyCount  =  0;
  m_duplicateCount =  0;
  m_testRunning    = true;
}

class SelfCheckStatusPrinter : public TimeoutHandler {
private:
  const EndGameKeyDefinition      &m_keydef;
  EndGameKeyWithOccupiedPositions &m_key;
  double                           m_lastProcTime;
  UINT64                           m_lastcheckKeyCount;
public:
  SelfCheckStatusPrinter(const EndGameKeyDefinition *keydef, EndGameKeyWithOccupiedPositions &key)
    : m_keydef(*keydef)
    , m_key(key)
  {
    m_lastcheckKeyCount = -1;
  }
  void handleTimeout(Timer &timer);
  String getSummaryString() const;
};

void SelfCheckStatusPrinter::handleTimeout(Timer &timer) {
  const KeyDefinitionSelfCheckInfo info = m_keydef.getSelfCheckInfo();
  if(!info.m_testRunning) return;
  const EndGameKey key = m_key;
  String keysPerSecStr;
  const double now = getProcessTime();
  if(info.m_checkKeyCount > m_lastcheckKeyCount) {
    const UINT64 n = (info.m_checkKeyCount - m_lastcheckKeyCount);
    keysPerSecStr = format(_T("%9s keys/sec."), format1000((UINT64)(n / ((now - m_lastProcTime)/1000000.0))).cstr());
  }
  m_lastcheckKeyCount  = info.m_checkKeyCount;
  m_lastProcTime       = now;
  verbose(_T("%14s. %s -> %14s [min,max]:[%s..%s] %s%20c")
         ,format1000(info.m_checkKeyCount).cstr()
         ,key.toString(m_keydef).cstr()
         ,format1000(m_keydef.keyToIndex(key)).cstr()
         ,format1000(info.m_minIndex).cstr()
         ,format1000(info.m_maxIndex).cstr()
         ,keysPerSecStr.cstr()
         ,'\r'
         );
}

String SelfCheckStatusPrinter::getSummaryString() const {
  const KeyDefinitionSelfCheckInfo &info         = m_keydef.getSelfCheckInfo();
  const UINT64                      distinctKeys = info.getDistinctKeys();
  String result;
  result =  format(_T("Keys checked   : %14s.\n"        ), format1000(info.m_checkKeyCount).cstr());
  result += format(_T("Distinct keys  : %14s.\n"        ), format1000(distinctKeys).cstr());
  result += format(_T("Minimum index  : %14s. Key:%s.\n"), format1000(info.m_minIndex).cstr(), m_keydef.indexToKey(info.m_minIndex).toString(m_keydef).cstr());
  result += format(_T("Maximum index  : %14s. Key:%s.\n"), format1000(info.m_maxIndex).cstr(), m_keydef.indexToKey(info.m_maxIndex).toString(m_keydef).cstr());
  result += format(_T("Indexsize()    : %14s.\n"        ), format1000(m_keydef.getIndexSize()).cstr());
  result += format(_T("Duplicate keys : %14s.\n"        ), format1000(info.m_duplicateCount).cstr());
  result += format(_T("Utilizationrate: %9.2lf%%, (%6.2lf%%)\n"), PERCENT(distinctKeys,m_keydef.getIndexSize()), PERCENT(distinctKeys,info.getRangeLength()));
  return result;
}

void EndGameKeyDefinition::doSelfCheck(bool checkSym, bool listUnused, bool orderByLength) const {
  verbose(_T("Running %scodec test for keydefinition %-15s [%s]. Indexsize:%s\n")
         ,checkSym?_T("extended "):EMPTYSTRING
         ,getCodecName().cstr()
         ,toString().cstr()
         ,format1000(getIndexSize()).cstr());
  EndGameKeyWithOccupiedPositions key;
  selfCheckInit();
  SelfCheckStatusPrinter printStatus(this, key);
  Timer intervalPrinter(1);
//#ifndef _DEBUG
  intervalPrinter.startTimer(1000,printStatus, true);
//#endif
  selfCheck(key);
  intervalPrinter.stopTimer();

  const UINT64 distinctKeyCount1 = selfCheckSummary(printStatus, listUnused, orderByLength);
  if(checkSym) {
    const UINT64 distinctKeyCount2 = checkSymmetries();
    if(distinctKeyCount2 != distinctKeyCount1) {
      verbose(_T("Minimal scan generated %s distinct keys, but allkeys scan generated %s distinct keys.\n")
             ,format1000(distinctKeyCount1).cstr()
             ,format1000(distinctKeyCount2).cstr());
      pause();
    }
  }
  verbose(_T("%s\n"), spaceString(60,'_').cstr());
}

void EndGameKeyDefinition::selfCheck(EndGameKeyWithOccupiedPositions &key) const {
  sym8PositionScanner(key, 0, true);
}

void EndGameKeyDefinition::selfCheckInit() const {
  m_selfCheckInfo.reset();
  m_usedIndex      = new BitSet((size_t)getIndexSize()); TRACE_NEW(m_usedIndex);
  m_checkStartTime = getProcessTime();
}

void EndGameKeyDefinition::pause() const {
  m_selfCheckInfo.m_testRunning = false;
  ::pause();
  m_selfCheckInfo.m_testRunning = true;
}

UINT64 EndGameKeyDefinition::selfCheckSummary(const SelfCheckStatusPrinter &statusPrinter, bool listUnused, bool orderByLength) const {
  _tprintf(_T("%-*s\n"), 150, _T(" "));
  const double usedTime     = getProcessTime() - m_checkStartTime;
  const UINT64 distinctKeys = m_selfCheckInfo.getDistinctKeys();

  verbose(_T("%s"), statusPrinter.getSummaryString().cstr());
  verbose(_T("Used time      : %10.3lf sec. %7.3lf nano sec/key.\n"), usedTime / 1000000, usedTime/m_selfCheckInfo.m_checkKeyCount*1000);

  if(listUnused) {
    listLongestUnusedSequence(*m_usedIndex, orderByLength);
  }

#ifdef __NEVER__ // LIST_UNUSED
  FILE *f = FOPEN(getTempFileName(toString() + _T("Unused.txt")), _T("w"));
  for(EndGamePosIndex i = m_selfCheckInfo.m_minIndex; i <= m_selfCheckInfo.m_maxIndex; i++) {
    if(m_usedIndex->contains(i)) continue;
    intptr_t index1;
    String keyStr;
    try {
      const EndGameKey key = indexToKey(i);
      index1 = keyToIndex(key);
      keyStr = key.toString(*this);
    } catch(Exception e) {
      index1 = -1;
      keyStr = e.what();
    }
    _ftprintf(f, _T("%14s -> %s -> %14s"), format1000(i).cstr(), keyStr.cstr(), format1000(index1).cstr());
    if(index1 >= 0 || !m_usedIndex->contains(index1)) {
      _ftprintf(f, _T(" : not used either"));
    }
    _ftprintf(f,_T("\n"));
    fflush(f);
  }
  fclose(f);
#endif // LIST_UNUSED

  SAFEDELETE(m_usedIndex);
  return distinctKeys;
}

void EndGameKeyDefinition::checkForBothPlayers(EndGameKey key) const {
  SymmetricTransformation st = getSymTransformation(key);
  if(st != 0) {
    verbose(_T("Key [%s] needs transformation %s to be valid\n")
           ,key.toString(*this).cstr()
           ,getSymmetricTransformationToString(st).cstr());
    st = getSymTransformation(key);
    pause();
  }

  forEachPlayer(p) {
    key.setPlayerInTurn(p);
    checkKey(key);
  }
}

void EndGameKeyDefinition::checkKey(EndGameKey key) const {
  m_selfCheckInfo.m_checkKeyCount++;
  const EndGamePosIndex index = keyToIndex(key);
  if(index < m_selfCheckInfo.m_minIndex) {
    m_selfCheckInfo.m_minIndex = index;
  } else if(index > m_selfCheckInfo.m_maxIndex) {
    m_selfCheckInfo.m_maxIndex = index;
  }

  if(index >= getIndexSize()) {
    verbose(_T("%s %14s:%s -> %s. Index size=%s\n")
           ,toString().cstr()
           ,format1000(m_selfCheckInfo.m_checkKeyCount).cstr()
           ,key.toString(*this).cstr()
           ,format1000(index).cstr()
           ,format1000(getIndexSize()).cstr());
    const EndGamePosIndex index = keyToIndex(key);
    pause();
  } else if(m_usedIndex->contains((size_t)index)) {
    m_selfCheckInfo.m_duplicateCount++;
    verbose(_T("Warning:Index %s already used:Key:[%s]\n")
           ,format1000(index).cstr()
           ,key.toString(*this).cstr());
    pause();
  } else {
    m_usedIndex->add((size_t)index);
  }
  const EndGameKey k1 = indexToKey(index);
  if(!keysEqual(k1,key)) {
    verbose(_T("%s %14s:%s -> %s -> %s\n")
           ,toString().cstr()
           ,format1000(m_selfCheckInfo.m_checkKeyCount).cstr()
           ,key.toString(*this).cstr()
           ,format1000(index).cstr()
           ,k1.toString(*this).cstr());
    const EndGamePosIndex index1 = keyToIndex(key);
    const EndGameKey      k2     = indexToKey(index1);
    pause();
  }
}

void EndGameKeyDefinition::sym8PositionScanner(EndGameKeyWithOccupiedPositions &key, int pIndex, bool allPreviousOnDiag, PositionScanner nextScanner) const {
  if(pIndex == m_totalPieceCount) {
    checkForBothPlayers(key);
  } else {
    switch(pIndex) {
    case 0:
      { for(int i = 0; i < ARRAYSIZE(s_whiteKingIndexToPos); i++) {
          const int pos = s_whiteKingIndexToPos[i];
          key.setPosition(0,pos);
          sym8PositionScanner(key, 1, IS_ONMAINDIAG1(pos), nextScanner);
          key.clearField(pos);
        }
      }
      break;
    case 1:
      { const int wkPos = key.getWhiteKingPosition();
        if(allPreviousOnDiag) {
          for(int i = 0; i < ARRAYSIZE(s_subDiagIndexToPos); i++) { // for all sub- and diagonal-positions
            const int pos = s_subDiagIndexToPos[i];
            if(POSADJACENT(wkPos, pos)) continue;
            key.setPosition(1, pos);
            if(nextScanner) {
              (this->*(nextScanner))(key, 2, IS_ONMAINDIAG1(pos));
            } else {
              sym8PositionScanner(key, 2, IS_ONMAINDIAG1(pos), nextScanner);
            }
            key.clearField(pos);
          }
        } else {
          for(int pos = 0; pos < 64; pos++) { // for all positions
            if(POSADJACENT(wkPos, pos)) continue;
            key.setPosition(1, pos);
            if(nextScanner) {
              (this->*(nextScanner))(key, 2, false);
            } else {
              sym8PositionScanner(key, 2, false, nextScanner);
            }
            key.clearField(pos);
          }
        }
      }
      break;
    default:
      { const int pIndexAdd1 = pIndex+1;
        if(allPreviousOnDiag) {
          for(int i = 0; i < ARRAYSIZE(s_subDiagIndexToPos); i++) { // for all sub- and diagonal-positions
            const int pos = s_subDiagIndexToPos[i];
            if(key.isOccupied(pos)) continue;
            key.setPosition(pIndex, pos);
            if(nextScanner) {
              (this->*(nextScanner))(key, pIndexAdd1, IS_ONMAINDIAG1(pos));
            } else {
              sym8PositionScanner(key, pIndexAdd1, IS_ONMAINDIAG1(pos), nextScanner);
            }
            key.clearField(pos);
          }
        } else {
          for(int pos = 0; pos < 64; pos++) { // for all positions
            if(key.isOccupied(pos)) continue;
            key.setPosition(pIndex, pos);
            if(nextScanner) {
              (this->*(nextScanner))(key, pIndexAdd1, false);
            } else {
              sym8PositionScanner(key, pIndexAdd1, false, nextScanner);
            }
            key.clearField(pos);
          }
        }
      }
      break;
    }
  }
}

class AllPositionScanner;

typedef void (AllPositionScanner::*ScannerFunction)();

class AllPositionScanner : public TimeoutHandler {
private:
  const EndGameKeyDefinition     &m_keydef;
  const int                       m_pieceCount;
  EndGameKeyWithOccupiedPositions m_key;
  ScannerFunction                 m_scannerFunctions[MAX_ENDGAME_PIECECOUNT];
  int                             m_pIndex;
  UINT64                          m_positionCount, m_lastPositionCount;
  double                          m_lastStatusTime;
  UINT64                          m_distinctPositionCount;
  BitSet                          m_usedIndex;

  void allPositions();
  void blackKingPositions();
  void pawnPositions();
  void setPosAndScanNext(int pos);
  void checkForBothPlayers();
  void checkSymmetry();
  void handleTimeout(Timer &timer);

public:
  AllPositionScanner(const EndGameKeyDefinition &keydef);
  UINT64 scanAllPositions();
};

AllPositionScanner::AllPositionScanner(const EndGameKeyDefinition &keydef)
: m_keydef(keydef)
, m_pieceCount(keydef.getPieceCount())
, m_usedIndex((size_t)keydef.getIndexSize())
, m_lastPositionCount(-1)
{
  for(int i = 0; i < m_pieceCount; i++) m_scannerFunctions[i] = NULL;
  m_scannerFunctions[0] = &AllPositionScanner::allPositions;
  m_scannerFunctions[1] = &AllPositionScanner::blackKingPositions;

  for(int i = 2; i < m_pieceCount; i++) {
    switch(m_keydef.getPieceType(i)) {
    case Queen :
    case Rook  :
    case Knight:
    case Bishop:
      m_scannerFunctions[i] = &AllPositionScanner::allPositions;
      break;
    case Pawn:
      m_scannerFunctions[i] = &AllPositionScanner::pawnPositions;
      break;
    default:
      throwException(_T("%s:Unknown piecetype:%d"), __TFUNCTION__, m_keydef.getPieceType(i));
    }
  }
}

UINT64 AllPositionScanner::scanAllPositions() {
  m_positionCount         = 0;
  m_distinctPositionCount = 0;
  m_pIndex                = 0;
  verbose(_T("Checking symmetric transformation...\n"));
  Timer doPrintStatus(1);
  doPrintStatus.startTimer(3000, *this, true);
  (this->*(m_scannerFunctions[0]))();
  doPrintStatus.stopTimer();
  verbose(_T("All %s positions checked. %s distinct keys found.\n")
         ,format1000(m_positionCount).cstr()
         ,format1000(m_distinctPositionCount).cstr());
//  m_keydef.listLongestUnusedSequence(m_usedIndex);
  return m_distinctPositionCount;
}

void AllPositionScanner::setPosAndScanNext(int pos) {
  if(m_key.isOccupied(pos)) return;
  m_key.setPosition(m_pIndex++, pos);

  if(m_pIndex == m_pieceCount) {
    checkForBothPlayers();
  } else {
    (this->*(m_scannerFunctions[m_pIndex]))();
  }
  m_pIndex--;
  m_key.clearField(pos);
}

void AllPositionScanner::checkForBothPlayers() {
  m_key.setPlayerInTurn(WHITEPLAYER);
  checkSymmetry();
  m_key.setPlayerInTurn(BLACKPLAYER);
  checkSymmetry();
}

void AllPositionScanner::checkSymmetry() {
  m_positionCount++;

  EndGameKey      key1 = m_keydef.getTransformedKey(m_key, m_keydef.getSymTransformation(m_key));
  EndGamePosIndex index;
  EndGameKey      key2 = m_keydef.getNormalizedKey(key1, &index);

  if(!m_usedIndex.contains((size_t)index)) {
    m_distinctPositionCount++;
    m_usedIndex.add((size_t)index);
  }
  if(!m_keydef.keysEqual(key1, key2)) {
    _tprintf(_T("transformedKey(%s) -> %s\n"), m_key.toString(m_keydef).cstr(), key1.toString(m_keydef).cstr());
    _tprintf(_T("normalizedKey( %s) -> %s\n"), key1.toString(m_keydef).cstr(), key2.toString(m_keydef).cstr());
    EndGameKey key3 = m_keydef.getTransformedKey(key1, m_keydef.getSymTransformation(key1));
    EndGameKey key4 = m_keydef.getNormalizedKey(key1, &index);(key2);
    _tprintf(_T("transformedKey(%s) -> %s\n"), key1.toString(m_keydef).cstr(), key3.toString(m_keydef).cstr());
    _tprintf(_T("normalizedKey( %s) -> %s\n"), key2.toString(m_keydef).cstr(), key4.toString(m_keydef).cstr());
    EndGameKey k1 = m_keydef.getTransformedKey(m_key, m_keydef.getSymTransformation(m_key));
    EndGameKey k2 = m_keydef.getNormalizedKey(key1, &index);(k1);
    bool k3 = m_keydef.keysEqual(k1, k2);
    pause();
  }
}

void AllPositionScanner::allPositions() {
  for(int pos = 0; pos < 64; pos++) {
    setPosAndScanNext(pos);
  }
}

void AllPositionScanner::blackKingPositions() {
  const int wkPos = m_key.getWhiteKingPosition();
  for(int pos = 0; pos < 64; pos++) {
    if(POSADJACENT(wkPos, pos)) {
      continue;
    }
    setPosAndScanNext(pos);
  }
}

void AllPositionScanner::pawnPositions() {
  for(int i = 0; i < ARRAYSIZE(EndGameKeyDefinition::s_pawnIndexToPos); i++) {
    setPosAndScanNext(EndGameKeyDefinition::s_pawnIndexToPos[i]);
  }
}

void AllPositionScanner::handleTimeout(Timer &timer) {
  const double now = getProcessTime();
  TCHAR infoStr[200];
  if (m_positionCount > m_lastPositionCount) {
    const UINT64 n = m_positionCount - m_lastPositionCount;
    _stprintf(infoStr, _T(" Keys/sec:%s."), format1000((UINT64)((double)n / ((now-m_lastStatusTime)/1000000.0))).cstr());
  } else {
    infoStr[0] = 0;
  }
  m_lastPositionCount = m_positionCount;
  m_lastStatusTime    = now;
  _tprintf(_T("Position %15s. %s\r"), format1000(m_positionCount).cstr(), infoStr);
}

UINT64 EndGameKeyDefinition::checkSymmetries() const {
  AllPositionScanner scanner(*this);
  return scanner.scanAllPositions();
}

class UnusedSequence {
private:
  static String genKeyString(const EndGameKeyDefinition *keydef, EndGamePosIndex index);
public:
  const EndGamePosIndex m_from, m_to;
  EndGamePosIndex getLength() const {
    return m_to - m_from + 1;
  }
  UnusedSequence(EndGamePosIndex from, EndGamePosIndex to) : m_from(from), m_to(to) {
  }

  String toString(const EndGameKeyDefinition *keydef) const;
};

String UnusedSequence::genKeyString(const EndGameKeyDefinition *keydef, EndGamePosIndex index) {
  try {
    return keydef->indexToKey(index).toString(*keydef);
  } catch(Exception e) {
    return format(_T("Invalid index:%s:%s"), format1000(index).cstr(), e.what());
  }
}

String UnusedSequence::toString(const EndGameKeyDefinition *keydef) const {
  return format(_T("%14s-%14s. length:%11s : %c%s - %s[ = [%s - %s]")
               ,format1000(m_from).cstr()
               ,format1000(m_to).cstr()
               ,format1000(getLength()).cstr()
               ,m_from?_T(']'):_T('[')
               ,genKeyString(keydef, m_from?(m_from - 1):m_from).cstr()
               ,genKeyString(keydef, m_to   + 1).cstr()
               ,genKeyString(keydef, m_from    ).cstr()
               ,genKeyString(keydef, m_to      ).cstr()
               );
}

typedef enum {
  COMPARE_BY_LENGTH
 ,COMPARE_BY_INDEX
} UnusedSequenceCompareField;

class UnusedSequenceComparator : public Comparator<UnusedSequence> {
private:
  const UnusedSequenceCompareField m_compareField;
public:
  UnusedSequenceComparator(UnusedSequenceCompareField field) : m_compareField(field) {
  }
  int compare(const UnusedSequence &e1, const UnusedSequence &e2) {
    switch(m_compareField) {
    case COMPARE_BY_LENGTH: return sign((intptr_t)e2.getLength() - (intptr_t)e1.getLength());
    case COMPARE_BY_INDEX : return sign((INT64)e1.m_from - (INT64)e2.m_from);
    default               : return sign((INT64)e1.m_from - (INT64)e2.m_from);
    }
  }
  AbstractComparator *clone() const {
    return new UnusedSequenceComparator(m_compareField);
  }
};

class UnusedPTQueue : public PriorityQueue<UnusedSequence> {
public:
  UnusedPTQueue(UnusedSequenceCompareField compareField) : PriorityQueue<UnusedSequence>(UnusedSequenceComparator(compareField)) {
  }
};

void EndGameKeyDefinition::listLongestUnusedSequence(BitSet &s, bool orderByLength, intptr_t sequenceMinSize) const {
  UnusedPTQueue ptq(orderByLength ? COMPARE_BY_LENGTH : COMPARE_BY_INDEX);
  intptr_t last = -1;
  for(Iterator<size_t> it = s.getIterator(); it.hasNext();) {
    const intptr_t i = it.next();
    if(i - last > sequenceMinSize) {
      ptq.add(UnusedSequence(last+1, i-1));
    }
    last = i;
  }
  if((intptr_t)s.getCapacity()-1 - last > sequenceMinSize) {
    ptq.add(UnusedSequence(last+1, s.getCapacity()-1));
  }

  FILE *f = MKFOPEN(getTempFileName(toString() + _T("UnusedSequences.txt")), _T("w"));
  for(int i = 0; !ptq.isEmpty(); i++) {
    const UnusedSequence us = ptq.remove();
    _ftprintf(f, _T("%s\n"), us.toString(this).cstr());
  }
  UINT64 totalUnused = s.getCapacity() - s.size();
  _ftprintf(f, _T("Total unused indices:%s\n"), format1000(totalUnused).cstr());
  fclose(f);
}

#endif // TABLEBASE_BUILDER
