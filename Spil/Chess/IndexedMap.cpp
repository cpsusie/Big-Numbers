#include "stdafx.h"
#include <ByteFile.h>
#include <CountedByteStream.h>
#include <BitStream.h>
#include <TimeEstimator.h>
#include "IndexedMap.h"

#ifndef NEWCOMPRESSION

#define WRITEMOVEBITS(bitCount, shift)                                                                \
  { forEachPlayer(p) {                                                                                \
      for(ep = &first()+(int)p; ep <= lastElement; ep += 2) {                                         \
        if(ep->isWinner()) {                                                                          \
          bs.putBits(ep->getMovesToEnd()>>(shift), bitCount);                                         \
        }                                                                                             \
      }                                                                                               \
    }                                                                                                 \
  }

#define READMOVEBITS(op, bitCount, shift)                                                             \
  { forEachPlayer(p) {                                                                                \
      int arrayIndex = 0;                                                                             \
      for(Iterator<size_t> it = winnerPositionSet.getIterator(); it.hasNext(); arrayIndex++) {        \
        if((it.next()&1) == (int)p) {                                                                 \
          BYTE bits;                                                                                  \
          if(bits = (BYTE)bs.getBits(bitCount)) {                                                     \
            positionInfoArray.op(arrayIndex, bits<<(shift));                                          \
          }                                                                                           \
        }                                                                                             \
      }                                                                                               \
    }                                                                                                 \
  }

#endif // NEWCOMPRESSION

#ifdef TABLEBASE_BUILDER

#ifdef _DEBUG

EndGamePosIndex IndexedMap::getCheckedIndex(EndGameKey key) const {
  const EndGamePosIndex result = m_keydef.keyToIndex(key);
  if(result >= m_indexSize) {
    EndGamePosIndex ii = m_keydef.keyToIndex(key);
    throwException(_T("Position [%s] has index %llu. Max=%llu")
                  ,key.toString(m_keydef).cstr(), result, m_indexSize-1);
  }
  return result;
}

#define GETELEMENT(key) (*this)[(size_t)getCheckedIndex(key)]

#else // _DEBUG

#define GETELEMENT(key) (*this)[(size_t)m_keydef.keyToIndex(key)]

#endif // _DEBUG

void IndexedMap::rethrowException(Exception &e, EndGameKey key) const {
  throwException(_T("%s. key:[%s], index:%s, size:%s\n")
                ,e.what()
                ,key.toString(m_keydef).cstr()
                ,format1000(m_keydef.keyToIndex(key)).cstr()
                ,format1000(size()).cstr());
}


IndexedMap::IndexedMap(const EndGameKeyDefinition &keydef)
: m_keydef(keydef)
, m_indexSize(keydef.getIndexSize())
{
}

void IndexedMap::allocate() {
  clear();
  setCapacity((size_t)m_indexSize);
  add(0,EndGameResult(), (size_t)m_indexSize);
}

const EndGameResult &IndexedMap::get(EndGameKey key) const {
  try {
    if(size() == 0) {
      throwException(_T("Index not loaded"));
    }
    return GETELEMENT(key);
  } catch(Exception e) {
    rethrowException(e, key);
    return first();
  }
}

EndGameResult &IndexedMap::get(EndGameKey key) {
  try {
    if(size() == 0) {
      allocate();
    }
    return GETELEMENT(key);
  } catch(Exception e) {
    rethrowException(e, key);
    return first();
  }
}

bool IndexedMap::remove(EndGameKey key) {
  if(size() == 0) {
    return false;
  }
  try {
    EndGameResult &e = GETELEMENT(key);
    if(!e.exists()) {
      return false;
    }
    e.clearExist();
    return true;
  } catch(Exception e) {
    rethrowException(e, key);
    return false;
  }
}

IndexedMap &IndexedMap::clearAllVisited() {
  if(size()) {
    const EndGameResult *end = &last();
    for(EndGameResult *ep = &first(); ep <= end;) {
      (ep++)->clearVisited();
    }
  }
  return *this;
}

IndexedMap &IndexedMap::clearAllChanged() {
  if(size()) {
    const EndGameResult *end = &last();
    for(EndGameResult *ep = &first(); ep <= end;) {
      (ep++)->clearChanged();
    }
  }
  return *this;
}

IndexedMap &IndexedMap::clearAllMarked() {
  if(size()) {
    const EndGameResult *end = &last();
    for(EndGameResult *ep = &first(); ep <= end;) {
      (ep++)->clearMark();
    }
  }
  return *this;
}

IndexedMap &IndexedMap::clearHelpInfo() {
  if(size()) {
    const EndGameResult *end = &last();
    for(EndGameResult *ep = &first(); ep <= end;) {
      (ep++)->clearHelpInfo();
    }
  }
  return *this;
}

IndexedMap &IndexedMap::markAllChanged() {
  if(size()) {
    const EndGameResult *end = &last();
    for(EndGameResult *ep = &first(); ep <= end; ep++) {
      if(ep->isChanged()) {
        ep->setMark();
      }
    }
  }
  return *this;
}

IndexedMap &IndexedMap::markAllVisited() {
  if(size()) {
    const EndGameResult *end = &last();
    for(EndGameResult *ep = &first(); ep <= end;ep++) {
      if(ep->isVisited()) {
        ep->setMark();
      }
    }
  }
  return *this;
}

void IndexedMap::convertIndex() {
  IndexedMap copy(m_keydef);
  copy.allocate();
  for(EndGameEntryIterator it = getEntryIterator(); it.hasNext();) {
    const EndGameEntry &e = it.next();
    EndGamePosIndex newIndex = m_keydef.keyToIndexNew(e.getKey());
    if(newIndex >= m_indexSize) {
      throwException(_T("Key [%s] gives wrong index=%s. max=%s")
                    ,e.getKey().toString(m_keydef).cstr()
                    ,format1000(newIndex).cstr()
                    ,format1000(m_indexSize-1).cstr()
                    );
    }
    if(copy[newIndex].exists()) {
      throwException(_T("Key [%s] already converted"), e.getKey().toString(m_keydef).cstr());
    }
    copy[newIndex] = e.getValue();
  }
  ((CompactArray<EndGameResult>&)*this) = copy;
}

#ifndef NEWCOMPRESSION

