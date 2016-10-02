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
  m_dbPath = _T("c:\\temp");
  m_metric = DEPTH_TO_CONVERSION;
}

bool EndGameGlobalProperties::setDbPath(const String &path) {
  FileNameSplitter newInfo(path);
  FileNameSplitter oldInfo(m_dbPath);
  const bool ret = newInfo.getFullPath() != oldInfo.getFullPath();
  const String oldPath = m_dbPath;
  m_dbPath = path;
  notifyPropertyChanged(ENDGAME_PATH, &oldPath, &m_dbPath);
  return ret;
}

void EndGameGlobalProperties::setMetric(TablebaseMetric m) {
  if(m != m_metric) {
    const TablebaseMetric oldMetric = m_metric;
    m_metric = m;
    notifyPropertyChanged(ENDGAME_METRIC, &oldMetric, &m_metric);
  }
}

EndGameGlobalProperties EndGameKeyDefinition::globalProperties;

const TCHAR *EndGameKeyDefinition::metricName[] = {
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
  globalProperties.addPropertyChangeListener(listener);
}

void EndGameKeyDefinition::removePropertyChangeListener(PropertyChangeListener *listener) { // static
  globalProperties.removePropertyChangeListener(listener);
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




int EndGameKeyDefinition::findRange2Equal(long f, unsigned long index) { // static 
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
    fidiv f
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

int EndGameKeyDefinition::findRange(const unsigned long *rangeTable, UINT size, unsigned long index) { // static 
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

EndGameKeyDefinition::EndGameKeyDefinition(PieceKey pk2) : m_totalPieceCount(3) {
  init(pk2);
}

EndGameKeyDefinition::EndGameKeyDefinition(PieceKey pk2, PieceKey pk3) : m_totalPieceCount(4) {
  init(pk2, pk3);
}

EndGameKeyDefinition::EndGameKeyDefinition(PieceKey pk2, PieceKey pk3, PieceKey pk4) : m_totalPieceCount(5) {
  init(pk2, pk3, pk4);
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
  throwInvalidArgumentException(_T("findKeyIndexByCount"), _T("pieceKey=%s %s, n=%d")
                               ,getPlayerNameEnglish(GET_PLAYER_FROMKEY(pieceKey))
                               ,getPieceTypeNameEnglish(GET_TYPE_FROMKEY(pieceKey))
                               ,n);
  return -1;
}

String PieceIndexMappings::toString() const {
  String result;
  for(int i = 0; i < ARRAYSIZE(m_pieceIndexMap); i++) {
    unsigned char pi = m_pieceIndexMap[i];
    if(pi == 0xff) {
      break;
    }
    result += format(_T("pieceIndexMap[%d]=%d\n"),i,pi);
  }
  forEachPlayer(p) {
    result += format(_T("ReverseMap(%s)"), getPlayerNameEnglish(p));
    TCHAR delimiter = _T(':');
    for(int i = 0; i < ARRAYSIZE(m_pieceReverseIndexMap[p]); i++, delimiter=_T(',')) {
      unsigned char ri = m_pieceReverseIndexMap[p][i];
      if(ri == 0xff) {
        break;
      }
      result += format(_T("%c%d"), delimiter, ri);
    }
    result += _T("\n");
  }
  return result;
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

void EndGameKeyDefinition::impossibleEncodingError(const EndGameKey &key) const {
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
  case 5 : key.setPosition4(GameKey::transform(key.getPosition4(), st)); // continue case
  case 4 : key.setPosition3(GameKey::transform(key.getPosition3(), st)); // continue case
  case 3 : key.setPosition2(GameKey::transform(key.getPosition2(), st));
           key.setPosition1(GameKey::transform(key.getPosition1(), st));
           key.setPosition0(GameKey::transform(key.getPosition0(), st));
           return key;
  default: invalidPieceCountError();
  }
  return key;
}

EndGameKey EndGameKeyDefinition::getNormalizedKey(const EndGameKey &key, UINT *index) const {
  static const TCHAR *msg = _T("Invalid EndGameKey:getNormalizedKey(%s):%s");
  const UINT ki = keyToIndex(key);
  if(index) *index = ki;
  const EndGameKey result = indexToKey(ki);
  validateKey(result, msg);
  return result;
}

void EndGameKeyDefinition::validateKey(const EndGameKey key, const TCHAR *msg) const {
  const UINT wkPos = key.getWhiteKingPosition();
  const UINT bkPos = key.getBlackKingPosition();
  if(KINGSADJACENT(wkPos, bkPos)) {
    throwException(msg, key.toString(*this).cstr(), key.toString(*this).cstr());
  }
  for(UINT i = 2; i < m_totalPieceCount; i++) {
    const UINT posi = key.getPosition(i);
    switch(i) {
    case 4: 
      if(posi == key.getPosition3()) {
        throwException(msg, key.toString(*this).cstr(), key.toString(*this).cstr());
      }
      // continue case      
    case 3:
      if(posi == key.getPosition2()) {
        throwException(msg, key.toString(*this).cstr(), key.toString(*this).cstr());
      }
      // continue case      
    case 2:
      if((posi == wkPos) || (posi == bkPos)) {
        throwException(msg, key.toString(*this).cstr(), key.toString(*this).cstr());
      }
      break;
    default: 
      invalidPieceCountError();
    }
  }
}


TCHAR *EndGameKeyDefinition::createKeyString(TCHAR *dst, const EndGameKey &key, bool initFormat) const {
  if(initFormat) {
    return _tcscpy(dst, createInitKeyString(key).cstr());
  }
  
  TCHAR blackStr[100];
  return _tcscat(createWhiteKeyString(dst, key), createBlackKeyString(blackStr, key));
}

static const TCHAR *shortNames[]    = { _T(""),_T("K"),_T("Q"),_T("R"),_T("B"),_T("N"),_T("") };
static const TCHAR *playerShortName = _T("WB");
#define GETFIELDNAME(pos) Game::fieldInfo[pos].m_name

TCHAR *EndGameKeyDefinition::createWhiteKeyString(TCHAR *dst, const EndGameKey &key) const {
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
  default: throwException(_T("Invalid number of white piece (=%d)"), getPieceCount(WHITEPLAYER));
           return _T("????");
  }
}

TCHAR *EndGameKeyDefinition::createBlackKeyString(TCHAR *dst, const EndGameKey &key) const {
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
  default: throwException(_T("Invalid number of black piece (=%d)"), getPieceCount(BLACKPLAYER));
           return _T("????");
  }
}

String EndGameKeyDefinition::createInitKeyString(const EndGameKey &key) const {
  String result = key.getPlayerInTurn()==WHITEPLAYER ? _T("WHITEPLAYER") : _T("BLACKPLAYER");
  for(int i = 0; i < m_totalPieceCount; i++) {
    result += _T(",");
    result += getFieldName(key.getPosition(i));
  }
  return toUpperCase(result);
}

#ifdef TABLEBASE_BUILDER

unsigned long EndGameKeyDefinition::keyToIndexNew(const EndGameKey &key) const { // should be overridden
  throwException(_T("keyToIndexNew:No conversion defined"));
  return 0;
}

EndGameKey EndGameKeyDefinition::indexToKeyNew(unsigned long index) const { // should be overridden
  throwException(_T("indexToKeyNew:No conversion defined"));
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
  throwInvalidArgumentException(_T("getPlayTransformation")
                               ,_T("EndGameKeyDefinition has signature %s. Game has %s")
                               ,kps.toString().cstr()
                               ,gps.toString().cstr()
                               );
  return 0;
}

void EndGameKeyDefinition::invalidSquareError(const EndGameKey &key) { // static
  throwException(_T("Position %s not contained in any square"), getFieldName(key.getWhiteKingPosition()));
}

void EndGameKeyDefinition::sym8DecisionSwitchError(int line) { // static
  throwException(_T("Line %d:SYM8DECISIONSWITCH dropped to the end"), line);
}

void EndGameKeyDefinition::pawnSymSwitchError(int line) { // static
  throwException(_T("Line %d:DECIDEPAWNSYMTRANSFORM2EQUALPAWNS dropped to the end"), line);
}

void EndGameKeyDefinition::invalidPieceCountError() const {
  throwException(_T("Invalid number of pieces (=%d) must be [3..5]"), m_totalPieceCount);
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
    return f2Offdiag(key.getPosition2())          ? trTrue : trFalse;                                         \
  }                                                                                                           \
}

