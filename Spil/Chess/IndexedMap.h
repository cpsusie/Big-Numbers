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
  unsigned int getCount();
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
  const unsigned long         m_indexSize;                                // keydef.getIndexSize()
  void allocate();
  void rethrowException(Exception &e, const EndGameKey &key) const;
#ifdef _DEBUG
  unsigned int getCheckedIndex(const EndGameKey &key) const;
#endif

public:
  IndexedMap(const EndGameKeyDefinition &keydef);

  const EndGameKeyDefinition &getKeyDefinition() const {
    return m_keydef;
  }

  const EndGameResult &get(const EndGameKey &key) const;
  bool isAllocated() const {
    return size() > 0;
  }

  EndGameResult &get(const EndGameKey &key);
  bool remove(const EndGameKey &key);

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
  void saveCompressed(ByteOutputStream &s, const TablebaseInfo &info) const;


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
  EndGameEntryIterator getIteratorWinnerEntries(unsigned int pliesToEnd); // All winner entries with specified pliesToEnd
  EndGameEntryIterator getIteratorAllWinnerEntries();                     // All winner entries
  EndGameEntryIterator getIteratorAllNonWinnerEntries();                  // All existing, undefined or draw entries
  EndGameEntryIterator getIteratorCheckmateEntries(Player winner);
  EndGameEntryIterator getIteratorStalemateEntries();
  EndGameEntryIterator getIteratorMinPliesToEnd(  Player winner, unsigned int minPliesToEnd);
  EndGameEntryIterator getIteratorExactPliesToEnd(Player winner, unsigned int pliesToEnd);
  EndGameEntryIterator getIteratorNonEmptyHelpInfo();
};

class PackedIndexedMap {
private:
  const EndGameKeyDefinition &m_keydef;
  const unsigned long         m_indexSize;                                // m_keydef.getIndexSize()
  const bool                  m_getResultEnabled;
  PackedArray                 m_statusArray;
  void rethrowException(Exception &e, const EndGameKey &key) const;
#ifdef _DEBUG
  unsigned int getCheckedIndex(const EndGameKey &key) const;
#endif
  static int findBitsPerItem(unsigned int maxPlies);
public:
  PackedIndexedMap(const EndGameKeyDefinition &keydef, bool enableGetResult, unsigned int maxPlies);
  void load(ByteInputStream &s);

  const EndGameKeyDefinition &getKeyDefinition() const {
    return m_keydef;
  }

  EndGamePositionStatus getPositionStatus(const EndGameKey &key) const;
  EndGameResult         getPositionResult(const EndGameKey &key) const;
  bool isAllocated() const {
    return m_statusArray.size() > 0;
  }
  void clear() {
    m_statusArray.clear();
  }
};

#else

class IndexedMap {
private:
  const EndGameKeyDefinition &m_keydef;
  unsigned char               m_canWinFlags;
  BitSetFileIndex            *m_positionIndex;
  PackedFileArray            *m_infoArray;

public:
  IndexedMap(const EndGameKeyDefinition &keydef);
  IndexedMap(const IndexedMap &src);                                      // not defined
  IndexedMap &operator=(const IndexedMap &src);                           // not defined
  ~IndexedMap();
  EndGameResult get(const EndGameKey &key) const;
  bool isAllocated() const {
    return m_positionIndex != NULL;
  }
  void decompress(ByteInputStream &s, const TablebaseInfo &info) const;
  void clear();
  void load();
};

#endif