void IndexedMap::saveCompressed(ByteOutputStream &s, const TablebaseInfo &info) const { // Saved in format used by IndexedMap::load(ByteInputStream &s)
                                                                                        // defined last in this file
  DEFINEMETHODNAME;
  const EndGamePosIndex indexSize = m_keydef.getIndexSize();
  if(info.m_indexCapacity != indexSize) {
    throwInvalidArgumentException(method, _T("info.indexCapacity=%I64u, keydef.indexSize=%I64u"), info.m_indexCapacity, indexSize);
  }
  const EndGameResult *ep          = &first();
  const EndGameResult *lastElement = getLastElement();
  unsigned char        canWinFlags = 0;
  for(INT64 i = indexSize; (canWinFlags != BOTHCANWIN) && i--; ep++) {
    switch(ep->getStatus()) {
    case EG_WHITEWIN: canWinFlags |= WHITECANWIN; break;
    case EG_BLACKWIN: canWinFlags |= BLACKCANWIN; break;
    default         :;
    }
  }

  if((info.getCanWinFlags() != canWinFlags) || (canWinFlags == 0)) {
    throwInvalidArgumentException(method, _T("CanWinFlags inconsistent. info.canWinFlags:%d, counted:%d")
                                 ,info.getCanWinFlags()
                                 ,canWinFlags
                                 );
  }

  UINT  maxPly          = 0;
  INT64 winnerPositions = 0;
  INT64 i;
  for(ep = &first(), i = indexSize; i--; ep++) {
    if(ep->isWinner()) {
      winnerPositions++;
      if(ep->getPliesToEnd() > maxPly) {
        maxPly = ep->getPliesToEnd();
      }
    }
  }

  if(winnerPositions != info.getWinnerPositionCount().getTotal()) {
    throwException(_T("Counted winnerPositions=%I64d != info.totalWinnerPosition:%s (=%I64u)")
                  ,winnerPositions
                  ,info.getWinnerPositionCount().toString().cstr()
                  ,info.getWinnerPositionCount().getTotal());
  }

  if(maxPly != info.m_maxPlies.getMax()) {
    throwInvalidArgumentException(method
                                 ,_T("Calculated maxPly=%u != info.maxPlies:%s")
                                 ,maxPly
                                 ,info.m_maxPlies.toString().cstr());
  }

  const int  maxMoves    = PLIESTOMOVES(maxPly);
  const BYTE bitsPerMove = getBitCount(maxMoves);

  BitOutputStream bs(s);

  for(ep = &first(), i = indexSize; i--; ep++) {
    bs.putBit(ep->isWinner() ? 1 : 0);
  }

  if(canWinFlags == BOTHCANWIN) { // No need for indicating who wins. A winposition is the one indicated by canWinFlags
    for(ep = &first(), i = indexSize; i--; ep++) {
      switch(ep->getStatus()) {
      case EG_WHITEWIN:
        bs.putBit(0);
        break;
      case EG_BLACKWIN:
        bs.putBit(1);
        break;
      default         :
        break;
      }
    }
  }

  switch(bitsPerMove) {
  case 1:
    WRITEMOVEBITS(1,0);
    break;
  case 2:
    WRITEMOVEBITS(2,0);
    break;
  case 3:
    WRITEMOVEBITS(2,0);
    WRITEMOVEBITS(1,2);
    break;
  default: // 4, 5, 6, 7, 8
    WRITEMOVEBITS(4,0);
    break;
  }

  switch(bitsPerMove) {
  case 5:
    WRITEMOVEBITS(1,4);
    break;
  case 6:
    WRITEMOVEBITS(2,4);
    break;
  case 7:
    WRITEMOVEBITS(2,4);
    WRITEMOVEBITS(1,6);
    break;
  case 8:
    WRITEMOVEBITS(4,4);
    break;
  }
}

#else // NEWCOMPRESSION

PositionTypeCounters IndexedMap::countWinnerPositionTypes() {
  PositionTypeCounters result;
  for(EndGameEntryIterator it = getIteratorAllWinnerEntries(); it.hasNext();) {
    result.updateCount(it.next());
  }
  return result;
}

void IndexedMap::pruneAllPliesByNP(NotPrunedFlags np) {
  const Player prunePlayer  = GETENEMY(NPGETPLAYER(np));
  const BYTE   pruneParity  = 1-NPGETPARITY(np);
  UINT64       prunedCount  = 0;
  UINT64       winnerCount  = 0;
  for(EndGameEntryIterator it = getIteratorAllWinnerEntries(); it.hasNext(); ) {
    EndGameEntry &e = it.next();
    winnerCount++;
    if  ((e.getKey().getPlayerInTurn()      == prunePlayer)
     || ((e.getValue().getMovesToEnd() & 1) == pruneParity))
      {
        e.getValue().prunePliesToEnd();
        prunedCount++;
      }
  }
  const UINT64 notPrunedCount = winnerCount - prunedCount;
  verbose(_T("Pruned positions:%14s (=%.2lf%%)  Not pruned:%14s (=%.2lf%%)\n")
         ,format1000(prunedCount).cstr()
         ,PERCENT(prunedCount, winnerCount)
         ,format1000(notPrunedCount).cstr()
         ,PERCENT(notPrunedCount, winnerCount)
         );
}