// Creates a Symmetric Transformation, that moves W.King to lower left triangle (bounded by a1,d1,d4)
// and if this is on diagonal a1-h8, moves B.King below this diagonal, ie to the triangle bounded by (a1,h1,h8).
// If B.king is on main diagonal1 too, then flip board so piece2 will be below this diagonal.
SymmetricTransformation EndGameKeyDefinition::getSym8Transformation3Men(const EndGameKey &key) { // static
  SYM8DECISIONSWITCH(DECIDESYM8TRANSFORM3MEN)
}

#define DECIDESYM8TRANSFORM4MEN(f1Offdiag, f2Offdiag, trTrue, trFalse)                                        \
{ if(f1Offdiag(key.getWhiteKingPosition())) {                                                                 \
    return f2Offdiag(key.getWhiteKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getBlackKingPosition())) {                                                          \
    return f2Offdiag(key.getBlackKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition2())) {                                                                  \
    return f2Offdiag(key.getPosition2())          ? trTrue : trFalse;                                         \
  } else {                                                                                                    \
    return f2Offdiag(key.getPosition3())          ? trTrue : trFalse;                                         \
  }                                                                                                           \
}

// Creates a Symmetric Transformation, that moves W.King to lower left triangle (bounded by a1,d1,d4)
// and if this is on diagonal a1-h8, moves B.King below this diagonal, ie to the triangle bounded by (a1,h1,h8).
// If B.king is on main diagonal too, then flip board so piece2 will be below this diagonal.
// if piece2 is on main diagonal too, flip board so piece3 will be below the diagonal
SymmetricTransformation EndGameKeyDefinition::getSym8Transformation4Men(const EndGameKey &key) { // static
  SYM8DECISIONSWITCH(DECIDESYM8TRANSFORM4MEN)
}

#define DECIDESYM8TRANSFORM5MEN(f1Offdiag, f2Offdiag, trTrue, trFalse)                                        \
{ if(f1Offdiag(key.getWhiteKingPosition())) {                                                                 \
    return f2Offdiag(key.getWhiteKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getBlackKingPosition())) {                                                          \
    return f2Offdiag(key.getBlackKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition2())) {                                                                  \
    return f2Offdiag(key.getPosition2())          ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition3())) {                                                                  \
    return f2Offdiag(key.getPosition3())          ? trTrue : trFalse;                                         \
  } else {                                                                                                    \
    return f2Offdiag(key.getPosition4())          ? trTrue : trFalse;                                         \
  }                                                                                                           \
}

