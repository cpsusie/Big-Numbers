#pragma once

#include <MFCUtil/InteractiveRunnable.h>
#include "AddrRange.h"

class ByteContainer {
public:
  virtual void      getBytes(unsigned __int64 start, UINT length, ByteArray &dst) = 0;
  virtual BYTE      getByte( unsigned __int64 addr) = 0;
  virtual unsigned __int64 getSize() const = 0;
};

class UpdatableByteContainer : public ByteContainer {
public:
  virtual void putBytes(unsigned __int64 start, ByteArray &src) = 0;
};

class SequentialByteContainer { // Emulates a FILE, with fseek and fread, given a ByteContainer
private:
  ByteContainer &m_bc;
  const __int64  m_size;
  __int64        m_position;
public:
  SequentialByteContainer(ByteContainer &bc);
  int fread(UINT count, BYTE *buffer);
  void fseek(__int64 pos);
};

class SearchMachine : public InteractiveRunnable {
private:
  bool           m_forwardSearch;
  __int64        m_startPosition;
  String         m_findWhat;
  ByteContainer *m_byteContainer;

  bool           m_finished;
  AddrRange      m_result;
  String         m_resultMessage;
  __int64        m_size;
  __int64        m_fileIndex;

  AddrRange doSearch();
public:

  SearchMachine();

  void prepareSearch(bool forwardSearch, __int64 startPosition = -1, const String &findWhat = _T(""), ByteContainer *byteContainer = NULL);
  // by using default-values for parameter 2,3,4, searchMachine will use the same values as last call. And Exception is thrown if
  // byteContainer isn't specified in this or previous calls

  bool isForwardSearch() const {
    return m_forwardSearch;
  }
  
  __int64 getStartPosition() const {
    return m_startPosition;
  }
  
  const String getFindWhat() const {
    return m_findWhat;
  }
// ---------------------- Functions to implement InteractiveRunnable ---------------
  UINT run();
  double getProgress() const;
  double getMaxProgress() const {
    return 1000;
  }
  String getTitle() {
    return m_forwardSearch ? _T("Find next occurrence") : _T("Find previous occurrence");
  }
  int getSupportedFeatures() {
    return IR_PROGRESSBAR | IR_INTERRUPTABLE;
  }
// ---------------------------------------------------------------------------------

  bool isSet() const {
    return m_findWhat.length() > 0;
  }
  bool isFinished() const {
    return m_finished;
  }

  const AddrRange &getResult() const {
    return m_result;
  }

  const String &getResultMessage() const {
    return m_resultMessage;
  }

  int getPatternLength() const;
};

class SearchPattern : public ByteArray, SettingsAccessor {
private:
  void convert(const String &pattern);
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