void IndexedMap::saveCompressed(BigEndianOutputStream &out, const TablebaseInfo &info) { // Saved in format used by IndexedMap::load(ByteInputStream &s)
  DEFINEMETHODNAME;                                                               // defined last in this file
  const EndGamePosIndex indexSize = m_keydef.getIndexSize();
  if(info.m_indexCapacity != indexSize) {
    throwInvalidArgumentException(method, _T("info.indexCapacity=%I64u, keydef.indexSize=%I64u"), info.m_indexCapacity, indexSize);
  }
  const EndGameResult *firstElement = &first();
  const EndGameResult *lastElement  = getLastElement();
  BYTE                 canWinFlags  = 0;
  for(const EndGameResult *ep = firstElement; (canWinFlags != BOTHCANWIN) && ep <= lastElement;) {
    switch((ep++)->getStatus()) {
    case EG_WHITEWIN: canWinFlags |= WHITECANWIN; break;
    case EG_BLACKWIN: canWinFlags |= BLACKCANWIN; break;
    default         :;
    }
  }

  if((info.getCanWinFlags() != canWinFlags) || (canWinFlags == 0)) {
    throwInvalidArgumentException(method, _T("CanWinFlags inconsistent. info.canWinFlags:%d, counted:%d")
                                 ,info.getCanWinFlags()
                                 ,canWinFlags
                                 );
  }

  UINT   maxPly          = 0;
  INT64  wpCount = 0;
  BitSet wpSet(indexSize);

  for(const EndGameResult *ep = firstElement; ep <= lastElement; ep++) {
    if(ep->isWinner()) {
      wpSet.add(ep - firstElement);
      wpCount++;
      if(ep->getPliesToEnd() > maxPly) {
        maxPly = ep->getPliesToEnd();
      }
    }
  }

  if(wpCount != info.getWinnerPositionCount().getTotal()) {
    throwException(_T("Counted winnerPositions=%I64d != info.totalWinnerPosition:%s (=%I64u)")
                  ,wpCount
                  ,info.getWinnerPositionCount().toString().cstr()
                  ,info.getWinnerPositionCount().getTotal());
  }

  if(maxPly != info.m_maxPlies.getMax()) {
    throwInvalidArgumentException(method
                                 ,_T("Calculated maxPly=%u != info.maxPlies:%s")
                                 ,maxPly
                                 ,info.m_maxPlies.toString().cstr());
  }

  out << wpSet;
  verbose(_T("wpSet.(capacity,size):(%14s,%14s)\n")
          ,format1000(wpSet.getCapacity()).cstr()
          ,format1000(wpSet.size()).cstr());

  wpSet.clear(); // save Space
  if(canWinFlags == BOTHCANWIN) { // No need for indicating who wins. A winposition is the one indicated by canWinFlags
    UINT64 count = 0;
    BitSet whoWinSet(wpCount);
    for(const EndGameResult *ep = firstElement;  ep <= lastElement; ep++) {
      switch(ep->getStatus()) {
      case EG_WHITEWIN:
        count++;                // 0-bit
        break;
      case EG_BLACKWIN:
        whoWinSet.add(count++); // 1-bit
        break;
      default         :         // do nothing
        break;
      }
    }
    out << whoWinSet;
    verbose(_T("whoWinSet.(capacity,size):(%14s,%14s)\n")
           ,format1000(whoWinSet.getCapacity()).cstr()
           ,format1000(whoWinSet.size()).cstr());
  }

  pruneAllPliesByNP(info.getNPFlags());

  BitSet npSet(wpCount);
  INT64  npCount = 0;
  for(const EndGameResult *ep = firstElement; ep <= lastElement; ep++) {
    if(ep->isWinner()) {
      if(ep->getPliesToEnd()) npSet.add(npCount);
      npCount++;
    }
  }
  out << npSet;
  verbose(_T("npSet.(capacity,size):(%14s,%14s)\n")
         ,format1000(npSet.getCapacity()).cstr()
         ,format1000(npSet.size()).cstr());


#define BUFFERSIZE 4096
  ByteArray buffer(BUFFERSIZE);
  UINT64 pliesByteCount = 0;
  for(const EndGameResult *ep = firstElement; ep <= lastElement; ep++) {
    if(ep->isWinner() && ep->getPliesToEnd()) {
      buffer.append(BYTE(ep->getMovesToEnd()>>1));
      if (buffer.size() == BUFFERSIZE) {
        out.putBytes(buffer.getData(), BUFFERSIZE);
        pliesByteCount += BUFFERSIZE;
        buffer.clear(-1);
      }
    }
  }
  if (!buffer.isEmpty()) {
    out.putBytes(buffer.getData(), buffer.size());
    pliesByteCount += buffer.size();
    buffer.clear();
  }
  verbose(_T("plyBytes:%14s\n"), format1000(pliesByteCount).cstr());
}

#endif // NEWCOMPRESSION

// -------------------------------------------- Iterators --------------------------------------------

class IndexedMapKeyIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  IndexedMap                 &m_map;
  const EndGameKeyDefinition &m_keydef;
  EndGameResult              *m_firstElement, *m_lastElement;
  EndGameResult              *m_current;
  EndGameKey                  m_returnKey;
  void first();
public:
  IndexedMapKeyIterator(IndexedMap &map);
  AbstractIterator *clone() {
    return new IndexedMapKeyIterator(*this);
  }
  bool hasNext() const {
    return m_current <= m_lastElement;
  }
  void *next();
  void remove();
};

DEFINECLASSNAME(IndexedMapKeyIterator);

IndexedMapKeyIterator::IndexedMapKeyIterator(IndexedMap &map)
: m_map(map)
, m_keydef(map.getKeyDefinition())
, m_firstElement((EndGameResult*)map.getFirstElement())
, m_lastElement( (EndGameResult*)map.getLastElement())
{
  first();
}

void IndexedMapKeyIterator::first() {
  for(m_current = m_firstElement; m_current <= m_lastElement; m_current++) {
    if(m_current->exists()) {
      return;
    }
  }
}

void *IndexedMapKeyIterator::next() {
  if(!hasNext()) {
    noNextElementError(s_className);
  }
  m_returnKey = m_keydef.indexToKey((m_current++) - m_firstElement);
  while(m_current <= m_lastElement && !m_current->exists()) {
    m_current++;
  }
  return &m_returnKey;
}

void IndexedMapKeyIterator::remove() {
  unsupportedOperationError(__TFUNCTION__);
}

class IndexedMapEntry : public AbstractEntry {
private:
  const EndGameKeyDefinition &m_keydef;
  const EndGameResult        *m_firstElement;
public:
  EndGameResult              *m_result;
  mutable EndGameKey          m_key;

  IndexedMapEntry(IndexedMap &map) : m_keydef(map.getKeyDefinition()), m_firstElement(map.getFirstElement()) {
  }
  const void *key() const;

  void *value() {
    return m_result;
  }

  const void *value() const {
    return m_result;
  }
};

const void *IndexedMapEntry::key() const {
  if(m_key.isEmpty()) {
    m_key = m_keydef.indexToKey(m_result - m_firstElement); // evaluate lazy
  }
  return &m_key;
}

// ---------------------------IndexedMapEntryIterator------------------------------------------------------------------

class IndexedMapEntryIterator;

