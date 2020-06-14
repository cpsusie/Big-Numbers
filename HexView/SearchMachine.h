#pragma once

#include <MFCUtil/InteractiveRunnable.h>
#include "AddrRange.h"

class ByteContainer {
public:
  virtual void   getBytes(UINT64 start, UINT length, ByteArray &dst) = 0;
  virtual BYTE   getByte( UINT64 addr) = 0;
  virtual UINT64 getSize() const = 0;
};

class UpdatableByteContainer : public ByteContainer {
public:
  virtual void putBytes(UINT64 start, ByteArray &src) = 0;
};

class SequentialByteContainer { // Emulates a FILE, with fseek and fread, given a ByteContainer
private:
  ByteContainer &m_bc;
  const INT64    m_size;
  INT64          m_position;
public:
  SequentialByteContainer(ByteContainer &bc);
  int  fread(UINT count, BYTE *buffer);
  void fseek(INT64 pos);
};

class SearchParameters {
public:
  bool           m_forwardSearch;
  INT64          m_startPosition;
  String         m_findWhat;
  SearchParameters() : m_forwardSearch(true), m_startPosition(0) {
  }
  inline bool isSet() const {
    return m_findWhat.length() > 0;
  }
  inline bool isForwardSearch() const {
    return m_forwardSearch;
  }
  inline INT64 getStartPosition() const {
    return m_startPosition;
  }
  inline const String &getFindWhat() const {
    return m_findWhat;
  }
};

class SearchMachine : public InteractiveRunnable {
private:
  SearchParameters m_searchParam;
  ByteContainer   *m_byteContainer;

  AddrRange        m_result;
  INT64            m_size;
  INT64            m_fileIndex;
  INT64            m_maxProgress;
  AddrRange doSearch();
public:
  SearchMachine();

  void prepareSearch(bool forwardSearch, INT64 startPosition, const String &findWhat, ByteContainer *byteContainer);
  void prepareSearch(const SearchParameters &param, ByteContainer *byteContainer);
  // by using default-values for parameter 2,3,4, searchMachine will use the same values as last call. And Exception is thrown if
  // byteContainer isn't specified in this or previous calls

  const SearchParameters &getSearchParameters() const {
    return m_searchParam;
  }
  inline bool isSet() const {
    return getSearchParameters().isSet();
  }
  inline bool isForwardSearch() const {
    return getSearchParameters().isForwardSearch();
  }
  inline INT64 getStartPosition() const {
    return getSearchParameters().getStartPosition();
  }
  inline const String &getFindWhat() const {
    return getSearchParameters().getFindWhat();
  }
// ---------------------- Functions to implement InteractiveRunnable ---------------
  UINT safeRun();
  double getMaxProgress() const {
    return (double)m_maxProgress;
  }
  double getProgress() const;
  String getTitle() {
    return isForwardSearch() ? _T("Find next occurrence") : _T("Find previous occurrence");
  }
  int getSupportedFeatures() {
    return IR_PROGRESSBAR | IR_INTERRUPTABLE;
  }
// ---------------------------------------------------------------------------------

  const AddrRange &getResult() const {
    return m_result;
  }
  int getPatternLength() const;
};

class SearchPattern : public ByteArray, SettingsAccessor {
private:
  SearchPattern &convert(const String &pattern);
public:
  SearchPattern(const String &pattern);
  SearchPattern &operator=(const String &pattern);
  String toString() const;
};

extern BYTE escape(const TCHAR *&s);
bool isOctDigit(TCHAR ch);
BYTE octToByte( int   ch);
BYTE decToByte( int   ch);
BYTE hexToByte( int   ch);
