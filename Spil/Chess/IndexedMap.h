#pragma once

#include <PackedArray.h>
#include "EndGameUtil.h"

#ifdef TABLEBASE_BUILDER

class EndGameEntryIterator : public Iterator<EndGameEntry> {
private:
  FILE *m_logFile;
public:
  EndGameEntryIterator(AbstractIterator *it) : Iterator<EndGameEntry>(it) {
    m_logFile = NULL;
  }
  ~EndGameEntryIterator() {
    setLog(false);
  }
  UINT64 getCount();
  double getPercentDone() const;
  double getMilliSecondsRemaining() const;
  double getMilliSecondsUsed() const;
  String getTimeRemainingStr() const {
    return secondsToString(getMilliSecondsRemaining(), false);
  }
  String getProgressStr() const;
  void setLog(bool log);
};

class IndexedMap : public CompactArray<EndGameResult> {
private:
  const EndGameKeyDefinition &m_keydef;
  const EndGamePosIndex       m_indexSize; // keydef.getIndexSize()
  void allocate();
  void rethrowException(Exception &e, EndGameKey key) const;
#ifdef _DEBUG
  EndGamePosIndex getCheckedIndex(EndGameKey key) const;
#endif

#ifdef NEWCOMPRESSION
  void pruneAllPliesByNP(NotPrunedFlags np);
#endif // NEWCOMPRESSION
public:
  IndexedMap(const EndGameKeyDefinition &keydef);

  const EndGameKeyDefinition &getKeyDefinition() const {
    return m_keydef;
  }

  const EndGameResult &get(EndGameKey key) const;
  bool isAllocated() const {
    return size() > 0;
  }

  EndGameResult &get(EndGameKey key);
  bool remove(EndGameKey key);
#ifdef NEWCOMPRESSION
  PositionTypeCounters countWinnerPositionTypes();
#endif // NEWCOMPRESSION
  IndexedMap &clearAllVisited();
  IndexedMap &clearAllChanged();
  IndexedMap &clearAllMarked();
  IndexedMap &clearHelpInfo();                                            // clear all visited, all changed, and all marked

  IndexedMap &markAllChanged();
  IndexedMap &markAllVisited();

  const EndGameResult *getFirstElement() const {
    return size() ? &first() : NULL;
  }

  const EndGameResult *getLastElement() const {
    return size() ? &last() : NULL;
  }

  void convertIndex();
#ifdef NEWCOMPRESSION
  void saveCompressed(   BigEndianOutputStream &out, const TablebaseInfo &info);
  void saveCompressedNew(BigEndianOutputStream &out, const TablebaseInfo &info);
#else // !NEWCOMPRESSION
  void saveCompressed(   ByteOutputStream &s, const TablebaseInfo &info) const;
  void saveCompressedNew(ByteOutputStream &s, const TablebaseInfo &info) const;
#endif // NEWCOMPRESSION

  EndGameKeyIterator   getKeyIterator();
  EndGameEntryIterator getEntryIterator();                                // All existing entries
  EndGameEntryIterator getIteratorUndefinedEntries();                     // All existing, undefined entries
  EndGameEntryIterator getIteratorVisitedEntries();                       // All existing, visited entries
  EndGameEntryIterator getIteratorUnvisitedEntries();                     // All existing, unvisited entries
  EndGameEntryIterator getIteratorUndefinedUnvisitedEntries();            // All existing, undefined, unvisited entries
  EndGameEntryIterator getIteratorChangedEntries();                       // All existing, changed entries
  EndGameEntryIterator getIteratorChangedUnvisitedEntries();              // All existing, changed, unvisited entries
  EndGameEntryIterator getIteratorMarkedEntries();                        // All marked entries
  EndGameEntryIterator getIteratorUnmarkedEntries();                      // All marked entries
  EndGameEntryIterator getIteratorWinnerEntries(UINT pliesToEnd);         // All winner entries with specified pliesToEnd
  EndGameEntryIterator getIteratorAllWinnerEntries();                     // All winner entries
  EndGameEntryIterator getIteratorAllNonWinnerEntries();                  // All existing, undefined or draw entries
  EndGameEntryIterator getIteratorCheckmateEntries(Player winner);
  EndGameEntryIterator getIteratorStalemateEntries();
  EndGameEntryIterator getIteratorMinPliesToEnd(  Player winner, UINT minPliesToEnd);
  EndGameEntryIterator getIteratorExactPliesToEnd(Player winner, UINT pliesToEnd);
  EndGameEntryIterator getIteratorNonEmptyHelpInfo();
};

class PackedIndexedMap {
private:
  const EndGameKeyDefinition &m_keydef;
  const EndGamePosIndex       m_indexSize;                                // m_keydef.getIndexSize()
  const bool                  m_getResultEnabled;
  PackedArray                 m_statusArray;
  void rethrowException(Exception &e, EndGameKey key) const;
#ifdef _DEBUG
  EndGamePosIndex getCheckedIndex(EndGameKey key) const;
#endif
  static int findBitsPerItem(UINT maxPlies);
public:
  PackedIndexedMap(const EndGameKeyDefinition &keydef, bool enableGetResult, UINT maxPlies);
  void load(ByteInputStream &s);

  const EndGameKeyDefinition &getKeyDefinition() const {
    return m_keydef;
  }

  EndGamePositionStatus getPositionStatus(EndGameKey key) const;
  EndGameResult         getPositionResult(EndGameKey key) const;
  bool isAllocated() const {
    return m_statusArray.size() > 0;
  }
  void clear() {
    m_statusArray.clear();
  }
};

#else // !TABLEBASE_BUILDER

#ifndef NEWCOMPRESSION

class IndexedMap {
private:
  const EndGameKeyDefinition &m_keydef;
  BYTE                        m_canWinFlags;
  FileBitSetIndex            *m_positionIndex;
  PackedFileArray            *m_infoArray;

public:
  IndexedMap(const EndGameKeyDefinition &keydef);
  IndexedMap(const IndexedMap &src);                                      // not defined
  IndexedMap &operator=(const IndexedMap &src);                           // not defined
  ~IndexedMap();
  EndGameResult get(EndGameKey key) const;
  bool isAllocated() const {
    return m_positionIndex != NULL;
  }
  void decompress(ByteInputStream &s, const TablebaseInfo &info) const;
  void clear();
  void load();
};

#else // NEWCOMPRESSION

class IndexedMap {
private:
  const EndGameKeyDefinition &m_keydef;
  BYTE                        m_canWinFlags;
  NotPrunedFlags              m_npFlags;
  FileBitSetIndex            *m_wpIndex;
  FileBitSetIndex            *m_npIndex;
  FileBitSet                 *m_whoWinSet;
  PackedFileArray            *m_infoArray;
  void init();
  IndexedMap(const IndexedMap &src);                                      // not defined
  IndexedMap &operator=(const IndexedMap &src);                           // not defined
public:
  IndexedMap(const EndGameKeyDefinition &keydef);
  ~IndexedMap();
  EndGameResult get(EndGameKey key) const;
  bool isAllocated() const {
    return m_wpIndex != NULL;
  }
  void decompress(BigEndianInputStream &s, const TablebaseInfo &info) const;
  void clear();
  void load();
};

#endif // NEWCOMPRESSION

#endif // TABLEBASE_BUILDER