class IndexedMapEntryIterator : public AbstractIterator, public ProgressProvider {
private:
  IndexedMapEntryIterator &operator=(const IndexedMapEntryIterator &src); // not implemented
protected:
  EndGameResult       *m_firstElement, *m_lastElement;
  EndGameResult       *m_current;
  IndexedMapEntry      m_entry;
  const UINT64         m_length;
  const Timestamp      m_startTime;
  TimeEstimator       *m_timeEstimator;
  bool                 m_odd;
  IndexedMapEntryIterator(const IndexedMapEntryIterator &src);
public:
  IndexedMapEntryIterator(IndexedMap &map);
  ~IndexedMapEntryIterator();
  bool hasNext() const {
    return m_current <= m_lastElement;
  }
  void remove() {
    unsupportedOperationError(__TFUNCTION__);
  }
  virtual UINT64 getCount() const = 0;

  inline double getPercentDone() const {
    return PERCENT(m_current - m_firstElement, m_length) + (m_odd?0:50.0);
  }

  inline double getMilliSecondsUsed() const {
    return diff(m_startTime, Timestamp(), TMILLISECOND);
  }

  inline double getMilliSecondsRemaining() const {
    return m_timeEstimator->getMilliSecondsRemaining();
  }
};

IndexedMapEntryIterator::IndexedMapEntryIterator(IndexedMap &map)
: m_entry(       map                                  )
, m_firstElement((EndGameResult*)map.getFirstElement())
, m_lastElement( (EndGameResult*)map.getLastElement() )
, m_length(      map.size()*2                         )
, m_odd(         true                                 )
{
  m_current = m_firstElement+1;
  m_timeEstimator = new TimeEstimator(*this);
  for(;; m_odd = false, m_current = m_firstElement) {
    for(; m_current <= m_lastElement; m_current+=2) {
      if(m_current->exists()) return;
    }
    if(!m_odd) break;
  }
}

IndexedMapEntryIterator::IndexedMapEntryIterator(const IndexedMapEntryIterator &src)
: m_entry(       src.m_entry       )
, m_firstElement(src.m_firstElement)
, m_lastElement( src.m_lastElement )
, m_current(     src.m_current     )
, m_length(      src.m_length      )
, m_startTime(   src.m_startTime   )
, m_odd(         src.m_odd         )
{
  m_timeEstimator = new TimeEstimator(*this);
}

IndexedMapEntryIterator::~IndexedMapEntryIterator() {
  delete m_timeEstimator;
}

UINT64 EndGameEntryIterator::getCount() {
  return ((IndexedMapEntryIterator*)(m_it))->getCount();
}

double EndGameEntryIterator::getPercentDone() const {
  return ((IndexedMapEntryIterator*)(m_it))->getPercentDone();
}

double EndGameEntryIterator::getMilliSecondsRemaining() const {
  return ((IndexedMapEntryIterator*)(m_it))->getMilliSecondsRemaining();
}

double EndGameEntryIterator::getMilliSecondsUsed() const {
  return ((IndexedMapEntryIterator*)(m_it))->getMilliSecondsUsed();
}

void EndGameEntryIterator::setLog(bool log) {
  if(m_logFile) {
    fclose(m_logFile);
    m_logFile = NULL;
  }
  if(log) {
    const String name = getTempFileName(format(_T("iterator%s.log"), Timestamp().toString(_T("yyyyMMddhhmmss")).cstr()));
    m_logFile = mkfopen(name, _T("w"));
  }
}

String EndGameEntryIterator::getProgressStr() const {
  if(m_logFile) {
    _ftprintf(m_logFile, _T("%.1lf %.2lf\n"), getMilliSecondsUsed()/1000.0, getPercentDone());
  }
  if(hasNext()) {
    return format(_T("%6.2lf%% %-10s"), getPercentDone(), secondsToString(getMilliSecondsRemaining(), false).cstr());
  } else {
    return format(_T("100.00%% %-10s"),                   secondsToString(getMilliSecondsUsed()     , false).cstr());
  }
}

// ---------------------------Generic filtered entryiterator -----------------------------------------------------------

#define DEFINE_FILTERED_ENTRYITERATOR(name, filter)                                       \
class name : public IndexedMapEntryIterator {                                             \
private:                                                                                  \
  const int m_value;                                                                      \
public:                                                                                   \
  name(IndexedMap &map, int value=0);                                                     \
  AbstractIterator *clone() { return new name(*this); }                                   \
  void *next();                                                                           \
  UINT64 getCount() const;                                                                \
};                                                                                        \
                                                                                          \
name::name(IndexedMap &map, int value) : IndexedMapEntryIterator(map), m_value(value) {   \
  for(;; m_odd = false, m_current = m_firstElement) {                                     \
    for(; m_current <= m_lastElement; m_current+=2) {                                     \
      if(filter(m_current)) return;                                                       \
    }                                                                                     \
    if(!m_odd) break;                                                                     \
  }                                                                                       \
}                                                                                         \
                                                                                          \
void *name::next() {                                                                      \
  assert(hasNext());                                                                      \
  m_entry.m_key.clear();                                                                  \
  m_entry.m_result = m_current;                                                           \
  m_current+=2;                                                                           \
  for(;;m_odd = false, m_current = m_firstElement) {                                      \
    for(; m_current <= m_lastElement; m_current += 2) {                                   \
      if(filter(m_current)) return &m_entry;                                              \
    }                                                                                     \
    if(!m_odd) break;                                                                     \
  }                                                                                       \
  return &m_entry;                                                                        \
}                                                                                         \
                                                                                          \
UINT64 name::getCount() const {                                                           \
  UINT64 count = 0;                                                                       \
  for(const EndGameResult *p = m_firstElement; p <= m_lastElement; p++) {                 \
    if(filter(p)) count++;                                                                \
  }                                                                                       \
  return count;                                                                           \
}


