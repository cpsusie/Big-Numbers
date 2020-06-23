#include "stdafx.h"
#include <ByteFile.h>
#include <CountedByteStream.h>
#include <BitStream.h>
#include <TimeEstimator.h>
#include "IndexedMap.h"

#if defined(TABLEBASE_BUILDER)

#if !defined(_DEBUG)
#define GETELEMENT(key) (*this)[(size_t)m_keydef.keyToIndex(key)]
#else // _DEBUG
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
    const EndGameEntry    &e          = it.next();
    const EndGamePosIndex  newIndex64 = m_keydef.keyToIndexNew(e.getKey());
    const size_t           newIndex   = (size_t)newIndex64;
    if(newIndex >= m_indexSize) {
      throwException(_T("Key [%s] gives wrong index=%s. max=%s")
                    ,e.getKey().toString(m_keydef).cstr()
                    ,format1000(newIndex).cstr()
                    ,format1000(m_indexSize-1).cstr()
                    );
    }
    if(copy[(size_t)newIndex].exists()) {
      throwException(_T("Key [%s] already converted"), e.getKey().toString(m_keydef).cstr());
    }
    copy[newIndex] = e.getValue();
  }
  ((CompactArray<EndGameResult>&)*this) = copy;
}

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
  AbstractIterator *clone()       override {
    return new IndexedMapKeyIterator(*this);
  }
  bool hasNext()            const override {
    return m_current <= m_lastElement;
  }
  void *next()                    override;
  void remove()                   override;
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
  const double         m_maxProgress;
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

  double getMaxProgress() const {
    return m_maxProgress;
  }
  double getProgress() const {
    return (double)(m_current - m_firstElement + (m_odd?0:m_maxProgress))/2;
  }

  inline double getMilliSecondsUsed() const {
    return Timestamp::diff(m_startTime, Timestamp(), TMILLISECOND);
  }

  inline double getMilliSecondsRemaining() const {
    return m_timeEstimator->getMilliSecondsRemaining();
  }
};

IndexedMapEntryIterator::IndexedMapEntryIterator(IndexedMap &map)
: m_entry(       map                                  )
, m_firstElement((EndGameResult*)map.getFirstElement())
, m_lastElement( (EndGameResult*)map.getLastElement() )
, m_maxProgress( (double)map.size()                   )
, m_odd(         true                                 )
{
  m_current = m_firstElement+1;
  m_timeEstimator = new TimeEstimator(*this); TRACE_NEW(m_timeEstimator);
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
, m_maxProgress( src.m_maxProgress )
, m_startTime(   src.m_startTime   )
, m_odd(         src.m_odd         )
{
  m_timeEstimator = new TimeEstimator(*this); TRACE_NEW(m_timeEstimator);
}

IndexedMapEntryIterator::~IndexedMapEntryIterator() {
  SAFEDELETE(m_timeEstimator);
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
  AbstractIterator *clone() override { return new name(*this); }                          \
  void *next()              override;                                                     \
  UINT64 getCount()   const override;                                                     \
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

#endif // TABLEBASE_BUILDER