// See comment on EndGameKeyDefinitionBase3Men and -4Men
SymmetricTransformation EndGameKeyDefinition::getSym8Transformation5Men(const EndGameKey &key) { // static
  SYM8DECISIONSWITCH(DECIDESYM8TRANSFORM5MEN)
}

#define DECIDESYM8TRANSFORM2EQUAL_OPPOSITESIDE(trTrue, trFalse, i, j)                                         \
{ const int pi = offDiagPosToIndex[GameKey::transform(key.getPosition##i(), trTrue)];                         \
  const int pj = offDiagPosToIndex[GameKey::transform(key.getPosition##j(), trTrue)];                         \
  if(pi >= 28) {                                                                                              \
    return (pi - 28 > pj) ? trTrue : trFalse;                                                                 \
  } else {                                                                                                    \
    return (pj - 28 > pi) ? trTrue : trFalse;                                                                 \
  }                                                                                                           \
}

#define DECIDESYM8TRANSFORM2EQUAL_OFFDIAG(diagSide, trTrue, trFalse, i, j)                                    \
{ if(diagSide(key.getPosition##i()) == diagSide(key.getPosition##j())) {                                      \
    return diagSide(key.getPosition##i()) ? trTrue : trFalse;                                                 \
  } else {                                                                                                    \
    DECIDESYM8TRANSFORM2EQUAL_OPPOSITESIDE(trTrue, trFalse, i, j);                                            \
  }                                                                                                           \
}

#define DECIDESYM8TRANSFORM4MEN2EQUAL(f1Offdiag, f2Offdiag, trTrue, trFalse)                                  \
{ if(f1Offdiag(key.getWhiteKingPosition())) {                                                                 \
    return f2Offdiag(key.getWhiteKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getBlackKingPosition())) {                                                          \
    return f2Offdiag(key.getBlackKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition2()) && f1Offdiag(key.getPosition3())) {                                 \
    DECIDESYM8TRANSFORM2EQUAL_OFFDIAG(f2Offdiag, trTrue, trFalse, 2, 3);                                      \
  } else if(f1Offdiag(key.getPosition2())) {                                                                  \
    return f2Offdiag(key.getPosition2())          ? trTrue : trFalse;                                         \
  } else { /* f1Offdiag(pos3) */                                                                              \
    return f2Offdiag(key.getPosition3())          ? trTrue : trFalse;                                         \
  }                                                                                                           \
}

SymmetricTransformation EndGameKeyDefinition::getSym8Transformation4Men2Equal(const EndGameKey &key) { // static 
  SYM8DECISIONSWITCH(DECIDESYM8TRANSFORM4MEN2EQUAL)
}

#define _DECIDESYM8TRANSFORM5MEN2EQUAL(f1Offdiag, f2Offdiag, trTrue, trFalse, solo, eq1, eq2)                 \
{ if(f1Offdiag(key.getWhiteKingPosition())) {                                                                 \
    return f2Offdiag(key.getWhiteKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getBlackKingPosition())) {                                                          \
    return f2Offdiag(key.getBlackKingPosition())  ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition##solo())) {                                                             \
    return f2Offdiag(key.getPosition##solo())     ? trTrue : trFalse;                                         \
  } else if(f1Offdiag(key.getPosition##eq1()) && f1Offdiag(key.getPosition##eq2())) {                         \
    DECIDESYM8TRANSFORM2EQUAL_OFFDIAG(f2Offdiag, trTrue, trFalse, eq1, eq2);                                  \
  } else if(f1Offdiag(key.getPosition##eq1())) {                                                              \
    return f2Offdiag(key.getPosition##eq1())      ? trTrue : trFalse;                                         \
  } else { /* f1Offdiag(pos[eq2]) */                                                                          \
    return f2Offdiag(key.getPosition##eq2())      ? trTrue : trFalse;                                         \
  }                                                                                                           \
}


#define DECIDESYM8TRANSFORM5MEN2EQUAL(f1Offdiag, f2Offdiag, trTrue, trFalse)                                  \
  _DECIDESYM8TRANSFORM5MEN2EQUAL(f1Offdiag, f2Offdiag, trTrue, trFalse, 2, 3, 4)

SymmetricTransformation EndGameKeyDefinition::getSym8Transformation5Men2Equal(const EndGameKey &key) { // static 
  SYM8DECISIONSWITCH(DECIDESYM8TRANSFORM5MEN2EQUAL)
}

#ifdef _DEBUG

static SymmetricTransformation decideSym8Transform3EqualFlipi(EndGameKey key, SymmetricTransformation trTrue, SymmetricTransformation trFalse, int i, int j, int k) {
  const int pi = EndGameKeyDefinition::offDiagPosToIndex[GameKey::transform(key.getPosition(i), trTrue )];
  const int pj = EndGameKeyDefinition::offDiagPosToIndex[GameKey::transform(key.getPosition(j), trFalse)];
  const int pk = EndGameKeyDefinition::offDiagPosToIndex[GameKey::transform(key.getPosition(k), trFalse)];
  return (pi > max(pj, pk)) ? trTrue : trFalse;
}

static SymmetricTransformation decideSym8Transform3EqualFlipij(EndGameKey key, SymmetricTransformation trTrue, SymmetricTransformation trFalse, int i, int j, int k) {
  const int pi = EndGameKeyDefinition::offDiagPosToIndex[GameKey::transform(key.getPosition(i), trTrue )];
  const int pj = EndGameKeyDefinition::offDiagPosToIndex[GameKey::transform(key.getPosition(j), trTrue )];
  const int pk = EndGameKeyDefinition::offDiagPosToIndex[GameKey::transform(key.getPosition(k), trFalse)];
  return (max(pi, pj) >= pk) ? trTrue : trFalse;
}

#define DECIDESYM8TRANSFORM3EQUAL_FLIPi( trTrue, trFalse, i, j, k) return decideSym8Transform3EqualFlipi( key, trTrue, trFalse, i, j, k)
#define DECIDESYM8TRANSFORM3EQUAL_FLIPij(trTrue, trFalse, i, j, k) return decideSym8Transform3EqualFlipij(key, trTrue, trFalse, i, j, k)

#else

// Assume key.position(i) needs flip below diagonal.
// return flip(pi) > max(pj,pk) ? trTrue : trFalse
#define DECIDESYM8TRANSFORM3EQUAL_FLIPi(trTrue, trFalse, i, j, k)                                             \
{ const int pi = offDiagPosToIndex[GameKey::transform(key.getPosition##i(), trTrue )];                        \
  const int pj = offDiagPosToIndex[GameKey::transform(key.getPosition##j(), trFalse)];                        \
  const int pk = offDiagPosToIndex[GameKey::transform(key.getPosition##k(), trFalse)];                        \
  return (pi > max(pj, pk)) ? trTrue : trFalse;                                                               \
}

// Assume key.position(i) and position(j) need flip below diagonal.
// return max(flip(pi),flip(pj)) >= pk ? trTrue : trFalse
#define DECIDESYM8TRANSFORM3EQUAL_FLIPij(trTrue, trFalse, i, j, k)                                            \
{ const int pi = offDiagPosToIndex[GameKey::transform(key.getPosition##i(), trTrue )];                        \
  const int pj = offDiagPosToIndex[GameKey::transform(key.getPosition##j(), trTrue )];                        \
  const int pk = offDiagPosToIndex[GameKey::transform(key.getPosition##k(), trFalse)];                        \
  return (max(pi, pj) >= pk) ? trTrue : trFalse;                                                              \
}

#endif

#define DECIDESYM8TRANSFORM3EQUAL(f1Offdiag, f2Offdiag, trTrue, trFalse)                                      \
{ if(f1Offdiag(key.getWhiteKingPosition())) {                                                                 \
    return f2Offdiag(key.getWhiteKingPosition())   ? trTrue : trFalse;            /* wk off diag      */      \
  } else if(f1Offdiag(key.getBlackKingPosition())) {                                                          \
    return f2Offdiag(key.getBlackKingPosition())   ? trTrue : trFalse;            /* wk on diag       */      \
  } else {                                                                        /* kings on    diag */      \
    switch(KEYBOOL3MASK(key, f1Offdiag, 2, 3, 4)) {                                                           \
    case 0: return trFalse;                                                       /* 2,3,4 on    diag */      \
    case 1: return f2Offdiag(key.getPosition2()  ) ? trTrue : trFalse;            /*   3,4 on    diag */      \
    case 2: return f2Offdiag(key.getPosition3()  ) ? trTrue : trFalse;            /* 2,  4 on    diag */      \
    case 3: DECIDESYM8TRANSFORM2EQUAL_OFFDIAG(f2Offdiag, trTrue, trFalse, 2, 3);  /*     4 on    diag */      \
    case 4: return f2Offdiag(key.getPosition4()  ) ? trTrue : trFalse;            /* 2,3   on    diag */      \
    case 5: DECIDESYM8TRANSFORM2EQUAL_OFFDIAG(f2Offdiag, trTrue, trFalse, 2, 4);  /*   3   on    diag */      \
    case 6: DECIDESYM8TRANSFORM2EQUAL_OFFDIAG(f2Offdiag, trTrue, trFalse, 3, 4);  /* 2     on    diag */      \
    case 7: switch(KEYBOOL3MASK(key, f2Offdiag, 2, 3, 4)) {                       /* none  on    diag */      \
            case 0: return trFalse;                                               /* 2,3,4 below diag */      \
            case 1: DECIDESYM8TRANSFORM3EQUAL_FLIPi( trTrue, trFalse, 2, 3, 4);   /*   3,4 below diag */      \
            case 2: DECIDESYM8TRANSFORM3EQUAL_FLIPi( trTrue, trFalse, 3, 2, 4);   /* 2,  4 below diag */      \
            case 3: DECIDESYM8TRANSFORM3EQUAL_FLIPij(trTrue, trFalse, 2, 3, 4);   /*     4 below diag */      \
            case 4: DECIDESYM8TRANSFORM3EQUAL_FLIPi( trTrue, trFalse, 4, 2, 3);   /* 2,3   below diag */      \
            case 5: DECIDESYM8TRANSFORM3EQUAL_FLIPij(trTrue, trFalse, 2, 4, 3);   /*   3   below diag */      \
            case 6: DECIDESYM8TRANSFORM3EQUAL_FLIPij(trTrue, trFalse, 3, 4, 2);   /* 2     below diag */      \
            case 7: return trTrue;                                                /* none  below diag */      \
            }                                                                                                 \
    }                                                                                                         \
  }                                                                                                           \
}

SymmetricTransformation EndGameKeyDefinition::getSym8Transformation5Men3Equal(const EndGameKey &key) { // static 
  SYM8DECISIONSWITCH(DECIDESYM8TRANSFORM3EQUAL)
}

SymmetricTransformation EndGameKeyDefinition::getPawnSymTransformation(const EndGameKey &key) { // static
  return IS_QUEENSIDE(key.getPosition2()) ? 0 : TRANSFORM_MIRRORCOL;
}

#define DECIDEPAWNSYMTRANSFORM2EQUALPAWNS(i,j)                                                                \
{ switch(KEYBOOL2MASK(key, IS_QUEENSIDE, i, j)) {                                                             \
  case 0: return TRANSFORM_MIRRORCOL;                                                                         \
  case 1:                                                                                                     \
    { const UINT pi = pawnPosToIndex[key.getPosition##i()];                                                   \
      const UINT pj = pawnPosToIndex[MIRRORCOLUMN(key.getPosition##j())];                                     \
      if(pi != pj) {                                                                                          \
        return (pi < pj) ? 0 : TRANSFORM_MIRRORCOL;                                                           \
      } else {                                                                                                \
        return IS_QUEENSIDE(key.getWhiteKingPosition()) ? 0 : TRANSFORM_MIRRORCOL;                            \
      }                                                                                                       \
    }                                                                                                         \
  case 2:                                                                                                     \
    { const UINT pi = pawnPosToIndex[MIRRORCOLUMN(key.getPosition##i())];                                     \
      const UINT pj = pawnPosToIndex[key.getPosition##j()];                                                   \
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

SymmetricTransformation EndGameKeyDefinition::get4Men2EqualPawnsSymTransformation(const EndGameKey &key) { // static
  DECIDEPAWNSYMTRANSFORM2EQUALPAWNS(2, 3)
}

SymmetricTransformation EndGameKeyDefinition::get5Men2EqualPawnsSymTransformation(const EndGameKey &key) { // static
  DECIDEPAWNSYMTRANSFORM2EQUALPAWNS(3, 4)
}

#ifdef _DEBUG

static SymmetricTransformation decidePawnTransform3EqualPawnsFlipi(EndGameKey key, int i, int j, int k) {
  const int pi = EndGameKeyDefinition::pawnPosToIndex[MIRRORCOLUMN(key.getPosition(i))];
  const int pj = EndGameKeyDefinition::pawnPosToIndex[key.getPosition(j)];
  const int pk = EndGameKeyDefinition::pawnPosToIndex[key.getPosition(k)];
  return (pi > max(pj, pk)) ? TRANSFORM_MIRRORCOL : 0;
}

static SymmetricTransformation decidePawnTransform3EqualPawnsFlipij(EndGameKey key, int i, int j, int k) {
  const int pi = EndGameKeyDefinition::pawnPosToIndex[MIRRORCOLUMN(key.getPosition(i))];
  const int pj = EndGameKeyDefinition::pawnPosToIndex[MIRRORCOLUMN(key.getPosition(j))];
  const int pk = EndGameKeyDefinition::pawnPosToIndex[key.getPosition(k)];
  return (max(pi, pj) >= pk) ? TRANSFORM_MIRRORCOL : 0;
}

#define DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPi( i, j, k) return decidePawnTransform3EqualPawnsFlipi( key, i, j, k)
#define DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPij(i, j, k) return decidePawnTransform3EqualPawnsFlipij(key, i, j, k)

#else

/* Assume p##i is on kingside. return flip(pi) > max(pj,pk) ? MIRRORCOL : 0 */
#define DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPi(i, j, k)                                                      \
{ const int pi = pawnPosToIndex[MIRRORCOLUMN(key.getPosition##i())];                                          \
  const int pj = pawnPosToIndex[key.getPosition##j()];                                                        \
  const int pk = pawnPosToIndex[key.getPosition##k()];                                                        \
  return (pi > max(pj, pk)) ? TRANSFORM_MIRRORCOL : 0;                                                        \
}

/* Assume p##i and p##j is on kingside. return (max(flip(pi), flip(pj)) >= pk) ? MIRRORCOL : 0 */
#define DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPij(i, j, k)                                                     \
{ const int pi = pawnPosToIndex[MIRRORCOLUMN(key.getPosition##i())];                                          \
  const int pj = pawnPosToIndex[MIRRORCOLUMN(key.getPosition##j())];                                          \
  const int pk = pawnPosToIndex[key.getPosition##k()];                                                        \
  return (max(pi, pj) >= pk) ? TRANSFORM_MIRRORCOL : 0;                                                       \
}

#endif

SymmetricTransformation EndGameKeyDefinition::get5Men3EqualPawnsSymTransformation(const EndGameKey &key) { // static
  switch(KEYBOOL3MASK(key, IS_KINGSIDE, 2, 3, 4)) { // similar to DECIDESYM8TRANSFORM3EQUAL, case 7:kings on diag, 2,3,4 off diag
  case 0 : return 0;                                                              /* 2,3,4 queenside */
  case 1 : DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPi( 2, 3, 4);                     /*   3,4 queenside */
  case 2 : DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPi( 3, 2, 4);                     /* 2,  4 queenside */
  case 3 : DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPij(2, 3, 4);                     /*     4 queenside */
  case 4 : DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPi( 4, 2, 3);                     /* 2,3   queenside */
  case 5 : DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPij(2, 4, 3);                     /*   3   queenside */
  case 6 : DECIDEPAWNSYMTRANSFORM3EQUALPAWNS_FLIPij(3, 4, 2);                     /* 2     queenside */
  case 7 : return TRANSFORM_MIRRORCOL;                                            /* none  queenside */
  }
  throwException(_T("get5Men3EqualPawnsSymTransformation:Unexpected mask:%d. Valid are [0..7]"), KEYBOOL3MASK(key, IS_KINGSIDE, 2, 3, 4));
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
    tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, F2, G2);
    tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, F3, G2);
    tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, F2, G3);
    tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, F3, G3);
    break;
  case 5:
    tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, F2, G2, H2);
    tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, F3, G2, H2);
    tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, F2, G3, H2);
    tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, F3, G3, H2);
    tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, F2, G2, H3);
    tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, F3, G2, H3);
    tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, F2, G3, H3);
    tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, F3, G3, H3);
    break;
  default:
    throwException(_T("m_totalPieceCount=%d"), m_totalPieceCount);
  }
}

String EndGameKeyDefinition::getCodecName() const {
  return format(_T("%dMen"), m_totalPieceCount);
}

void EndGameKeyDefinition::doSelfCheck(bool checkSym) const {
  verbose(_T("Running %scodec test for keydefinition %-15s ([%s]). Indexsize:%s\n")
         ,checkSym?_T("extended "):_T("")
         ,getCodecName().cstr()
         ,toString().cstr()
         ,format1000(getIndexSize()).cstr());
  selfCheckInit();
  selfCheck();
  const UINT distinctKeyCount1 = selfCheckSummary();
  if(checkSym) {
    const UINT distinctKeyCount2 = checkSymmetries();
    if(distinctKeyCount2 != distinctKeyCount1) {
      verbose(_T("Minimal scan generated %s distinct keys, but allkeys scan generated %s distinct keys.\n")
             ,format1000(distinctKeyCount1).cstr(), format1000(distinctKeyCount2).cstr());
      pause();
    }
  }
  verbose(_T("%s\n"), spaceString(60,'_').cstr());
}

void EndGameKeyDefinition::selfCheck() const {
  EndGameKeyWithOccupiedPositions key;
  sym8PositionScanner(key, 0, true);
}

void EndGameKeyDefinition::selfCheckInit() const {
  m_minIndex       = -1;
  m_maxIndex       =  0;
  m_checkKeyCount  =  0;
  m_duplicateCount =  0;
  m_usedIndex      = new BitSet(getIndexSize());
  m_checkStartTime = getProcessTime();
}

UINT EndGameKeyDefinition::selfCheckSummary() const {
  _tprintf(_T("%-*s\n"), 140, _T(" "));
  const double usedTime           = getProcessTime() - m_checkStartTime;
  const UINT distinctKeys = m_checkKeyCount  - m_duplicateCount;

  verbose(_T("Keys checked   : %11s.\n"), format1000(m_checkKeyCount).cstr());
  verbose(_T("Distinct keys  : %11s.\n"), format1000(distinctKeys).cstr());
  verbose(_T("Minimum index  : %11s. Key:%s.\n"), format1000(m_minIndex).cstr(), indexToKey(m_minIndex).toString(*this).cstr());
  verbose(_T("Maximum index  : %11s. Key:%s.\n"), format1000(m_maxIndex).cstr(), indexToKey(m_maxIndex).toString(*this).cstr());
  verbose(_T("Indexsize()    : %11s.\n"), format1000(getIndexSize()).cstr());
  verbose(_T("Duplicate keys : %11s.\n"), format1000(m_duplicateCount).cstr());
  verbose(_T("Utilizationrate: %5.2lf%%, (%5.2lf%%)\n"), PERCENT(distinctKeys,getIndexSize()), PERCENT(distinctKeys,(m_maxIndex-m_minIndex+1)));
  verbose(_T("Used time      : %6.3lf sec. %.3lf nano sec/key.\n"), usedTime / 1000000, usedTime/m_checkKeyCount*1000);

/*
  listLongestUnusedSequence(*m_usedIndex);

  FILE *f = FOPEN(getTempFileName(toString() + _T("Unused.txt")), _T("w"));
  for(UINT i = m_minIndex; i <= m_maxIndex; i++) {
    if(!m_usedIndex->contains(i)) {
      int index1;
      String keyStr;
      try {
        const EndGameKey key = indexToKey(i);
        index1 = keyToIndex(key);
        keyStr = key.toString(*this);
      } catch(Exception e) {
        index1 = -1;
        keyStr = e.what();
      }
      _ftprintf(f, _T("%11s -> %s -> %11s"), format1000(i).cstr(), keyStr.cstr(), format1000(index1).cstr());
      if(index1 >= 0 || !m_usedIndex->contains(index1)) {
        _ftprintf(f, _T(" : not used either"));
      }
      _ftprintf(f,_T("\n"));
      fflush(f);
    }
  }
  fclose(f);
*/

  delete m_usedIndex;
  m_usedIndex = NULL;
  return distinctKeys;
}

void EndGameKeyDefinition::checkForBothPlayers(EndGameKey &key) const {
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

void EndGameKeyDefinition::checkKey(const EndGameKey &key) const {
  const unsigned long index = keyToIndex(key);
  if(index < m_minIndex) {
    m_minIndex = index;
  } else if(index > m_maxIndex) {
    m_maxIndex = index;
  }

/*
  if(index == 33939540) {
    verbose(_T("\nKey:[%s] -> index %s -> [%s]\n"), key.toString(*this).cstr(), format1000(index).cstr(), indexToKey(index).toString(*this).cstr());
    pause();
  }
*/
  if(index >= getIndexSize()) {
    verbose(_T("%s %11s:%s -> %s. Index size=%s\n")
           ,toString().cstr()
           ,format1000(m_checkKeyCount).cstr()
           ,key.toString(*this).cstr(), format1000(index).cstr(), format1000(getIndexSize()).cstr());
    const unsigned long index = keyToIndex(key);
    pause();
  } else if(m_usedIndex->contains(index)) {
    m_duplicateCount++;
    verbose(_T("Warning:index %s already used:Key:[%s]\n"), format1000(index).cstr(), key.toString(*this).cstr());
    pause();
  } else {
    m_usedIndex->add(index);
  }
  const EndGameKey k1 = indexToKey(index);
  if(!match(k1,key)) {
    verbose(_T("%s %11s:%s -> %s -> %s\n")
           ,toString().cstr()
           ,format1000(m_checkKeyCount).cstr()
           ,key.toString(*this).cstr(), format1000(index).cstr(), k1.toString(*this).cstr());
    const unsigned long index1 = keyToIndex(key);
    const EndGameKey k2 = indexToKey(index1);

    pause();
  }
  if(++m_checkKeyCount % 2000000 == 0) {
    verbose(_T("%11s. %s -> %11s -> %s [min,max]:[%s..%s]%20c")
          ,format1000(m_checkKeyCount).cstr()
          ,key.toString(*this).cstr()
          ,format1000(index).cstr()
          ,k1.toString(*this).cstr()
          ,format1000(m_minIndex).cstr()
          ,format1000(m_maxIndex).cstr()
          ,'\r'
          );
  }
}

void EndGameKeyDefinition::sym8PositionScanner(EndGameKeyWithOccupiedPositions &key, int pIndex, bool allPreviousOnDiag, PositionScanner nextScanner) const {
  if(pIndex == m_totalPieceCount) {
    checkForBothPlayers(key);
  } else {
    switch(pIndex) {
    case 0:
      { for(int i = 0; i < ARRAYSIZE(whiteKingIndexToPos); i++) {
          const int pos = whiteKingIndexToPos[i];
          key.setPosition0(pos);
          sym8PositionScanner(key, 1, IS_ONMAINDIAG1(pos), nextScanner);
          key.clearField(pos);
        }
      }
      break;
    case 1:
      { const int wkPos = key.getWhiteKingPosition();
        if(allPreviousOnDiag) { 
          for(int i = 0; i < ARRAYSIZE(subDiagIndexToPos); i++) { // for all sub- and diagonal-positions
            const int pos = subDiagIndexToPos[i];
            if(KINGSADJACENT(wkPos, pos)) {
              continue;
            }
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
            if(KINGSADJACENT(wkPos, pos)) {
              continue;
            }
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
          for(int i = 0; i < ARRAYSIZE(subDiagIndexToPos); i++) { // for all sub- and diagonal-positions
            const int pos = subDiagIndexToPos[i];
            if(key.isOccupied(pos)) {
              continue;
            }
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
            if(key.isOccupied(pos)) {
              continue;
            }
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

typedef EndGameKey (EndGameKeyDefinition::*NormalizeKeyFunction)(const EndGameKey &key, UINT *index) const;

typedef void (AllPositionScanner::*ScannerFunction)();

class AllPositionScanner {
private:
  const EndGameKeyDefinition     &m_keydef;
  const int                       m_pieceCount;
  EndGameKeyWithOccupiedPositions m_key;
  ScannerFunction                 m_scannerFunctions[MAX_ENDGAME_PIECECOUNT];
  int                             m_pIndex;
  UINT                            m_positionCount;
  UINT                            m_distinctPositionCount;
  BitSet                          m_usedIndex;

  void allPositions();
  void blackKingPositions();
  void pawnPositions();
  void setPosAndScanNext(int pos);
  void checkForBothPlayers();
  void checkSymmetry();
public:
  AllPositionScanner(const EndGameKeyDefinition &keydef);
  UINT scanAllPositions();
};

AllPositionScanner::AllPositionScanner(const EndGameKeyDefinition &keydef) 
: m_keydef(keydef)
, m_pieceCount(keydef.getPieceCount())
, m_usedIndex(keydef.getIndexSize())
{
  m_scannerFunctions[0] = allPositions;
  m_scannerFunctions[1] = blackKingPositions;
  m_scannerFunctions[2] = m_scannerFunctions[3] = m_scannerFunctions[4] = NULL;

  for(int i = 2; i < m_pieceCount; i++) {
    switch(m_keydef.getPieceType(i)) {
    case Queen :
    case Rook  :
    case Knight:
    case Bishop:
      m_scannerFunctions[i] = allPositions;
      break;
    case Pawn:
      m_scannerFunctions[i] = pawnPositions;
      break;
    default:
      throwException(_T("Unknown piecetype:%d"), m_keydef.getPieceType(i));
    }
  }
}

UINT AllPositionScanner::scanAllPositions() {
  m_positionCount         = 0;
  m_distinctPositionCount = 0;
  m_pIndex                = 0;
  verbose(_T("Checking symmetric transformation...\n"));
  (this->*(m_scannerFunctions[0]))();
  verbose(_T("All %s positions checked. %s distinct keys found.\n"), format1000(m_positionCount).cstr(), format1000(m_distinctPositionCount).cstr());
//  m_keydef.listLongestUnusedSequence(m_usedIndex);
  return m_distinctPositionCount;
}

void AllPositionScanner::setPosAndScanNext(int pos) {
  if(m_key.isOccupied(pos)) {
    return;
  }
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
  if(++m_positionCount % 1000000 == 0) {
    _tprintf(_T("Position %s\r"), format1000(m_positionCount).cstr());
  }

  EndGameKey key1 = m_keydef.getTransformedKey(m_key, m_keydef.getSymTransformation(m_key));
  UINT       index;
  EndGameKey key2 = m_keydef.getNormalizedKey(key1, &index);

  if(!m_usedIndex.contains(index)) {
    m_distinctPositionCount++;
    m_usedIndex.add(index);
  }
  if(!m_keydef.match(key1, key2)) {
    _tprintf(_T("transformedKey(%s) -> %s\n"), m_key.toString(m_keydef).cstr(), key1.toString(m_keydef).cstr());
    _tprintf(_T("normalizedKey( %s) -> %s\n"), key1.toString(m_keydef).cstr(), key2.toString(m_keydef).cstr());
    EndGameKey key3 = m_keydef.getTransformedKey(key1, m_keydef.getSymTransformation(key1));
    EndGameKey key4 = m_keydef.getNormalizedKey(key1, &index);(key2);
    _tprintf(_T("transformedKey(%s) -> %s\n"), key1.toString(m_keydef).cstr(), key3.toString(m_keydef).cstr());
    _tprintf(_T("normalizedKey( %s) -> %s\n"), key2.toString(m_keydef).cstr(), key4.toString(m_keydef).cstr());
    EndGameKey k1 = m_keydef.getTransformedKey(m_key, m_keydef.getSymTransformation(m_key));
    EndGameKey k2 = m_keydef.getNormalizedKey(key1, &index);(k1);
    bool k3 = m_keydef.match(k1, k2);
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
    if(KINGSADJACENT(wkPos, pos)) {
      continue;
    }
    setPosAndScanNext(pos);
  }
}

void AllPositionScanner::pawnPositions() {
  for(int i = 0; i < ARRAYSIZE(EndGameKeyDefinition::pawnIndexToPos); i++) {
    setPosAndScanNext(EndGameKeyDefinition::pawnIndexToPos[i]);
  }
}

UINT EndGameKeyDefinition::checkSymmetries() const {
  AllPositionScanner scanner(*this);
  return scanner.scanAllPositions();
}

class UnusedSequence {
private:
  static String genKeyString(const EndGameKeyDefinition *keydef, unsigned long index);
public:
  const unsigned long m_from, m_to;
  unsigned long getLength() const {
    return m_to - m_from + 1;
  }
  UnusedSequence(unsigned long from, unsigned long to) : m_from(from), m_to(to) {
  }

  String toString(const EndGameKeyDefinition *keydef) const;
};

String UnusedSequence::genKeyString(const EndGameKeyDefinition *keydef, unsigned long index) {
  try {
    return keydef->indexToKey(index).toString(*keydef);
  } catch(Exception e) {
    return format(_T("Invalid index:%lu:%s"), index, e.what());
  }
}

String UnusedSequence::toString(const EndGameKeyDefinition *keydef) const {
  return format(_T("%11s-%11s. length:%9s : ]%s - %s[=[%s - %s]")
               ,format1000(m_from).cstr()
               ,format1000(m_to).cstr()
               ,format1000(getLength()).cstr()
               ,genKeyString(keydef, m_from - 1).cstr()
               ,genKeyString(keydef, m_to   + 1).cstr()
               ,genKeyString(keydef, m_from    ).cstr()
               ,genKeyString(keydef, m_to      ).cstr()
               );
}

class UnusedSequenceComparator : public Comparator<UnusedSequence> {
public:
  int compare(const UnusedSequence &e1, const UnusedSequence &e2) {
    return e2.getLength() - e1.getLength();
  }
  AbstractComparator *clone() const {
    return new UnusedSequenceComparator();
  }
};

void EndGameKeyDefinition::listLongestUnusedSequence(BitSet &s, UINT sequenceMinSize) const {
  PriorityQueue<UnusedSequence> ptQueue(UnusedSequenceComparator());
  long last = -1;
  for(Iterator<UINT> it = s.getIterator(); it.hasNext();) {
    const int i = it.next();
    if(i - last > sequenceMinSize) {
      ptQueue.add(UnusedSequence(last+1, i-1));
    }
    last = i;
  }
  if(s.getCapacity()-1 - last > sequenceMinSize) {
    ptQueue.add(UnusedSequence(last+1, s.getCapacity()-1));
  }

  FILE *f = MKFOPEN(getTempFileName(toString() + _T("UnusedSequences.txt")), _T("w"));
  for(int i = 0; !ptQueue.isEmpty(); i++) {
    const UnusedSequence us = ptQueue.remove();
    _ftprintf(f, _T("%s\n"), us.toString(this).cstr());
  }
  UINT totalUnused = s.getCapacity() - s.size();
  _ftprintf(f, _T("Total unused indices:%s\n"), format1000(totalUnused).cstr());
  fclose(f);
}

#endif