#define EXIST_FILTER(              p) (p->exists()                           )
#define UNDEFINED_FILTER(          p) (p->exists()         && !p->isDefined())
#define VISITED_FILTER(            p) (p->exists()         &&  p->isVisited())
#define CHANGED_FILTER(            p) (p->exists()         &&  p->isChanged())
#define UNVISITED_FILTER(          p) (p->exists()         && !p->isVisited())
#define UNDEFINED_UNVISITED_FILTER(p) (UNDEFINED_FILTER(p) && !p->isVisited())
#define CHANGED_UNVISITED_FILTER(  p) (UNVISITED_FILTER(p) &&  p->isChanged())
#define MARKED_FILTER(             p) (p->isMarked()                         )
#define UNMARKED_FILTER(           p) (p->exists()         && !p->isMarked() )
#define ALLWINNER_FILTER(          p) (p->isWinner()                         )
#define WHITEWIN_FILTER(           p) (p->getStatus()      == EG_WHITEWIN    )
#define BLACKWIN_FILTER(           p) (p->getStatus()      == EG_BLACKWIN    )
#define ALLNONWINNER_FILTER(       p) (p->exists()         && !p->isWinner() )
#define WIN_INPLIES_FILTER(        p) (ALLWINNER_FILTER(p) && (p->getPliesToEnd() == m_value ))
#define STALEMATE_FILTER(          p) ((p->getStatus()     == EG_DRAW) && (p->getPliesToEnd() == 0))
#define WHITEWIN_MINPLIES_FILTER(  p) (WHITEWIN_FILTER(p)  && ((int)p->getPliesToEnd() >= m_value ))
#define BLACKWIN_MINPLIES_FILTER(  p) (BLACKWIN_FILTER(p)  && ((int)p->getPliesToEnd() >= m_value ))
#define WHITEWIN_MAXPLIES_FILTER(  p) (WHITEWIN_FILTER(p)  && ((int)p->getPliesToEnd() <= m_value ))
#define BLACKWIN_MAXPLIES_FILTER(  p) (BLACKWIN_FILTER(p)  && ((int)p->getPliesToEnd() <= m_value ))
#define WHITEWIN_EXACTPLIES_FILTER(p) (WHITEWIN_FILTER(p)  && ((int)p->getPliesToEnd() == m_value ))
#define BLACKWIN_EXACTPLIES_FILTER(p) (BLACKWIN_FILTER(p)  && ((int)p->getPliesToEnd() == m_value ))
#define NONEMPTY_HELPINFO_FILTER(  p) (p->hasHelpInfo())

DEFINE_FILTERED_ENTRYITERATOR(IteratorAllEntries               , EXIST_FILTER               )
DEFINE_FILTERED_ENTRYITERATOR(IteratorUndefinedEntries         , UNDEFINED_FILTER           )
DEFINE_FILTERED_ENTRYITERATOR(IteratorVisitedEntries           , VISITED_FILTER             )
DEFINE_FILTERED_ENTRYITERATOR(IteratorChangedEntries           , CHANGED_FILTER             )
DEFINE_FILTERED_ENTRYITERATOR(IteratorUnvisitedEntries         , UNVISITED_FILTER           )
DEFINE_FILTERED_ENTRYITERATOR(IteratorUndefinedUnvisitedEntries, UNDEFINED_UNVISITED_FILTER )
DEFINE_FILTERED_ENTRYITERATOR(IteratorChangedUnvisitedEntries  , CHANGED_UNVISITED_FILTER   )
DEFINE_FILTERED_ENTRYITERATOR(IteratorMarkedEntries            , MARKED_FILTER              )
DEFINE_FILTERED_ENTRYITERATOR(IteratorUnmarkedEntries          , UNMARKED_FILTER            )
DEFINE_FILTERED_ENTRYITERATOR(IteratorAllWinnerEntries         , ALLWINNER_FILTER           )
DEFINE_FILTERED_ENTRYITERATOR(IteratorWinnerEntries            , WIN_INPLIES_FILTER         )
DEFINE_FILTERED_ENTRYITERATOR(IteratorAllNonWinnerEntries      , ALLNONWINNER_FILTER        )
DEFINE_FILTERED_ENTRYITERATOR(IteratorStalemateEntries         , STALEMATE_FILTER           )
DEFINE_FILTERED_ENTRYITERATOR(IteratorWhiteWinMinPliesToEnd    , WHITEWIN_MINPLIES_FILTER   )
DEFINE_FILTERED_ENTRYITERATOR(IteratorBlackWinMinPliesToEnd    , BLACKWIN_MINPLIES_FILTER   )
DEFINE_FILTERED_ENTRYITERATOR(IteratorWhiteWinMaxPliesToEnd    , WHITEWIN_MAXPLIES_FILTER   )
DEFINE_FILTERED_ENTRYITERATOR(IteratorBlackWinMaxPliesToEnd    , BLACKWIN_MAXPLIES_FILTER   )
DEFINE_FILTERED_ENTRYITERATOR(IteratorWhiteWinExactPliesToEnd  , WHITEWIN_EXACTPLIES_FILTER )
DEFINE_FILTERED_ENTRYITERATOR(IteratorBlackWinExactPliesToEnd  , BLACKWIN_EXACTPLIES_FILTER )
DEFINE_FILTERED_ENTRYITERATOR(IteratorNonEmptyHelpInfo         , NONEMPTY_HELPINFO_FILTER   )

EndGameKeyIterator IndexedMap::getKeyIterator() {
  return EndGameKeyIterator(new IndexedMapKeyIterator(*this));
}

EndGameEntryIterator IndexedMap::getEntryIterator() {
  return EndGameEntryIterator(new IteratorAllEntries(*this));
}

EndGameEntryIterator IndexedMap::getIteratorUndefinedEntries() {
  return EndGameEntryIterator(new IteratorUndefinedEntries(*this));
}

EndGameEntryIterator IndexedMap::getIteratorVisitedEntries() {
  return EndGameEntryIterator(new IteratorVisitedEntries(*this));
}

EndGameEntryIterator IndexedMap::getIteratorUnvisitedEntries() {
  return EndGameEntryIterator(new IteratorUnvisitedEntries(*this));
}

EndGameEntryIterator IndexedMap::getIteratorUndefinedUnvisitedEntries() {
  return EndGameEntryIterator(new IteratorUndefinedUnvisitedEntries(*this));
}

EndGameEntryIterator IndexedMap::getIteratorChangedEntries() {
  return EndGameEntryIterator(new IteratorChangedEntries(*this));
}

EndGameEntryIterator IndexedMap::getIteratorChangedUnvisitedEntries() {
  return EndGameEntryIterator(new IteratorChangedUnvisitedEntries(*this));
}

EndGameEntryIterator IndexedMap::getIteratorMarkedEntries() {
  return EndGameEntryIterator(new IteratorMarkedEntries(*this));
}

EndGameEntryIterator IndexedMap::getIteratorUnmarkedEntries() {
  return EndGameEntryIterator(new IteratorUnmarkedEntries(*this));
}

EndGameEntryIterator IndexedMap::getIteratorWinnerEntries(UINT pliesToEnd) {
  return EndGameEntryIterator(new IteratorWinnerEntries(*this, pliesToEnd));
}

EndGameEntryIterator IndexedMap::getIteratorAllWinnerEntries() {
  return EndGameEntryIterator(new IteratorAllWinnerEntries(*this));
}

EndGameEntryIterator IndexedMap::getIteratorAllNonWinnerEntries() {
  return EndGameEntryIterator(new IteratorAllNonWinnerEntries(*this));
}


EndGameEntryIterator IndexedMap::getIteratorCheckmateEntries(Player winner) {
  return EndGameEntryIterator((winner == WHITEPLAYER)
                              ? (AbstractIterator*)new IteratorWhiteWinMaxPliesToEnd(*this, 0)
                              : (AbstractIterator*)new IteratorBlackWinMaxPliesToEnd(*this, 0)
                             );
}

EndGameEntryIterator IndexedMap::getIteratorStalemateEntries() {
  return EndGameEntryIterator(new IteratorStalemateEntries(*this));
}

EndGameEntryIterator IndexedMap::getIteratorMinPliesToEnd(Player winner, UINT minPliesToEnd) {
  return EndGameEntryIterator((winner == WHITEPLAYER)
                              ? (AbstractIterator*)new IteratorWhiteWinMinPliesToEnd(*this, minPliesToEnd)
                              : (AbstractIterator*)new IteratorBlackWinMinPliesToEnd(*this, minPliesToEnd)
                             );
}

EndGameEntryIterator IndexedMap::getIteratorExactPliesToEnd(Player winner, UINT pliesToEnd) {
  return EndGameEntryIterator((winner == WHITEPLAYER)
                              ? (AbstractIterator*)new IteratorWhiteWinExactPliesToEnd(*this, pliesToEnd)
                              : (AbstractIterator*)new IteratorBlackWinExactPliesToEnd(*this, pliesToEnd)
                             );
}

EndGameEntryIterator IndexedMap::getIteratorNonEmptyHelpInfo() {
  return EndGameEntryIterator(new IteratorNonEmptyHelpInfo(*this));
}

#else // !TABLEBASE_BUILDER ie chess-program

#ifndef NEWCOMPRESSION

IndexedMap::IndexedMap(const EndGameKeyDefinition &keydef) : m_keydef(keydef) {
  m_positionIndex       = NULL;
  m_infoArray           = NULL;
}

IndexedMap::~IndexedMap() {
  clear();
}

void IndexedMap::clear() {
  if(m_positionIndex) {
    delete m_positionIndex;
    m_positionIndex = NULL;
  }
  if(m_infoArray) {
    delete m_infoArray;
    m_infoArray = NULL;
  }
}

class DecompressedHeader : public TablebaseInfo {
public:
  BYTE m_bitsPerEntry;
  UINT m_bitSetIndexOffset;
  UINT m_arrayStartOffset;
  DecompressedHeader(const TablebaseInfo &info);
  DecompressedHeader(const String        &fileName);
  void save(ByteOutputStream &s);
  void load(ByteInputStream  &s);
};

DecompressedHeader::DecompressedHeader(const TablebaseInfo &info) : TablebaseInfo(info) {
  m_bitsPerEntry      = 0;
  m_bitSetIndexOffset = 0;
  m_arrayStartOffset  = 0;
}

DecompressedHeader::DecompressedHeader(const String &fileName) {
  load(ByteInputFile(fileName));
}

void DecompressedHeader::save(ByteOutputStream &s) {
  s.putBytes((BYTE*)this, sizeof(DecompressedHeader));
}

void DecompressedHeader::load(ByteInputStream &s) {
  s.getBytesForced((BYTE*)this, sizeof(DecompressedHeader));
}

// Read format written with IndexedMap::saveCompressed(ByteOutputStream &s);
void IndexedMap::decompress(ByteInputStream &s, const TablebaseInfo &info) const {
  const INT64 indexSize = m_keydef.getIndexSize();
  if(info.m_indexCapacity != indexSize) {
    throwInvalidArgumentException(__TFUNCTION__, _T("info.indexCapacity=%s, keydef.indexSize=%s")
                                 ,format1000(info.m_indexCapacity).cstr()
                                 ,format1000(indexSize).cstr());
  }

  const BYTE  canWinFlags  = info.getCanWinFlags();
  const int   maxPly       = info.m_maxPlies.getMax();
  const int   maxMoves     = PLIESTOMOVES(maxPly);
  const BYTE  bitsPerMove  = getBitCount(maxMoves);
  const BYTE  bitsPerEntry = bitsPerMove+((canWinFlags == BOTHCANWIN)?1:0);
  // maybe need a bit to white/black win, which will be the first bit for every entry

  BitInputStream bs(s);

  BitSet winnerPositionSet((size_t)indexSize);
  for(int index = 0; index < indexSize; index++) {
    if(bs.getBit()) {
      winnerPositionSet.add(index);
    }
  }
  const UINT64 winnerPositions = winnerPositionSet.size();

  if(winnerPositions != info.getWinnerPositionCount().getTotal()) {
    throwException(_T("Counted winnerPositions=%I64u != info.totalWinnerPosition:%s (=%s)")
                  ,winnerPositions
                  ,info.getWinnerPositionCount().toString().cstr()
                  ,format1000(info.getWinnerPositionCount().getTotal()).cstr());
  }

  BitSetIndex positionIndex(winnerPositionSet);

  PackedArray positionInfoArray(bitsPerEntry);
  positionInfoArray.setCapacity(winnerPositions);
  positionInfoArray.addZeroes(0,winnerPositions);

  if(canWinFlags == BOTHCANWIN) {
    for(UINT64 i = 0; i < winnerPositions; i++) {
      if(bs.getBit()) {
        positionInfoArray.set(i, 1);
      }
    }
  }

  switch(bitsPerMove) {
  case 1 :
    if(canWinFlags == BOTHCANWIN) {
      READMOVEBITS(or , 1, 1);
    } else {
      READMOVEBITS(set, 1, 0)
    }
    break;
  case 2:
    if(canWinFlags == BOTHCANWIN) {
      READMOVEBITS(or , 2, 1);
    } else {
      READMOVEBITS(set, 2, 0)
    }
    break;
  case 3 :
    if(canWinFlags == BOTHCANWIN) {
      READMOVEBITS(or , 2, 1);
      READMOVEBITS(or , 1, 3)
    } else {
      READMOVEBITS(set, 2, 0);
      READMOVEBITS(or , 1, 2)
    }
    break;
  default: // 4, 5, 6, 7, 8
    if(canWinFlags == BOTHCANWIN) {
      READMOVEBITS(or , 4, 1);
    } else {
      READMOVEBITS(set, 4, 0);
    }
    break;
  }

  switch(bitsPerMove) {
  case 5:
    if(canWinFlags == BOTHCANWIN) {
      READMOVEBITS(or , 1, 5);
    } else {
      READMOVEBITS(or , 1, 4);
    }
    break;
  case 6:
    if(canWinFlags == BOTHCANWIN) {
      READMOVEBITS(or , 2, 5);
    } else {
      READMOVEBITS(or , 2, 4);
    }
    break;
  case 7:
    if(canWinFlags == BOTHCANWIN) {
      READMOVEBITS(or , 2, 5);
      READMOVEBITS(or , 1, 7);
    } else {
      READMOVEBITS(or , 2, 4);
      READMOVEBITS(or , 1, 6);
    }
    break;
  case 8:
    if(canWinFlags == BOTHCANWIN) {
      READMOVEBITS(or , 4, 5);
    } else {
      READMOVEBITS(or , 4, 4);
    }
    break;
  }

  // save decompressed format
  ByteOutputFile          file(m_keydef.getDecompressedFileName());
  ByteCounter             outputCounter;
  CountedByteOutputStream out(outputCounter, file);

  DecompressedHeader header(info); // parts undefined. defined it below and rewrite it

  header.save(out);
  header.m_bitSetIndexOffset = (UINT)outputCounter.getCount();
  positionIndex.save(out);
  header.m_arrayStartOffset  = (UINT)outputCounter.getCount();
  positionInfoArray.save(out);
  header.m_bitsPerEntry      = bitsPerEntry;
  file.seek(0);
  header.save(file);

#ifdef __NEVER__
  FILE *logFile = FOPEN(_T("c:\\temp\\decomp.txt"), _T("w"));
  int arrayIndex = 0;
  for(Iterator<UINT> it = winnerPositionSet.getIterator(); it.hasNext();) {
    UINT             index  = it.next();
    const EndGameKey key    = m_keydef.indexToKey(index);
    UINT             entry  = positionInfoArray.get(arrayIndex++);
    Player           winner;
    if(canWinFlags == BOTHCANWIN) {
      winner = (entry & 1) ? BLACKPLAYER : WHITEPLAYER;
      entry >>= 1;
    } else {
      winner = (canWinFlags&WHITECANWIN)?WHITEPLAYER:BLACKPLAYER;
    }

    EndGameResult eg((winner == WHITEPLAYER) ? EG_WHITEWIN : EG_BLACKWIN, (2*entry) + ((key.getPlayerInTurn() != winner ) ? 1 : 0));

    _ftprintf(logFile, _T("%s:%s\n"), key.toString(m_keydef).cstr(), eg.toString(true).cstr());
  }
  fclose(logFile);
#endif // __NEVER__

}

void IndexedMap::load() {
  clear();
  const String fileName = m_keydef.getDecompressedFileName();
  const DecompressedHeader header(fileName);
  m_canWinFlags   = header.getCanWinFlags();
  m_positionIndex = new FileBitSetIndex(fileName, header.m_bitSetIndexOffset);
  m_infoArray     = new PackedFileArray(fileName, header.m_arrayStartOffset);
//  verbose(_T("%s"), m_positionIndex->getInfoString().cstr());
}

EndGameResult IndexedMap::get(EndGameKey key) const {
  if(!isAllocated()) {
    throwException(_T("%s:Index not loaded"), __TFUNCTION__);
  }
  const intptr_t index = m_positionIndex->getIndex((size_t)m_keydef.keyToIndex(key));

  if(index < 0) {
    return EndGameResult(EG_DRAW, 0);
  } else if(m_canWinFlags == BOTHCANWIN) {
    const UINT         info         = m_infoArray->get(index);
    const Player       winner       = (info & 1) ? BLACKPLAYER : WHITEPLAYER;
    const Player       playerInTurn = key.getPlayerInTurn();
    const int          movesToEnd   = info >> 1;
    return EndGameResult((winner == WHITEPLAYER) ? EG_WHITEWIN : EG_BLACKWIN, (2*movesToEnd) + ((playerInTurn != winner ) ? 1 : 0));
  } else {
    const UINT         info         = m_infoArray->get(index);
    const Player       winner       = (m_canWinFlags|WHITECANWIN)?WHITEPLAYER:BLACKPLAYER;;
    const Player       playerInTurn = key.getPlayerInTurn();
    const int          movesToEnd   = info;
    return EndGameResult((winner == WHITEPLAYER) ? EG_WHITEWIN : EG_BLACKWIN, (2*movesToEnd) + ((playerInTurn != winner ) ? 1 : 0));
  }
}

#else // NEWCOMPRESSION

class DecompressedHeader : public TablebaseInfo {
public:
  BYTE   m_bitsPerEntry;
  UINT64 m_wpIndexAddress;
  UINT64 m_npIndexAddress;
  UINT64 m_whoWinAddress;
  UINT64 m_arrayAddress;

  DecompressedHeader(const TablebaseInfo &info);
  DecompressedHeader(const String &fileName) {
    load(ByteInputFile(fileName));
  }
  void save(ByteOutputStream &s) {
    s.putBytes((BYTE*)this, sizeof(DecompressedHeader));
  }
  void load(ByteInputStream  &s) {
    s.getBytesForced((BYTE*)this, sizeof(DecompressedHeader));
  }
};

DecompressedHeader::DecompressedHeader(const TablebaseInfo &info) : TablebaseInfo(info) {
  m_bitsPerEntry   = 0;
  m_wpIndexAddress = 0;
  m_npIndexAddress = 0;
  m_whoWinAddress  = 0;
  m_arrayAddress   = 0;
}

IndexedMap::IndexedMap(const EndGameKeyDefinition &keydef) : m_keydef(keydef) {
  init();
}

void IndexedMap::init() {
  m_wpIndex   = NULL;
  m_npIndex   = NULL;
  m_whoWinSet = NULL;
  m_infoArray = NULL;
}

IndexedMap::~IndexedMap() {
  clear();
}

void IndexedMap::clear() {
  delete m_wpIndex;
  delete m_npIndex;
  delete m_whoWinSet;
  delete m_infoArray;

  init();
}

// Read format written with IndexedMap::saveCompressed(BigEndianOutputStream &s);
void IndexedMap::decompress(BigEndianInputStream &in, const TablebaseInfo &info) const {
  const INT64 indexSize = m_keydef.getIndexSize();
  if(info.m_indexCapacity != indexSize) {
    throwInvalidArgumentException(__TFUNCTION__, _T("info.indexCapacity=%s, keydef.indexSize=%s")
                                 ,format1000(info.m_indexCapacity).cstr()
                                 ,format1000(indexSize).cstr());
  }

  const BYTE  canWinFlags  = info.getCanWinFlags();
  const bool  hasWhoWinSet = (canWinFlags == BOTHCANWIN); // do we need a BitSet to indicate who wins, or is the winner always the same
  const int   maxPly       = info.m_maxPlies.getMax();
  const int   maxMoves     = PLIESTOMOVES(maxPly);
  const BYTE  bitsPerMove  = getBitCount(maxMoves);
  const BYTE  bitsPerEntry = bitsPerMove - 1;

  BitSet wpSet(10);
  in >> wpSet;
  const UINT64 wpCount = wpSet.size();

  if(wpCount != info.getWinnerPositionCount().getTotal()) {
    throwException(_T("Counted winnerPositions=%I64u != info.totalWinnerPosition:%s (=%s)")
                  ,wpCount
                  ,info.getWinnerPositionCount().toString().cstr()
                  ,format1000(info.getWinnerPositionCount().getTotal()).cstr());
  }

  BitSet whoWinSet(10);
  if(hasWhoWinSet) {
    in >> whoWinSet;
    if(whoWinSet.getCapacity() != wpCount) {
      throwException(_T("WhoWinSet.capacity==%s != wpCount(=%s)")
                    ,format1000(whoWinSet.getCapacity()).cstr()
                    ,format1000(wpCount).cstr());
    }
  }
  BitSet npSet(10);
  in >> npSet;
  if(npSet.getCapacity() != wpCount) {
    throwException(_T("npSet.capacity==%s != winnerPositions(=%s)")
                  ,format1000(npSet.getCapacity()).cstr()
                  ,format1000(wpCount).cstr());
  }

  const UINT64 npCount = npSet.size();
  PackedArray infoArray(max(1,bitsPerEntry));
  infoArray.setCapacity(npCount);
  infoArray.addZeroes(0,npCount);

#define BUFFERSIZE 4096
  BYTE buffer[BUFFERSIZE];
  for (UINT64 i = 0; i < npCount;) {
    const UINT n       = (UINT)min(ARRAYSIZE(buffer), (npCount-i));
    const BYTE *bufEnd = buffer + n;
    in.getBytesForced(buffer, n);
    for(const BYTE *src = buffer; src < bufEnd;) {
      infoArray.set(i++, *(src++));
    }
  }

  // save decompressed format
  ByteOutputFile          file(m_keydef.getDecompressedFileName());
  ByteCounter             outputCounter;
  CountedByteOutputStream out(outputCounter, file);

  DecompressedHeader      header(info); // some parts undefined. define it below and rewrite it
  BitSetIndex             wpIndex(wpSet);
  BitSetIndex             npIndex(npSet);

  header.save(out);
  header.m_wpIndexAddress     = outputCounter.getCount();
  wpIndex.save(out);

  if(bitsPerEntry > 0) {
    header.m_npIndexAddress   = outputCounter.getCount();
    npIndex.save(out);
  }
  if(hasWhoWinSet) {
    header.m_whoWinAddress    = outputCounter.getCount();
    whoWinSet.save(out);
  }
  if(bitsPerEntry > 0) {
    header.m_arrayAddress = outputCounter.getCount();
    infoArray.save(out);
  }
  header.m_bitsPerEntry       = bitsPerEntry;
  file.seek(0);
  header.save(file);
}

void IndexedMap::load() {
  clear();
  const String fileName = m_keydef.getDecompressedFileName();
  DecompressedHeader header(fileName);
  m_canWinFlags    = header.getCanWinFlags();
  m_npFlags        = header.getNPFlags();
  m_wpIndex = new FileBitSetIndex(fileName, header.m_wpIndexAddress);
  m_npIndex     = new FileBitSetIndex(fileName, header.m_npIndexAddress);
  if (m_canWinFlags == BOTHCANWIN) {
    m_whoWinSet = new FileBitSet(fileName, header.m_whoWinAddress);
  }
  m_infoArray      = new PackedFileArray(fileName, header.m_arrayAddress);
}

EndGameResult IndexedMap::get(EndGameKey key) const {
  if(!isAllocated()) {
    throwException(_T("%s:Index not loaded"), __TFUNCTION__);
  }
  const intptr_t index1 = m_wpIndex->getIndex((size_t)m_keydef.keyToIndex(key));
  if(index1 < 0) {
    return EGR_DRAW;
  } else {
    Player winner;
    if(m_canWinFlags == BOTHCANWIN) {
      winner = m_whoWinSet->contains(index1) ? BLACKPLAYER : WHITEPLAYER;
    } else {
      winner = (m_canWinFlags | WHITECANWIN) ? WHITEPLAYER : BLACKPLAYER;
    }
    const EndGamePositionStatus status = (winner==WHITEPLAYER) ? EG_WHITEWIN : EG_BLACKWIN;
    const Player playerInTurn = key.getPlayerInTurn();
    const Player NPPlayer     = NPGETPLAYER(m_npFlags);
    if(winner == NPPlayer) {
      const intptr_t index2 = m_npIndex->getIndex(index1);
      if(index2 >= 0) {
        const int info = m_infoArray->get(index2);
        const int movesToEnd   = (info << 1) | NPGETPARITY(m_npFlags);
        return EndGameResult(status, (2*movesToEnd) + ((playerInTurn != winner ) ? 1 : 0));
      }
    }
    return EndGameResult(status, 0);
  }
}

#endif // NEWCOMPRESSION

#endif // TABLEBASE_BUILDER
